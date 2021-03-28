#include <KibiTest.h>

#include <stdio.h>
#include <stdlib.h>
#include <Bulwark.h>
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
  if (dup(outputFile) < 0) {
    printf("Error - could not duplicate output file descriptor.\n");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }
  close(outputFile);
}

static void afterAllCleanup() {
  /* Delete output file */
  unlink(OUTPUT_FILE_NAME);
}

static void restoreStdoutToNormal() {
  fflush(stdout);
  close(STDOUT_FILENO);
  if (dup(originalStdout) < 0) {
    printf("Error - could not duplicate output file descriptor.\n");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }
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

  /* Read everything in file */
  if (fgets(outputContents, MAX_OUTPUT_LENGTH, filePointer) == NULL) {
    printf("Error - could not read output file contents.\n");
    fflush(stdout);
    exit(EXIT_FAILURE);
  }
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
  Bulwark_Immediate_SetDrawPosition(POSITION_X, POSITION_Y);
  restoreStdoutToNormal();

  Kibi_AssertTrue(equal(readLatestFromOutputFile(), expected));
}

void setBackgroundColor16() {
  const int COLOR16 = 8;
  const int brightnessSpecifier = 10;
  const int colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%dm", ANSI_ESCAPE_CODE, brightnessSpecifier, colorSpecifier);

  interceptOutput(Bulwark_Immediate_SetBackgroundColor16(COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundColor16() {
  const int COLOR16 = 8;
  const int brightnessSpecifier = 9;
  const int colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%dm", ANSI_ESCAPE_CODE, brightnessSpecifier, colorSpecifier);

  interceptOutput(Bulwark_Immediate_SetForegroundColor16(COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundAndBackgroundColor16() {
  const int COLOR16 = 8;
  const int foregroundBrightness = 9;
  const int backgroundBrightness = 10;
  const int colorSpecifier = 0;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%d%d;%d%dm", ANSI_ESCAPE_CODE, foregroundBrightness, colorSpecifier, backgroundBrightness, colorSpecifier);

  interceptOutput(Bulwark_Immediate_SetForegroundAndBackgroundColor16(COLOR16, COLOR16), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundColor256() {
  const int COLOR256 = 128;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[38;5;%dm", ANSI_ESCAPE_CODE, COLOR256);

  interceptOutput(Bulwark_Immediate_SetForegroundColor256(COLOR256), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setBackgroundColor256() {
  const int COLOR256 = 128;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[48;5;%dm", ANSI_ESCAPE_CODE, COLOR256);

  interceptOutput(Bulwark_Immediate_SetBackgroundColor256(COLOR256), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void setForegroundAndBackgroundColor256() {
  const int BACKGROUND256 = 128;
  const int FOREGROUND256 = 255;
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[38;5;%d;48;5;%dm", ANSI_ESCAPE_CODE, FOREGROUND256, BACKGROUND256);

  interceptOutput(Bulwark_Immediate_SetForegroundAndBackgroundColor256(FOREGROUND256, BACKGROUND256), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void clearForegroundAndBackgroundColor() {
  char expected[MAX_OUTPUT_LENGTH] = "";
  char actual[MAX_OUTPUT_LENGTH] = "";
  sprintf(expected, "%c[%dm", ANSI_ESCAPE_CODE, 0);

  interceptOutput(Bulwark_Immediate_ClearForegroundAndBackgroundColor(), actual);

  Kibi_AssertTrue(equal(actual, expected));
}

void drawCharacter() {
  const char expected = 'c';
  char actual[2] = "";

  interceptOutput(Bulwark_Immediate_DrawCharacter(expected), actual);

  Kibi_AssertTrue(expected == actual[0]);
}

void drawString() {
  const char *expected = "string";
  char actual[MAX_OUTPUT_LENGTH] = "";

  interceptOutput(Bulwark_Immediate_DrawString(expected), actual);

  Kibi_AssertTrue(equal(expected, actual));
}

int main() {

  Kibi_Test(setDrawPosition);
  Kibi_Test(setForegroundColor16);
  Kibi_Test(setBackgroundColor16);
  Kibi_Test(setForegroundAndBackgroundColor16);
  Kibi_Test(setForegroundColor256);
  Kibi_Test(setBackgroundColor256);
  Kibi_Test(setForegroundAndBackgroundColor256);
  Kibi_Test(clearForegroundAndBackgroundColor);
  Kibi_Test(drawCharacter);
  Kibi_Test(drawString);

  afterAllCleanup();
  
  return 0;
}
