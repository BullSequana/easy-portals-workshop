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
#include <ezportals.h>
#include "challenges.h"

#define NUM_TASKS 9

void *compute_task(void *p)
{
	unsigned long id = (unsigned long)p;

	return NULL;
}

void *visualization_task(void *p)
{
	unsigned long id = (unsigned long)p;

	return NULL;
}

int main(void)
{
	pthread_t t[NUM_TASKS];
	unsigned long i;

	init_cells();
	init(NUM_TASKS);

	for (i = 0; i < NUM_TASKS; i++) {
		if (i != NUM_TASKS - 1)
			pthread_create(&t[i], NULL, &compute_task, (void *)i);
		else
			pthread_create(&t[i], NULL, &visualization_task, (void *)i);
	}

	for (i = 0; i < NUM_TASKS; i++) {
		pthread_join(t[i], NULL);
	}
}
