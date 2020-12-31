#include "Bulwark.h"
#include "Internal.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <poll.h>

#define INPUT_BUFFER_SIZE_BYTES 8

/* Function definitions */
bool Bulwark_HasEventsInQueue() {
  return !EventQueue_IsEmpty();
}

void Bulwark_ReadNextEventInQueue(BulwarkEvent *output) {
  EventQueue_ReadAndConsumeEvent(output);
}

void Bulwark_WaitForNextEvent(BulwarkEvent *output) {
  BulwarkEvent event;

  if (read(STDIN_FILENO, &event.character, 1) < 0) {
    Log_Error("Could not read character from STDIN_FILENO");
    exit(EXIT_FAILURE);
  }

  event.type = BULWARK_EVENT_TYPE_INPUT;
  EventQueue_AddEvent(&event);
  output->character = event.character;
  output->type = event.type;
}

void Bulwark_PollEvents() {
  struct pollfd fds;
  fds.fd = STDIN_FILENO;
  fds.events = POLLIN; /* POLLIN => There is data to read. */
  const int numFileDescriptors = 1;
  const int TIMEOUT = 0;
  char buffer[INPUT_BUFFER_SIZE_BYTES + 1] = "";

  const int pollReturn = poll(&fds, numFileDescriptors, TIMEOUT);
  if (pollReturn < 0) {
    Log_Error("Could not read character from STDIN_FILENO");
    exit(EXIT_FAILURE);
  } else if (pollReturn > 0) {
    if (fds.revents & POLLIN) {
      const ssize_t bytesRead = read(fds.fd, buffer, INPUT_BUFFER_SIZE_BYTES);
      if (bytesRead < 0) {
        Log_Error("Could not read input from poll.");
        exit(EXIT_FAILURE);
      }
      
      int i;
      for (i = 0; i < bytesRead; i++) {
        BulwarkEvent event;
        event.type = BULWARK_EVENT_TYPE_INPUT;
        event.character = buffer[i];
        EventQueue_AddEvent(&event);
      }
    }
  }
}
