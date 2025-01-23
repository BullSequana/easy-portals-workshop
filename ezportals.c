/*
 * Copyright (C) Bull S.A.S - 2025
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <portals4.h>
#include <portals4_bxiext.h>

#include "ezportals.h"

#define NI_OPTIONS PTL_NI_PHYSICAL|PTL_NI_MATCHING
#define EQ_SIZE 65535
#define PT_INDEX 6

#define EZP_ASSERT(condition, message) do { \
	if (!(condition)) { \
		fprintf(stderr, "%s\n", message); \
		fflush(stderr); \
	} \
	assert(condition); \
} while (0)

struct ezContext {
	int id;
	unsigned int pid_num_start;
	ptl_handle_ni_t nih;
	ptl_process_t self;
	ptl_handle_eq_t eqh;
	ptl_pt_index_t pti;
	ptl_handle_ct_t cth;
	char str[PTL_EV_STR_SIZE];
	bool print_events;
};

thread_local struct ezContext ezc;
pthread_barrier_t ezBarrier;

void init(int num_threads)
{
	pthread_barrier_init(&ezBarrier, NULL, num_threads);
}

void barrier(void)
{
	pthread_barrier_wait(&ezBarrier);	
}

void ezpInit(int id)
{
	int rc;
	const char *env;
	unsigned int group;
	unsigned int iface;
	ptl_ni_limits_t desired;

	/* Get GROUP identifier to define NIC hw ressource
	 * to be used (iface, pids)
	 * GROUP   iface   pids
	 * 0       0       3000-3015
	 * 1       0       3100-3115
	 * 2       0       3200-3215
	 * 3       0       3300-3315
	 * 4       1       3000-3015
	 * ...
	 * 15      3       3300-3315
	 * */
	ezc.pid_num_start = 0;
	env = getenv("EZP_GROUP");
	if (env) {
		rc = sscanf(env, "%u", &group);
		EZP_ASSERT(rc == 1, "Invalid EZP_GROUP value");
		EZP_ASSERT(group >= 0 && group < 16, "Invalid EZP_GROUP value (must be between 0 and 15)");
		iface = group / 4;
		ezc.pid_num_start = 3000 + (group % 4) * 100;
	}
	EZP_ASSERT(ezc.pid_num_start != 0, "Missing configuration with EZP_GROUP environment variable");

	env = getenv("EZP_PRINT_EVENTS");
	if (env) 
		ezc.print_events = true;
	else
		ezc.print_events = false;

	/* Allow each thread to get a distinct PID */
	rc = setenv("PORTALS4_REUSE_DEV", "0", 1);
	EZP_ASSERT(rc == 0, "setenv failed");

	rc = PtlInit();
	EZP_ASSERT(rc == PTL_OK, "PtlInit failed");

	/* Initialize thread communication context */
	ezc.nih = PTL_INVALID_HANDLE;
	ezc.eqh = PTL_INVALID_HANDLE;
	ezc.pti = UINT_MAX;

	EZP_ASSERT(id >= 0 && id < 16, "ezpInit(): invalid id (must be between 0 and 15)");
	ezc.id = id;

	/* Set desired limits */
	memset(&desired, 0, sizeof(desired));
	desired.max_entries = 65536;
	desired.max_mds = 65536;
	desired.max_cts = 65536;
	desired.max_eqs = 1;
	desired.max_pt_index = 10;
	desired.max_triggered_ops = 1024;

	rc = PtlNIInit(iface, NI_OPTIONS, ezc.pid_num_start + id,
		       &desired, NULL, &ezc.nih);
	EZP_ASSERT(rc == PTL_OK, "PtlNIInit() failed");

	rc = PtlGetPhysId(ezc.nih, &ezc.self);
	EZP_ASSERT(rc == PTL_OK, "PtlGetPhysId() failed");
	//printf("DEBUG: self nid=%d pid=%d\n", ezc.self.phys.nid, ezc.self.phys.pid);

	rc = PtlCTAlloc(ezc.nih, &ezc.cth);
	EZP_ASSERT(rc == PTL_OK, "PtlCTAlloc() failed");
}

void ezpEQAlloc(void)
{
	int rc;

	EZP_ASSERT(PtlHandleIsEqual(ezc.eqh, PTL_INVALID_HANDLE), "ezpEQAlloc(): EQ has already been allocated");

	rc = PtlEQAlloc(ezc.nih, EQ_SIZE, &ezc.eqh);
	EZP_ASSERT(rc == PTL_OK, "PtlEQAlloc() failed");
}

char *ezpEQWait(void)
{
	int rc;
	ptl_event_t event;

	rc = PtlEQWait(ezc.eqh, &event);
	EZP_ASSERT(rc == PTL_OK, "PtlEQWait() failed");

	snprintf(ezc.str, PTL_EV_STR_SIZE, "[%d] %s event : %s\n",
		 ezc.id, PtlToStr(event.type, PTL_STR_EVENT),
		 PtlToStr(event.ni_fail_type, PTL_STR_FAIL_TYPE));

	if (ezc.print_events)
		printf("%s", ezc.str);

	return ezc.str;
}

