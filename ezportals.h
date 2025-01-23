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

#pragma once

#include <sys/types.h>
#include <portals4.h>

/* Memory Descriptor (MD)
 * Memory region ready to be used by the initiator
 * for data transfers */
typedef ptl_handle_md_t ezpMD;

/* Match Entry (ME)
 * Memory region exposed at the target
 * for data transfers.
 * The ME tag is used as a matching criteria
 * at message reception */
typedef ptl_handle_me_t ezpME;


#define EZP_TAG_ANY UINT64_MAX

/* General Functions */
void init(int num_threads);
void barrier(void);

/* ezpInit
 * id : communication endpoint identifier
 *
 * Initializes the current thread for Portals communications.
 * The thread is identified on the network by the specified id.
 * Valid id values are in the range [0, 16[.
 */
void ezpInit(int id);

/* ezpEQAlloc
 *
 * Allocate an Event Queue (EQ) for communication events.
 * Events are generated by the NIC to notify the completion of
 * operations.
 * Initiator events
 *   ACK   : completion of PUT operation
 *   REPLY : completion of GET operation
 * Target events
 *   LINK  : completion of MEAppend operation
 *   PUT   : completion of PUT operation
 *   GET   : completion of GET operation
 */
void ezpEQAlloc(void);

/* ezpEQWait
 *
 * Block the calling thread until there is an event in the EQ.
 * It returns the next event as a string describing event type
 * and status, and removes the event from the EQ.
 * Note that environment variable EZP_PRINT_EVENTS can be set
 * to print every events.
 */
char *ezpEQWait(void);

/* ezpMDBind
 * addr    : memory region start address
 * length  : memory region length
 *
 * Prepare a memory region to be used by the initiator
 * for data transfers.
 * Return a Memory Descriptor (MD)
 */
ezpMD ezpMDBind(void *addr, size_t length);

/* ezpMEAppend
 * addr    : memory region start address
 * length  : memory region length
 * tag     : integer value used as a matching criteria
 *
 * Prepare a memory region to be used at the target of
 * a single network operation.
 * The Match Entry (ME) is append at the tail of the Portals
 * message reception chain.
 * When a ME is selected as the target of a network operation
 * it is removed from the Portals message reception chain.
 * Return a Match Entry (ME)
 */
ezpME ezpMEAppend(void *addr, size_t length, uint64_t tag);

/* ezpPut
 * md : Memory Descriptor describing the memory to be sent
 * offset : offset from the start of the MD
 * length : length of the memory region to be sent
 * dest_id : destination endpoint id
 * tag : tag to use for ME selection at the target
 *
 * Initiates an asynchronous data transfer from the current endpoint
 * (initiator) to the destination endpoint (target) identified by dest_id.
 * The specified tag is used at the target for ME selection.
 * Data is read from the memory region described by the specified MD,
 * offset and length. It is writen into the memory region described
 * by the selected ME.
 */
void ezpPut(ezpMD md, loff_t offset, size_t length, int dest_id, uint64_t tag);

/* ezpGet
 * md : Memory Descriptor describing the memory into which the requested data is received
 * offset : offset from the start of the MD
 * length : length of the memory region to be sent
 * dest_id : destination endpoint id
 * tag : tag to use for ME selection at the target
 *
 * Initiates an asynchronous data transfer from the destination endpoint
 * (target) identified by dest_id to the current endpoint (initiator).
 * The specified tag is used at the target for ME selection.
 * Data is read from the memory region described by the selected ME.
 * It is writen into the memory region described by the specified MD,
 * offset and length.
 */
void ezpGet(ezpMD md, loff_t offset, size_t length, int dest_id, uint64_t tag);


/* ezpTrigPut
 * md : Memory Descriptor describing the memory to be sent
 * offset : offset from the start of the MD
 * length : length of the memory region to be sent
 * dest_id : destination endpoint id
 * tag : tag to use for ME selection at the target
 * threshold: value used in triggering condition
 *
 * Register a Put operation that will be triggered
 * when the number of target data transfer events (PUT/GET)
 * is greater or equal the specified threshold
 */
void ezpTrigPut(ezpMD md, loff_t offset, size_t length, int dest_id, uint64_t tag, int threshold);

/* ezpTrigGet
 * md : Memory Descriptor describing the memory into which the requested data is received
 * offset : offset from the start of the MD
 * length : length of the memory region to be sent
 * dest_id : destination endpoint id
 * tag : tag to use for ME selection at the target
 * threshold: value used in triggering condition
 *
 * Register a Get operation that will be triggered
 * when the number of target data transfer events (PUT/GET)
 * is greater or equal the specified threshold
 */
void ezpTrigGet(ezpMD md, loff_t offset, size_t length, int dest_id, uint64_t tag, int threshold);

