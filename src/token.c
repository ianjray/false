#include "token.h"

#ifdef HAS_ASPRINTF_GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>

struct token token_make_number(int number)
{
    struct token token;
    token.tok = tokNumber;
    token.u.number = number;
    return token;
}

struct token token_make_variable(int variable)
{
    struct token token;
    token.tok = tokVariable;
    token.u.variable = variable;
    return token;
}

struct token token_make_lambda(struct slice lambda)
{
    struct token token;
    token.tok = tokLambda;
    token.u.lambda = lambda;
    return token;
}

char *token_print(struct token token)
{
    char *buf = NULL;
    switch (token.tok) {
        case tokNumber:
            asprintf(&buf, "%d", token.u.number);
            break;
        case tokVariable:
            asprintf(&buf, "%c", token.u.variable);
            break;
        case tokLambda:
            asprintf(&buf, "[%*.*s]", (int)slice_length(token.u.lambda), (int)slice_length(token.u.lambda), token.u.lambda.buf);
            break;
    }
    return buf;
}
