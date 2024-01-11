#include "primlib.h"
#include "puzzles.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH gfx_screenWidth()
#define SCREEN_HEIGHT gfx_screenHeight()
#define NUMBER_OF_BLOCKS_VERTICALLY 20
#define NUMBER_OF_BLOCKS_HORIZONTALLY 10
#define BORDER 40
#define BORDERED_WIDTH (SCREEN_WIDTH - (2 * BORDER))
#define BORDERED_HEIGHT (SCREEN_HEIGHT - (2 * BORDER))
#define BORDER_COLOUR GREEN
#define BOARD_WIDTH (BORDERED_WIDTH / 3)
#define BLOCK_TYPES 7
#define BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT 4
#define FRAMES_PER_SECOND 20
#define DELAY 16
#define DELAY_LOOSING 2000
#define NUMBER_OF_SQUARES_IN_PIECE 16

int current_block[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};
int playing = true;

void drawing_grid(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	gfx_line(BORDER, BORDER, SCREEN_WIDTH - BORDER, BORDER, BORDER_COLOUR);
	gfx_line(BORDER, SCREEN_HEIGHT - BORDER, SCREEN_WIDTH - BORDER, SCREEN_HEIGHT - BORDER, BORDER_COLOUR);
	gfx_line(BORDER, BORDER, BORDER, SCREEN_HEIGHT - BORDER, BORDER_COLOUR);
	gfx_line(SCREEN_WIDTH - BORDER, BORDER, SCREEN_WIDTH - BORDER, SCREEN_HEIGHT - BORDER, BORDER_COLOUR);

	int pixel_width_and_height;

	if (BORDERED_HEIGHT / NUMBER_OF_BLOCKS_VERTICALLY >= BORDERED_WIDTH / NUMBER_OF_BLOCKS_HORIZONTALLY)
	{
		pixel_width_and_height = BORDERED_WIDTH / NUMBER_OF_BLOCKS_HORIZONTALLY;
	}
	else if (BORDERED_HEIGHT / NUMBER_OF_BLOCKS_VERTICALLY < BORDERED_WIDTH / NUMBER_OF_BLOCKS_HORIZONTALLY)
	{
		pixel_width_and_height = BORDERED_HEIGHT / NUMBER_OF_BLOCKS_VERTICALLY;
	}

	for (int level_counter = 1; level_counter <= NUMBER_OF_BLOCKS_VERTICALLY; level_counter++)
	{
		for (int column_counter = 1; column_counter <= NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			gfx_rect(BORDER + BOARD_WIDTH,
					 SCREEN_HEIGHT - BORDER - level_counter * pixel_width_and_height,
					 BORDER + BOARD_WIDTH + column_counter * pixel_width_and_height,
					 SCREEN_HEIGHT - BORDER - (level_counter - 1) * pixel_width_and_height, WHITE);
		}
	}

	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1))
			{
				gfx_filledRect(BORDER + BOARD_WIDTH + ((column_counter)*pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - row_counter) * pixel_width_and_height)),
							   BORDER + BOARD_WIDTH + ((column_counter + 1) * pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - (row_counter + 1)) * pixel_width_and_height)), GREEN);
			}
			if ((gamestate[row_counter][column_counter] == 2))
			{
				gfx_filledRect(BORDER + BOARD_WIDTH + ((column_counter)*pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - row_counter) * pixel_width_and_height)),
							   BORDER + BOARD_WIDTH + ((column_counter + 1) * pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - (row_counter + 1)) * pixel_width_and_height)), YELLOW);
			}
			if ((gamestate[row_counter][column_counter] == 3))
			{
				gfx_filledRect(BORDER + BOARD_WIDTH + ((column_counter)*pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - row_counter) * pixel_width_and_height)),
							   BORDER + BOARD_WIDTH + ((column_counter + 1) * pixel_width_and_height),
							   SCREEN_HEIGHT - (BORDER + ((NUMBER_OF_BLOCKS_VERTICALLY - (row_counter + 1)) * pixel_width_and_height)), RED);
			}
		}
	}
}

void get_random_block(int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	int random_block_kind = rand() % BLOCK_TYPES;
	for (int r = 0; r < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; r++)
	{
		for (int i = 0; i < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; j++)
			{
				current_block_rotations[r][i][j] = pieces[random_block_kind][r][i][j];
			}
		}
	}
}

