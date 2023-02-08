#include "slice.h"

struct slice slice_make(const char *buf, size_t len)
{
    struct slice s = { NULL, NULL };
    if (buf) {
        s.buf = buf;
        s.end = buf + len;
    }
    return s;
}

size_t slice_length(const struct slice slice)
{
    return (size_t)(slice.end - slice.buf);
}
