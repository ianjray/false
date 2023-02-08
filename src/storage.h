#pragma once

#include "token.h"

/// Clear storage.
void storage_clear(void);

/// Get variable @c.
/// @return token Token.
struct token storage_get(int c);

/// Set variable @c c to token @c token.
void storage_set(int c, struct token token);
