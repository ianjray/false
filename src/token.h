#pragma once

#include "slice.h"

enum tok {
    tokNone,
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

struct token token_none(void);

/// @return token Number.
struct token token_make_number(int number);

/// @return token Variable.
struct token token_make_variable(int variable);

/// @return token Lambda.
struct token token_make_lambda(struct slice lambda);
