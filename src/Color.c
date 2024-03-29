/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

#include "Bulwark.h"
#include "Internal.h"

#include <stdio.h>
#include <stdlib.h>

#define COLOR_MODE_MASK   0x0F000000
#define COLOR_RED_MASK    0x00FF0000
#define COLOR_BLUE_MASK   0x0000FF00
#define COLOR_GREEN_MASK  0x000000FF
#define COLOR_256_MASK    0x000000FF
#define COLOR_16_MASK     0x000000FF

#define ANSI_FOREGROUND_BRIGHTNESS_NORMAL 3;
#define ANSI_FOREGROUND_BRIGHTNESS_BOLD 9
#define ANSI_BACKGROUND_BRIGHTNESS_NORMAL 4
#define ANSI_BACKGROUND_BRIGHTNESS_BOLD 10

static uint32_t currentForegroundColor;
static uint32_t currentBackgroundColor;
static uint32_t currentClearColor;
static bool foregroundColorCleared;
static bool backgroundColorCleared;

/* Private function declarations */
static void setForegroundColor16(int color16);
static void setBackgroundColor16(int color16);
static void setForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16);
static void setForegroundColor256(int color256);
static void setBackgroundColor256(int color256);
static void setForegroundAndBackgroundColor256(int foregroundColor256, int backgroundColor256);

BulwarkColor *BulwarkColor_Create16(uint8_t color16) {
  BulwarkColor *color = malloc(sizeof *color);
  color->mode = BULWARK_COLOR_MODE_16;
  color->color16 = color16;

  return color;
}

BulwarkColor *BulwarkColor_Create256ByCode(uint8_t colorCode256) {
  BulwarkColor *color = malloc(sizeof *color);
  color->mode = BULWARK_COLOR_MODE_256;
  color->color256 = colorCode256;

  return color;
}

/**
 * Each r, g, b needs to be in the range [0, 5]
 */
BulwarkColor *BulwarkColor_Create256(uint8_t r, uint8_t g, uint8_t b) {
  BulwarkColor *color = malloc(sizeof *color);
  color->mode = BULWARK_COLOR_MODE_256;
  color->color256 = 16 + (36 * r) + (16 * g) + b;

  return color;
}

void BulwarkColor_Destroy(BulwarkColor *color) {
  free(color);
}

/* Function definitions */
void Bulwark_SetForegroundColor(const BulwarkColor *color) {
  currentForegroundColor = Color_GenerateColorCodeForColor(color);
  foregroundColorCleared = false;
}

void Bulwark_SetBackgroundColor(const BulwarkColor *color) {
  currentBackgroundColor = Color_GenerateColorCodeForColor(color);
  backgroundColorCleared = false;
}

uint32_t Color_GenerateColorCodeForColor(const BulwarkColor *color) {
  uint32_t colorCode = color->mode << 24; /* Left-most byte is the mode byte */
  colorCode |= (color->r << 16);
  colorCode |= (color->g << 8);
  colorCode |= (color->b << 0);
  colorCode |= (color->color16 << 0);
  colorCode |= (color->color256 << 0);

  return colorCode;
}

void Color_ExtractColorFromCode(uint32_t colorCode, BulwarkColor *result) {
  const uint8_t mode = (colorCode & COLOR_MODE_MASK) >> 24;
  result->mode = mode;
  
  if (mode == BULWARK_COLOR_MODE_16) {
      const uint8_t color16 = (colorCode & COLOR_16_MASK) >> 0;
      result->color16 = color16;
  } else if (mode ==BULWARK_COLOR_MODE_256) {
      const uint8_t color256 = (colorCode & COLOR_256_MASK) >> 0;
      result->color256 = color256;
  } else if (mode == BULWARK_COLOR_MODE_RGB) {
      const uint8_t red = (colorCode & COLOR_RED_MASK) >> 16;
      const uint8_t green = (colorCode & COLOR_GREEN_MASK) >> 8;
      const uint8_t blue = (colorCode & COLOR_BLUE_MASK) >> 0;
      result->r = red;
      result->g = green;
      result->b = blue;
  } else {
      Log_Error("Cannot extract color from color code because the mode is invalid. (mode=%d)\n", mode);
      exit(-1);
  }
}

void Bulwark_SetForegroundAndBackgroundColor(const BulwarkColor *foregroundColor, const BulwarkColor *backgroundColor) {
  currentForegroundColor = Color_GenerateColorCodeForColor(foregroundColor);
  currentBackgroundColor = Color_GenerateColorCodeForColor(backgroundColor);
  foregroundColorCleared = false;
  backgroundColorCleared = false;
}

uint32_t Color_GetForegroundColorCode() {
  return currentForegroundColor;
}

uint32_t Color_GetBackgroundColorCode() {
  return currentBackgroundColor;
}

void Bulwark_SetClearColor(const BulwarkColor *color) {
  currentClearColor = Color_GenerateColorCodeForColor(color);
}

uint32_t Color_GetClearColorCode() {
  return currentClearColor;
}

void Bulwark_ClearForegroundAndBackgroundColor() {
  currentForegroundColor = currentClearColor;
  currentBackgroundColor = currentClearColor;
  foregroundColorCleared = true;
  backgroundColorCleared = true;
}

bool Color_IsForegroundColorCleared() {
  return foregroundColorCleared;
}
bool Color_IsBackgroundColorCleared() {
  return backgroundColorCleared;
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

  Color_GnerateForegroundAnsiColorInfoFromColor16(color16, &ansiForegroundColorInfo);

  printf(ansi("%d%d"), ansiForegroundColorInfo.brightnessSpecifier, ansiForegroundColorInfo.colorSpecifier);
}

static void setBackgroundColor16(int color16) {
  AnsiColorInfo16 ansiBackgroundColorInfo;

  Color_GenerateBackgroundAnsiColorInfoFromColor16(color16, &ansiBackgroundColorInfo);

  printf(ansi("%d%d"), ansiBackgroundColorInfo.brightnessSpecifier, ansiBackgroundColorInfo.colorSpecifier);
}

static void setForegroundAndBackgroundColor16(int foregroundColor16, int backgroundColor16) {
  AnsiColorInfo16 ansiForegroundColorInfo;
  AnsiColorInfo16 ansiBackgroundColorInfo;

  Color_GnerateForegroundAnsiColorInfoFromColor16(foregroundColor16, &ansiForegroundColorInfo);
  Color_GenerateBackgroundAnsiColorInfoFromColor16(backgroundColor16, &ansiBackgroundColorInfo);

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

void Color_GnerateForegroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_FOREGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
  }
}

void Color_GenerateBackgroundAnsiColorInfoFromColor16(int color16, AnsiColorInfo16 *output) {
  if (color16 < 8) {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_NORMAL;
    output->colorSpecifier = color16;
  } else {
    output->brightnessSpecifier = ANSI_BACKGROUND_BRIGHTNESS_BOLD;
    output->colorSpecifier = color16 - 8;
  }
}
