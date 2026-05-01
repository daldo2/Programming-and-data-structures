#include "primlib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
	int kind;
	int rotation;
	int x;
	int y;
} Piece;

typedef struct {
	int row;
	int col;
} Pivot;

#define WIDTH 18
#define HEIGHT 33
#define START_OF_BOARD_X (gfx_screenWidth() * 2 / 5) + 1
#define END_OF_BOARD_X (gfx_screenWidth() * 3 / 5)
#define START_OF_BOARD_Y 200
#define END_OF_BOARD_Y (START_OF_BOARD_Y + SIZE * HEIGHT)

#define SIZE (gfx_screenWidth() / 5 / WIDTH)
#define START_POSSITION (START_OF_BOARD_X + SIZE * 5)
#define FALL_SPEED 30
#define DELAY 10

int getKey();
void movement(int key, Piece* p, Piece* nextP, int gameField[WIDTH][HEIGHT]);
void drawNextPiece(Piece p);
void drawBoard(int gameField[WIDTH][HEIGHT]);
void drawPiece(Piece p);
void endGame();
int isValidPosition(Piece p, int gameField[WIDTH][HEIGHT]);
void lockPiece(Piece* p, Piece* nextP, int gameField[WIDTH][HEIGHT]);
void clearLines(int gameField[WIDTH][HEIGHT]);
Pivot getPivotPosition(Piece p);

