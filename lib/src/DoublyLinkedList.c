 /********************
 *  DoublyLinkedList.c
 *
 * This file is part of libvoids which is licensed under the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#include "HeapAllocation.h"
#include "DoublyLinkedList.h"


DLListNode *DLList_insert(DLListNode **dllHead,
                          void *pData,
                          vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    DLListNode *new_node = NULL;

    if (dllHead) {
        new_node = VdsMalloc(sizeof(DLListNode));

        if (new_node) {
            new_node->pData = pData;
            new_node->prv = NULL;
            new_node->nxt = *dllHead;

            if (*dllHead)
                (*dllHead)->prv = new_node;

            *dllHead = new_node;
        } else
            tmp_err = VDS_MALLOC_FAIL;

    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

DLListNode *DLList_append(DLListNode **dllHead,
                          void *pData,
                          vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    DLListNode *new_node = NULL;

    if (dllHead) {
        new_node = VdsMalloc(sizeof(DLListNode));

        if (new_node) {
            new_node->pData = pData;
            new_node->nxt = new_node->prv = NULL;

            if (!(*dllHead)) {
                *dllHead = new_node;
            } else {
                DLListNode *curr;

                for (curr = *dllHead; curr->nxt; curr = curr->nxt);

                curr->nxt = new_node;
                new_node->prv = curr;
            }
        } else
            tmp_err = VDS_MALLOC_FAIL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

DLListNode *DLList_concat(DLListNode *dll_1,
                          DLListNode *dll_2,
                          vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;

    if (dll_1 && dll_2) {
        DLListNode *tail_1;

        for (tail_1 = dll_1; tail_1->nxt; tail_1 = tail_1->nxt);

        tail_1->nxt = dll_2;
        dll_2->prv = tail_1;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return dll_1;
}

DLListNode *DLList_insertAfter(DLListNode *dllPrev,
                               void *pData,
                               vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    DLListNode *new_node = NULL;

    if (dllPrev) {
        new_node = VdsMalloc(sizeof(DLListNode));

        if (new_node) {
            new_node->pData = pData;
            new_node->nxt = dllPrev->nxt;
            new_node->prv = dllPrev;

            dllPrev->nxt = new_node;
        } else
            tmp_err = VDS_MALLOC_FAIL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

void *DLList_deleteNode(DLListNode **dllHead,
                        DLListNode *dllToDelete,
                        vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_INVALID_ARGS;
    void *pRet = NULL;

    if (dllHead && *dllHead && dllToDelete) {
        DLListNode *curr;

        for (curr = *dllHead; curr && (curr != dllToDelete); curr = curr->nxt);

        if (curr) {
            pRet = curr->pData;
               
            if (curr->nxt)
                curr->nxt->prv = curr->prv;

            if (curr->prv)
                curr->prv->nxt = curr->nxt;
            else
                *dllHead = curr->nxt;

            VdsFree(curr);
            tmp_err = VDS_SUCCESS;
        }
    }

    SAVE_ERR(err, tmp_err);

    return pRet;
}

void *DLList_deleteData(DLListNode **dllHead,
                        void *pToDelete,
                        vdsUserCompareFunc DataCmp,
                        vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_INVALID_ARGS;
    void *pRet = NULL;

    if (dllHead && *dllHead && pToDelete) {
        DLListNode *curr;

        for (curr = *dllHead; curr && DataCmp(curr->pData, pToDelete); curr = curr->nxt);

        if (curr) {
            pRet = pToDelete;
               
            if (curr->nxt)
                curr->nxt->prv = curr->prv;

            if (curr->prv)
                curr->prv->nxt = curr->nxt;
            else
                *dllHead = curr->nxt;

            VdsFree(curr);
            tmp_err = VDS_SUCCESS;
        }
    }

    SAVE_ERR(err, tmp_err);

    return pRet;
}

DLListNode *DLList_at(DLListNode *dllHead,
                      size_t idx,
                      vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    DLListNode *curr = NULL;

    if (dllHead) {
        size_t i = 0;
        for (curr = dllHead; curr && i < idx; curr = curr->nxt, i++);
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return curr;
}

DLListNode *DLList_find(DLListNode *dllHead,
                        void *pToFind,
                        vdsUserCompareFunc DataCmp,
                        vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;
    DLListNode *curr = NULL;

    if (dllHead && DataCmp)
        for (curr = dllHead; curr && DataCmp(curr->pData, pToFind); curr = curr->nxt);
    else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return curr;
}

void DLList_traverse(DLListNode *dllHead,
                     vdsUserDataFunc handleData,
                     vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;

    if (dllHead && handleData)
        for (DLListNode *curr = dllHead; curr; curr = curr->nxt)
            handleData(curr->pData);
     else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}

void DLList_destroy(DLListNode **dllHead,
                    vdsUserDataFunc freeData,
                    vdsErrCode *err)
{
    vdsErrCode tmp_err = VDS_SUCCESS;

    if (dllHead) {
        DLListNode *curr, *tmp;

        for (curr = *dllHead; curr;) {
            if (freeData)
                freeData(curr->pData);

            tmp = curr;
            curr = curr->nxt;
            VdsFree(tmp);
        }

        *dllHead = NULL;
    } else
        tmp_err = VDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}