int finding_block_width(int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	int width = 0;

	for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
	{
		for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
		{
			if (current_block_rotations[0][row_counter][column_counter] == 1 || current_block_rotations[0][row_counter][column_counter] == 2)
			{
				width++;
				break;
			}
		}
	}

	return width;
}

int should_spawn_higher_by_how_much(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY], int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	int lift = 0;
	int block_width = finding_block_width(current_block_rotations);
	for (int row_counter = 3; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < block_width; column_counter++)
		{
			if (gamestate[row_counter][(NUMBER_OF_BLOCKS_HORIZONTALLY / 4) + column_counter] == 3)
			{
				lift++;
				break;
			}
		}
	}
	return lift;
}

int finding_lowest_taken_row_of_current_block(int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	for (int row_counter = 3; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
		{
			if (current_block_rotations[0][row_counter][column_counter] == 1 || current_block_rotations[0][row_counter][column_counter] == 2)
			{
				return row_counter;
			}
		}
	}
	return -1;
}

int finding_block_height(int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	int height = 0;
	for (int row_counter = 3; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
		{
			if (current_block_rotations[0][row_counter][column_counter] == 1 || current_block_rotations[0][row_counter][column_counter] == 2)
			{
				height++;
				break;
			}
		}
	}
	return height;
}

bool end_game(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int ones_and_twos_counter = 0;
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY - 1; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter++)
		{
			if (gamestate[row_counter][column_counter] == 1 || gamestate[row_counter][column_counter] == 2)
			{
				ones_and_twos_counter++;
				if (ones_and_twos_counter < 4)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void loosing_screen()
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), RED);
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "You lost :(", BLACK);
	gfx_updateScreen();
}

void put_block_above_grid(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY], int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT])
{
	int lift = should_spawn_higher_by_how_much(gamestate, current_block_rotations);
	int space_for_block = 3 - lift;
	int block_height = finding_block_height(current_block_rotations);
	int value;
	if (lift == 0 || (lift > 0 && block_height < space_for_block))
	{
		for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
		{
			for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
			{
				value = current_block_rotations[0][row_counter][column_counter];
				if (value == 1 || value == 2)
				{
					gamestate[row_counter][(2 * (NUMBER_OF_BLOCKS_HORIZONTALLY / 4)) + column_counter] = value;
				}
			}
		}
	}

	if (lift > 0 && block_height >= space_for_block)
	{
		int starting_placement_row = 3 - lift;
		int lowest_taken_row = finding_lowest_taken_row_of_current_block(current_block_rotations);
		int current_block_counter = 0;
		for (int row_counter_lift = starting_placement_row; row_counter_lift >= 0; row_counter_lift--)
		{
			for (int column_counter_lift = 0; column_counter_lift < 4; column_counter_lift++)
			{
				gamestate[row_counter_lift][(2 * (NUMBER_OF_BLOCKS_HORIZONTALLY / 4)) + column_counter_lift] = current_block_rotations[0][lowest_taken_row - current_block_counter][column_counter_lift];
			}
			current_block_counter++;
			if (lowest_taken_row - current_block_counter < 0)
			{

				break;
			}
		}
		if (end_game(gamestate))
		{
			loosing_screen();
			SDL_Delay(1000);
			loosing_screen();
			playing = false;
			loosing_screen();
		}
	}
}

void print_array(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			printf("%d ", gamestate[row_counter][column_counter]);
		}
		printf("\n");
	}
}

void move_block_down(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int temporary_storage;
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY; row_counter >= 0; row_counter--)
	{
		for (int column_counter = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter >= 0; column_counter--)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				temporary_storage = gamestate[row_counter][column_counter];
				gamestate[row_counter][column_counter] = gamestate[row_counter + 1][column_counter];
				gamestate[row_counter + 1][column_counter] = temporary_storage;
			}
		}
	}
}

bool can_move_down(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int maybe_can_move = true;
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				if (row_counter == NUMBER_OF_BLOCKS_VERTICALLY - 1 || (gamestate[row_counter + 1][column_counter] == 3))
				{
					maybe_can_move = false;
					return false;
				}
				else if ((row_counter != 1) && (gamestate[row_counter + 1][column_counter] != 3))
				{
					maybe_can_move = true;
				}
			}
		}
	}
	if (maybe_can_move == true)
	{
		return true;
	}
	return false;
}

