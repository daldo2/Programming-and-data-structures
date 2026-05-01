#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_POLES 5
#define NUMBER_OF_DISCS 37

#define DISTANCE (gfx_screenWidth() / (NUMBER_OF_POLES * 4 + 2))
#define DISTANCE_OF_POLES (DISTANCE * 4)
#define RADIUS (DISTANCE / 4)
#define DISK_HEIGHT (START_OF_PYRAMID - TOP_OF_PYRAMID) / (NUMBER_OF_DISCS + 1)
#define DISK_RADIUS ((DISTANCE_OF_POLES / 2.2) / NUMBER_OF_DISCS)
#define WIDTH (DISK_RADIUS / 2)
#define START_OF_PYRAMID (gfx_screenHeight() * 9 / 10)
#define TOP_OF_PYRAMID (gfx_screenHeight() * 6 / 10)
#define SPEED (DISTANCE/20)
#define DELAY 6

void fill_array(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS]);
void draw_poles();
void draw_discs(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS]);
int check_what_pressed(int pressed[2]);
int is_valid_pressed(int pressed[2]);
int swap_pole(int pressed[2], int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS]);
void animate_swap(int from, int to, int disk, int source_index,
				  int target_index,
				  int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS]);
void render_frame(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS], int disk,
				  int x, int y);
void reset_pressed(int pressed[2]);
int win(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS]);
void endScreen(int pressed[2]);

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}
	int pressed[2] = {-1, -1};
	int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS];
	fill_array(myArray);

	do {
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		if (win(myArray) == 0) {
			endScreen(pressed);
			return 0;
		}
		if (is_valid_pressed(pressed) == 0) {
			swap_pole(pressed, myArray);
		}
		draw_poles();
		draw_discs(myArray);
		gfx_updateScreen();
		SDL_Delay(DELAY);
	} while (check_what_pressed(pressed) != 1);
}

void fill_array(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS])
{
	for (int i = 0; i < NUMBER_OF_DISCS; i++) {
		myArray[0][i] = NUMBER_OF_DISCS - i;
	}
	for (int j = 1; j <= NUMBER_OF_POLES - 1; j++)
		for (int i = 0; i < NUMBER_OF_DISCS; i++) {
			myArray[j][i] = 0;
		}
}

void draw_poles()
{
	gfx_filledRect(0, START_OF_PYRAMID, gfx_screenWidth() - 1,
				   gfx_screenHeight() - 1, GREEN);
	for (int i = 1; i <= NUMBER_OF_POLES; i++) {
		gfx_filledRect(i * DISTANCE_OF_POLES - DISTANCE - WIDTH, TOP_OF_PYRAMID,
					   i * DISTANCE_OF_POLES - DISTANCE + WIDTH,
					   START_OF_PYRAMID, YELLOW);
	}
}

void draw_discs(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS])
{

	for (int j = 0; j < NUMBER_OF_POLES; j++) {
		int disk_number = 0;

		for (int i = 0; i < NUMBER_OF_DISCS; i++) {
			int disk = myArray[j][i];

			if (disk != 0) {
				disk_number++;
				gfx_filledRect(
					DISTANCE_OF_POLES - DISTANCE - disk * DISK_RADIUS +
						DISTANCE_OF_POLES * j,
					START_OF_PYRAMID - disk_number * DISK_HEIGHT + DISK_HEIGHT,
					DISTANCE_OF_POLES - DISTANCE + disk * DISK_RADIUS +
						DISTANCE_OF_POLES * j,
					START_OF_PYRAMID - disk_number * DISK_HEIGHT + 1, RED);

				gfx_rect(DISTANCE_OF_POLES - DISTANCE - disk * DISK_RADIUS +
							 DISTANCE_OF_POLES * j ,
						 START_OF_PYRAMID - disk_number * DISK_HEIGHT +
							 DISK_HEIGHT + 1,
						 DISTANCE_OF_POLES - DISTANCE + disk * DISK_RADIUS +
							 DISTANCE_OF_POLES * j + 1,
						 START_OF_PYRAMID - disk_number * DISK_HEIGHT , BLACK);
			}
		}
	}
}

int is_valid_pressed(int pressed[2])
{
	if (pressed[0] > NUMBER_OF_POLES) {
		reset_pressed(pressed);
		return 1;
	}
	else if (pressed[1] > NUMBER_OF_POLES) {
		reset_pressed(pressed);
		return 1;
	}
	else if (pressed[0] == -1 || pressed[1] == -1) {
		return 1;
	}
	else {
		return 0;
	}
}

