 /********************
 *  LinearHashtable.c
 *
 * This file is part of EduDS data structure library which is licensed under
 * the 2-Clause BSD License
 *
 * Copyright (c) 2015, 2016, 2017 George Koskeridis <georgekoskerid@outlook.com>
 * All rights reserved.
  ***********************************************************************************/


#include "MemoryAllocation.h"
#include "LinearHashtable.h"
#include "HashFunctions.h"

#define SET_OCCUPIED(x) (x) = 1
#define SET_DELETED(x) (x) = 2

#define IS_OCCUPIED(x) ((x) == 1)
#define IS_DELETED(x) ((x) == 2)


static int rehash(LinHashtable *table, UserDataCallback EdsFreeData);


LinHashtable *LinHash_init(size_t size,
                           UserCompareCallback KeyCmp,
                           UserHashFuncCallback Hash,
                           int rehash,
                           EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    LinHashtable *lintable = NULL;

    if (KeyCmp && size > 3) {

        lintable = EdsMalloc(sizeof(LinHashtable));

        if (lintable) {

            lintable->array = EdsCalloc(size, sizeof(HashArrayElement));

            if (lintable->array) {
                //if the user didn't give a custom hashing algorithm, we default to either
                //the multiplication or division hashing methods
                if (!Hash) {

                    //if the array size is a power of two
                    if ( !(size & (size - 1)) ) //we default to mult
                        lintable->Hash = HashMult;
                    else
                        lintable->Hash = HashDiv; //else we default to div which works better when the array size isn't a power of two

                } else {
                    lintable->Hash = Hash;
                }

                lintable->KeyCmp = KeyCmp;
                lintable->size = size;
                lintable->rehash = rehash;
                lintable->total_elements = 0;

            } else {
                EdsFree(lintable);
                lintable = NULL;
                tmp_err = EDS_MALLOC_FAIL;
            }

        } else
            tmp_err = EDS_MALLOC_FAIL;
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);


    return lintable;
}

int rehash(LinHashtable *table, UserDataCallback EdsFreeData)
{
    HashArrayElement *old_array = table->array; //save the old array
    size_t old_size = table->size;

    table->size *= 2;

    //allocate the new array that has twice the size
    table->array = EdsCalloc(table->size, sizeof(HashArrayElement));
    if (!table->array) {
        table->array = old_array;
        return 0;
    }

    //loop through each element of the old array
    for (size_t i = 0; i < old_size; i++) {

        //hash each valid element of the old array, into the new array that has twice the size
        if (IS_OCCUPIED(old_array[i].state)) {

            //the hash index has changed because the array size has changed
            size_t hash_idx = table->Hash(old_array[i].key_hash, table->size);
            size_t offset = 0, tmp_idx;

            do {
                tmp_idx = (hash_idx + offset) % table->size;

                if (!IS_OCCUPIED(table->array[tmp_idx].state)) {

                    table->array[tmp_idx] = old_array[i];

                    break;
                }

                offset++;

            } while (offset < table->size);

            continue;
        }

        if (IS_DELETED(old_array[i].state) && EdsFreeData)
            EdsFreeData((void*)&old_array[i].item);

    }

    EdsFree(old_array);

    return 1;
}

void *LinHash_insert(LinHashtable *table,
                     void *pData,
                     void *pKey,
                     size_t key_size,
                     UserDataCallback EdsFreeData,
                     EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    void *new_key = NULL;

    if (table && pKey && key_size && (table->total_elements < table->size)) {

        size_t key_hash = HashCode(pKey, key_size);
        size_t hash_idx = table->Hash(key_hash, table->size);
        size_t offset = 0, tmp_idx;

        do {
            tmp_idx = (hash_idx + offset) % table->size;

            //if we found an available position on the array, during linear probing, we store
            //our new key and data there, and save the pointer to that position to return it
            if (!IS_OCCUPIED(table->array[tmp_idx].state)) {

                //in case there is already a deleted element in the position that we're about to add
                //the new element to the array, we have to cleanup with the user-supplied cleanup function
                if (IS_DELETED(table->array[tmp_idx].state) && EdsFreeData)
                    EdsFreeData((void *)&table->array[tmp_idx].item);

                table->array[tmp_idx].item.pData = pData;
                table->array[tmp_idx].item.pKey = pKey;
                SET_OCCUPIED(table->array[tmp_idx].state);
                //saving the pre-computed hashcode
                table->array[tmp_idx].key_hash = key_hash;

                table->total_elements++; //successful insertion
                new_key = pKey;
                break;

            } else if (!table->KeyCmp(table->array[tmp_idx].item.pKey, pKey)) {
                //if the same key is already in the table then the insertion has failed
                break;
            }

            offset++;

        } while (offset < table->size);

        //@TODO: handle possible case where insertion fails and we have to rehash the table
        //(if rehashing is enabled) when offset >= table->size and try to re-insert the
        //element with a recursive call to LinHash_insert

        if (table->rehash) //if the load factor is greater than 0.5 we rehash the table
            if ( ((float)table->total_elements / table->size) >= 0.5 )
                if (!rehash(table, EdsFreeData))
                    tmp_err = EDS_MALLOC_FAIL;

    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
        

    return new_key;
}

KeyValuePair LinHash_delete(LinHashtable *table,
                            void *pKey,
                            size_t key_size,
                            EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    KeyValuePair item = { 0 };

    if (table && pKey && key_size) {
        size_t hash_idx = table->Hash(HashCode(pKey, key_size), table->size);
        size_t offset = 0, tmp_idx;

        do {
            tmp_idx = (hash_idx + offset) % table->size;

            if (IS_OCCUPIED(table->array[tmp_idx].state) && table->KeyCmp(table->array[tmp_idx].item.pKey, pKey)) {

                SET_DELETED(table->array[tmp_idx].state);
                item = table->array[tmp_idx].item;

                table->total_elements--; //successful deletion
                break;

            }

            offset++;

        } while (offset < table->size);

    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return item;
}

HashArrayElement *LinHash_find(LinHashtable *table,
                               void *pKey,
                               size_t key_size,
                               EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;
    HashArrayElement *to_find = NULL;

    if (table && pKey && key_size) {
        size_t hash_idx = table->Hash(HashCode(pKey, key_size), table->size);
        size_t offset = 0, tmp_idx;

        do {
            tmp_idx = (hash_idx + offset) % table->size;

            if (IS_OCCUPIED(table->array[tmp_idx].state) &&
                !table->KeyCmp(table->array[tmp_idx].item.pKey, pKey)) {

                to_find = &table->array[tmp_idx];
                break;

            }

            offset++;

        } while (offset < table->size);

    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);

    return to_find;
}

void LinHash_destroy(LinHashtable **table,
                     UserDataCallback EdsFreeData,
                     EdsErrCode *err)
{
    EdsErrCode tmp_err = EDS_SUCCESS;

    if (table && *table) {

        for (size_t i = 0; i < (*table)->size; i++)
            if (EdsFreeData &&
                (IS_OCCUPIED((*table)->array[i].state) || IS_DELETED((*table)->array[i].state)))
                EdsFreeData((void *)&(*table)->array[i].item);

        EdsFree((*table)->array);
        EdsFree(*table);
        *table = NULL;
    } else
        tmp_err = EDS_INVALID_ARGS;

    SAVE_ERR(err, tmp_err);
}
