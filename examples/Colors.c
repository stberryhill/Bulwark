#include <Bulwark.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void drawString(const int xOffset, const int yOffset, const char *string) {
  Bulwark_ClearForegroundAndBackgroundColor();

  int i;
  for (i = 0; i < strlen(string); i++) {
    Bulwark_Immediate_SetDrawPosition(xOffset + i, yOffset);
    Bulwark_Immediate_DrawCharacter(string[i]);
  }
}

void draw16Colors(const int xOffset, const int yOffset) {
  int i;
  int colorCode = 0;
  int y;
  int lineY = yOffset;
  for (y = 0; y < 2; y++) {
    for (i = 0; i < 8 * 3; i += 3) {
      BulwarkColor color;
      color.mode = BULWARK_COLOR_MODE_16;
      color.color16 = colorCode;
      Bulwark_Immediate_SetForegroundColor(&color);

      
      Bulwark_Immediate_SetDrawPosition(xOffset + i + 2, lineY);
      Bulwark_Immediate_DrawCharacter('#');


      const int codeOffset = colorCode < 10 ? 2 : 1;
      Bulwark_Immediate_SetDrawPosition(xOffset + i + codeOffset, lineY + 1);
      char colorString[3] = "";
      sprintf(colorString, "%d", colorCode);
      Bulwark_Immediate_DrawString(colorString);
      colorCode++;
    }
    lineY += 3;
  }
}

void draw256Colors(const int xOffset, const int yOffset) {
  int i, k;
  for (k = 0; k < 16; k++) {
    for (i = 0; i < 16; i++) {
      const int color256 = (k * 16) + i;
      BulwarkColor color;
      color.mode = BULWARK_COLOR_MODE_256;
      color.color256 = color256;
      Bulwark_Immediate_SetForegroundColor(&color);
      Bulwark_Immediate_SetDrawPosition(xOffset + i, k + yOffset);
      Bulwark_Immediate_DrawCharacter('#');
    }
  }
}

int main() {
  Bulwark_Initialize();

  const int x = (Bulwark_GetWindowWidth() / 2) - 8;
  const int color16X = (Bulwark_GetWindowWidth() / 2) - 12;
  const int y = (Bulwark_GetWindowHeight() / 2) - 14;
  
  drawString(x, y, "256 color palette");
  draw256Colors(x, y + 1);
  drawString(color16X, y + 18, "16 color palette");
  draw16Colors(color16X, y + 19);

  BulwarkEvent *event = BulwarkEvent_Create();
  bool running = true;

  while (running) {
    Bulwark_WaitForNextEvent(event);
    
    if (BulwarkEvent_GetCharacter(event) == ' ') {
      running = false;
    }
  }

  Bulwark_Quit();

  return 0;
}