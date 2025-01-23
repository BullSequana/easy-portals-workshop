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
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>

#include "viz.h"

struct termios initial_mode;

void exit_raw_mode(void)
{
	puts("\e[?25h");
	puts("\e[?1049l");
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &initial_mode);
}

void enter_raw_mode(void)
{
	struct termios raw;

	tcgetattr(STDIN_FILENO, &initial_mode);
	raw = initial_mode;

	raw.c_iflag &= ~(IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	puts("\e[?1049h");
	puts("\e[?25l");
}

int get_window_size(int *rows, int *cols)
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
		return -1;

	*rows = ws.ws_row;
	*cols = ws.ws_col;

	return 0;
}

#define print(s) write(STDOUT_FILENO, s, sizeof(s) - 1)
#define print_nul_term(s)                                                                          \
	do {                                                                                       \
		size_t _len = strlen(s);                                                           \
		write(STDOUT_FILENO, s, _len);                                                     \
	} while (0)

void draw_box(size_t w_rows, size_t w_cols)
{
	print("\e[H\e[2J");
	print("┏");
	for (int i = 1; i < w_cols - 1; i++)
		print("━");
	print("┓\e[E");

	for (int i = 1; i < w_rows - 1; i++) {
		print("┃\e[1000C┃\e[E");
	}

	print("┗");
	for (int i = 1; i < w_cols - 1; i++)
		print("━");
	print("┛");
}

void display_image(struct image *img, size_t w_rows, size_t w_cols)
{
	size_t cols = img->cols;
	size_t rows = img->rows;
	size_t col_offset = 0;
	size_t row_offset = 0;

	if (img->cols > w_cols - 2)
		cols = w_cols - 2;

	if (img->rows > (w_rows - 2) * 2)
		rows = (w_rows - 2) * 2;

	if (cols < w_cols - 2) {
		col_offset = ((w_cols - 2) - cols) / 2;
	}

	if (rows < (w_rows - 2) * 2) {
		row_offset = ((w_rows - 2) * 2 - rows) / 2 / 2;
	}

	print("\e[2;2H");

	for (size_t i = 0; i < row_offset; i++) {
		print("\e[E\e[C");
	}

	for (size_t row = 0; row < rows; row += 2) {
		for (size_t i = 0; i < col_offset; i++)
			print("\e[C");

		for (size_t col = 0; col < cols; col++) {
			struct pixel *px_hi = &img->image[row * img->rows + col];

			print("\e[48;2;");
			print_nul_term(px_hi->r);
			print(";");
			print_nul_term(px_hi->g);
			print(";");
			print_nul_term(px_hi->b);
			print("m");

			if (row + 1 < rows) {
				struct pixel *px_lo = &img->image[(row + 1) * img->rows + col];
				print("\e[38;2;");
				print_nul_term(px_lo->r);
				print(";");
				print_nul_term(px_lo->g);
				print(";");
				print_nul_term(px_lo->b);
				print("m");
			}

			print("▄");
		}
		print("\e[E\e[C");
	}
	print("\e[0m");
}
