#include "storage.h"

#include <ctype.h>

static struct {
    struct token token[26];
} storage;

void storage_clear(void)
{
    for (size_t i = 0; i < sizeof(storage.token)/sizeof(*storage.token); ++i) {
        storage.token[i] = token_make_number(0);
    }
}

struct token storage_get(int c)
{
    if (!islower(c)) {
        return token_make_number(0);
    }
    return storage.token[c - 'a'];
}

void storage_set(int c, struct token token)
{
    if (!islower(c)) {
        return;
    }
    storage.token[c - 'a'] = token;
}
