#pragma once

#include "token.h"

/// Clear storage.
void storage_clear(void);

/// Set variable @c c to token @c token.
void storage_set(int c, struct token token);

/// Get variable @c.
/// @return token Token.
struct token storage_get(int c);
