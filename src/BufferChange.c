#include "Bulwark.h"
#include "Internal.h"

int BufferChange_GetStringSize(const BufferChange *change) {
  const int ansiSequenceBaseLength = 3;
  const int fgNumDigits = -1;
  const int foregroundStringSize = ( == BULWARK_COLOR_MODE_256) * (5 + fgNumDigits);
  const int backgroundStringSize = 5 + bgNumDigits;
  const int numOfSemicolonSeparators = 2;

  const int changeStringSize = ansiSequenceBaseLength + foregroundStringSize + backgroundStringSize + numOfSemicolonSeparators 
}
void BufferChange_Render(const BufferChange *change) {
  printf();
}
