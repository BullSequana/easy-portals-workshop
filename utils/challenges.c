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

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdatomic.h>
#include "challenges.h"
#include "viz.h"

#define INPUT_FILE "challenge2/climate_earth.ppm"

static int cell_seq[NUM_CELLS];
static sig_atomic_t stop = 0;
static int window_rows, window_cols;
static struct image initial_image;
static struct image current_image;

struct image read_ppm_image(const char *path)
{
	struct image parsed;
	unsigned long read;
	char buffer[16];
	FILE *img;
	size_t size;
	uint8_t *pixels;

	img = fopen(path, "rb");
	if (img == NULL) {
		fprintf(stderr, "opening %s: %s\n", path, strerror(errno));
		exit(1);
	}

	if ((read = fread(buffer, 1, 3, img)) != 3) {
		fprintf(stderr, "Failed to read header (read %lu bytes)\n", read);
		goto err;
	}

	if (memcmp(buffer, "P6\n", 3) != 0) {
		fprintf(stderr, "File is not a P3 image\n");
		goto err;
	}

	if (fscanf(img, "%lu %lu\n", &parsed.cols, &parsed.rows) != 2) {
		fprintf(stderr, "Failed to read image size\n");
		goto err;
	}

	fgets(buffer, sizeof(buffer), img);
	if (strcmp(buffer, "255\n") != 0) {
		fprintf(stderr, "Only supports images with 255 colors, got %s\n", buffer);
		goto err;
	}

	size = parsed.cols * parsed.rows;
	pixels = calloc(size, 3);
	if ((read = fread(pixels, 3, size, img)) != size) {
		fprintf(stderr, "Did not fetch enough pixels, got %lu expected %lu\n", read, size);
		goto err;
	}

	parsed.image = calloc(size, sizeof(struct pixel));
	for (size_t i = 0; i < size; i++) {
		sprintf(parsed.image[i].r, "%u", pixels[3 * i + 0]);
		sprintf(parsed.image[i].g, "%u", pixels[3 * i + 1]);
		sprintf(parsed.image[i].b, "%u", pixels[3 * i + 2]);
	}

	free(pixels);

	return parsed;

err:
	fclose(img);
	exit(1);
}

static void render_window()
{
	draw_box(window_rows, window_cols);
	display_image(&current_image, window_rows, window_cols);
}

static void sigint(int signal)
{
	(void)signal;
	stop = 1;
}

static void sigwinch(int signal)
{
	(void)signal;
	if (get_window_size(&window_rows, &window_cols) < 0) {
		stop = 1;
	}
	render_window();
}

void init_cells(void)
{
	int i;
	int r, s;
	int t;

	initial_image = read_ppm_image(INPUT_FILE);

	/* fill the cell id sequence and then shuffle */
	for (i = 0; i < NUM_CELLS; i++) {
		cell_seq[i] = i;
	}
	s = 0;
	for (i = 0; i < 10*NUM_CELLS; i++) {
		r = rand() % NUM_CELLS;
		t = cell_seq[s];
		cell_seq[s] = cell_seq[r];
		cell_seq[r] = t;
		s = r;
	}
}

int get_next_cell(void)
{
	static atomic_uint idx = 0;
	unsigned int c;

	c = atomic_fetch_add(&idx, 1);

	if (c >= NUM_CELLS)
		return -1;

	return cell_seq[c];
}

void process_cell(int cell, void *buf)
{
	/* simulate 10ms comutation time */
	usleep(10000);

	/* fill image portion into caller's buffer */
	memcpy(buf, ((char *)initial_image.image) + cell * CELL_SIZE, CELL_SIZE);
}

void start_rendering(void *buf)
{
	struct sigaction sa;

	sa.sa_handler = sigint;
	sigaction(SIGINT, &sa, NULL);

	sa.sa_handler = sigwinch;
	sigaction(SIGWINCH, &sa, NULL);

	if (get_window_size(&window_rows, &window_cols) < 0)
		assert(0);

	current_image.cols = initial_image.cols;
	current_image.rows = initial_image.rows;
	current_image.image = buf;

	for (int i = 0; i < current_image.cols * current_image.rows; i++) {
		sprintf(current_image.image[i].r, "%u", 0);
		sprintf(current_image.image[i].g, "%u", 0);
		sprintf(current_image.image[i].b, "%u", 0);
	}

	enter_raw_mode();
}

void render_cells(void)
{
	render_window();
}

void end_rendering(void)
{
	while (!stop) {
		char c = 0;
		read(STDIN_FILENO, &c, 1);
		switch (c) {
		case 0:
			continue;
		case 'q':
			return;
		}
	}
	exit_raw_mode();
}

