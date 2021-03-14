#ifndef _BULWARK_H
#define _BULWARK_H

#include "Color16Definitions.h"

#include <stdint.h>
#include <stdbool.h>

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
void Bulwark_Initialize();
void Bulwark_Quit();

/* Event functions */
void Bulwark_PollEvents();
bool Bulwark_HasEventsInQueue();
void Bulwark_ReadNextEventInQueue(BulwarkEvent *output);
void Bulwark_WaitForNextEvent(BulwarkEvent *output);
BulwarkEvent *BulwarkEvent_Create();
void BulwarkEvent_Destroy(BulwarkEvent *event);
BulwarkEventType BulwarkEvent_GetType(BulwarkEvent *event);
char BulwarkEvent_GetCharacter(BulwarkEvent *event);

/* Color functions */
void Bulwark_SetForegroundColor(const BulwarkColor *color);
void Bulwark_SetBackgroundColor(const BulwarkColor *color);
void Bulwark_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor);
void Bulwark_ClearForegroundAndBackgroundColor();

/* Immediate color functions */
void Bulwark_Immediate_SetForegroundColor(const BulwarkColor *color);
void Bulwark_Immediate_SetBackgroundColor(const BulwarkColor *color);
void Bulwark_Immediate_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor);
void Bulwark_Immediate_ClearForegroundAndBackgroundColor();

/* Drawing functions */
void Bulwark_DrawCharacter(int x, int y, char character);
void Bulwark_DrawString(int x, int y, const char *string, uint16_t stringLength);
void Bulwark_UpdateScreen();

/* Immediate drawing functions */
void Bulwark_Immediate_SetDrawPosition(int x, int y);
void Bulwark_Immediate_DrawCharacter(char character);
void Bulwark_Immediate_DrawString(const char *string);

/* Cursor style functions */
void Bulwark_SetCursorVisible(bool cursorVisible);

/* Window functions */
int Bulwark_GetWindowWidth();
int Bulwark_GetWindowHeight();

/* Sound functions */
void Bulwark_PlayBellSound();

#endif
