#include "file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int file_read_fully(const char* path, char **bufp)
{
    FILE *fp;
    size_t sz;
    int ret = 0;
    char *buffer;

    if (!path || !bufp) {
        return -EINVAL;
    }

    fp = fopen(path, "rb");
    if (!fp) {
	return -errno;
    }

    fseek(fp, 0L, SEEK_END);
    sz = (size_t)ftell(fp);
    rewind(fp);

    buffer = (char*)malloc(sz + 1);
    if (!buffer) {
        ret = -errno;

    } else {
        size_t n = fread(buffer, sizeof(char), sz, fp);
        if (n != sz) {
            ret = -errno;
            free(buffer);

        } else {
            buffer[n] = 0;
        }
    }

    fclose(fp);
    *bufp = buffer;
    return ret;
}
