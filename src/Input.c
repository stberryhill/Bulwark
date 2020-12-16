#include "Bulwark.h"
#include "Internal.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <pthread.h>

/* Private variables */
static pthread_t inputThread;

/* Private function declarations */
static void *inputThreadLoop(void *unused);

/* Function definitions */
bool Bulwark_HasEventsInQueue() {
  return !EventQueue_IsEmpty();
}

void Bulwark_ReadNextEventInQueue(BulwarkEvent *output) {
  EventQueue_ReadAndConsumeEvent(output);
}

void Bulwark_WaitForNextEvent(BulwarkEvent *output) {
  if (read(STDIN_FILENO, &output->character, 1) < 0) {
    Log_Error("Could not read character from STDIN_FILENO");
    exit(EXIT_FAILURE);
  }

  output->type = BULWARK_EVENT_TYPE_INPUT;
}

void Input_StartAsyncThread() {
  Log_Info("Starting async input thread...");

  if (pthread_create(&inputThread, NULL, inputThreadLoop, NULL)) {
    Log_Error("Could not create async input thread");
    exit(EXIT_FAILURE);
  }
}

void Input_StopAsyncThread() {
  pthread_exit(NULL);
  Log_Info("...Stopped async input thread");
}

static void *inputThreadLoop(void *unused) {
  BulwarkEvent *event = malloc(sizeof *event);

  bool keepGoing = true;
  while (keepGoing) {
    if (read(STDIN_FILENO, &event->character, 1) < 0) {
      Log_Error("Could not read character from STDIN_FILENO\n");
      exit(EXIT_FAILURE);
    }

    if (event->character == EOF) {
      keepGoing = false;
    }

    event->type = BULWARK_EVENT_TYPE_INPUT;
    EventQueue_AddEvent(event);
  }

  return NULL;
}
