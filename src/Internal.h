#ifndef _BULWARK_INTERNAL_H
#define _BULWARK_INTERNAL_H

#include "Bulwark.h"

#define MAX_ANSI_ESCAPE_SEQUENCE_STRING_LENGTH 12
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
  BulwarkEvent events[MAX_EVENTS];
  uint8_t eventCount;
  uint8_t writePosition;
  uint8_t readPosition;
} EventQueue;

typedef struct Buffer {
  uint16_t width;
  uint16_t height;
  uint8_t **colorCodes;
  char **characters;
} Buffer;

typedef struct BufferChange {
  uint8_t newColorCode;
  char newCharacter;
  uint16_t positionX;
  uint16_t positionY;
  struct BufferChange *frameChange;
} BufferChange;

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
void Buffer_Resize(const uint16_t width, const uint16_t height);
uint8_t Buffer_GetColorCodeAtPosition(const uint16_t x, const uint16_t y);
char Buffer_GetCharacterAtPosition(const uint16_t x, const uint16_t y);
void Buffer_SetCharacterAndColorCodeAtPosition(const uint16_t x, const uint16_t y, const char character, const uint8_t colorCode);

#endif