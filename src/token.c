#include "token.h"

struct token token_none(void)
{
    struct token token;
    token.tok = tokNone;
    token.u.number = 0;
    return token;
}

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
