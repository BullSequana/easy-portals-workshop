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

/****** Challenge 2 ******/

/* The space is divided into a mesh of NUM_CELLS cells
 * Each cell is represented by CELL_SIZE bytes of data */
#define NUM_CELLS 10000
#define CELL_SIZE (12*16)

/* init_cells()
 * Initialize the mesh of cells */
void init_cells(void);

/* get_next_cell()
 * return a cell identifier
 * Each cell identifier is returned once.
 * After NUM_CELLS calls, it returns -1 to
 * indicate all cell identifiers have been returned */
int get_next_cell(void);

/* process_cell()
 * cell_buf: pointer on a buffer of size CELL_SIZE 
 * Perform computation on the cell and provide
 * resulting data into the specified buffer */
void process_cell(int cell, void *cell_buf);

/* start_rendering
 * mesh_buf: pointer on a buffer of size CELL_SIZE * NUM_CELLS
 * Prepare to process the specified buffer as the global computation
 * result */
void start_rendering(void *mesh_buf);

/* render_cells
 * Update the display by processing the buffer specified
 * in start_rendering */
void render_cells(void);

/* end_rendering
 * Free the display after the user hit the 'q' key */
void end_rendering(void);