void move_block_down_fast(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int temporary_storage;
	while (can_move_down(gamestate))
	{
		for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY; row_counter >= 0; row_counter--)
		{
			for (int column_counter = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter >= 0; column_counter--)
			{
				if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
				{
					temporary_storage = gamestate[row_counter][column_counter];
					gamestate[row_counter][column_counter] = gamestate[row_counter + 1][column_counter];
					gamestate[row_counter + 1][column_counter] = temporary_storage;
				}
			}
		}
	}
}

bool check_if_can_move_right(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		if ((gamestate[row_counter][NUMBER_OF_BLOCKS_HORIZONTALLY - 1] == 1) || (gamestate[row_counter][NUMBER_OF_BLOCKS_HORIZONTALLY - 1] == 2))
		{
			return false;
		}
	}
	return true;
}

bool check_if_can_move_left(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		if ((gamestate[row_counter][0] == 1) || (gamestate[row_counter][0] == 2))
		{
			return false;
		}
	}
	return true;
}

void stabilize_gamestate(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{

	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				gamestate[row_counter][column_counter] = 3;
			}
		}
	}
}

bool is_loosing(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{

	for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
	{
		if (gamestate[0][column_counter] == 3)
		{
			return true;
		}
	}

	return false;
}

void move_block_right(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int temporary_storage;

	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY; row_counter >= 0; row_counter--)
	{
		for (int column_counter = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter >= 0; column_counter--)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				temporary_storage = gamestate[row_counter][column_counter];
				gamestate[row_counter][column_counter] = gamestate[row_counter][column_counter + 1];
				gamestate[row_counter][column_counter + 1] = temporary_storage;
			}
		}
	}
}

void move_block_left(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int temporary_storage;
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY - 1; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				temporary_storage = gamestate[row_counter][column_counter];
				gamestate[row_counter][column_counter] = gamestate[row_counter][column_counter - 1];
				gamestate[row_counter][column_counter - 1] = temporary_storage;
			}
		}
	}
}

bool is_not_colliding_with_block_on_right(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY - 1; row_counter >= 0; row_counter--)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				if (gamestate[row_counter][column_counter + 1] == 3)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool is_not_colliding_with_block_on_left(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY - 1; row_counter >= 0; row_counter--)
	{
		for (int column_counter = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter >= 0; column_counter--)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				if (gamestate[row_counter][column_counter - 1] == 3)
				{
					return false;
				}
			}
		}
	}
	return true;
}

void clearing_full_rows(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = NUMBER_OF_BLOCKS_VERTICALLY - 1; row_counter >= 0; row_counter--)
	{
		bool is_full = true;
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if (gamestate[row_counter][column_counter] == 0)
			{
				is_full = false;
			}
		}
		if (is_full)
		{
			for (int column_counter_clearing = 0; column_counter_clearing < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter_clearing++)
			{
				gamestate[row_counter][column_counter_clearing] = 0;
			}
			for (int row_counter_moving_down = row_counter; row_counter_moving_down > 0; row_counter_moving_down--)
			{
				for (int column_counter_moving_down = 0; column_counter_moving_down < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter_moving_down++)
				{
					gamestate[row_counter_moving_down][column_counter_moving_down] = gamestate[row_counter_moving_down - 1][column_counter_moving_down];
				}
			}
			row_counter++;
		}
	}
}

int find_top_left_row(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				return row_counter;
			}
		}
	}
	return 0;
}

int find_top_left_column(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
	{
		for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
		{
			if ((gamestate[row_counter][column_counter] == 1) || (gamestate[row_counter][column_counter] == 2))
			{
				return column_counter;
			}
		}
	}
	return 0;
}

