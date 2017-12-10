#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <voids.h>

#define VDS_ERR_FATAL(func, err) \
    func; \
    if (err != VDS_SUCCESS) { \
        printf("Function call \"%s\" failed with error \"%s\"\n", #func, VdsErrString(err)); \
        return 1; \
    }

int main(int argc, char *argv[])
{
    vdsErrCode err;
    RingBuffer *buff;

    VDS_ERR_FATAL(buff = RingBuffer_init(10, &err), err);

    srand(time(NULL));

    printf("buff->size = %lu\n", (unsigned  long)buff->size);
    for (int i = 0; i < 10; i++) {
        int *data = malloc(sizeof data);

        *data = (int)rand();
        printf("Pushing data %d!\n", *data);

        VDS_ERR_FATAL(RingBuffer_write(buff, data, &err), err);
    }

    putchar('\n');
    for (int i = 0; i < 4; i++) {
        int *data;

        VDS_ERR_FATAL(data = RingBuffer_read(buff, &err), err);

        if (data) {
            printf("Data %d popped!\n", *data);
            free(data);
        }
    }

    putchar('\n');
    for (int i = 0; i < 6; i++) {
        int *data;

        VDS_ERR_FATAL(data = RingBuffer_read(buff, &err), err);

        if (data) {
            printf("Data %d popped!\n", *data);
            free(data);
        }
    }

    if (!RingBuffer_read(buff, NULL)) {
        printf("No more data to read from the buffer!\n\n");
    }

    VDS_ERR_FATAL(RingBuffer_resize(&buff, 20, &err), err);
    for (int i = 0; i < 15; i++) {
        int *data = malloc(sizeof data);

        *data = (int)rand();
        printf("Pushing data %d!\n", *data);

        VDS_ERR_FATAL(RingBuffer_write(buff, data, &err), err);
    }

    putchar('\n');
    for (int i = 0; i < 17; i++) {
        int *data;

        VDS_ERR_FATAL(data = RingBuffer_read(buff, &err), err);

        if (data) {
            printf("Data %d popped!\n", *data);
            free(data);
        }
    }

    RingBuffer_destroy(&buff, NULL, &err);
    return 0;
}
