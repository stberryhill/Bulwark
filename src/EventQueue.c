#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* Private variables */
static const uint8_t MAX_QUEUE_POSITION = MAX_EVENTS - 1;

/* Private function declarations */
static void exitWithErrorIfQueueAlreadyFull();
static uint8_t getNextQueuePositionFromCurrentPosition(uint8_t currentPosition);

/*  Function definitions */
BulwarkEventQueue *BulwarkEventQueue_Create() {
  BulwarkEventQueue *queue = malloc(sizeof *queue);
  queue->eventCount = 0;
  queue->writePosition = 0;
  queue->readPosition = 0;

  return queue;
}

void BulwarkEventQueue_Destroy(BulwarkEventQueue *queueToDestroy) {
  free(queueToDestroy);
}

void BulwarkEventQueue_AddEvent(BulwarkEventQueue *queue, const BulwarkEvent *event) {
  exitWithErrorIfQueueAlreadyFull(queue);

  const int nextPosition = getNextQueuePositionFromCurrentPosition(queue->writePosition);

  queue->events[nextPosition] = *event;
  queue->eventCount++;
}

static void exitWithErrorIfQueueAlreadyFull(BulwarkEventQueue *queue) {
  if (queue->eventCount == MAX_EVENTS) {
    printf("Error - queue is already full\n");  
  }
}

void BulwarkEventQueue_ReadAndConsumeEvent(BulwarkEventQueue *queue, BulwarkEvent *output) {
  if (queue->eventCount == 0) {
    return;
  }

  *output = queue->events[queue->readPosition];

  queue->readPosition = getNextQueuePositionFromCurrentPosition(queue->readPosition);
  queue->eventCount--;
}

bool BulwarkEventQueue_IsEmpty(BulwarkEventQueue *queue) {
  return queue->eventCount == 0;
}

static uint8_t getNextQueuePositionFromCurrentPosition(uint8_t currentPosition) {
  return currentPosition == MAX_QUEUE_POSITION ? 0 : currentPosition + 1;
}