int current_rotation_state(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{

	// uzupelnianie array for comparing
	int x_coord_of_top_left = find_top_left_column(gamestate);
	int y_coord_of_top_left = find_top_left_row(gamestate);

	int array_for_comparing[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};
	int ones_and_twos_count = 0;

	for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
	{
		for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
		{
			if ((y_coord_of_top_left + row_counter) < NUMBER_OF_BLOCKS_VERTICALLY && (x_coord_of_top_left + column_counter) < NUMBER_OF_BLOCKS_HORIZONTALLY)
			{
				array_for_comparing[row_counter][column_counter] = gamestate[y_coord_of_top_left + row_counter][x_coord_of_top_left + column_counter];
				if (array_for_comparing[row_counter][column_counter] == 1 || array_for_comparing[row_counter][column_counter] == 2)
				{
					ones_and_twos_count++;
					if (ones_and_twos_count == 4)
					{
						break;
					}
				}
			}
		}
		if (ones_and_twos_count == 4)
		{
			break;
		}
	}
	// porownywanie z pieces(szukanie typu numeru rotacji)

	int type, rotation, rows, columns;
	int block_count = 0;
	for (type = 0; type < BLOCK_TYPES; type++)
	{
		for (rotation = 0; rotation < 4; rotation++)
		{
			for (rows = 0; rows < 4; rows++)
			{
				for (columns = 0; columns < 4; columns++)
				{
					if (array_for_comparing[rows][columns] == pieces[type][rotation][rows][columns])
					{
						block_count++;
						if (block_count == NUMBER_OF_SQUARES_IN_PIECE)
						{
							return rotation;
						}
					}
				}
			}
			block_count = 0;
		}
	}
	return 0;
}

int current_block_type(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{

	// uzupelnianie array for comparing
	int x_coord_of_top_left = find_top_left_column(gamestate);
	int y_coord_of_top_left = find_top_left_row(gamestate);

	int array_for_comparing[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};
	int ones_and_twos_count = 0;

	for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
	{
		for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
		{
			if ((y_coord_of_top_left + row_counter) < 20 && (x_coord_of_top_left + column_counter) < 10)
			{
				array_for_comparing[row_counter][column_counter] = gamestate[y_coord_of_top_left + row_counter][x_coord_of_top_left + column_counter];
				if (array_for_comparing[row_counter][column_counter] == 1 || array_for_comparing[row_counter][column_counter] == 2)
				{
					ones_and_twos_count++;
					if (ones_and_twos_count == 4)
					{
						break;
					}
				}
			}
		}
		if (ones_and_twos_count == 4)
		{
			break;
		}
	}

	int type, rotation, rows, columns;
	int block_count = 0;
	for (type = 0; type < BLOCK_TYPES; type++)
	{
		for (rotation = 0; rotation < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; rotation++)
		{
			for (rows = 0; rows < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; rows++)
			{
				for (columns = 0; columns < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; columns++)
				{
					if (array_for_comparing[rows][columns] == pieces[type][rotation][rows][columns])
					{
						block_count++;
						if (block_count == NUMBER_OF_SQUARES_IN_PIECE)
						{
							return type;
						}
					}
				}
			}
			block_count = 0;
		}
	}
	return 0;
}

bool can_the_block_rotate_stable_blocks(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	// Copy the gamestate
	int block_type = current_block_type(gamestate);
	// printf("%d\n", block_type);
	int rotation_type = current_rotation_state(gamestate);
	// printf("%d\n", rotation_type);

	for (int column_counter_vector_shift = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter_vector_shift >= 0; column_counter_vector_shift--)
	{
		for (int row_counter_vector_shift = 0; row_counter_vector_shift < NUMBER_OF_BLOCKS_VERTICALLY; row_counter_vector_shift++)
		{
			if (gamestate[row_counter_vector_shift][column_counter_vector_shift] == 2)
			{
				if (gamestate[row_counter_vector_shift][column_counter_vector_shift + 1] == 3 || gamestate[row_counter_vector_shift + 1][column_counter_vector_shift + 1] == 3 || gamestate[row_counter_vector_shift - 1][column_counter_vector_shift + 1] == 3 || gamestate[row_counter_vector_shift - 1][column_counter_vector_shift] == 3 || gamestate[row_counter_vector_shift + 1][column_counter_vector_shift] == 3 || gamestate[row_counter_vector_shift - 1][column_counter_vector_shift - 1] == 3 || gamestate[row_counter_vector_shift][column_counter_vector_shift - 1] == 3 || gamestate[row_counter_vector_shift + 1][column_counter_vector_shift - 1] == 3)
				{
					return false;
				}
				if (block_type == 1 && (column_counter_vector_shift != 1) && (column_counter_vector_shift != NUMBER_OF_BLOCKS_HORIZONTALLY - 1))
				{
					if (rotation_type == 0)
					{
						if (gamestate[row_counter_vector_shift + 2][column_counter_vector_shift] == 3)
						{
							return false;
						}
					}
					if (rotation_type == 1)
					{
						if (gamestate[row_counter_vector_shift][column_counter_vector_shift - 2] == 3)
						{
							return false;
						}
					}
					if (rotation_type == 2)
					{
						if (gamestate[row_counter_vector_shift - 2][column_counter_vector_shift] == 3)
						{
							return false;
						}
					}
					if (rotation_type == 3)
					{
						if(column_counter_vector_shift+2<NUMBER_OF_BLOCKS_HORIZONTALLY){
						if (gamestate[row_counter_vector_shift][column_counter_vector_shift + 2] == 3)
						{
							return false;
						}
						}
					}
				}
			}
		}
	}
	return true;
}

void rotate_piece(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int current_rotation = current_rotation_state(gamestate);
	int current_block_shape = current_block_type(gamestate);

	int next_rotation = current_rotation + 1;
	if (next_rotation > 3)
	{
		next_rotation = 0;
	}

	int rotated_block[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};
	int original_block[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};

	// filling original_block
	for (int row_counter_spin = 0; row_counter_spin < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter_spin++)
	{
		for (int column_counter_spin = 0; column_counter_spin < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter_spin++)
		{
			original_block[row_counter_spin][column_counter_spin] = pieces[current_block_shape][current_rotation][row_counter_spin][column_counter_spin];
			// printf("%d ", original_block[row_counter_spin][column_counter_spin]);
		}
		// printf("\n");
	}
	// printf("\n/////\n");
	//   filling rotated block
	for (int row_counter_spin = 0; row_counter_spin < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter_spin++)
	{
		for (int column_counter_spin = 0; column_counter_spin < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter_spin++)
		{
			rotated_block[row_counter_spin][column_counter_spin] = pieces[current_block_shape][next_rotation][row_counter_spin][column_counter_spin];
			// printf("%d ", rotated_block[row_counter_spin][column_counter_spin]);
		}
		// printf("\n");
	}

	int x_position_of_two_or_block = -1;
	int y_position_of_two_or_block = -1;

	int x_position_of_two_rot_block = -1;
	int y_position_of_two_rot_block = -1;

	//  finding the position of 2 in the original block

	for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
	{
		for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
		{
			if (original_block[column_counter][row_counter] == 2)
			{
				x_position_of_two_or_block = column_counter;
				y_position_of_two_or_block = row_counter;
				break; // exit the inner loop
			}
		}
		if (x_position_of_two_or_block != -1 && y_position_of_two_or_block != -1)
		{
			break; // exit the outer loop
		}
	}

	// finding the position of 2 in the rotated block

	for (int column_counter = 0; column_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter++)
	{
		for (int row_counter = 0; row_counter < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter++)
		{
			if (rotated_block[column_counter][row_counter] == 2)
			{
				x_position_of_two_rot_block = column_counter;
				y_position_of_two_rot_block = row_counter;
				break; // exit the inner loop
			}
		}
		if (x_position_of_two_rot_block != -1 && y_position_of_two_rot_block != -1)
		{
			break; // exit the outer loop
		}
	}

	int x_shift = x_position_of_two_or_block - x_position_of_two_rot_block;
	int y_shift = y_position_of_two_or_block - y_position_of_two_rot_block;

	int where_to_put_block_top_left_x = find_top_left_column(gamestate);
	int where_to_put_block_top_left_y = find_top_left_row(gamestate);

	for (int column_counter_vector_shift = NUMBER_OF_BLOCKS_HORIZONTALLY - 1; column_counter_vector_shift >= 0; column_counter_vector_shift--)
	{
		for (int row_counter_vector_shift = 0; row_counter_vector_shift < NUMBER_OF_BLOCKS_VERTICALLY; row_counter_vector_shift++)
		{
			if (gamestate[row_counter_vector_shift][column_counter_vector_shift] == 1)
			{
				gamestate[row_counter_vector_shift][column_counter_vector_shift] = 0;
			}
		}
	}

	// putting the block without vector shift yet

	for (int row_counter_no_vector = 0; row_counter_no_vector < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; row_counter_no_vector++)
	{
		for (int column_counter_no_vector = 0; column_counter_no_vector < BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT; column_counter_no_vector++)
		{
			int value = rotated_block[row_counter_no_vector][column_counter_no_vector];
			int new_y = where_to_put_block_top_left_y + row_counter_no_vector + x_shift;
			int new_x = where_to_put_block_top_left_x + column_counter_no_vector + y_shift;
			if ((value == 1 || value == 2))
			{
				// fprintf(stderr,"%d %d\n",new_y,new_x);
				gamestate[new_y][new_x] = value;

				// printf("%d, %d\n", where_to_put_block_top_left_y + row_counter_no_vector + x_shift,where_to_put_block_top_left_x + column_counter_no_vector + y_shift);
			}
		}
	}
}

bool can_the_block_rotate_sides(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
		{
			if ((gamestate[row_counter][0] == 2) || (gamestate[row_counter][NUMBER_OF_BLOCKS_HORIZONTALLY - 1] == 2) || (gamestate[0][column_counter] == 2) || (gamestate[NUMBER_OF_BLOCKS_VERTICALLY - 1][column_counter] == 2))
			{
				return false;
			}
		}
	}
	return true;
}

bool can_the_block_rotate_near_sides(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	int block_type = current_block_type(gamestate);
	int rotation_type = current_rotation_state(gamestate);

	for (int row_counter = 0; row_counter < NUMBER_OF_BLOCKS_VERTICALLY; row_counter++)
	{
		if (gamestate[row_counter][NUMBER_OF_BLOCKS_HORIZONTALLY - 2] == 2 && block_type == 1 && rotation_type == 3)
		{
			return false;
		}
		if (gamestate[row_counter][1] == 2 && block_type == 1 && rotation_type == 1)
		{
			return false;
		}
	}

	for (int column_counter = 0; column_counter < NUMBER_OF_BLOCKS_HORIZONTALLY; column_counter++)
	{
		if (gamestate[1][column_counter] == 2 && block_type == 1 && rotation_type == 2)
		{
			return false;
		}
		if (gamestate[NUMBER_OF_BLOCKS_VERTICALLY - 2][column_counter] == 2 && block_type == 1 && rotation_type == 0)
		{
			return false;
		}
	}

	return true;
}

void drawing_view(int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY])
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), BLACK);
	drawing_grid(gamestate);
}