int swap_pole(int pressed[2], int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS])
{
	int from = pressed[0] - 1;
	int to = pressed[1] - 1;
	int first_index = 0;
	int second_index = 0;

	int smallest = NUMBER_OF_DISCS + 1;
	for (int i = 0; i < NUMBER_OF_DISCS; i++) {
		int current = myArray[from][i];
		if (current <= smallest && current > 0) {
			smallest = current;
			first_index = i;
		}
	}
	if (smallest == NUMBER_OF_DISCS + 1) {
		return 1;
	}

	for (int i = 0; i < NUMBER_OF_DISCS; i++) {
		int current = myArray[to][i];
		if (current == 0) {
			break;
		}
		if (current <= smallest) {
			return 1;
		}
		second_index++;
	}

	int swap_from = myArray[from][first_index];
	myArray[from][first_index] = 0;

	animate_swap(pressed[0], pressed[1], smallest, first_index, second_index,
				 myArray);

	myArray[to][second_index] = swap_from;
	reset_pressed(pressed);
	return 0;
}

void animate_swap(int from, int to, int disk, int source_index,
				  int target_index,
				  int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS])
{
	int start_x = from * DISTANCE_OF_POLES - DISTANCE;
	int target_x = to * DISTANCE_OF_POLES - DISTANCE;
	int current_x = start_x;
	int current_y = START_OF_PYRAMID - (source_index + 1) * DISK_HEIGHT;
	int end_y = START_OF_PYRAMID - (target_index + 1) * DISK_HEIGHT;
	int peak_y = TOP_OF_PYRAMID - 3 * DISK_HEIGHT;

	while (current_y > peak_y) {
		current_y -= SPEED;
		if (current_y < peak_y) {
			current_y = peak_y;
		}
		render_frame(myArray, disk, current_x, current_y);
	}

	int direction_x = (target_x > start_x) ? SPEED : -SPEED;
	while (abs(target_x - current_x) > SPEED) {
		current_x += direction_x;
		render_frame(myArray, disk, current_x, current_y);
	}
	current_x = target_x;
	render_frame(myArray, disk, current_x, current_y);

	while (current_y < end_y) {
		current_y += SPEED;
		if (current_y > end_y)
			current_y = end_y;
		render_frame(myArray, disk, current_x, current_y);
	}
}

void render_frame(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS], int disk,
				  int x, int y)
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	draw_poles();
	draw_discs(myArray);

	gfx_filledRect(x - disk * DISK_RADIUS, y + DISK_HEIGHT,
				   x + disk * DISK_RADIUS, y + 1, RED);
	gfx_rect(x - disk * DISK_RADIUS, y + DISK_HEIGHT + 1, x + disk * DISK_RADIUS + 1, y,
			 BLACK);

	gfx_updateScreen();
	SDL_Delay(DELAY);
}

void reset_pressed(int pressed[2])
{
	pressed[0] = -1;
	pressed[1] = -1;
}

int win(int myArray[NUMBER_OF_POLES][NUMBER_OF_DISCS])
{
	for (int i = 0; i < NUMBER_OF_DISCS; i++) {
		if (myArray[NUMBER_OF_POLES - 1][i] == 0) {
			return 1;
		}
	}
	return 0;
}

void endScreen(int pressed[2])
{
	do {
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		gfx_textout(gfx_screenWidth() * 9 / 20, gfx_screenHeight() / 2,
					"Congratulations", CYAN);
		gfx_updateScreen();
		usleep(8000);
	} while (check_what_pressed(pressed) != 1);
}

int check_what_pressed(int pressed[2])
{
	int index = 0;
	if (pressed[0] != -1 && pressed[1] != -1) {
		reset_pressed(pressed);
	}
	else if (pressed[0] != -1) {
		index = 1;
	}

	switch (gfx_pollkey()) {
	case SDLK_1:
		pressed[index] = 1;
		break;
	case SDLK_2:
		pressed[index] = 2;
		break;
	case SDLK_3:
		pressed[index] = 3;
		break;
	case SDLK_4:
		pressed[index] = 4;
		break;
	case SDLK_5:
		pressed[index] = 5;
		break;
	case SDLK_6:
		pressed[index] = 6;
		break;
	case SDLK_7:
		pressed[index] = 7;
		break;
	case SDLK_8:
		pressed[index] = 8;
		break;
	case SDLK_9:
		pressed[index] = 9;
		break;
	case SDLK_0:
		pressed[index] = 10;
		break;
	case SDLK_ESCAPE:
		return 1;
		break;
	case SDLK_RETURN:
		return 1;
		break;
	default:
		break;
	}
	return 0;
}
