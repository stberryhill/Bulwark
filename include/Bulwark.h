#ifndef _BULWARK_H
#define _BULWARK_H

#include "Color16Definitions.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct BulwarkEvent BulwarkEvent;

typedef enum BulwarkEventType {
  BULWARK_EVENT_TYPE_INPUT,
  BULWARK_EVENT_TYPE_WINDOW_RESIZE
} BulwarkEventType;

void Bulwark_Initialize();
void Bulwark_Quit();

/* Event functions */
void Bulwark_WaitForNextEvent(BulwarkEvent *output);
BulwarkEvent *BulwarkEvent_Create();
void BulwarkEvent_Destroy(BulwarkEvent *event);
BulwarkEventType BulwarkEvent_GetType(BulwarkEvent *event);
char BulwarkEvent_GetCharacter(BulwarkEvent *event);

/* Color functions */
void Bulwark_SetForegroundColor16(int color16);
void Bulwark_SetBackgroundColor16(int color16);
void Bulwark_SetForegroundAndBackgroundColors16(int foregroundColor16, int backgroundColor16);
void Bulwark_ClearForegroundColor();
void Bulwark_ClearBackgroundColor();
void Bulwark_ClearForegroundAndBackgroundColor();

/* Drawing functions */
void Bulwark_SetDrawPosition(int x, int y);
void Bulwark_DrawCharacter(char character);
void Bulwark_DrawString(const char *string);

/* Cursor style functions */
void Bulwark_SetCursorVisible(bool cursorVisible);

/* Window functions */
int Bulwark_GetWindowWidth();
int Bulwark_GetWindowHeight();

/* Sound functions */
void Bulwark_PlayBellSound();

#endif