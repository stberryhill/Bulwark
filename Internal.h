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
  uint16 newWindowWidth;
  uint16 newWindowHeight;
};

typedef struct AnsiColorInfo16 {
  uint8 brightnessSpecifier;
  uint8 colorSpecifier;
} AnsiColorInfo16;

typedef struct BulwarkEventQueue {
  BulwarkEvent events[MAX_EVENTS];
  uint8 eventCount;
  uint8 writePosition;
  uint8 readPosition;
} BulwarkEventQueue;

BulwarkEventQueue *BulwarkEventQueue_Create();
void BulwarkEventQueue_Destroy(BulwarkEventQueue *queueToDestroy);
void BulwarkEventQueue_AddEvent(BulwarkEventQueue *queue, const BulwarkEvent *event);
void BulwarkEventQueue_ReadAndConsumeEvent(BulwarkEventQueue *queue, BulwarkEvent *output);
bool BulwarkEventQueue_IsEmpty(BulwarkEventQueue *queue);

#endif