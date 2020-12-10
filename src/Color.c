#include "Bulwark.h"
#include "Internal.h"

#include <stdio.h>

#define WITH ";"
#define FG_256 "38;5;"
#define BG_256 "48;5;"
#define ansi(sequence) "\x1b[" sequence "m"

#define ANSI_FOREGROUND_BRIGHTNESS_NORMAL 3;
#define ANSI_FOREGROUND_BRIGHTNESS_BOLD 9
#define ANSI_BACKGROUND_BRIGHTNESS_NORMAL 4
#define ANSI_BACKGROUND_BRIGHTNESS_BOLD 10

/* Private function declarations */
static void generateForegroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);
static void generateBackgroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output);

void Bulwark_SetForegroundColor16(int color16) {
  AnsiColorInfo16 ansiForegroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(color16, &ansiForegroundColorInfo);

  printf(ansi("%d%d"), ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier);
}

void Bulwark_SetBackgroundColor16(int color16) {
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateBackgroundAnsiColorInfoFromColor16(color16, &ansiBackgroundColorInfo);

  printf(ansi("%d%d"), ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16) {
  AnsiColorInfo16 ansiForegroundColorInfo;
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(foregroundColor16, &ansiForegroundColorInfo);
  generateBackgroundAnsiColorInfoFromColor16(backgroundColor16, &ansiBackgroundColorInfo);

  printf(ansi("%d%d" WITH "%d%d"),
          ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier,
          ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

void Bulwark_SetForegroundColor256(int color256) {
  printf(ansi(FG_256 "%d"), color256);
}

void Bulwark_SetBackgroundColor256(int color256) {
  printf(ansi(BG_256 "%d"), color256);
}

void Bulwark_SetForegroundAndBackgroundColor256(int foregroundColor256, int backgroundColor256) {
  printf(ansi(FG_256 "%d" WITH BG_256 "%d"), foregroundColor256, backgroundColor256);
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
