#include "Bulwark.h"
#include "Internal.h"

static const uint8_t CLEAR_COLOR_CODE = 0;
static const char CLEAR_CHARACTER = ' ';

static Buffer *buffer;

void Buffer_Initialize(const uint16_t width, const uint16_t height) {
  buffer = malloc(sizeof *buffer);

  buffer->characters = malloc(height * (sizeof buffer->characters));
  buffer->colorCodes = malloc(height * (sizeof buffer->characters));

  int j;
  for (j = 0; j < height; j++) {
    buffer->characters[j] = malloc(width * (sizeof *buffer->characters[j]));
    buffer->colorCodes[j] = malloc(width * (sizeof *buffer->characters[j]));

    int i;
    for (i = 0; i < width; i++) {
      buffer->characters[j][i] = CLEAR_CHARACTER;
      buffer->colorCodes[j][i] = CLEAR_COLOR_CODE;
    }
  }

  buffer->width = width;
  buffer->height = height;
}

void Buffer_Resize(const uint16_t width, const uint16_t height) {
  if (buffer->height > height) {
    int j;
    for (j = height; j < buffer->height; j++) {
      free(buffer->characters[j]);
    }

    /* TODO: reallloc topmost array to new size */
  } else if (buffer->height < height) {
    /* TODO: reallloc topmost array to new size */

    int j;
    for (j = buffer->height; j < height; j++) {
      int i;
      for (i = 0; i < buffer->width; i++) {
        buffer->characters[j][i] = CLEAR_CHARACTER;
        buffer->colorCodes[j][i] = CLEAR_COLOR_CODE;
      }
    }
  }

  buffer->width = width;
  buffer->height = height;
}

uint8_t Buffer_GetColorCodeAtPosition(const uint16_t x, const uint16_t y) {
  return buffer->colorCodes[y][x];
}

char Buffer_GetCharacterAtPosition(const uint16_t x, const uint16_t y) {
  return buffer->characters[y][x];
}

void Buffer_SetCharacterAndColorCodeAtPosition(const uint16_t x, const uint16_t y, const char character, const uint8_t colorCode) {
  if (x > buffer->width || y > buffer->height) {
    Log_Error("Buffer can't set character and color code at position because position is out of bounds: (%d, %d)", x, y);
    exit(-1);
  }

  buffer->characters[y][x] = character;
  buffer->colorCodes[y][x] = colorCode;
}
