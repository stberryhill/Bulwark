/*
 * License: Zlib
 * Copyright (c) Sean Tyler Berryhill (sean.tyler.berryhill@gmail.com)
 */

#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <string.h>

/* Private variables */
static BufferChangeList *bufferChangeList;

/* Private function declarations */
static BufferChangeList *createList();
static void freeListItem(BufferChangeListNode *node);
static BufferChangeListNode * getListHead(BufferChangeList *list);
static BufferChangeListNode *getLastNodeOfList(BufferChangeList *list);
static int getListSize(BufferChangeList *list);
static BufferChange *addChangeToList(BufferChangeList *list, const BufferChange change);
static void removeChangeFromList(BufferChangeListNode *node);

/* Function definitions */
void BufferChangeList_Initialize() {
    bufferChangeList = createList();
}

BufferChangeList *createList() {
    BufferChangeList *list = malloc(sizeof *list);
    list->head = 0;
    list->lastNode = 0;
    list->size = 0;

    return list;
}

void BufferChangeList_Destroy() {
    BufferChangeList_Clear();
    free(bufferChangeList);
}

void freeListItem(BufferChangeListNode *node) {
    free(node->data);
    free(node);
}

BufferChangeListNode *BufferChangeList_GetHead() {
    return getListHead(bufferChangeList);
}

BufferChangeListNode *getListHead(BufferChangeList *list) {
    return list->head;
}

BufferChangeListNode *BufferChangeList_GetLastNode() {
    return getLastNodeOfList(bufferChangeList);
}

BufferChangeListNode *getLastNodeOfList(BufferChangeList *list) {
    return list->lastNode;
}

int BufferChangeList_GetSize() {
    return getListSize(bufferChangeList);
}

int getListSize(BufferChangeList *list) {
    return list->size;
}

void BufferChangeList_Clear() {
    if (bufferChangeList) {
        BufferChangeListNode *node = bufferChangeList->head;
        int i;
        for (i = 0; i < bufferChangeList->size; i++) {
            const BufferChange *change = node->data;
            BufferChangeListNode *next = node->next;
            Buffer_ClearPendingChangeAtPosition(change->positionX, change->positionY);
            freeListItem(node);
            node = next;
        }

        bufferChangeList->lastNode = 0;
        bufferChangeList->size = 0;
    }
}

void BufferChangeList_AddChange(const BufferChange change) {
    const uint16_t x = change.positionX;
    const uint16_t y = change.positionY;

    if (Buffer_HasPendingChangeAtPosition(x, y)) {
        /* Merge change with already existing one */
        Log_Info("Merged");
        BufferChange *existingChange = Buffer_GetPendingChangeAtPosition(x, y)->data;
        existingChange->newCharacter = change.newCharacter;
        if (change.newBackgroundColor != CLEAR_COLOR_CODE) {
            existingChange->newBackgroundColor = change.newBackgroundColor;
        }
        if (change.newForegroundColor != CLEAR_COLOR_CODE) {
            existingChange->newForegroundColor = change.newForegroundColor;
        }

        if (Buffer_IsChangeRedundant(existingChange)) {
            /* Remove redundant change */
            Log_Info("Redundant change removed");
            removeChangeFromList(Buffer_GetPendingChangeAtPosition(x, y));
            Buffer_ClearPendingChangeAtPosition(x, y);
            Buffer_MarkUpToDateAtPosition(x, y);
        }
    } else {
        Log_Info("New Change added");
        addChangeToList(bufferChangeList, change);
        Buffer_MarkPendingChange(BufferChangeList_GetLastNode());
    }
}

BufferChange *addChangeToList(BufferChangeList *list, const BufferChange change) {

    BufferChangeListNode *newNode = malloc(sizeof *newNode);
    newNode->data = malloc(sizeof *newNode->data);
    memcpy(newNode->data, &change, sizeof(*newNode->data));
    newNode->next = 0;

    if (list->size == 0) {
        newNode->prev = 0;
        list->head = newNode;
        
    } else {
        BufferChangeListNode *node = list->head;
        while (node->next) {
            node = node->next;
        }

        newNode->prev = node;
        node->next = newNode;
    }

    list->lastNode = newNode;
    list->size++;

    return newNode->data;
}

void removeChangeFromList(BufferChangeListNode *node) {
    if (bufferChangeList->size == 0) {
        return;
    } else {
        BufferChangeListNode *next = node->next;
        BufferChangeListNode *prev = node->prev;
        BufferChange *data = node->data;

        Log_Info("remove char=%c, fg=%d, bg=%d, x=%d, y=%d", data->newCharacter, data->newForegroundColor, data->newBackgroundColor, data->positionX, data->positionY);

        freeListItem(node);

        if (node == bufferChangeList->head) {
            if (next) {
                next->prev = 0;
            }
            bufferChangeList->head = next;
        } else {
            prev->next = next;

            if (next) {
                next->prev = prev;
            }
        }

        bufferChangeList->size--;
    }
}
