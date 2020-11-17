#include <KibiTest.h>

#include <stdio.h>
#include <stdlib.h>
#include "../Bulwark.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>

#define MAX_OUTPUT_LENGTH 256

#define interceptOutput(functionCallWithParameters, outputString) ({\
  redirectStdoutToOutputFile();\
  functionCallWithParameters;\
  restoreStdoutToNormal();\
  strcpy(outputString, readLatestFromOutputFile());\
})

const char OUTPUT_FILE_NAME[] = "Output.txt";
const char ANSI_ESCAPE_CODE = 27;
int originalStdout;

static int equal(const char string1[], const char string2[]) {
  return strcmp(string1, string2) == 0;
}

/*static void duplicateFileDescriptor(int source, int destination) {
  if (dup2(source, destination) < 0) {
    printf("Error - could not duplicate file descriptor %d to %d.\n", source, destination);
    exit(EXIT_FAILURE);
  }
}*/

static void redirectStdoutToOutputFile() {
  int outputFile = open(OUTPUT_FILE_NAME, O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
  if (outputFile < 0) {
    printf("Error - could not open output file for redirecting.\n");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }

  fflush(stdout);
  originalStdout = dup(STDOUT_FILENO);
  close(STDOUT_FILENO);
  dup(outputFile);
  close(outputFile);
}

static void afterAllCleanup() {
  /* Delete output file */
  unlink(OUTPUT_FILE_NAME);
}

static void restoreStdoutToNormal() {
  fflush(stdout);
  close(STDOUT_FILENO);
  dup(originalStdout);
  close(originalStdout);
}

static void clearFileThatsAlreadyOpen(FILE *filePointer) {
  const char OVERWRITE_MODE[] = "w";
  filePointer = freopen(NULL, OVERWRITE_MODE, filePointer);
}


static const char *readLatestFromOutputFile() {
  const char READ_MODE[] = "r";
  static char outputContents[MAX_OUTPUT_LENGTH];

  /* Clear old output */
  int i;
  for (i = 0; i < MAX_OUTPUT_LENGTH; i++) { outputContents[i] = '\0'; }

  /* Read new output */
  FILE *filePointer;
  if ((filePointer = fopen(OUTPUT_FILE_NAME, READ_MODE)) == NULL) {
    printf("Error - could not open output file for reading.");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }

  fgets(outputContents, MAX_OUTPUT_LENGTH, filePointer); /* Read everything in file */
  clearFileThatsAlreadyOpen(filePointer); /* Clear before next output comes */
  fclose(filePointer);

  return outputContents;
}

void setDrawPosition() {
  int POSITION_X = 8;
  int POSITION_Y = 16;
  char expected[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d;%dH", ANSI_ESCAPE_CODE, POSITION_Y + 1, POSITION_X + 1);

  redirectStdoutToOutputFile();
  Bulwark_SetDrawPosition(POSITION_X, POSITION_Y);
  restoreStdoutToNormal();

  Kibi_AssertTrue(equal(readLatestFromOutputFile(), expected));
}

void setBackgroundColor16() {
  const uint8 COLOR16 = 8;
  const uint8 brightnessSpecifier = 10;
  const uint8 colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%dm", ANSI_ESCAPE_CODE, brightnessSpecifier, colorSpecifier);

  interceptOutput(Bulwark_SetBackgroundColor16(COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundColor16() {
  const uint8 COLOR16 = 8;
  const uint8 brightnessSpecifier = 9;
  const uint8 colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%dm", ANSI_ESCAPE_CODE, brightnessSpecifier, colorSpecifier);

  interceptOutput(Bulwark_SetForegroundColor16(COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundAndBackgroundColors16() {
  const uint8 COLOR16 = 8;
  const uint8 foregroundBrightness = 9;
  const uint8 backgroundBrightness = 10;
  const uint8 colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%d;%d%dm", ANSI_ESCAPE_CODE, foregroundBrightness, colorSpecifier, backgroundBrightness, colorSpecifier);

  interceptOutput(Bulwark_SetForegroundAndBackgroundColors16(COLOR16, COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void drawCharacter() {
  const char expected = 'c';
  char actual[2] = "";

  interceptOutput(Bulwark_DrawCharacter(expected), actual);

  Kibi_AssertTrue(expected == actual[0]);
}

void drawString() {
  const char *expected = "string";
  char actual[MAX_OUTPUT_LENGTH] = "";

  interceptOutput(Bulwark_DrawString(expected), actual);

  Kibi_AssertTrue(equal(expected, actual));
}

void playBellSound() {
  const char expected = '\007';
  char actual[2] = "";

  interceptOutput(Bulwark_PlayBellSound(), actual);

  Kibi_AssertTrue(expected == actual[0]);
}

int main() {

  Kibi_Test(setDrawPosition);
  Kibi_Test(setBackgroundColor16);
  Kibi_Test(setForegroundColor16);
  Kibi_Test(setForegroundAndBackgroundColors16);
  Kibi_Test(drawCharacter);
  Kibi_Test(drawString);
  /*Kibi_Test(playBellSound);*/

  afterAllCleanup();
  
  return 0;
}