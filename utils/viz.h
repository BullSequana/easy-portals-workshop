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

#include <stddef.h>

/* Each pixel is a string, representing a number between 0 & 255 (inclusive) */
struct pixel {
	char r[4];
	char g[4];
	char b[4];
};

struct image {
	struct pixel *image;
	size_t cols;
	size_t rows;
};

/* Initialize visualiation library */
void enter_raw_mode(void);
/* Cleanup visualiation (must be called) */
void exit_raw_mode(void);

/* Returns the current window size */
int get_window_size(int *w_rows, int *w_cols);

/* Draw a box around the current window */
void draw_box(size_t w_rows, size_t w_cols);
/* Display an image inside a box */
void display_image(struct image *img, size_t w_rows, size_t w_cols);
