#include "false.h"

#include "code-point.h"
#include "slice.h"
#include "stack.h"
#include "storage.h"
#include "token.h"

#include <ctype.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

/// Interpreter instance data.
static struct {
    /// Host interface.
    struct config config;

    /// Used for error management.
    jmp_buf env;

    /// Slice representing current processing unit.
    struct slice slice;
} g_;

__attribute__((noreturn))
static void fatal(const char *msg)
{
    g_.config.fatal(g_.config, g_.slice.buf, msg);
    longjmp(g_.env, 1);
}

static void log_trace(wchar_t wc)
{
    if (g_.config.log_trace) {
        g_.config.log_trace(g_.config, wc, g_.slice.buf);
    }
}

static void log_stack_operation(const char *op, const char *dump)
{
    g_.config.log_stack(g_.config, op, dump);
}

/// @return int Zero for false, and minus one for true (per the 'False' specification).
static int truth(bool boolean)
{
    if (boolean) {
        return -1;
    } else {
        return 0;
    }
}

/// @return bool Comparison result
static bool compare(void)
{
    struct token y = stack_pop();
    struct token x = stack_pop();
    if (y.tok != x.tok) {
        fatal("stack type mismatch");
    }
    switch (y.tok) {
        case tokNumber:
            return x.u.number == y.u.number;
        case tokVariable:
            return x.u.variable == y.u.variable;
        case tokLambda:
            return false;
    }
    return false; // UNREACHABLE
}

/// Dispatch a stack operation.
static wchar_t dispatch(wchar_t wc)
{
    switch (wc) {
        case '`':
            fatal("unsupported code injection");

        case '\\':
            stack_swap();
            break;

        case '$':
            stack_dup();
            break;

        case '%':
            stack_drop();
            break;

        case '@':
            stack_rot();
            break;

        case LATIN_SMALL_LETTER_O_WITH_STROKE:
        case 'O':
            stack_pick((size_t)stack_pop_number());
            break;

        case '=':
            stack_push(token_make_number(truth(compare())));
            break;

        case '+':
        case '-':
        case '*':
        case '/':
        case '>':
        case '&':
        case '|':
        {
            int y = stack_pop_number();
            int x = stack_pop_number();
            switch (wc) {
                case '+':
                    stack_push(token_make_number(x + y));
                    break;
                case '-':
                    stack_push(token_make_number(x - y));
                    break;
                case '*':
                    stack_push(token_make_number(x * y));
                    break;
                case '/':
                    if (y == 0) {
                        fatal("divide by zero");
                    }
                    stack_push(token_make_number(x / y));
                    break;
                case '>':
                    stack_push(token_make_number(truth(x > y)));
                    break;
                case '&':
                    stack_push(token_make_number(x & y));
                    break;
                case '|':
                    stack_push(token_make_number(x | y));
                    break;
            }
            break;
        }

        case '_':
            stack_push(token_make_number(-stack_pop_number()));
            break;

        case '~':
            stack_push(token_make_number(~stack_pop_number()));
            break;

        case '^':
            stack_push(token_make_number(g_.config.input()));
            break;

        case '.':
            g_.config.emit_number(stack_pop_number());
            break;

        case ',':
            g_.config.emit_char((char)stack_pop_number());
            break;

        case LATIN_SMALL_LETTER_SHARP_S:
        case 'B':
            g_.config.flush();
            break;

        case ':':
            {
                int v = stack_pop_variable();
                storage_set(v, stack_pop());
            }
            break;

        case ';':
            stack_push(storage_get(stack_pop_variable()));
            break;

        default:
            return wc;
    }

    return 0;
}

static void check_shift_operands(int x, int y)
{
    if (x < 0) {
        fatal("shifting a negative signed value is undefined");
    } else if (y < 0) {
        fatal("shift count is negative");
    } else if (y >= 32) {
        fatal("shift count >= width of type");
    }
}

static void process(struct slice s);

/// Dispatch an extended operation.
static wchar_t dispatch_extended(wchar_t wc)
{
    switch (wc) {
        case POUND_SIGN:
            stack_over();
            break;

        case PER_MILLE_SIGN:
            stack_nip();
            break;

        case EURO_SIGN:
            stack_tuck();
            break;

        case LATIN_CAPITAL_LETTER_O_WITH_STROKE:
            stack_2dup();
            break;

        case SECTION_SIGN:
            stack_push(token_make_number((int)stack_size()));
            break;

        case REGISTERED_SIGN:
            stack_reverse();
            break;

        case TRADE_MARK_SIGN:
            stack_roll((size_t)stack_pop_number());
            break;

        case NOT_EQUAL_TO:
            stack_push(token_make_number(truth(!compare())));
            break;

        case '<':
        case LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK:
        case RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK:
        case DIVISION_SIGN:
        case LESS_THAN_OR_EQUAL_TO:
        case GREATER_THAN_OR_EQUAL_TO:
        case XOR:
        {
            int y = stack_pop_number();
            int x = stack_pop_number();
            switch (wc) {
                case '<':
                    stack_push(token_make_number(truth(x < y)));
                    break;
                case LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK:
                    check_shift_operands(x, y);
                    stack_push(token_make_number(x << y));
                    break;
                case RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK:
                    check_shift_operands(x, y);
                    stack_push(token_make_number(x >> y));
                    break;
                case DIVISION_SIGN:
                    if (y == 0) {
                        fatal("divide by zero");
                    } else {
                        div_t d = div(x, y);
                        stack_push(token_make_number(d.rem));
                        stack_push(token_make_number(d.quot));
                    }
                    break;
                case LESS_THAN_OR_EQUAL_TO:
                    stack_push(token_make_number(truth(x <= y)));
                    break;
                case GREATER_THAN_OR_EQUAL_TO:
                    stack_push(token_make_number(truth(x >= y)));
                    break;
                case XOR:
                    stack_push(token_make_number(x ^ y));
                    break;
            }
            break;
        }

        case INTEGRAL:
            if (!stack_pop_number()) {
                fatal("assertion failed");
            }
            break;

        case INVERTED_QUESTION_MARK:
        {
            struct slice false_branch = stack_pop_lambda();
            struct slice true_branch = stack_pop_lambda();
            if (stack_pop_number()) {
                process(true_branch);
            } else {
                process(false_branch);
            }
            break;
        }

        default:
            return wc;
    }

    return 0;
}

