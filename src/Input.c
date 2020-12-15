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
static void enableRawInput();
static void installTerminalResizeCallback();
static void terminalResizeCallback(int i);
static void setupTerminalToSendResizeEventsWhenResized();
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
  if (pthread_create(&inputThread, NULL, inputThreadLoop, NULL)) {
    Log_Error("Could not create async input thread");
    exit(EXIT_FAILURE);
  }
}

void Input_StopAsyncThread() {
  pthread_exit(NULL);
}

static void *inputThreadLoop(void *unused) {
  BulwarkEvent *event = malloc(sizeof *event);

  if (read(STDIN_FILENO, &event->character, 1) < 0) {
    Log_Error("Could not read character from STDIN_FILENO\n");
    exit(EXIT_FAILURE);
  }

  event->type = BULWARK_EVENT_TYPE_INPUT;
  EventQueue_AddEvent(event);
}

static void enableRawInput() {
  struct termios termiosRaw;
  tcgetattr(STDIN_FILENO, &termiosRaw);
  termiosRaw.c_lflag &= (~ECHO & ~ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosRaw);
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

  EventQueue_AddEvent(&event);
}

static void setupTerminalToSendResizeEventsWhenResized() {
  installTerminalResizeCallback();
}
