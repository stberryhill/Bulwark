#include <Bulwark.h>
#include <unistd.h>
#include <stdio.h>
#include <QuickFile.h>
#include <string.h>
#include <tempus.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_ASCII_LOGO_WIDTH 256
#define MAX_ASCII_LOGO_HEIGHT 40

bool running;
FILE *logFile;

int getRandomColor16NotBlack();
int getRandomScreenX();
int getRandomScreenY();
int xorshift32(int seed);

void traceAsciiLogo(const char *fileName);
void traceCharMap(char charMap[MAX_ASCII_LOGO_HEIGHT][MAX_ASCII_LOGO_WIDTH], const int width, const int height);
void traceContiguousRegion(char charMap[MAX_ASCII_LOGO_HEIGHT][MAX_ASCII_LOGO_WIDTH], const int width, const int height, const int startX, const int startY);

void drawPalette() {
    int i, j;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            Bulwark_SetForegroundAndBackgroundColors16(j, i);
            Bulwark_SetDrawPosition(i, j);
            Bulwark_DrawCharacter('*');
        }
    }
}

void traceAsciiLogo(const char *fileName) {
    QfFile *logoFile = qf_OpenFile(fileName, QF_FILE_MODE_READ);

    char charMap[MAX_ASCII_LOGO_HEIGHT][MAX_ASCII_LOGO_WIDTH];
    int lineNumber = 0;
    int maxWidth = 0;

    while (!qf_ReachedEndOfFile(logoFile)) {
        qf_ReadLine(logoFile, MAX_ASCII_LOGO_WIDTH, charMap[lineNumber]);
        fprintf(logFile, "%s\n", charMap[lineNumber]);
        fflush(logFile);

        const int stringWidth = strlen(charMap[lineNumber]);

        if (stringWidth > maxWidth) {
            maxWidth = stringWidth;
        }

        lineNumber++;
    }

    Bulwark_SetForegroundColor16(BULWARK_COLOR16_BRIGHT_CYAN);

    traceCharMap(charMap, maxWidth, lineNumber + 1);

    qf_CloseFile(logoFile);
}

void traceCharMap(char charMap[MAX_ASCII_LOGO_HEIGHT][MAX_ASCII_LOGO_WIDTH], const int width, const int height) {
    int lastColumnScanned = 0;

    while (lastColumnScanned < width) {
        int row = 0;
        while (row < height) {
            if (charMap[row][lastColumnScanned] != ' ') {
                traceContiguousRegion(charMap, width, height, lastColumnScanned, row);
            }
            row++;
        }

        lastColumnScanned++;
    }
}

void traceContiguousRegion(char charMap[MAX_ASCII_LOGO_HEIGHT][MAX_ASCII_LOGO_WIDTH], const int width, const int height, const int startX, const int startY) {
    int x = startX;
    int y = startY;

    bool reachedEndOfRegion = false;

    do {
        t_sleepMilliseconds(2);
        Bulwark_SetDrawPosition(x, y);
        Bulwark_DrawCharacter(charMap[y][x]);
        charMap[y][x] = ' ';
        
        if (y + 1 < height && charMap[y + 1][x] != ' ') {
            y++;
        } else if (x + 1 < width && charMap[y][x + 1] != ' ') {
            x++;
        } else if (y - 1 > 0 && charMap[y - 1][x] != ' ') {
            y--;
        } else if (x - 1 > 0 && charMap[y][x - 1] != ' ') {
            x--;
        } else {
            reachedEndOfRegion = true;
        }
    } while (!reachedEndOfRegion);
}

void snakeIt() {
    const int MAX_ITERATIONS = 256;
    const long DELAY_MILLISECONDS = 1;
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

        fprintf(logFile, "x=%d, y=%d, nextX=%d, nextY=%d, stepX=%d, stepY=%d\n", x, y, nextX, nextY, stepX, stepY);
        fflush(logFile);

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

    logFile = fopen("./mainLog.txt", "w");

    BulwarkEvent *event = BulwarkEvent_Create();

    Bulwark_SetCursorVisible(false);
    /*traceAsciiLogo("resources/logo-big.txt");*/
    snakeIt();

    Bulwark_SetForegroundAndBackgroundColors16(BULWARK_COLOR16_BLACK, BULWARK_COLOR16_BLUE);

    bool running = true;
    while (running) {
        Bulwark_WaitForNextEvent(event);
        Bulwark_SetDrawPosition(25, 1);
        char character = BulwarkEvent_GetCharacter(event);

        Bulwark_DrawCharacter(character);

        if (character == EOF) {
            running = false;
        }
    }

    fclose(logFile);
    BulwarkEvent_Destroy(event);
    Bulwark_Quit();

    return 0;
}