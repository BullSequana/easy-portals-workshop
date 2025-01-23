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
#include <ezportals.h>


void *initiator(void *p)
{
	return NULL;
}

void *target(void *p)
{
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t t1, t2;
	char *message;

	/* Get command line parameter */
	if (argc != 2) {
		printf("usage: %s message\n", argv[0]);
		return 1;
	}
	message = argv[1];

	init(2);

	pthread_create(&t1, NULL, &initiator, message);
	pthread_create(&t2, NULL, &target, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
