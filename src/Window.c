#include "Internal.h"

#include <sys/ioctl.h>
#include <signal.h>

/* Private variables */
static int windowWidth;
static int windowHeight;

/* Private function declarations */
static void readInitialWindowSize();
static void installTerminalResizeCallback();
static void terminalResizeCallback(int i);
static void setupTerminalToSendResizeEventsWhenResized();

/* Function definitions */
int Bulwark_GetWindowWidth() {
  return windowWidth;
}

int Bulwark_GetWindowHeight() {
  return windowHeight;
}

void Window_StartSizeListener() {
  readInitialWindowSize();
  setupTerminalToSendResizeEventsWhenResized();
}

static void readInitialWindowSize() {
  struct winsize windowSize;
	ioctl(0, TIOCGWINSZ, (char *) &windowSize);
	windowWidth = windowSize.ws_col;
  windowHeight = windowSize.ws_row;
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
