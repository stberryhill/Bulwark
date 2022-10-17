/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

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
static bool screenShouldBeCleared;
static bool quitAlreadyCalled = false;

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
static void copyStringAndIncrementByLength(const char *source, char *destination, int *numberToIncrementByCopiedLength);
static int min(int a, int b);

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
  if (!quitAlreadyCalled) {
    quitAlreadyCalled = true;

    Buffer_Destroy();
    BufferChangeList_Destroy();
    clearBufferAndKillScrollback();
    exitAlternateBufferModeSinceWeEnteredUponInitialization();
    restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized();

    EventQueue_Destroy();
    Log_Close();
    Bulwark_SetCursorVisible(true);
  }
}

static void exitAlternateBufferModeSinceWeEnteredUponInitialization() {
  printf("%s[%s", ANSI_ESCAPE_SEQUENCE_START, ANSI_EXIT_ALTERNATE_BUFFER_MODE);
}

static void restoreTerminalSettingsToWhatTheyWereBeforeWeInitialized() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosBeforeQuickTermInitialized);
}

void Bulwark_DrawCharacter(int x, int y, char character) {
  if (x < 0 || y < 0 || x >= Bulwark_GetWindowWidth() || y >= Bulwark_GetWindowHeight()) {
    return;
  }

  const int foregroundColorInBuffer = Buffer_GetForegroundColorCodeAtPosition(x, y);
  const int backgroundColorInBuffer = Buffer_GetBackgroundColorCodeAtPosition(x, y);
  
  const bool characterIsDifferentThanBuffer = Buffer_GetCharacterAtPosition(x, y) != character;
  const bool foregroundColorIsDifferentThanBuffer = !Color_IsForegroundColorCleared() && (foregroundColorInBuffer != Color_GetForegroundColorCode());
  const bool backgroundColorIsDifferentThanBuffer = !Color_IsBackgroundColorCleared() && (backgroundColorInBuffer != Color_GetBackgroundColorCode());

  if(characterIsDifferentThanBuffer || foregroundColorIsDifferentThanBuffer || backgroundColorIsDifferentThanBuffer) {
    BufferChange change;
    change.newCharacter = character;
    change.newForegroundColor = Color_IsForegroundColorCleared() ? foregroundColorInBuffer : Color_GetForegroundColorCode();
    change.newBackgroundColor = Color_IsBackgroundColorCleared() ? backgroundColorInBuffer : Color_GetBackgroundColorCode();
    change.positionX = x;
    change.positionY = y;
    BufferChangeList_AddChange(change);

    Buffer_SetCharacterAndColorCodesAtPosition(x, y, character, change.newForegroundColor, change.newBackgroundColor);
  } else {
    /* Could try to avoid adding a change if it already will be accomplished by a change before it */
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
  screenShouldBeCleared = true;
}

void Bulwark_UpdateScreen() {
  if (BufferChangeList_GetSize() == 0) {
    return;
  }

  BufferChangeListNode *node = BufferChangeList_GetHead();

  BulwarkColor foregroundColor;
  BulwarkColor backgroundColor;
  BufferChange *change;
  const int sizePerChange = 28;
  const int bufferSizeChanges = 73;
  char buffer[(sizePerChange * bufferSizeChanges) + 1]; /* Roughly 2kB */
  
  int changeCount;
  int byteCount;
  const int bufferPassesNecessary = (BufferChangeList_GetSize() / bufferSizeChanges) + 1;
  Log_Info("Number of changes = %d, buffer Passes necessary=%d", BufferChangeList_GetSize(), bufferPassesNecessary);
  int bufferPassCount;
  for (bufferPassCount = 0; bufferPassCount < bufferPassesNecessary; bufferPassCount++) {
  
    const int changesLeft = BufferChangeList_GetSize() - (bufferPassCount * bufferSizeChanges);
    const int changesInThisBuffer = min(changesLeft, bufferSizeChanges);
    Log_Info("Changes left = %d, Change in this buffer = %d", changesLeft, changesInThisBuffer);

    int bufferPos = 0;
    int changeCount;
    for (changeCount = 0; changeCount < changesInThisBuffer; changeCount++) {
      Log_Info("bufferPos = %d", bufferPos);
      change = node->data;
      Log_Info("Drawing at (%d, %d) color: %d", change->positionX, change->positionY, change->newBackgroundColor & 0x000000FF);

      /* TODO, only add color change to buffer if needed for foreground & background */

      AnsiColorInfo16 ansiColorInfo;
      char color16String[28];

      /* Foreground */
      Color_ExtractColorFromCode(change->newForegroundColor, &foregroundColor);

      switch (foregroundColor.mode) {
        case BULWARK_COLOR_MODE_16:
          Color_GnerateForegroundAnsiColorInfoFromColor16(foregroundColor.color16, &ansiColorInfo);
          bufferPos += sprintf(buffer + bufferPos, ansi("%d%d"), ansiColorInfo.brightnessSpecifier, ansiColorInfo.colorSpecifier);
          break;
        case BULWARK_COLOR_MODE_256:
          bufferPos += sprintf(buffer + bufferPos, ansi(FG_256 "%d"), foregroundColor.color256);
          break;
        case BULWARK_COLOR_MODE_RGB:
          /* TOOD: implement */
          Log_Error("RGB colors not yet implemented");
          exit(EXIT_FAILURE);
          break;
        default:
          Log_Error("Unknown foreground color mode %d", foregroundColor.mode);
          exit(EXIT_FAILURE);
          break;
      }

      /* Background */
      Color_ExtractColorFromCode(change->newBackgroundColor, &backgroundColor);

      switch (backgroundColor.mode) {
        case BULWARK_COLOR_MODE_16:
          Color_GenerateBackgroundAnsiColorInfoFromColor16(backgroundColor.color16, &ansiColorInfo);
          bufferPos += sprintf(buffer + bufferPos, ansi("%d%d"), ansiColorInfo.brightnessSpecifier, ansiColorInfo.colorSpecifier);
          break;
        case BULWARK_COLOR_MODE_256:
          bufferPos += sprintf(buffer + bufferPos, ansi(BG_256 "%d"), backgroundColor.color256);
          break;
        case BULWARK_COLOR_MODE_RGB:
          /* TOOD: implement */
          Log_Error("RGB colors not yet implemented\n");
          exit(EXIT_FAILURE);
          break;
        default:
          Log_Error("Unknown background color mode %d", backgroundColor.mode);
          exit(EXIT_FAILURE);
          break;
      }

      bufferPos += sprintf(buffer + bufferPos, "%s[%d;%dH", ANSI_ESCAPE_SEQUENCE_START, change->positionY + 1, change->positionX + 1);
      buffer[bufferPos++] = change->newCharacter;

      /* Update buffer contents to match screen */
      Buffer_SetCharacterAndColorCodesAtPosition(change->positionX, change->positionY, change->newCharacter, change->newForegroundColor, change->newBackgroundColor);
      Buffer_MarkUpToDateAtPosition(change->positionX, change->positionY);

      node = node->next;
    }
      Log_Info("POS: %d", bufferPos);
      Log_Info("buffer: %s", buffer);
      buffer[bufferPos++] = '\0';
      printf("%s", buffer);
  }

  BufferChangeList_Clear();

  if (screenShouldBeCleared) {
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
        if (!Buffer_IsUpToDateAtPosition(x, y)) {
          if(Buffer_GetCharacterAtPosition(x, y) != clearCharacter || Buffer_GetForegroundColorCodeAtPosition(x, y) != clearColorCode || Buffer_GetBackgroundColorCodeAtPosition(x, y) != clearColorCode) {
            Log_Info("Clearing...");
            Bulwark_Immediate_SetDrawPosition(x, y);
            Bulwark_Immediate_DrawCharacter(clearCharacter);
            Buffer_SetCharacterAndColorCodesAtPosition(x, y, clearCharacter, clearColorCode, clearColorCode);
          }
        } else {
          Buffer_MarkOutdatedAtPosition(x, y);
        }
      }
    }

    screenShouldBeCleared = false;
  }
}

static void copyStringAndIncrementByLength(const char *source, char *destination, int *numberToIncrementByCopiedLength) {
  int i;
  for (i = 0; source[i] != '\0'; i++) {
    destination[i] = source[i];
  }

  *numberToIncrementByCopiedLength += i;
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

void Bulwark_Immediate_ClearForegroundAndBackgroundColor() {
  printf("%s[%dm", ANSI_ESCAPE_SEQUENCE_START, ANSI_CLEAR_FORMATTING);
}

static int min(int a, int b) {
  return a * (a < b) + b * (b < a);
}