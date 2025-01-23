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

#define NUM_TASKS 16

void *root_task(void *p)
{
	return NULL;
}

void *nonroot_task(void *p)
{
	return NULL;
}

int main(void)
{
	pthread_t t[NUM_TASKS];
	unsigned long i;

	init(NUM_TASKS);

	for (i = 0; i < NUM_TASKS; i++) {
		if (i == 0)
			pthread_create(&t[i], NULL, &root_task, (void *)i);
		else
			pthread_create(&t[i], NULL, &nonroot_task, (void *)i);
	}

	for (i = 0; i < NUM_TASKS; i++) {
		pthread_join(t[i], NULL);
	}

	return 0;
}
