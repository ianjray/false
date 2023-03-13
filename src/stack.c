#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
    struct token *stack;
    size_t depth;
    void (*fatal)(const char *msg);
    void (*log)(const char *op, const char *dump);
} stack_;

void stack_init(void (*fatal)(const char *msg), void (*log)(const char *op, const char *dump))
{
    stack_.depth = 0;
    free(stack_.stack);
    stack_.stack = NULL;
    stack_.fatal = fatal;
    stack_.log = log;
}

bool stack_empty(void)
{
    return stack_.depth == 0;
}

size_t stack_size(void)
{
    return stack_.depth;
}

/// Log stack operation.
static void slog(const char *op, char *rhs)
{
    if (!stack_.log) {
        free(rhs);
        return;
    }

    if (rhs) {
        if (*rhs == '[' && strlen(rhs) > 4) {
            // Concise format.
            strcpy(rhs, "[...");
        }
    } else {
        rhs = strdup("");
    }

    rhs = (char *)realloc(rhs, strlen(rhs) + 1 /*TAB*/ + 1 /*NUL*/);
    strcat(rhs, "\t");

    for (size_t i = 0; i < stack_.depth; ++i) {
        char *buf = token_print(stack_.stack[i]);
        rhs = (char *)realloc(rhs, strlen(rhs) + 1 /*SPC*/ + strlen(buf) + 1 /*NUL*/);
        strcat(rhs, " ");
        strcat(rhs, buf);
        free(buf);
    }

    stack_.log(op, rhs);
    free(rhs);
}

/// Push @c token.
static void push(struct token token)
{
    stack_.stack = (struct token *)realloc(stack_.stack, (stack_.depth + 1) * sizeof(struct token));
    stack_.stack[stack_.depth++] = token;
}

/// Test that stack contains at least @c n elements.
static void require(size_t n)
{
    if (stack_.depth < n) {
        stack_.fatal("stack underflow");
    }
}

/// Duplicate top of stack.
static void dup(void)
{
    require(1);
    push(stack_.stack[stack_.depth - 1]);
}

/// Pop token.
static struct token pop(void)
{
    require(1);
    return stack_.stack[--stack_.depth];
}

/// Rotate top three elements.
static void rot(void)
{
    struct token x2 = pop();
    struct token x1 = pop();
    struct token x = pop();
    push(x1);
    push(x2);
    push(x);
}

/// Swap top two elements.
static void swap(void)
{
    struct token x1 = pop();
    struct token x = pop();
    push(x1);
    push(x);
}

void stack_push(struct token token)
{
    push(token);
    slog("push", token_print(token));
}

void stack_dup(void)
{
    dup();
    slog("dup", NULL);
}

void stack_drop(void)
{
    pop();
    slog("drop", NULL);
}

void stack_swap(void)
{
    swap();
    slog("swap", NULL);
}

void stack_rot(void)
{
    rot();
    slog("rot", NULL);
}

void stack_over(void)
{
    require(2);
    push(stack_.stack[stack_.depth - 2]);
    slog("over", NULL);
}

void stack_nip(void)
{
    swap();
    pop();
    slog("nip", NULL);
}

void stack_tuck(void)
{
    dup();
    rot();
    rot();
    slog("tuck", NULL);
}

void stack_2dup(void)
{
    require(2);
    push(stack_.stack[stack_.depth - 2]);
    push(stack_.stack[stack_.depth - 2]);
    slog("2dup", NULL);
}

void stack_pick(size_t n)
{
    require(n + 1);
    push(stack_.stack[stack_.depth - 1 - n]);
    slog("pick", NULL);
}

struct token stack_pop(void)
{
    struct token token = pop();
    slog("pop", token_print(token));
    return token;
}

/// Pop token matching @c tok or fail with fatal error.
static struct token stack_pop_tok(enum tok tok)
{
    struct token token = stack_pop();
    if (token.tok != tok) {
        stack_.fatal("stack type mismatch");
    }
    return token;
}

int stack_pop_number(void)
{
    return stack_pop_tok(tokNumber).u.number;
}

int stack_pop_variable(void)
{
    return stack_pop_tok(tokVariable).u.variable;
}

struct slice stack_pop_lambda(void)
{
    return stack_pop_tok(tokLambda).u.lambda;
}
