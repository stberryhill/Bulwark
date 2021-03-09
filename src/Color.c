#include "Bulwark.h"
#include "Internal.h"

#include <stdio.h>
#include <stdlib.h>

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
static void setForegroundColor16(int color16);
static void setBackgroundColor16(int color16);
static void setForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16);
static void setForegroundColor256(int color256);
static void setBackgroundColor256(int color256);
static void setForegroundAndBackgroundColor256(int foregroundColor256, int backgroundColor256);

/* Function definitions */
void Bulwark_SetForegroundColor(const BulwarkColor *color) {
  /* TODO: Implement */
}

void Bulwark_SetBackgroundColor(const BulwarkColor *color) {
  /* TODO: Implement */
}

void Bulwark_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor) {
  /* TODO: Implement */
}

void Bulwark_Immediate_SetForegroundColor(const BulwarkColor *color) {
  if (color->mode == BULWARK_COLOR_MODE_16) {
    setForegroundColor16(color->color16);
  } else if (color->mode == BULWARK_COLOR_MODE_256) {
    setForegroundColor256(color->color256);
  }
}

void Bulwark_Immediate_SetBackgroundColor(const BulwarkColor *color) {
  if (color->mode == BULWARK_COLOR_MODE_16) {
    setBackgroundColor16(color->color16);
  } else if (color->mode == BULWARK_COLOR_MODE_256) {
    setBackgroundColor256(color->color256);
  }
}

void Bulwark_Immediate_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor) {
  if (foregroundColor->mode == BULWARK_COLOR_MODE_16) {
    setForegroundAndBackgroundColor16(foregroundColor->color16, backgroundColor->color16);
  } else if (foregroundColor->mode == BULWARK_COLOR_MODE_256) {
    setForegroundAndBackgroundColor256(foregroundColor->color256, backgroundColor->color256);
  }
}

static void setForegroundColor16(int color16) {
  AnsiColorInfo16 ansiForegroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(color16, &ansiForegroundColorInfo);

  printf(ansi("%d%d"), ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier);
}

static void setBackgroundColor16(int color16) {
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateBackgroundAnsiColorInfoFromColor16(color16, &ansiBackgroundColorInfo);

  printf(ansi("%d%d"), ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

static void setForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16) {
  AnsiColorInfo16 ansiForegroundColorInfo;
  AnsiColorInfo16 ansiBackgroundColorInfo;

  generateForegroundAnsiColorInfoFromColor16(foregroundColor16, &ansiForegroundColorInfo);
  generateBackgroundAnsiColorInfoFromColor16(backgroundColor16, &ansiBackgroundColorInfo);

  printf(ansi("%d%d" WITH "%d%d"),
          ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier,
          ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

static void setForegroundColor256(int color256) {
  printf(ansi(FG_256 "%d"), color256);
}

static void setBackgroundColor256(int color256) {
  printf(ansi(BG_256 "%d"), color256);
}

static void setForegroundAndBackgroundColor256(int foregroundColor256, int backgroundColor256) {
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
