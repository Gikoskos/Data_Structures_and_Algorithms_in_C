/***********************************************\
*               LinearHashtable.c               *
*           George Koskeridis (C) 2017          *
\***********************************************/

#include <stdlib.h>
#include "LinearHashtable.h"
#include "HashFunctions.h"


LinHashtable *LinHash_init(size_t size, UserCompareCallback KeyCmp, UserHashFuncCallback Hash)
{
    LinHashtable *lintable = NULL;

    if (KeyCmp && size > 0) {

        lintable = malloc(sizeof(LinHashtable));

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
        lintable->array = calloc(size, sizeof(HashArrayElement));
        lintable->size = size;
    }


    return lintable;
}

HashArrayElement *LinHash_insert(LinHashtable *table, void *pData, void *pKey, size_t key_size, UserDataCallback freeData)
{
    HashArrayElement *new_elem = NULL;

    if (table && pKey && key_size) {
        size_t hash = table->Hash(pKey, key_size, table->size);
        size_t item_idx = hash;

        do {
            //if we found an available position on the array, during linear probing, we store
            //our new key and data there, and save the pointer to that position to return it
            if (!table->array[item_idx].occupied) {

                //in case there is already a deleted element in the position that we're about to add
                //the new element to the array, we have to cleanup somehow
                if (table->array[item_idx].deleted && freeData)
                    freeData((void *)&table->array[item_idx].item);

                table->array[item_idx].item.pData = pData;
                table->array[item_idx].item.pKey = pKey;
                table->array[item_idx].occupied = 1;
                table->array[item_idx].deleted = 0;
                new_elem = &table->array[item_idx];
                break;

            } else if (!table->KeyCmp(table->array[item_idx].item.pKey, pKey)) {
                //if the same key is already in the table then the insertion has failed
                break;
            }

            //if we hit the last element of the array, during the linear probe, item_idx starts from 0
            item_idx = (item_idx == table->size - 1) ? 0 : (item_idx + 1);

        } while (item_idx != hash);
    }
        

    return new_elem;
}

KeyValuePair LinHash_delete(LinHashtable *table, void *pKey, size_t key_size)
{
    KeyValuePair item = { 0 };

    if (table && pKey && key_size) {
        size_t hash = table->Hash(pKey, key_size, table->size);
        size_t item_idx = hash;

        do {

            if (table->array[item_idx].occupied && table->KeyCmp(table->array[item_idx].item.pKey, pKey)) {
                table->array[item_idx].occupied = 0;
                table->array[item_idx].deleted = 1;
                item = table->array[item_idx].item;
                break;
            }

            item_idx = (item_idx == table->size - 1) ? 0 : (item_idx + 1);
        } while (item_idx != hash);
    }

    return item;
}

HashArrayElement *LinHash_find(LinHashtable *table, void *pKey, size_t key_size)
{
    HashArrayElement *to_find = NULL;

    if (table && pKey && key_size) {
        size_t hash = table->Hash(pKey, key_size, table->size);
        size_t item_idx = hash;

        do {

            if (!table->array[item_idx].deleted && 
                table->array[item_idx].occupied &&
                table->KeyCmp(table->array[item_idx].item.pKey, pKey)) {

                to_find = &table->array[item_idx];
                break;

            }

            item_idx = (item_idx == table->size - 1) ? 0 : (item_idx + 1);
        } while (item_idx != hash);

    }

    return to_find;
}

void LinHash_destroy(LinHashtable **table, UserDataCallback freeData)
{
    if (table && *table) {

        for (size_t i = 0; i < (*table)->size; i++)
            if (freeData && ((*table)->array[i].occupied || (*table)->array[i].deleted))
                freeData((void *)&(*table)->array[i].item);

        free((*table)->array);
        free(*table);
        *table = NULL;
    }
}
