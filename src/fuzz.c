#include "false.h"

#include "code-point.h"

#include <assert.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void nop_fatal(const struct config config, const char *pos, const char *msg)
{
    (void)config;
    (void)pos;
    (void)msg;
}

static void nop_log_trace(const struct config config, wchar_t wc, const char *pos)
{
    (void)config;
    (void)wc;
    (void)pos;
}

static void nop_log_stack(const struct config config, const char *op, const char *dump)
{
    (void)config;
    (void)op;
    (void)dump;
}

static void nop_emit_number(int x)
{
    (void)x;
}

static void nop_emit_wchar(wchar_t wc)
{
    (void)wc;
}

static void nop_emit_char(char c)
{
    (void)c;
}

static int nop_input(void)
{
    return 1;
}

static void nop_flush(void)
{
}

int main(void)
{
    /* Ad-hoc fuzz test of all supported symbols.
     * The iterations and range variables were chosen to get good code coverage.
     * This test aims to identify crashes caused by malformed input. */

    static const wchar_t operator_array[] = {
        '`',
        '\\',
        '$',
        '%',
        '@',
        LATIN_SMALL_LETTER_O_WITH_STROKE,
        'O',
        '+',
        '-',
        '*',
        '/',
        '<',
        '=',
        '>',
        '&',
        '|',
        '_',
        '~',
        '^',
        '.',
        ',',
        LATIN_SMALL_LETTER_SHARP_S,
        'B',
        ':',
        ';',
        '{',
        '\'',
        '"',
        '[',
        ']',
        '}',
        '!',
        '?',
        '#',
        POUND_SIGN,
        SECTION_SIGN,
        REGISTERED_SIGN,
        LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK,
        RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK,
        INVERTED_QUESTION_MARK,
        LATIN_CAPITAL_LETTER_O_WITH_STROKE,
        DIVISION_SIGN,
        PER_MILLE_SIGN,
        EURO_SIGN,
        TRADE_MARK_SIGN,
        INTEGRAL,
        NOT_EQUAL_TO,
        LESS_THAN_OR_EQUAL_TO,
        GREATER_THAN_OR_EQUAL_TO,
        XOR,
    };

    char *args[] = { "stdin" };

    const size_t iterations = 10000;
    const int range = 500;

    size_t ws_size = 1;
    size_t digit_size = 10;
    size_t letter_size = 26;
    size_t operator_size = sizeof(operator_array) / sizeof(*operator_array);
    size_t invalid_size = 1;
    size_t alphabet_size = ws_size + digit_size + letter_size + operator_size + invalid_size;

    struct config config;

    config.argc = sizeof(args) / sizeof(*args);
    config.argv = args;

    config.fatal       = nop_fatal;
    config.log_trace   = nop_log_trace;
    config.log_stack   = nop_log_stack;
    config.emit_number = nop_emit_number;
    config.emit_wchar  = nop_emit_wchar;
    config.emit_char   = nop_emit_char;
    config.input       = nop_input;
    config.flush       = nop_flush;

    // Test data includes UTF-8 encoded multibyte characters.
    setlocale(LC_ALL, "en_US.UTF-8");

    srand(0);

    for (size_t iteration = 0; iteration < iterations; ++iteration) {
        size_t len = (size_t)(rand() % range);
        char *buf = (char *)malloc(len * 4);  // Include overhead for multibyte characters.
        size_t width = 0;

        for (size_t i = 0; i < len; ++i) {
            size_t r = (size_t)((size_t)rand() % alphabet_size);
            wchar_t wc;

            if (r < ws_size) {
                wc = ' ';
            } else {
                r -= ws_size;
                if (r < digit_size) {
                    wc = '0' + (char)r;
                } else {
                    r -= digit_size;
                    if (r < letter_size) {
                        wc = 'a' + (char)r;
                    } else {
                        r -= letter_size;
                        if (r < operator_size) {
                            wc = operator_array[r];
                        } else {
                            r -= operator_size;
                            if (r < invalid_size) {
                                wc = 'A';
                            } else {
                                r -= invalid_size;
                                abort();
                            }
                        }
                    }
                }
            }

            width += wcrtomb(&buf[width], wc, NULL);
        }

        buf[width] = 0;

        config.str = buf;
        interpret(config);
        free(buf);
    }

    return 0;
}
