/*
 * author: S. Tyler Berryhill
 * email: sean.tyler.berryhill@gmail.com
 */

#ifndef _BULWARK_INTERNAL_H
#define _BULWARK_INTERNAL_H

#include "Bulwark.h"
#include <stdint.h>

#define WITH ";"
#define FG_256 "38;5;"
#define BG_256 "48;5;"
#define ansi(sequence) "\x1b[" sequence "m"

#define CLEAR_COLOR_CODE 1 << 31

#define MAX_ANSI_ESCAPE_SEQUENCE_STRING_LENGTH 12
#define MAX_POSITION_ESCAPE_SEQUENCE_STRING_LENGTH 15
#define MAX_EVENTS 64

typedef struct AnsiEscapeSequence {
  char string[MAX_ANSI_ESCAPE_SEQUENCE_STRING_LENGTH];
} AnsiEscapeSequence;

struct BulwarkEvent {
  BulwarkEventType type;
  /* For input event */
  int key;
  int modifier;
  char character;

  /* For window resize event */
  int newWindowWidth;
  int newWindowHeight;
};

typedef struct AnsiColorInfo16 {
  int brightnessSpecifier;
  int colorSpecifier;
} AnsiColorInfo16;

typedef struct EventQueue {
  struct BulwarkEvent events[MAX_EVENTS];
  uint8_t eventCount;
  uint8_t writePosition;
  uint8_t readPosition;
} EventQueue;

typedef struct Buffer {
  uint16_t width;
  uint16_t height;
  uint32_t **foregroundColorCodes;
  uint32_t **backgroundColorCodes;
  char **characters;
} Buffer;

typedef struct BufferChange {
  uint32_t newBackgroundColor;
  uint32_t newForegroundColor;
  char newCharacter;
  uint16_t positionX;
  uint16_t positionY;
} BufferChange;

typedef struct BufferChangeList {
  struct BufferChangeListNode *head;
  struct BufferChangeListNode *lastNode;
  int size;
} BufferChangeList;

typedef struct BufferChangeListNode {
  struct BufferChangeListNode *next;
  struct BufferChangeListNode *prev;
  struct BufferChange *data;
} BufferChangeListNode;

void EventQueue_Initialize();
void EventQueue_Destroy();
void EventQueue_AddEvent(const BulwarkEvent *event);
void EventQueue_ReadAndConsumeEvent(BulwarkEvent *output);
bool EventQueue_IsEmpty();

void Window_StartSizeListener();

void Log_Open();
void Log_Info(const char *message, ...);
void Log_Error(const char *message, ...);
void Log_Close();

void Buffer_Initialize(const uint16_t width, const uint16_t height);
void Buffer_Destroy();
void Buffer_Resize(const uint16_t width, const uint16_t height);
uint32_t Buffer_GetForegroundColorCodeAtPosition(const uint16_t x, const uint16_t y);
uint32_t Buffer_GetBackgroundColorCodeAtPosition(const uint16_t x, const uint16_t y);
char Buffer_GetCharacterAtPosition(const uint16_t x, const uint16_t y);
void Buffer_SetCharacterAndColorCodesAtPosition(const uint16_t x, const uint16_t y, const char character, const uint16_t foregroundColorCode, const uint16_t backgroundColorCode);
void Buffer_MarkWholeBufferDirty();
void Buffer_MarkUpToDateAtPosition(const uint16_t x, const uint16_t y);
void Buffer_MarkOutdatedAtPosition(const uint16_t x, const uint16_t y);
bool Buffer_IsUpToDateAtPosition(const uint16_t x, const uint16_t y);

uint32_t Color_GetForegroundColorCode();
uint32_t Color_GetBackgroundColorCode();
uint32_t Color_GetClearColorCode();
uint32_t Color_GenerateColorCodeForColor(const BulwarkColor *color);
void Color_GnerateForegroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);
void Color_GenerateBackgroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);
void Color_ExtractColorFromCode(uint32_t colorCode, BulwarkColor *result);
bool Color_IsForegroundColorCleared();
bool Color_IsBackgroundColorCleared();

void BufferChangeList_Initialize();
void BufferChangeList_Destroy();
BufferChangeListNode *BufferChangeList_GetHead();
BufferChangeListNode *BufferChangeList_GetLastNode();
int BufferChangeList_GetSize();
void BufferChangeList_Clear();
void BufferChangeList_AddChange(const BufferChange change);

#endif