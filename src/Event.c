/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

#include "Bulwark.h"
#include "Internal.h"
#include <stdlib.h>

BulwarkEvent *BulwarkEvent_Create() {
  BulwarkEvent *event = malloc(sizeof *event);
  return event;
}

void BulwarkEvent_Destroy(BulwarkEvent *event) {
  free(event);
}

BulwarkEventType BulwarkEvent_GetType(BulwarkEvent *event) {
  return event->type;
}

char BulwarkEvent_GetCharacter(BulwarkEvent *event) {
  return event->character;
}