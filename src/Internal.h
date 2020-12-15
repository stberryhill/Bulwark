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
  int eventCount;
  int writePosition;
  int readPosition;
} EventQueue;

void EventQueue_Initialize();
void EventQueue_Destroy();
void EventQueue_AddEvent(const BulwarkEvent *event);
void EventQueue_ReadAndConsumeEvent(BulwarkEvent *output);
bool EventQueue_IsEmpty();

void Input_StartAsyncThread();
void Input_StopAsyncThread();

#endif