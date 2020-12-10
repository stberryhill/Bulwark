#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>

/* Private variables */
static BulwarkEventQueue *eventQueue;

/* Private function declarations */
static void enableRawInput();
static void initializeEventQueue();
static void installTerminalResizeCallback();
static void terminalResizeCallback(int i);
static void setupTerminalToSendResizeEventsWhenResized();

/* Function definitions */
bool Bulwark_HasEventsInQueue() {
  return !BulwarkEventQueue_IsEmpty(eventQueue);
}

void Bulwark_ReadNextEventInQueue(BulwarkEvent *output) {
  BulwarkEventQueue_ReadAndConsumeEvent(eventQueue, output);
}

void Bulwark_WaitForNextEvent(BulwarkEvent *output) {
  if (read(STDIN_FILENO, &output->character, 1) < 0) {
    fprintf(logFileDescriptor, "Error - could not read character from STDIN_FILENO\n");
    exit(-1);
  }
  fflush(logFileDescriptor);
  output->type = BULWARK_EVENT_TYPE_INPUT;
}

static void enableRawInput() {
  struct termios termiosRaw;
  tcgetattr(STDIN_FILENO, &termiosRaw);
  termiosRaw.c_lflag &= (~ECHO & ~ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosRaw);
}

static void initializeEventQueue() {
  eventQueue = BulwarkEventQueue_Create();
}

static void installTerminalResizeCallback() {
  /* Our child process will receive the SIGWINCH signal whenever the terminal is resized */
  signal(SIGWINCH, terminalResizeCallback);
}

static void terminalResizeCallback(int i) {
  struct winsize newWindowSize;

	ioctl(1, TIOCGWINSZ, &newWindowSize);

  /* Set these so GetWindowWidth and GetWindowHight functions stay up-to-date. */
	windowWidth = newWindowSize.ws_col;
	windowHeight = newWindowSize.ws_row;

  /* Queue window resize event */
  BulwarkEvent event;
  event.type = BULWARK_EVENT_TYPE_WINDOW_RESIZE;
  event.newWindowWidth = windowWidth;
  event.newWindowHeight = windowHeight;

  BulwarkEventQueue_AddEvent(eventQueue, &event);
}

static void setupTerminalToSendResizeEventsWhenResized() {
  installTerminalResizeCallback();
}
