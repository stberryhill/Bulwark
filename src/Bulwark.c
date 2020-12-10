#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>

/* Private variables */
static const int ANSI_CLEAR_FORMATTING = 0;
static const char ANSI_ESCAPE_SEQUENCE_START[] = "\x1b";
static const char ANSI_ENTER_ALTERNATE_BUFFER_MODE[] = "?1049h";
static const char ANSI_EXIT_ALTERNATE_BUFFER_MODE[] = "?1049l";
static const char ANSI_CLEAR_BUFFER_AND_KILL_SCROLLBACK[] = "2J";
static const char ANSI_HIDE_CURSOR[] = "\x1b[?25h";
static const char ANSI_SHOW_CURSOR[] = "\x1b[?25l";
static struct termios termiosBeforeQuickTermInitialized;
static int windowWidth;
static int windowHeight;
static FILE *logFileDescriptor;

/* Private function declarations */
static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
static void readInitialWindowWidthAndHeight();
static void enableRawInput();
static void prepareBuffer();
static void enterAlternateBufferSoWeDontMessUpPastTerminalHistory();
static void exitAlternateBufferModeSinceWeEnteredUponInitialization();
static void clearBufferAndKillScrollback();
static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();
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

void Bulwark_WaitForNextEvent(BulwarkEvent *output) {
  fflush(logFileDescriptor);
  if (read(STDIN_FILENO, &output->character, 1) < 0) {
    fprintf(logFileDescriptor, "Error - could not read character from STDIN_FILENO\n");
    exit(-1);
  }
  fflush(logFileDescriptor);
  output->type = BULWARK_EVENT_TYPE_INPUT;
}

void Bulwark_SetDrawPosition(int x, int y) {
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

void Bulwark_ClearForegroundAndBackgroundColor() {
  printf("%s[%dm", ANSI_ESCAPE_SEQUENCE_START, ANSI_CLEAR_FORMATTING);
}

int Bulwark_GetWindowWidth() {
  return windowWidth;
}

int Bulwark_GetWindowHeight() {
  return windowHeight;
}
