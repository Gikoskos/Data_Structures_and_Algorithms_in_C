 /********************
 *  Treap.h
 *
 * This file is part of EduDS data structure library which is licensed under
 * the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#ifndef EduDS_TREAP_H__
#define EduDS_TREAP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "BinaryTreeHeap.h" //for HeapPropertyType
#include "RandomState.h"


typedef struct _TreapNode {
    KeyValuePair item;
    struct _TreapNode *left, *right, *parent;
    int priority; //heap priority for each node. randomized on the node's creation
} TreapNode;

typedef struct _Treap {
    TreapNode *root;
    void *rand_gen_state; //opaque type to store the state for the PRNG
    HeapPropertyType property;
    UserCompareCallback KeyCmp;
} Treap;


EduDS_API Treap *Treap_init(UserCompareCallback KeyCmp,
                            HeapPropertyType property,
                            unsigned int seed,
                            EdsErrCode *err);

EduDS_API TreapNode *Treap_insert(Treap *treap,
                                  void *pKey,
                                  void *pData,
                                  EdsErrCode *err);

EduDS_API KeyValuePair Treap_deleteNode(Treap *treap,
                                        TreapNode *treapToDelete,
                                        EdsErrCode *err);

EduDS_API KeyValuePair Treap_deleteByKey(Treap *treap,
                                         void *pKey,
                                         EdsErrCode *err);

EduDS_API TreapNode *Treap_findNode(Treap *treap,
                                    void *pKey,
                                    EdsErrCode *err);

EduDS_API void *Treap_findData(Treap *treap,
                               void *pKey,
                               EdsErrCode *err);

EduDS_API void Treap_traverse(Treap *treap,
                              TreeTraversalMethod traversal,
                              UserDataCallback callback,
                              EdsErrCode *err);

EduDS_API void Treap_destroy(Treap **treap,
                             UserDataCallback freeData,
                             EdsErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //EduDS_TREAP_H__
