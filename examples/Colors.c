#include <Bulwark.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void drawString(const int xOffset, const int yOffset, const char *string) {
  Bulwark_ClearForegroundAndBackgroundColor();

  int i;
  for (i = 0; i < strlen(string); i++) {
    Bulwark_SetDrawPosition(xOffset + i, yOffset);
    Bulwark_DrawCharacter(string[i]);
  }
}

void draw16Colors(const int xOffset, const int yOffset) {
  int i;
  for (i = 0; i < 16; i++) {
    Bulwark_SetForegroundColor16(i);
    Bulwark_SetDrawPosition(xOffset + i, yOffset);
    Bulwark_DrawCharacter('#');
  }
}

void draw256Colors(const int xOffset, const int yOffset) {
  int i, k;
  for (k = 0; k < 16; k++) {
    for (i = 0; i < 16; i++) {
      const int color256 = (k * 16) + i;
      Bulwark_SetForegroundColor256(color256);
      Bulwark_SetDrawPosition(xOffset + i, k + yOffset);
      Bulwark_DrawCharacter('#');
    }
  }
}

int main() {
  Bulwark_Initialize();

  const int x = (Bulwark_GetWindowWidth() / 2) - 8;
  const int y = (Bulwark_GetWindowHeight() / 2) - 10;
  
  drawString(x, y, "256 color palette");
  draw256Colors(x, y + 1);
  drawString(x, y + 18, "16 color palette");
  draw16Colors(x, y + 19);

  BulwarkEvent *event = BulwarkEvent_Create();
  bool running = true;

  while (running) {
    Bulwark_WaitForNextEvent(event);
    
    if (BulwarkEvent_GetCharacter(event) == EOF) {
      running = false;
    }
  }

  Bulwark_Quit();

  return 0;
}