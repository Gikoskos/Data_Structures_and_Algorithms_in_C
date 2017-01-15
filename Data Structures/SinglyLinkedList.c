/***********************************************\
*              SinglyLinkedList.c               *
*           George Koskeridis (C) 2016          *
\***********************************************/

#include <stdlib.h>
#include "SinglyLinkedList.h"

SLListNode *insertNodeSLList(SLListNode **sllHead, void *pData)
{
    SLListNode *new_node = NULL;

    if (sllHead) {
        new_node = malloc(sizeof(SLListNode));

        new_node->pData = pData;
        new_node->nxt = *sllHead;

        *sllHead = new_node;
    }

    return new_node;
}

SLListNode *appendNodeSLList(SLListNode **sllHead, void *pData)
{
    SLListNode *new_node = NULL;

    if (sllHead) {
        new_node = malloc(sizeof(SLListNode));

        new_node->pData = pData;
        new_node->nxt = NULL;

        if (!(*sllHead)) {
            *sllHead = new_node;
        } else {
            SLListNode *curr;

            for (curr = *sllHead; curr->nxt; curr = curr->nxt);

            curr->nxt = new_node;
        }
    }

    return new_node;
}

void *deleteNodeSLList(SLListNode **sllHead, SLListNode *sllToDelete)
{
    void *pRet = NULL;

    if (sllHead && sllToDelete) {
        SLListNode *curr, *prev = NULL;

        for (curr = *sllHead; curr && (curr != sllToDelete); curr = curr->nxt)
            prev = curr;

        if (curr) {
            pRet = curr->pData;

            if (prev) {
                prev->nxt = curr->nxt;
            } else {
                *sllHead = curr->nxt;
            }

            free(curr);
        }
    }

    return pRet;
}

SLListNode *findNodeSLList(SLListNode *sllHead, void *pToFind)
{
    SLListNode *curr;

    for (curr = sllHead; curr && (curr->pData != pToFind); curr = curr->nxt);

    return curr;
}

void printSLList(SLListNode *sllHead, CustomDataCallback printData)
{
    for (SLListNode *curr = sllHead; curr; curr = curr->nxt)
        printData(curr->pData);
}

void deleteSLList(SLListNode **sllHead, CustomDataCallback freeData)
{
    if (sllHead) {
        SLListNode *curr, *tmp;

        for (curr = *sllHead; curr;) {
            if (freeData)
                freeData(curr->pData);

            tmp = curr;
            curr = curr->nxt;
            free(tmp);
        }

        *sllHead = NULL;
    }
}
