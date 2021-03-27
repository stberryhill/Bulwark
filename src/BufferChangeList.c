#include "Bulwark.h"
#include "Internal.h"

#include <stdlib.h>
#include <string.h>

/* Private variables */
static BufferChangeList *bufferChangeList;

/* Private function declarations */
static BufferChangeList *createList();
static void freeListItems(BufferChangeList *list);
static BufferChangeListNode * getListHead(BufferChangeList *list);
static BufferChangeListNode *getLastNodeOfList(BufferChangeList *list);
static int getListSize(BufferChangeList *list);
static BufferChange *addChangeToList(BufferChangeList *list, const BufferChange change);

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
    freeListItems(bufferChangeList);
    free(bufferChangeList);
}

void freeListItems(BufferChangeList *list) {
    if (list) {
        BufferChangeListNode *n = list->head;
        int i;
        for (i = 0; i < list->size; i++) {
            BufferChangeListNode *next = n->next;
            free(n->data);
            free(n);
            n = next;
        }
    }
    
    list->lastNode = 0;
    list->size = 0;
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
    freeListItems(bufferChangeList);
}

void BufferChangeList_AddChange(const BufferChange change) {
    addChangeToList(bufferChangeList, change);
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
