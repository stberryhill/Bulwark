#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>

/* Private variables */
static const int ANSI_FOREGROUND_BRIGHTNESS_NORMAL = 3;
static const int ANSI_FOREGROUND_BRIGHTNESS_BOLD = 9;
static const int ANSI_BACKGROUND_BRIGHTNESS_NORMAL = 4;
static const int ANSI_BACKGROUND_BRIGHTNESS_BOLD = 10;
static const char ANSI_ESCAPE_SEQUENCE_START[] = "\x1b";
static const char ANSI_ENTER_ALTERNATE_BUFFER_MODE[] = "?1049h";
static const char ANSI_EXIT_ALTERNATE_BUFFER_MODE[] = "?1049l";
static const char ANSI_CLEAR_BUFFER_AND_KILL_SCROLLBACK[] = "2J";
static const char ANSI_HIDE_CURSOR[] = "\x1b[?25h";
static const char ANSI_SHOW_CURSOR[] = "\x1b[?25l";
static BulwarkEventQueue *eventQueue;
static struct termios termiosBeforeQuickTermInitialized;
static uint16 windowWidth;
static uint16 windowHeight;
static FILE *logFileDescriptor;

/* Private function declarations */
static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
static void readInitialWindowWidthAndHeight();
static void enableRawInput();
static void initializeEventQueue();
static void setupTerminalToSendResizeEventsWhenResized();
static void installTerminalResizeCallback();
static void terminalResizeCallback(int i);
static void prepareBuffer();
static void enterAlternateBufferSoWeDontMessUpPastTerminalHistory();
static void exitAlternateBufferModeSinceWeEnteredUponInitialization();
static void clearBufferAndKillScrollback();
static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();
static void generateForegroundAnsiColorInfoFromColor16(uint8 color16, AnsiColorInfo16 *output);
static void generateBackgroundAnsiColorInfoFromColor16(uint8 color16, AnsiColorInfo16 *output);
static void clearBufferAndKillScrollback();
static void disableBufferingOnStdoutSoPrintfWillGoThroughImmediately();
static void ensureWeStillCleanUpIfProgramStoppedWithCtrlC();

/* Function definitions */
void Bulwark_Initialize() {
  logFileDescriptor = fopen("Log.txt", "w");
  storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
  ensureWeStillCleanUpIfProgramStoppedWithCtrlC();
  readInitialWindowWidthAndHeight();
  enableRawInput();
  prepareBuffer();
  initializeEventQueue();
  setupTerminalToSendResizeEventsWhenResized();
}

static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit() {
  tcgetattr(STDIN_FILENO, &termiosBeforeQuickTermInitialized);
}

static void ensureWeStillCleanUpIfProgramStoppedWithCtrlC() {
  atexit(Bulwark_Quit);
}

static void readInitialWindowWidthAndHeight() {
  struct winsize windowSize;
	ioctl(0, TIOCGWINSZ, (char *) &windowSize);
	windowWidth = windowSize.ws_col;
  windowHeight = windowSize.ws_row;
}

static void enableRawInput() {
  struct termios termiosRaw;
  tcgetattr(STDIN_FILENO, &termiosRaw);
  termiosRaw.c_lflag &= (~ECHO & ~ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &termiosRaw);
}

static void prepareBuffer() {
  enterAlternateBufferSoWeDontMessUpPastTerminalHistory();
  clearBufferAndKillScrollback();
  disableBufferingOnStdoutSoPrintfWillGoThroughImmediately();
}

static void enterAlternateBufferSoWeDontMessUpPastTerminalHistory() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_ENTER_ALTERNATE_BUFFER_MODE);
}

static void clearBufferAndKillScrollback() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_CLEAR_BUFFER_AND_KILL_SCROLLBACK);
}

static void disableBufferingOnStdoutSoPrintfWillGoThroughImmediately() {
  setvbuf(stdout, NULL, _IONBF, 0);
}

static void initializeEventQueue() {
  eventQueue = BulwarkEventQueue_Create();
}

static void setupTerminalToSendResizeEventsWhenResized() {
  installTerminalResizeCallback();
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

void Bulwark_Quit() {
  clearBufferAndKillScrollback();
  exitAlternateBufferModeSinceWeEnteredUponInitialization();
  restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();
  fclose(logFileDescriptor);
}

static void exitAlternateBufferModeSinceWeEnteredUponInitialization() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_EXIT_ALTERNATE_BUFFER_MODE);
}

static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosBeforeQuickTermInitialized);
}

bool Bulwark_HasEventsInQueue() {
  return !BulwarkEventQueue_IsEmpty(eventQueue);
}

void Bulwark_ReadNextEventInQueue(BulwarkEvent *output) {
  BulwarkEventQueue_ReadAndConsumeEvent(eventQueue, output);
}

void Bulwark_WaitForNextEvent(BulwarkEvent *output) {
  fprintf(logFileDescriptor, "Waiting for event...\n");
  fflush(logFileDescriptor);
  read(STDIN_FILENO, &output->character, 1);
  fprintf(logFileDescriptor, "... event Recieved, Character: %c\n", output->character);
  fflush(logFileDescriptor);
  output->type = BULWARK_EVENT_TYPE_INPUT;
}

void Bulwark_SetForegroundColor16(uint8 color16) {
  AnsiColorInfo16 ansiForegroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(color16, &ansiForegroundColorInfo);

  printf("%s[%d%dm", ANSI_ESCAPE_SEQUENCE_START, ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier);
}

void Bulwark_SetBackgroundColor16(uint8 color16) {
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateBackgroundAnsiColorInfoFromColor16(color16, &ansiBackgroundColorInfo);

  printf("%s[%d%dm", ANSI_ESCAPE_SEQUENCE_START, ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetForegroundAndBackgroundColors16(uint8 foregroundColor16, uint8 backgroundColor16) {
  AnsiColorInfo16 ansiForegroundColorInfo;
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(foregroundColor16, &ansiForegroundColorInfo);
  generateBackgroundAnsiColorInfoFromColor16(backgroundColor16, &ansiBackgroundColorInfo);

  printf("%s[%d%d;%d%dm", ANSI_ESCAPE_SEQUENCE_START,
          ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier,
          ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetDrawPosition(uint16 x, uint16 y) {
  printf("%s[%d;%dH", ANSI_ESCAPE_SEQUENCE_START, y+1, x+1);
}

void Bulwark_DrawCharacter(char character) {
  putchar(character);
}

void Bulwark_DrawString(const char *string) {
  printf("%s", string);;
}

void Bulwark_SetCursorVisible(bool cursorVisible) {
  if (cursorVisible) {
    printf("%s", ANSI_SHOW_CURSOR);
  } else {
    printf("%s", ANSI_HIDE_CURSOR);
  }
}

static void generateForegroundAnsiColorInfoFromColor16(uint8 color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
  }
}

static void generateBackgroundAnsiColorInfoFromColor16(uint8 color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
  }
}

uint16 Bulwark_GetWindowWidth() {
  return windowWidth;
}

uint16 Bulwark_GetWindowHeight() {
  return windowHeight;
}