char pieces[7][4][4][4] = {
	{{{2, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{2, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{2, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{2, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	{{{1, 2, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 0, 0, 0}, {2, 0, 0, 0}, {1, 0, 0, 0}, {1, 0, 0, 0}},
	 {{1, 1, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 0, 0, 0}, {1, 0, 0, 0}, {2, 0, 0, 0}, {1, 0, 0, 0}}},
	{{{1, 0, 0, 0}, {2, 0, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
	 {{1, 2, 1, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 1, 0, 0}, {0, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
	 {{0, 0, 1, 0}, {1, 2, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	{{{0, 1, 0, 0}, {0, 2, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}},
	 {{1, 0, 0, 0}, {1, 2, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 1, 0, 0}, {2, 0, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 2, 1, 0}, {0, 0, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	{{{0, 1, 0, 0}, {2, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 2, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{0, 1, 0, 0}, {1, 2, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 1, 0, 0}, {0, 2, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	{{{1, 0, 0, 0}, {2, 1, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
	 {{0, 2, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 0, 0, 0}, {1, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
	 {{0, 1, 1, 0}, {1, 2, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}},
	{{{1, 0, 0, 0}, {2, 1, 0, 0}, {1, 0, 0, 0}, {0, 0, 0, 0}},
	 {{1, 2, 1, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
	 {{0, 1, 0, 0}, {1, 2, 0, 0}, {0, 1, 0, 0}, {0, 0, 0, 0}},
	 {{0, 1, 0, 0}, {1, 2, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}}};

int main(int argc, char* argv[])
{
	if (gfx_init()) {
		exit(3);
	}

	srand(time(NULL));
	int gameField[WIDTH][HEIGHT] = {0};
	int keyPressed;
	int fallCounter = 0;
	Piece currentPiece = {rand() % 7, rand() % 4, 4, 0};
	Piece nextPiece = {rand() % 7, rand() % 4, 0, 0};
	do {
		keyPressed = getKey();
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		fallCounter++;
		if (fallCounter >= FALL_SPEED) {
			fallCounter = 0;
			Piece testPiece = currentPiece;
			testPiece.y += 1;
			if (isValidPosition(testPiece, gameField)) {
				currentPiece = testPiece;
			}
			else {
				lockPiece(&currentPiece, &nextPiece, gameField);
				if (!isValidPosition(currentPiece, gameField)) {
					break;
				}
			}
		}

		drawBoard(gameField);
		movement(keyPressed, &currentPiece, &nextPiece, gameField);
		if (!isValidPosition(currentPiece, gameField)) {
			break;
		}
		drawPiece(currentPiece);
		drawNextPiece(nextPiece);
		gfx_updateScreen();
		SDL_Delay(DELAY);
	} while (keyPressed != 5);
	endGame();
	return 0;
}

int getKey()
{
	int keyList[6] = {SDLK_LEFT,  SDLK_RIGHT,  SDLK_DOWN,
					  SDLK_SPACE, SDLK_ESCAPE, SDLK_RETURN};
	int currentKey = gfx_pollkey();
	for (int i = 0; i < 6; i++) {
		if (keyList[i] == currentKey) {
			return i + 1;
		}
	}
	return 0;
}

void movement(int key, Piece* p, Piece* nextP, int gameField[WIDTH][HEIGHT])
{
	Piece testPiece = *p;
	switch (key) {
	case 1:
		testPiece.x -= 1;
		break;
	case 2:
		testPiece.x += 1;
		break;
	case 3:
		while (isValidPosition(testPiece, gameField)) {
			testPiece.y += 1;
		}
		testPiece.y -= 1;
		*p = testPiece;
		lockPiece(p, nextP, gameField);
		return;
	case 4:
		testPiece.rotation = (testPiece.rotation + 1) % 4;
		break;
	}

	if (isValidPosition(testPiece, gameField)) {
		*p = testPiece;
	}
}

Pivot getPivotPosition(Piece p)
{
	Pivot pivot = {0, 0};
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			if (pieces[p.kind][p.rotation][r][c] == 2) {
				pivot.row = r;
				pivot.col = c;
				return pivot;
			}
		}
	}
	return pivot;
}

int isValidPosition(Piece p, int gameField[WIDTH][HEIGHT])
{
	Pivot pivot = getPivotPosition(p);

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (pieces[p.kind][p.rotation][row][col] != 0) {
				int boardX = p.x + col - pivot.col;
				int boardY = p.y + row - pivot.row;

				if (boardX < 0 || boardX >= WIDTH || boardY >= HEIGHT) {
					return 0;
				}
				if (boardY >= 0 && gameField[boardX][boardY] != 0) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void lockPiece(Piece* p, Piece* nextP, int gameField[WIDTH][HEIGHT])
{
	Pivot pivot = getPivotPosition(*p);
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (pieces[p->kind][p->rotation][row][col] != 0) {
				int boardX = p->x + col - pivot.col;
				int boardY = p->y + row - pivot.row;
				if (boardY >= 0 && boardX >= 0 && boardX < WIDTH) {
					gameField[boardX][boardY] = 1;
				}
			}
		}
	}

	clearLines(gameField);

	*p = *nextP;
	p->x = 4;
	p->y = 0;

	nextP->kind = rand() % 7;
	nextP->rotation = rand() % 4;
}

void clearLines(int gameField[WIDTH][HEIGHT])
{
	for (int y = HEIGHT - 1; y >= 0; y--) {
		int isFull = 1;
		for (int x = 0; x < WIDTH; x++) {
			if (gameField[x][y] == 0) {
				isFull = 0;
				break;
			}
		}
		if (isFull) {
			for (int shiftY = y; shiftY > 0; shiftY--) {
				for (int x = 0; x < WIDTH; x++) {
					gameField[x][shiftY] = gameField[x][shiftY - 1];
				}
			}
			for (int x = 0; x < WIDTH; x++) {
				gameField[x][0] = 0;
			}
			y++;
		}
	}
}

void drawNextPiece(Piece p)
{
	Pivot pivot = getPivotPosition(p);
	int startX = START_OF_BOARD_X + SIZE * WIDTH + 1 + SIZE * 2;
	int startY = START_OF_BOARD_Y;

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (pieces[p.kind][p.rotation][row][col] != 0) {
				int px = startX + (col - pivot.col) * SIZE;
				int py = startY + (row - pivot.row) * SIZE;
				enum color c = (pieces[p.kind][p.rotation][row][col] == 2)
								   ? YELLOW
								   : GREEN;
				gfx_filledRect(px, py, px + SIZE, py + SIZE, c);
			}
		}
	}
}

void drawBoard(int gameField[WIDTH][HEIGHT])
{
	gfx_line(START_OF_BOARD_X - 1, START_OF_BOARD_Y, START_OF_BOARD_X - 1,
			 END_OF_BOARD_Y, CYAN);
	gfx_line(START_OF_BOARD_X + SIZE * WIDTH + 1 , START_OF_BOARD_Y,START_OF_BOARD_X + SIZE * WIDTH + 1,
			 END_OF_BOARD_Y, CYAN);

	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			if (gameField[x][y] != 0) {
				int px = START_OF_BOARD_X + x * SIZE;
				int py = START_OF_BOARD_Y + y * SIZE;
				gfx_filledRect(px, py, px + SIZE, py + SIZE, RED);
			}
		}
	}
}

void drawPiece(Piece p)
{
	Pivot pivot = getPivotPosition(p);

	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			if (pieces[p.kind][p.rotation][row][col] != 0) {
				int px = START_OF_BOARD_X + (p.x + col - pivot.col) * SIZE;
				int py = START_OF_BOARD_Y + (p.y + row - pivot.row) * SIZE;
				enum color c = (pieces[p.kind][p.rotation][row][col] == 2)
								   ? YELLOW
								   : GREEN;
				gfx_filledRect(px, py, px + SIZE, py + SIZE, c);
			}
		}
	}
}

void endGame()
{
	int keyPressed;
	do {
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
					   BLACK);
		gfx_textout(gfx_screenWidth() / 2 - 40, gfx_screenHeight() / 2,
					"You lost", YELLOW);
		keyPressed = getKey();
		gfx_updateScreen();
		SDL_Delay(DELAY);
	} while (keyPressed != 5 && keyPressed != 6);
}
