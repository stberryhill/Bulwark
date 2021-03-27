#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>

static const uint32_t CLEAR_COLOR_CODE = 0 | (1 << 31);
static const char CLEAR_CHARACTER = ' ';

static Buffer *buffer;

void Buffer_Initialize(const uint16_t width, const uint16_t height) {
  buffer = malloc(sizeof *buffer);

  buffer->characters = malloc(height * (sizeof *buffer->characters));
  buffer->foregroundColorCodes = malloc(height * (sizeof *buffer->foregroundColorCodes));
  buffer->backgroundColorCodes = malloc(height * (sizeof *buffer->backgroundColorCodes));

  int j;
  for (j = 0; j < height; j++) {
    buffer->characters[j] = malloc(width * (sizeof *buffer->characters[j]));
    buffer->foregroundColorCodes[j] = malloc(width * (sizeof *buffer->foregroundColorCodes[j]));
    buffer->backgroundColorCodes[j] = malloc(width * (sizeof *buffer->backgroundColorCodes[j]));


    int i;
    for (i = 0; i < width; i++) {
      buffer->characters[j][i] = CLEAR_CHARACTER;
      buffer->foregroundColorCodes[j][i] = CLEAR_COLOR_CODE;
      buffer->backgroundColorCodes[j][i] = CLEAR_COLOR_CODE;
    }
  }

  buffer->width = width;
  buffer->height = height;
}

void Buffer_Destroy() {
  /* Free inner arrays first */
  int y;
  for (y = 0; y < buffer->height; y++) {
    free(buffer->characters[y]);
    free(buffer->foregroundColorCodes[y]);
    free(buffer->backgroundColorCodes[y]);
  }

  /* Free outer arrays last */
  free(buffer->characters);
  free(buffer->foregroundColorCodes);
  free(buffer->backgroundColorCodes);
}

void Buffer_Resize(const uint16_t width, const uint16_t height) {
  /* First, resize height */
  if (buffer->height > height) {
    /* 1. Free char arrays that will be removed */
    int j;
    for (j = height; j < buffer->height; j++) {
      free(buffer->characters[j]);
    }

    /* 2. Remove extra character arrays to reach new smaller size */
    buffer->characters = realloc(buffer->characters, height);

  } else if (buffer->height < height) {
    /* 1. Reallocate to include new rows */
    buffer->characters = realloc(buffer->characters, height);

    int j;
    for (j = buffer->height; j < height; j++) {
      /* 2. Allocate row characters */
      buffer->characters[j] = malloc(buffer->width * (sizeof *buffer->characters[j]));

      int i;
      for (i = 0; i < buffer->width; i++) {
        /* 3. Set new characters to clear */
        buffer->characters[j][i] = CLEAR_CHARACTER;
        buffer->foregroundColorCodes[j][i] = CLEAR_COLOR_CODE;
      }
    }
  }

  buffer->height = height;

  /* Next resize width */
  if (buffer->width != width) {
    int j;
    for (j = 0; j < height; j++) {
      /* Just a simple realloc */
      buffer->characters[j] = realloc(buffer->characters[j], width * (sizeof buffer->characters[j][0]));
    }
  }

  buffer->width = width;
}

uint32_t Buffer_GetForegroundColorCodeAtPosition(const uint16_t x, const uint16_t y) {
  return buffer->foregroundColorCodes[y][x];
}

uint32_t Buffer_GetBackgroundColorCodeAtPosition(const uint16_t x, const uint16_t y) {
  return buffer->backgroundColorCodes[y][x];
}

char Buffer_GetCharacterAtPosition(const uint16_t x, const uint16_t y) {
  return buffer->characters[y][x];
}

void Buffer_SetCharacterAndColorCodesAtPosition(const uint16_t x, const uint16_t y, const char character, const uint16_t foregroundColorCode, const uint16_t backgroundColorCode) {
  if (x > buffer->width || y > buffer->height) {
    Log_Error("Buffer can't set character and color code at position because position is out of bounds: (%d, %d)", x, y);
    exit(-1);
  }

  buffer->characters[y][x] = character;
  buffer->foregroundColorCodes[y][x] = foregroundColorCode;
  buffer->backgroundColorCodes[y][x] = backgroundColorCode;
}

void Buffer_MarkWholeBufferDirty() {
  int y;
  for (y = 0; y < buffer->height; y++) {
    int x;
    for (x = 0; x < buffer->width; x++) {
      Buffer_MarkUpToDateAtPosition(x, y);
    }
  }
}

void Buffer_MarkUpToDateAtPosition(const uint16_t x, const uint16_t y) {
  /* Uses (otherwise unused) topmost bit of background color code as dirty flag for this position. Dirty positions will be cleared when screen is updated. */
  buffer->backgroundColorCodes[y][x] |= (1 << 31);
}

void Buffer_MarkOutdatedAtPosition(const uint16_t x, const uint16_t y) {
  buffer->backgroundColorCodes[y][x] &= 0x0FFFFFFF;
}

bool Buffer_IsUpToDateAtPosition(const uint16_t x, const uint16_t y) {
  /* Check if position is dirty by checking the dirty flag. Topmost bit of background color code. */
  return (buffer->backgroundColorCodes[y][x] & 0xF0000000) != 0;
}
