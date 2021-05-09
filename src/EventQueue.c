/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

/* Private variables */
static const uint8_t MAX_QUEUE_POSITION = MAX_EVENTS - 1;
static EventQueue *queue;

/* Private function declarations */
static void exitWithErrorIfQueueAlreadyFull();
static uint8_t getNextQueuePositionFromCurrentPosition(uint8_t currentPosition);

/*  Function definitions */
void EventQueue_Initialize() {
  queue = malloc(sizeof *queue);
  queue->eventCount = 0;
  queue->writePosition = 0;
  queue->readPosition = 0;
}

void EventQueue_Destroy() {
  free(queue);
}

void EventQueue_AddEvent(const BulwarkEvent *event) {
  exitWithErrorIfQueueAlreadyFull();

  queue->events[queue->writePosition] = *event;
  queue->writePosition = getNextQueuePositionFromCurrentPosition(queue->writePosition);
  queue->eventCount++;
}

static void exitWithErrorIfQueueAlreadyFull() {
  if (queue->eventCount == MAX_EVENTS) {
    Log_Error("Queue is already full, latest events are being dropped.");  
  }
}

void EventQueue_ReadAndConsumeEvent(BulwarkEvent *output) {
  if (queue->eventCount == 0) {
    return;
  }

  *output = queue->events[queue->readPosition];

  queue->readPosition = getNextQueuePositionFromCurrentPosition(queue->readPosition);
  queue->eventCount--;
}

bool EventQueue_IsEmpty() {
  return queue->eventCount == 0;
}

static uint8_t getNextQueuePositionFromCurrentPosition(uint8_t currentPosition) {
  return currentPosition == MAX_QUEUE_POSITION ? 0 : currentPosition + 1;
}
