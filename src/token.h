#pragma once

#include "slice.h"

enum tok {
    tokNumber,
    tokVariable,
    tokLambda
};

struct token {
    enum tok tok;
    union {
        int number;
        struct slice lambda;
        int variable;
    } u;
};

/// @return token Number.
struct token token_make_number(int number);

/// @return token Variable.
struct token token_make_variable(int variable);

/// @return token Lambda.
struct token token_make_lambda(struct slice lambda);

/// Print token to dynamically allocated buffer.
char *token_print(struct token token);