ezpMD ezpMDBind(void *addr, size_t length)
{
	int rc;
	ptl_md_t md;
	ptl_handle_md_t mdh;

	md.start = addr;
	md.length = length;
	md.options = PTL_MD_EVENT_SEND_DISABLE;
	md.eq_handle = ezc.eqh;
	md.ct_handle = PTL_CT_NONE;

	rc = PtlMDBind(ezc.nih, &md, &mdh);
	EZP_ASSERT(rc == PTL_OK, "PtlMDBind() failed");

	return mdh;
}

ezpME ezpMEAppend(void *addr, size_t length, uint64_t tag)
{
	int rc;
	ptl_me_t me;
	ptl_handle_me_t meh;

	/* Allocate PTE if not yet done */
	if (ezc.pti == UINT_MAX) {
		EZP_ASSERT(!PtlHandleIsEqual(ezc.eqh, PTL_INVALID_HANDLE), "ezpMEAppend(): EQ needs to be allocated first");
		rc = PtlPTAlloc(ezc.nih, 0, ezc.eqh, PT_INDEX, &ezc.pti);
		EZP_ASSERT(rc == PTL_OK, "PtlPTAlloc() failed");
		EZP_ASSERT(ezc.pti == PT_INDEX, "PtlPTAlloc() returned an unexpected index");
	}

	me.start = addr;
	me.length = length;
	me.ct_handle = ezc.cth;
	me.uid = PTL_UID_ANY;
	me.options = PTL_ME_OP_PUT | PTL_ME_OP_GET |
		     PTL_ME_USE_ONCE | PTL_ME_EVENT_UNLINK_DISABLE |
		     PTL_ME_EVENT_CT_COMM;
	me.match_id.phys.nid = PTL_NID_ANY;
	me.match_id.phys.pid = PTL_PID_ANY;
	me.match_bits = tag;
	me.ignore_bits = tag == EZP_TAG_ANY ? 0xffffffffffffffffULL: 0;

	rc = PtlMEAppend(ezc.nih, ezc.pti, &me, PTL_PRIORITY_LIST,
			 NULL, &meh);
	EZP_ASSERT(rc == PTL_OK, "PtlMEAppend() failed");

	return meh;
}

void ezpPut(ezpMD mdh, loff_t offset, size_t length, int dest, uint64_t tag)
{
	int rc;
	ptl_process_t peer;

	peer.phys.nid = ezc.self.phys.nid;
	peer.phys.pid = ezc.pid_num_start + dest;

	EZP_ASSERT(tag != EZP_TAG_ANY, "ezpPut(): Invalid tag specified, EZP_TAG_ANY is not allowed");

	rc = PtlPut(mdh, offset, length, PTL_ACK_REQ, peer,
		    PT_INDEX, tag, 0, NULL, 0);
	EZP_ASSERT(rc == PTL_OK, "PtlPut() failed");
}

void ezpGet(ezpMD mdh, loff_t offset, size_t length, int dest, uint64_t tag)
{
	int rc;
	ptl_process_t peer;

	peer.phys.nid = ezc.self.phys.nid;
	peer.phys.pid = ezc.pid_num_start + dest;

	EZP_ASSERT(tag != EZP_TAG_ANY, "ezpGet(): Invalid tag specified, EZP_TAG_ANY is not allowed");

	rc = PtlGet(mdh, offset, length, peer,
		    PT_INDEX, tag, 0, 0);
	EZP_ASSERT(rc == PTL_OK, "PtlGet() failed");
}

void ezpTrigPut(ezpMD mdh, loff_t offset, size_t length, int dest, uint64_t tag, int threshold)
{
	int rc;
	ptl_process_t peer;

	peer.phys.nid = ezc.self.phys.nid;
	peer.phys.pid = ezc.pid_num_start + dest;

	EZP_ASSERT(tag != EZP_TAG_ANY, "ezpTrigPut(): Invalid tag specified, EZP_TAG_ANY is not allowed");

	rc = PtlTriggeredPut(mdh, offset, length, PTL_ACK_REQ, peer, PT_INDEX, tag, 0, NULL, 0, ezc.cth, threshold);
	EZP_ASSERT(rc == PTL_OK, "PtlTriggeredPut() failed");
}

void ezpTrigGet(ezpMD mdh, loff_t offset, size_t length, int dest, uint64_t tag, int threshold)
{
	int rc;
	ptl_process_t peer;

	peer.phys.nid = ezc.self.phys.nid;
	peer.phys.pid = ezc.pid_num_start + dest;

	EZP_ASSERT(tag != EZP_TAG_ANY, "ezpTrigGet(): Invalid tag specified, EZP_TAG_ANY is not allowed");

	rc = PtlTriggeredGet(mdh, offset, length, peer, PT_INDEX, tag, 0, NULL, ezc.cth, threshold);
	EZP_ASSERT(rc == PTL_OK, "PtlTriggeredGet() failed");
}

