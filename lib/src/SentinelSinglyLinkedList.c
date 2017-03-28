 /********************
 *  SentinelSinglyLinkedList.c
 *
 * This file is part of EduDS data structure library which is licensed under
 * the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#include "MemoryAllocation.h"
#include "SentinelSinglyLinkedList.h"


SSLList *SSLList_init(EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;

    SSLList *newList = EdsMalloc(sizeof(SSLList));

    if (newList) {

        newList->head = EdsMalloc(sizeof(SLListNode));

        if (newList->head) {
            newList->sentinel = newList->head;
        } else {
            EdsFree(newList);
            newList = NULL;
            tmp_err = EDS_MALLOC_FAIL;
        }

    } else
        tmp_err = EDS_MALLOC_FAIL;

    SAVE_ERR(err, tmp_err);

    return newList;
}

SLListNode *SSLList_insert(SSLList *ssllList,
                           void *pData,
                           EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    SLListNode *new_node = NULL;

    if (ssllList) {

        new_node = EdsMalloc(sizeof(SLListNode));

        if (new_node) {

            new_node->pData = pData;

            new_node->nxt = ssllList->head;
            ssllList->head = new_node;

        } else
            tmp_err = EDS_MALLOC_FAIL;

    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

SLListNode *SSLList_append(SSLList *ssllList,
                           void *pData,
                           EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    SLListNode *new_node = NULL;

    if (ssllList) {
        if (ssllList->sentinel == ssllList->head) {

            new_node = SSLList_insert(ssllList, pData, &tmp_err);
            
        } else {
            SLListNode *curr;

            new_node = EdsMalloc(sizeof(SLListNode));

            if (new_node) {

                new_node->pData = pData;
                new_node->nxt = ssllList->sentinel;

                for (curr = ssllList->head; curr->nxt != ssllList->sentinel; curr = curr->nxt);

                curr->nxt = new_node;

            } else
                tmp_err = EDS_MALLOC_FAIL;
        }
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return new_node;
}

SSLList *SSLList_concat(SSLList *ssll1,
                        SSLList *ssll2,
                        EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    SSLList *ret = NULL;

    if (ssll1 && ssll2) {
        if (ssll1->sentinel != ssll1->head && ssll2->sentinel != ssll2->head) {

            SLListNode *tail1;

            for (tail1 = ssll1->head; tail1->nxt != ssll1->sentinel; tail1 = tail1->nxt);

            tail1->nxt = ssll2->head;

            //order of EdsFree()'s is important here
            EdsFree(ssll1->sentinel);

            ssll1->sentinel = ssll2->sentinel;

            EdsFree(ssll2);

            ret = ssll1;
        }
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return ret;
}

void *SSLList_deleteNode(SSLList *ssllList,
                         SLListNode *sllToDelete,
                         EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_INVALID_ARGS;
    void *pRet = NULL;

    if (ssllList && sllToDelete) {
        SLListNode *curr, *prev = NULL;

        ssllList->sentinel->pData = NULL;

        for (curr = ssllList->head; curr->pData && curr != sllToDelete; curr = curr->nxt)
            prev = curr;

        if (curr != ssllList->sentinel) {
            pRet = sllToDelete->pData;

            if (prev)
                prev->nxt = curr->nxt;
            else
                ssllList->head = curr->nxt;

            EdsFree(curr);
            tmp_err = EDS_SUCCESS;
        }
    }

    SAVE_ERR(err, tmp_err);

    return pRet;
}

void *SSLList_deleteData(SSLList *ssllList,
                         void *pData,
                         UserCompareCallback DataCmp,
                         EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_INVALID_ARGS;
    void *pRet = NULL;

    if (ssllList && DataCmp) {
        SLListNode *curr, *prev = NULL;

        ssllList->sentinel->pData = pData;
        for (curr = ssllList->head; DataCmp(curr->pData, pData); curr = curr->nxt)
            prev = curr;

        if (curr != ssllList->sentinel) {
            pRet = pData;

            if (prev)
                prev->nxt = curr->nxt;
            else
                ssllList->head = curr->nxt;

            EdsFree(curr);
            tmp_err = EDS_SUCCESS;
        }
    }

    SAVE_ERR(err, tmp_err);

    return pRet;
}

SLListNode *SSLList_at(SSLList *ssllList,
                       size_t idx,
                       EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    SLListNode *ret = NULL;

    if (ssllList) {
        SLListNode *curr;
        size_t i = 0;

        ssllList->sentinel->pData = NULL;
        for (curr = ssllList->head; curr && i < idx; curr = curr->nxt, i++);

        ret = (curr != ssllList->sentinel) ? curr : NULL;
            
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return ret;
}

SLListNode *SSLList_find(SSLList *ssllList,
                         void *pToFind,
                         UserCompareCallback DataCmp,
                         EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    SLListNode *curr = NULL;

    if (ssllList && DataCmp) {
        if (ssllList->sentinel != ssllList->head) {

            ssllList->sentinel->pData = pToFind;
            for (curr = ssllList->head; DataCmp(curr->pData, pToFind); curr = curr->nxt);

            if (curr == ssllList->sentinel)
                curr = NULL;
        }
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return curr;
}

void SSLList_traverse(SSLList *ssllList,
                      UserDataCallback handleData,
                      EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;

    if (ssllList && handleData && (ssllList->head != ssllList->sentinel))
        for (SLListNode *curr = ssllList->head; curr != ssllList->sentinel; curr = curr->nxt)
            handleData(curr->pData);
    else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}

void SSLList_destroy(SSLList **ssllList,
                     UserDataCallback EdsFreeData,
                     EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;

    if (ssllList && *ssllList) {

        SLListNode *curr, *tmp;

        for (curr = (*ssllList)->head; curr != (*ssllList)->sentinel;) {
            if (EdsFreeData)
                EdsFreeData(curr->pData);

            tmp = curr;
            curr = curr->nxt;
            EdsFree(tmp);
        }

        EdsFree((*ssllList)->sentinel);
        EdsFree(*ssllList);
        *ssllList = NULL;

    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}
