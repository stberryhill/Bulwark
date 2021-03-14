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
static const char ANSI_HIDE_CURSOR[] = "\x1b[?25l";
static const char ANSI_SHOW_CURSOR[] = "\x1b[?25h";
static struct termios termiosBeforeQuickTermInitialized;

/* Private function declarations */
static void setClearColorAndBackgroundColorToBlackInitially();
static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
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
  Log_Open();
  storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit();
  ensureWeStillCleanUpIfProgramStoppedWithCtrlC();
  enableRawInput();
  prepareBuffer();

  EventQueue_Initialize();
  Window_StartSizeListener();
  setClearColorAndBackgroundColorToBlackInitially();
  Buffer_Initialize(Bulwark_GetWindowWidth(), Bulwark_GetWindowHeight());
  BufferChangeList_Initialize();
}

static void setClearColorAndBackgroundColorToBlackInitially() {
  BulwarkColor color;
  color.mode = BULWARK_COLOR_MODE_16;
  color.color16 = BULWARK_COLOR16_BLACK;
  Bulwark_SetClearColor(&color);
  Bulwark_SetBackgroundColor(&color);
}

static void storeTerminalSettingsSoWeCanRestoreThemWhenWeQuit() {
  tcgetattr(STDIN_FILENO, &termiosBeforeQuickTermInitialized);
}

static void ensureWeStillCleanUpIfProgramStoppedWithCtrlC() {
  atexit(Bulwark_Quit);
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
  Buffer_Destroy();
  BufferChangeList_Destroy();
  clearBufferAndKillScrollback();
  exitAlternateBufferModeSinceWeEnteredUponInitialization();
  restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();

  EventQueue_Destroy();
  Log_Close();
}

static void exitAlternateBufferModeSinceWeEnteredUponInitialization() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_EXIT_ALTERNATE_BUFFER_MODE);
}

static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosBeforeQuickTermInitialized);
}

void Bulwark_DrawCharacter(int x, int y, char character) {
  const bool characterIsDifferentThanBuffer = Buffer_GetCharacterAtPosition(x, y) != character;
  const bool foregroundColorIsDifferentThanBuffer = Buffer_GetForegroundColorCodeAtPosition(x, y) != Color_GetForegroundColorCode();
  const bool backgroundColorIsDifferentThanBuffer = Buffer_GetBackgroundColorCodeAtPosition(x, y) != Color_GetBackgroundColorCode();

  if(characterIsDifferentThanBuffer || foregroundColorIsDifferentThanBuffer || backgroundColorIsDifferentThanBuffer) {
    BufferChange change;
    change.newCharacter = character;
    change.newForegroundColor = Color_GetForegroundColorCode();
    change.newBackgroundColor = Color_GetBackgroundColorCode();
    change.positionX = x;
    change.positionY = y;
    BufferChangeList_AddChange(change);
  }
}

void Bulwark_DrawString(int x, int y, const char *string, uint16_t stringLength) {
  if (stringLength > Bulwark_GetWindowWidth()) {
    Log_Error("Can't draw string because length is greater than total terminal width. Total width is %d, string length is %d", Bulwark_GetWindowWidth(), stringLength);
    exit(-1);
  }

  int i;
  for (i = 0; i < stringLength; i++) {
    Bulwark_DrawCharacter(x + i, y, string[i]);
  }
}

void Bulwark_ClearScreen() {
  BufferChangeList_Clear(); /* Clear change list to aovid confusing behavior of screen clearing but still drawing changes added before clearing the screen */
  Buffer_MarkWholeBufferDirty();
}

void Bulwark_UpdateScreen() {
  BufferChangeListNode *node = BufferChangeList_GetHead();

  BulwarkColor foregroundColor;
  BulwarkColor backgroundColor;
  BufferChange *change;
  if (BufferChangeList_GetSize() > 0) {
    do {
      /* TODO: Optimize. */
      change = node->data;
      Color_ExtractColorFromCode(change->newForegroundColor, &foregroundColor);
      Color_ExtractColorFromCode(change->newBackgroundColor, &backgroundColor);

      Bulwark_Immediate_SetForegroundAndBackgroundColor(&foregroundColor, &backgroundColor);
      Bulwark_Immediate_SetDrawPosition(change->positionX, change->positionY);
      Bulwark_Immediate_DrawCharacter(change->newCharacter);

      /* Update buffer contents to match screen */
      Buffer_SetCharacterAndColorCodesAtPosition(change->positionX, change->positionY, change->newCharacter, change->newForegroundColor, change->newBackgroundColor);
    } while ((node = node->next) != NULL);
  }


  const uint32_t clearColorCode = Color_GetClearColorCode();
  BulwarkColor clearColor;
  Color_ExtractColorFromCode(clearColorCode, &clearColor);
  const char clearCharacter = ' ';

  /* TODO: cashs current colors so we can restore after clearing here */
  /* TODO: Provide method to set foreground and background by color code instead of BulwarkColor */
  Bulwark_Immediate_SetForegroundAndBackgroundColor(&clearColor, &clearColor);
  int y;
  for (y = 0; y < Bulwark_GetWindowHeight(); y++) {
    int x;
    for (x = 0; x < Bulwark_GetWindowWidth(); x++) {
      if (Buffer_IsDirtyAtPosition(x, y)) {
        Bulwark_DrawCharacter(x, y, clearCharacter);
        Buffer_SetCharacterAndColorCodesAtPosition(x, y, clearCharacter, clearColorCode, clearColorCode);
      }
    }
  }
}

void Bulwark_Immediate_SetDrawPosition(int x, int y) {
  printf("%s[%d;%dH", ANSI_ESCAPE_SEQUENCE_START, y+1, x+1);
}

void Bulwark_Immediate_DrawCharacter(char character) {
  putchar(character);
}

void Bulwark_Immediate_DrawString(const char *string) {
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
