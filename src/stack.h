#pragma once

#include "slice.h"
#include "token.h"

#include <stdbool.h>

/// Initialise stack.
void stack_init(void (*fatal)(const char *msg), void (*log)(const char *op, const char *dump));

/// @return bool True if stack is empty.
bool stack_empty(void);

/// @return size_t Stack size.
size_t stack_size(void);

/// Push @c token.
void stack_push(struct token token);

/// Duplicate top of stack.
/// @note Calls @c fatal on underflow.
void stack_dup(void);

/// Drop top of stack.
/// @note Calls @c fatal on underflow.
void stack_drop(void);

/// Swap top two elements.
/// @note Calls @c fatal on underflow.
void stack_swap(void);

/// Rotate top three elements.
/// @note Calls @c fatal on underflow.
void stack_rot(void);

/// Duplicate second item on stack.
/// @note Calls @c fatal on underflow.
void stack_over(void);

/// Drop second item on stack.
/// @note Calls @c fatal on underflow.
void stack_nip(void);

/// Insert a copy of the top value into the stack two values from the top.
/// @note Calls @c fatal on underflow.
void stack_tuck(void);

//// Duplicate the top two stack items.
/// @note Calls @c fatal on underflow.
void stack_2dup(void);

/// Pick element @c n.
/// @note Calls @c fatal on underflow.
void stack_pick(size_t n);

/// Pop.
/// @note Calls @c fatal on underflow.
/// @return token Token.
struct token stack_pop(void);

/// Pop number.
/// @note Calls @c fatal if top of stack is the wrong type.
/// @return int Number.
int stack_pop_number(void);

/// Pop variable.
/// @note Calls @c fatal if top of stack is the wrong type.
/// @return int Variable.
int stack_pop_variable(void);

/// Pop lambda.
/// @note Calls @c fatal if top of stack is the wrong type.
/// @return slice Lambda.
struct slice stack_pop_lambda(void);