/// Process a slice of symbols.
static void process(struct slice s)
{
    struct slice lambda = slice_make(NULL, 0);
    wchar_t state = 0;
    int number = 0;
    wchar_t nested = 0;
    int nesting = 0;
    size_t width = 0;
    mbstate_t mbstate;
    struct slice tmp;

    memset(&mbstate, 0, sizeof(mbstate));

    tmp = g_.slice;
    g_.slice = s;

    for (;;) {
        wchar_t wc;

        if (g_.slice.buf + width == g_.slice.end) {
            break;
        }

        g_.slice.buf += width;

        width = mbrtowc(&wc, g_.slice.buf, (size_t)(g_.slice.end - g_.slice.buf), &mbstate);
        if (width == 0 || width > 4) {
            fatal("bad multi-byte character");
        }

        switch (state) {
            case 0:
                if (iswdigit(wc)) {
                    number = wc - '0';
                    state = '1';
                    log_trace(wc);
                    continue;
                }
                break;

            case '1':
                if (iswdigit(wc)) {
                    number *= 10;
                    number += wc - '0';
                    log_trace(wc);
                    continue;
                }
                stack_push(token_make_number(number));
                state = 0;
                break;

            case '{':
                if (wc == '}') {
                    state = 0;
                }
                continue;

            case '\'':
                stack_push(token_make_number(wc));
                state = 0;
                continue;

            case '"':
                if (wc == '"') {
                    state = 0;
                } else {
                    g_.config.emit_wchar(wc);
                }
                continue;

            case '[':
                if (!nesting) {
                    lambda = slice_make(g_.slice.buf, 0);
                    nested = 0;
                    nesting = 1;
                }

                if (nested == '\'') {
                    nested = 0;
                } else if (nested && wc == nested) {
                    nested = 0;
                } else if (nested) {
                    ;
                } else if (wc == '\'') {
                    nested = wc;
                } else if (wc == '{') {
                    nested = '}';
                } else if (wc == '"') {
                    nested = wc;
                } else if (wc == '[') {
                    ++nesting;
                } else if (wc == ']') {
                    if (--nesting == 0) {
                        stack_push(token_make_lambda(lambda));
                        state = 0;
                    }
                }

                lambda.end += width;
                continue;
        }

        if (iswspace(wc)) {
            continue;
        }

        switch (wc) {
            case '{':
            case '\'':
            case '"':
            case '[':
                state = wc;
                continue;

            case ']':
            case '}':
                fatal("unbalanced symbol");
        }

        log_trace(wc);

        switch (wc) {
            case '!':
                process(stack_pop_lambda());
                break;

            case '?':
                {
                    struct slice body = stack_pop_lambda();
                    if (stack_pop_number()) {
                        // True is non-zero.
                        process(body);
                    }
                }
                break;

            case '#':
                {
                    struct slice body = stack_pop_lambda();
                    struct slice cond = stack_pop_lambda();
                    for (;;) {
                        process(cond);
                        if (!stack_pop_number()) {
                            break;
                        }
                        process(body);
                    }
                }
                break;

            default:
                wc = dispatch(wc);

                if (g_.config.extensions) {
                    wc = dispatch_extended(wc);
                }

                if (iswlower(wc)) {
                    stack_push(token_make_variable(wc));

                } else if (wc) {
                    fatal("unknown symbol");
                }
                break;
        }
    }

    switch (state) {
        case 0:
            break;

        case '1':
            stack_push(token_make_number(number));
            break;

        default:
            fatal("unterminated statement");
    }

    g_.slice = tmp;
}

int interpret(struct config config)
{
    int r = 1;

    g_.config = config;
    g_.slice = slice_make(NULL, 0);

    if (setjmp(g_.env) == 0) {
        int v;

        if (config.argc == 0) {
            fatal("too few arguments");
        }

        // Skip filename.
        --config.argc;
        ++config.argv;

        if (config.argc > 25) {
            // a = argc, b..z = args
            fatal("too many arguments");
        }

        stack_init(fatal, g_.config.log_stack ? log_stack_operation : NULL);

        storage_clear();

        v = 'a';
        storage_set(v++, token_make_number(config.argc));

        while (config.argc-- > 0) {
            const char *arg = *config.argv++;
            char *end = NULL;
            storage_set(v++, token_make_number((int)strtol(arg, &end, 0)));
            if (end && end == arg) {
                fatal("non-numeric argument");
            }
        }

        process(slice_make(config.str, strlen(config.str)));

        if (!stack_empty()) {
            fatal("stack not empty");
        }

        r = 0;
    }

    return r;
}
