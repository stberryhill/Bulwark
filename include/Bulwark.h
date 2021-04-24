#ifndef _BULWARK_H
#define _BULWARK_H

#include "BulwarkAPIMacro.h"
#include <stdint.h>
#include <stdbool.h>

#define BULWARK_COLOR16_BLACK 0
#define BULWARK_COLOR16_RED 1
#define BULWARK_COLOR16_GREEN 2
#define BULWARK_COLOR16_YELLOW 3
#define BULWARK_COLOR16_BLUE 4
#define BULWARK_COLOR16_MAGENTA 5
#define BULWARK_COLOR16_CYAN 6
#define BULWARK_COLOR16_WHITE 7
#define BULWARK_COLOR16_GRAY 8
#define BULWARK_COLOR16_BRIGHT_RED 9
#define BULWARK_COLOR16_BRIGHT_GREEN 10
#define BULWARK_COLOR16_BRIGHT_YELLOW 11
#define BULWARK_COLOR16_BRIGHT_BLUE 12
#define BULWARK_COLOR16_BRIGHT_MAGENTA 13
#define BULWARK_COLOR16_BRIGHT_CYAN 14
#define BULWARK_COLOR16_BRIGHT_WHITE 15

typedef struct BulwarkEvent BulwarkEvent;

typedef enum BulwarkEventType {
  BULWARK_EVENT_TYPE_INPUT,
  BULWARK_EVENT_TYPE_WINDOW_RESIZE
} BulwarkEventType;

enum BulwarkColorMode {
  BULWARK_COLOR_MODE_16,
  BULWARK_COLOR_MODE_256,
  BULWARK_COLOR_MODE_RGB,
  BULWARK_COLOR_MODE_COUNT
};

typedef struct BulwarkColor {
  /* Color to use for 16-color mode */
  uint8_t color16;

  /* Color to use for 256-color mode */
  uint8_t color256;

  /* Color to use for full RGB mode (Not yet implemented) */
  uint8_t r;
  uint8_t g;
  uint8_t b;

  enum BulwarkColorMode mode;
} BulwarkColor;

/* Setup/teardown functions */
BULWARK_API void Bulwark_Initialize();
BULWARK_API void Bulwark_Quit();

/* Event functions */
void BULWARK_API Bulwark_PollEvents();
bool BULWARK_API Bulwark_HasEventsInQueue();
void BULWARK_API Bulwark_ReadNextEventInQueue(BulwarkEvent *output);
void BULWARK_API Bulwark_WaitForNextEvent(BulwarkEvent *output);
BulwarkEvent BULWARK_API *BulwarkEvent_Create();
void BULWARK_API BulwarkEvent_Destroy(BulwarkEvent *event);
BulwarkEventType BULWARK_API BulwarkEvent_GetType(BulwarkEvent *event);
char BULWARK_API BulwarkEvent_GetCharacter(BulwarkEvent *event);

/* Color functions */
BULWARK_API void Bulwark_SetForegroundColor(const BulwarkColor *color);
BULWARK_API void Bulwark_SetBackgroundColor(const BulwarkColor *color);
BULWARK_API void Bulwark_SetClearColor(const BulwarkColor *color);
BULWARK_API void Bulwark_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor);
BULWARK_API void Bulwark_ClearForegroundAndBackgroundColor();

/* Immediate color functions */
BULWARK_API void Bulwark_Immediate_SetForegroundColor(const BulwarkColor *color);
BULWARK_API void Bulwark_Immediate_SetBackgroundColor(const BulwarkColor *color);
BULWARK_API void Bulwark_Immediate_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor);
BULWARK_API void Bulwark_Immediate_ClearForegroundAndBackgroundColor();

/* Drawing functions */
BULWARK_API void Bulwark_DrawCharacter(int x, int y, char character);
BULWARK_API void Bulwark_DrawString(int x, int y, const char *string, uint16_t stringLength);
BULWARK_API void Bulwark_ClearScreen();
BULWARK_API void Bulwark_UpdateScreen();

/* Immediate drawing functions */
BULWARK_API void Bulwark_Immediate_SetDrawPosition(int x, int y);
BULWARK_API void Bulwark_Immediate_DrawCharacter(char character);
BULWARK_API void Bulwark_Immediate_DrawString(const char *string);

/* Cursor style functions */
BULWARK_API void Bulwark_SetCursorVisible(bool cursorVisible);

/* Window functions */
BULWARK_API int Bulwark_GetWindowWidth();
BULWARK_API int Bulwark_GetWindowHeight();

/* Sound functions */
BULWARK_API void Bulwark_PlayBellSound();

#endif
