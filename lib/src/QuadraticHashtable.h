/***********************************************\
*              QuadraticHashtable.h             *
*           George Koskeridis (C) 2017          *
\***********************************************/

#ifndef __EduDS_QUADRATICHASHTABLE_H
#define __EduDS_QUADRATICHASHTABLE_H


#ifdef __cplusplus
extern "C" {
#endif

#include "Common.h"
#include "LinearHashtable.h" //for HashArrayElement


typedef struct _QuadHashtable {
    HashArrayElement *array;
    UserCompareCallback KeyCmp;
    UserHashFuncCallback Hash;
    size_t size;
    size_t total_elements;
} QuadHashtable;


EduDS_API QuadHashtable *QuadHash_init(size_t size,
                                       UserCompareCallback KeyCmp,
                                       UserHashFuncCallback Hash,
                                       EduDSErrCode *err);

EduDS_API void *QuadHash_insert(QuadHashtable *table,
                                void *pData,
                                void *pKey,
                                size_t key_size,
                                UserDataCallback freeData,
                                EduDSErrCode *err);

EduDS_API KeyValuePair QuadHash_delete(QuadHashtable *table,
                                       void *pKey,
                                       size_t key_size,
                                       EduDSErrCode *err);

EduDS_API HashArrayElement *QuadHash_find(QuadHashtable *table,
                                          void *pKey,
                                          size_t key_size,
                                          EduDSErrCode *err);

EduDS_API void QuadHash_destroy(QuadHashtable **table,
                                UserDataCallback freeData,
                                EduDSErrCode *err);

#ifdef __cplusplus
}
#endif

#endif //__EduDS_QUADRATICHASHTABLE_H
