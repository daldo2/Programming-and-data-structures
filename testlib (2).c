#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define PI 3.14
#define RADIUS_MAX 300

#define NUMBER_OF_VERTICIES 5
#define ROTATION_ANGLE 0.05

void update(float tab_x[], float tab_y[], int* radius, float* alpha,
			int* counter);
void draw(float tab_x[], float tab_y[]);

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}

	float tab_x[NUMBER_OF_VERTICIES];
	float tab_y[NUMBER_OF_VERTICIES];
	float alpha = 0;
	int counter = 0;
	int radius = 0;

	while (true) {
		update(tab_x, tab_y, &radius, &alpha, &counter);
		draw(tab_x, tab_y);

		SDL_Delay(20);

		if (gfx_pollkey() != -1) {
			break;
		}
	}

	gfx_getkey();
	return 0;
}

void update(float tab_x[], float tab_y[], int* radius, float* alpha,
			int* counter)
{
	(*alpha) += ROTATION_ANGLE;

	(*counter)++;
	if (*counter < RADIUS_MAX) {
		(*radius) += 1;
	}
	else if (*counter > RADIUS_MAX * 2) {
		*counter = 0;
	}
	else {
		(*radius) -= 1;
	}

	while (*alpha > 2 * PI) {
		*alpha -= 2 * PI;
	}

	for (int i = 0; i < NUMBER_OF_VERTICIES; i++) {
		float current_angle = (*alpha + PI * 2 / NUMBER_OF_VERTICIES * i);
		tab_x[i] = gfx_screenWidth() / 2 + abs(*radius) * cos(current_angle);
		tab_y[i] = gfx_screenHeight() / 2 + abs(*radius) * sin(current_angle);
	}
}

void draw(float tab_x[], float tab_y[])
{
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1, BLACK);
	for (int i = 0; i < NUMBER_OF_VERTICIES; i++) {
		gfx_line(tab_x[i], tab_y[i], tab_x[(i + 1) % NUMBER_OF_VERTICIES],
				 tab_y[(i + 1) % NUMBER_OF_VERTICIES], CYAN);
	}
	gfx_updateScreen();
}
