/***********************************************\
*                 CircularBuffer.c              *
*           George Koskeridis (C) 2017          *
\***********************************************/

#include <stdlib.h>
#include <stdint.h> //for SIZE_MAX
#include "CircularBuffer.h"

CircularBuffer *newCircularBuffer(size_t buff_size)
{
    CircularBuffer *cBuffNew = NULL;

    if (buff_size) {

        cBuffNew = calloc(1, sizeof(CircularBuffer));

        cBuffNew->buff = malloc(sizeof(void*) * buff_size);

        cBuffNew->size = buff_size;

    }

    return cBuffNew;
}

void resizeCircularBuffer(CircularBuffer **cBuff, size_t new_size)
{
    if (new_size) {

        if (cBuff) {

            if (*cBuff) {

                //can only resize to a bigger buffer
                if ((*cBuff)->size < new_size) {
                    (*cBuff)->buff = realloc((*cBuff)->buff, sizeof(void*) * new_size);
                    (*cBuff)->size = new_size;
                }

            } else {
                *cBuff = newCircularBuffer(new_size);
            }

        }

    }
}

void pushCircularBuffer(CircularBuffer *cBuff, void *pData)
{
    if (cBuff) {

        cBuff->buff[cBuff->write] = pData;

        cBuff->write++;
        if (cBuff->write >= cBuff->size)
            cBuff->write = 0;

        //prevent overflow
        if (cBuff->available < SIZE_MAX)
            cBuff->available++;
    }
}
void *popCircularBuffer(CircularBuffer *cBuff)
{
    void *pData = NULL;

    if (cBuff) {

        if (cBuff->available) {
            pData = cBuff->buff[cBuff->read];

            cBuff->available--;
            cBuff->read++;
            if (cBuff->read >= cBuff->size)
                cBuff->read = 0;
        }

    }

    return pData;
}

void deleteCircularBuffer(CircularBuffer **cBuff, CustomDataCallback freeData)
{
    if (cBuff) {

        if (*cBuff) {

            if (freeData) {
                for (size_t i = 0; i <= (*cBuff)->size; i++)
                    freeData((*cBuff)->buff[i]);
            }

            free((*cBuff)->buff);
            free(*cBuff);

            *cBuff = NULL;

        }

    }
}
