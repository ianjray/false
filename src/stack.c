#include "stack.h"

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
    stack_.fatal = fatal;
    stack_.log = log;
}

bool stack_empty(void)
{
    return stack_.depth == 0;
}

static char * token_print(struct token token)
{
    char * buf = NULL;
    switch (token.tok) {
        case tokNone:
            asprintf(&buf, "");
            break;
        case tokNumber:
            asprintf(&buf, "%d", token.u.number);
            break;
        case tokVariable:
            asprintf(&buf, "%c", token.u.variable);
            break;
        case tokLambda:
            asprintf(&buf, "[…");
            break;
    }
    return buf;
}

static void log(const char *op, struct token token)
{
    if (!stack_.log) {
        return;
    }

    char * dump = token_print(token);
    dump = (char *)realloc(dump, strlen(dump) + 1 + 1);
    strcat(dump, "\t");

    for (size_t i = 0; i < stack_.depth; ++i) {
        dump = (char *)realloc(dump, strlen(dump) + 1 + 1);
        strcat(dump, " ");

        char * buf = token_print(stack_.stack[i]);
        dump = (char *)realloc(dump, strlen(dump) + 1 + strlen(buf));
        strcat(dump, buf);
        free(buf);
    }

    stack_.log(op, dump);
    free(dump);
}

/// Push @c token.
static void push(struct token token)
{
    stack_.stack = (struct token *)realloc(stack_.stack, (stack_.depth + 1) * sizeof(struct token));
    stack_.stack[stack_.depth++] = token;
}

/// Duplicate top of stack.
static void dup(void)
{
    if (stack_.depth == 0) {
        stack_.fatal("stack underflow");
    }
    push(stack_.stack[stack_.depth - 1]);
}

/// Pop token.
static struct token pop(void)
{
    if (stack_.depth == 0) {
        stack_.fatal("stack underflow");
    }
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

void stack_push(struct token token)
{
    push(token);
    log("push", token);
}

void stack_dup(void)
{
    dup();
    log("dup", token_none());
}

void stack_swap(void)
{
    struct token x1 = pop();
    struct token x = pop();
    push(x1);
    push(x);
    log("swap", token_none());
}

void stack_rot(void)
{
    rot();
    log("rot", token_none());
}

void stack_pick(size_t n)
{
    if (n >= stack_.depth) {
        stack_.fatal("stack underflow");
    }
    stack_push(stack_.stack[stack_.depth - 1 - n]);
    log("pick", token_none());
}

struct token stack_pop(void)
{
    struct token token = pop();
    log("pop", token);
    return token;
}

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
