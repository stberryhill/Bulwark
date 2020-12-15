#include <Bulwark.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <tempus.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_ASCII_LOGO_WIDTH 256
#define MAX_ASCII_LOGO_HEIGHT 40

/* Random functions */
int getRandomColor16NotBlack();
int getRandomScreenX();
int getRandomScreenY();
int xorshift32(int seed);

/* Snake around the terminal, changing colors occasionally */
void snakeIt() {
    const int MAX_ITERATIONS = 256;
    const uint64_t DELAY_MILLISECONDS = 1;
    int iterations = 0;

    int x = 0;
    int y = 0;
    int nextX, nextY;
    int stepX, stepY;

    while (iterations < MAX_ITERATIONS) {
        Bulwark_SetBackgroundColor16(getRandomColor16NotBlack());

        nextX = getRandomScreenX();
        nextY = getRandomScreenY();

        stepX = nextX < x ? -1 : 1;
        stepY = nextY < y ? -1 : 1;

        while (x != nextX) {
            x += stepX;
            Bulwark_SetDrawPosition(x, y);
            Bulwark_DrawCharacter(' ');
            t_sleepMilliseconds(DELAY_MILLISECONDS);
        }

        while (y != nextY) {
            y += stepY;
            Bulwark_SetDrawPosition(x, y);
            Bulwark_DrawCharacter(' ');
            t_sleepMilliseconds(DELAY_MILLISECONDS);
        }

        iterations++;
    }
}

int getRandomColor16NotBlack() {
    static int seedX = 3;
    seedX = xorshift32(seedX);
    return (seedX % 15) + 1;
}

int getRandomScreenX() {
    static int seedX = 2;
    seedX = xorshift32(seedX);
    return seedX % Bulwark_GetWindowWidth();
}

int getRandomScreenY() {
    static int seedY = 1;
    seedY = xorshift32(seedY);
    return seedY % Bulwark_GetWindowHeight();
}

int xorshift32(int seed) {
  int next = seed;

  next ^= next << 13;
  next ^= next >> 17;
  next ^= next << 5;

  return next;
}

int main() {
    Bulwark_Initialize();

    BulwarkEvent *event = BulwarkEvent_Create();

    Bulwark_SetCursorVisible(false);
    snakeIt();

    BulwarkEvent_Destroy(event);
    Bulwark_Quit();

    return 0;
}