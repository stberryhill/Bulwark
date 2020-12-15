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
static const int ANSI_FOREGROUND_BRIGHTNESS_NORMAL = 3;
static const int ANSI_FOREGROUND_BRIGHTNESS_BOLD = 9;
static const int ANSI_BACKGROUND_BRIGHTNESS_NORMAL = 4;
static const int ANSI_BACKGROUND_BRIGHTNESS_BOLD = 10;
static const char ANSI_COLOR256_FOREGROUND_SEQUENCE[] = "38;5";
static const char ANSI_COLOR256_BACKGROUND_SEQUENCE[] = "48;5";
static const char ANSI_ESCAPE_SEQUENCE_START[] = "\x1b";
static const char ANSI_ENTER_ALTERNATE_BUFFER_MODE[] = "?1049h";
static const char ANSI_EXIT_ALTERNATE_BUFFER_MODE[] = "?1049l";
static const char ANSI_CLEAR_BUFFER_AND_KILL_SCROLLBACK[] = "2J";
static const char ANSI_HIDE_CURSOR[] = "\x1b[?25h";
static const char ANSI_SHOW_CURSOR[] = "\x1b[?25l";
static struct termios termiosBeforeQuickTermInitialized;
static int windowWidth;
static int windowHeight;

/* Private function declarations */
static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
static void readInitialWindowWidthAndHeight();
static void enableRawInput();
static void prepareBuffer();
static void enterAlternateBufferSoWeDontMessUpPastTerminalHistory();
static void exitAlternateBufferModeSinceWeEnteredUponInitialization();
static void clearBufferAndKillScrollback();
static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();
static void generateForegroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);
static void generateBackgroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);
static void clearBufferAndKillScrollback();
static void disableBufferingOnStdoutSoPrintfWillGoThroughImmediately();
static void ensureWeStillCleanUpIfProgramStoppedWithCtrlC();

/* Function definitions */
void Bulwark_Initialize() {
  Log_Open();
  storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
  ensureWeStillCleanUpIfProgramStoppedWithCtrlC();
  readInitialWindowWidthAndHeight();
  enableRawInput();
  prepareBuffer();
  initializeEventQueue();
  setupTerminalToSendResizeEventsWhenResized();

  EventQueue_Initialize();
  Input_StartAsyncThread();
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

  Input_StopAsyncThread();
  EventQueue_Destroy();
  Log_Close();
}

static void exitAlternateBufferModeSinceWeEnteredUponInitialization() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_EXIT_ALTERNATE_BUFFER_MODE);
}

static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosBeforeQuickTermInitialized);
}

void Bulwark_SetForegroundColor16(int color16) {
  AnsiColorInfo16 ansiForegroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(color16, &ansiForegroundColorInfo);

  printf("%s[%d%dm", ANSI_ESCAPE_SEQUENCE_START, ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier);
}

void Bulwark_SetBackgroundColor16(int color16) {
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateBackgroundAnsiColorInfoFromColor16(color16, &ansiBackgroundColorInfo);

  printf("%s[%d%dm", ANSI_ESCAPE_SEQUENCE_START, ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16) {
  AnsiColorInfo16 ansiForegroundColorInfo;
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(foregroundColor16, &ansiForegroundColorInfo);
  generateBackgroundAnsiColorInfoFromColor16(backgroundColor16, &ansiBackgroundColorInfo);

  printf("%s[%d%d;%d%dm", ANSI_ESCAPE_SEQUENCE_START,
          ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier,
          ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetForegroundColor256(int color256) {
  printf("%s[%s;%dm", ANSI_ESCAPE_SEQUENCE_START, ANSI_COLOR256_FOREGROUND_SEQUENCE, color256);
}

void Bulwark_SetBackgroundColor256(int color256) {
  printf("%s[%s;%dm", ANSI_ESCAPE_SEQUENCE_START, ANSI_COLOR256_BACKGROUND_SEQUENCE, color256);
}

void Bulwark_SetForegroundAndBackgroundColor256(int foregroundColor256, int backgroundColor256) {
  printf("%s[%s;%d;%s;%dm", ANSI_ESCAPE_SEQUENCE_START, ANSI_COLOR256_FOREGROUND_SEQUENCE, foregroundColor256, ANSI_COLOR256_BACKGROUND_SEQUENCE, backgroundColor256);
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

static void generateForegroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
  }
}

static void generateBackgroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
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