int main(int argc, char *argv[])
{
	if (gfx_init())
	{
		exit(3);
	}

	srand(time(NULL));
	int gamestate[NUMBER_OF_BLOCKS_VERTICALLY][NUMBER_OF_BLOCKS_HORIZONTALLY] = {0};
	int current_block_rotations[BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT][BLOCK_SHAPE_ARRAY_WIDTH_AND_HEIGHT] = {0};
	int frame_counter = 0;
	int user_input;
	get_random_block(current_block_rotations);
	put_block_above_grid(gamestate, current_block_rotations);

	while (playing)
	{
		frame_counter++;
		user_input = gfx_pollkey();
		if (user_input == SDLK_RIGHT)
		{
			if (check_if_can_move_right(gamestate) && is_not_colliding_with_block_on_right(gamestate))
			{
				move_block_right(gamestate);
			}
		}
		else if (user_input == SDLK_LEFT)
		{
			if (check_if_can_move_left(gamestate) && is_not_colliding_with_block_on_left(gamestate))
			{
				move_block_left(gamestate);
			}
		}
		else if (user_input == SDLK_DOWN)
		{
			move_block_down_fast(gamestate);
		}
		else if ((user_input == SDLK_SPACE) && (can_the_block_rotate_sides(gamestate)) && (can_the_block_rotate_stable_blocks(gamestate)) && (can_the_block_rotate_near_sides(gamestate)))
		{
			// int rotation = current_rotation_state(gamestate);
			// int type = current_block_type(gamestate);
			// printf("\n");
			// printf("%d\n", type);
			// printf("%d\n", rotation);
			// printf("\n");
			// playing = 0;
			rotate_piece(gamestate);
		}

		// and spinning

		if (frame_counter % FRAMES_PER_SECOND == 0)
		{
			if (can_move_down(gamestate))
			{
				move_block_down(gamestate);
			}
			else
			{
				clearing_full_rows(gamestate);
				stabilize_gamestate(gamestate);
				if (is_loosing(gamestate))
				{
					loosing_screen();
					SDL_Delay(DELAY_LOOSING);
					playing = false;
				}

				get_random_block(current_block_rotations);
				put_block_above_grid(gamestate, current_block_rotations);
			}
		}
		// print_array(gamestate);
		// printf("\n");
		if (playing)
		{
			drawing_view(gamestate);
		}
		gfx_updateScreen();
		SDL_Delay(DELAY);
	}

	return 0;
}
