#pragma once

#include <stddef.h>

struct slice {
    const char *buf;
    const char *end;
};

/// @return slice New slice describing buffer @c buf of length @c len.
struct slice slice_make(const char *buf, size_t len);

/// @return size_t Length of slice.
size_t slice_length(const struct slice slice);
