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

static void nop_emit_char(wchar_t wc)
{
    (void)wc;
}

static int nop_input(void)
{
    return 1;
}

static wchar_t output[1024];
static size_t output_len;

static void capture_emit_number(int x)
{
    output_len += (size_t)swprintf(&output[output_len], sizeof(output) - output_len, L"%d", x);
}

static void capture_emit_char(wchar_t wc)
{
    output[output_len++] = wc;
}

int main(void)
{
    struct config config;

    config.fatal       = nop_fatal;
    config.log_trace   = nop_log_trace;
    config.log_stack   = nop_log_stack;
    config.emit_number = capture_emit_number;
    config.emit_char   = capture_emit_char;
    config.input       = nop_input;

    // Test data includes UTF-8 encoded multibyte characters.
    setlocale(LC_ALL, "en_US.UTF-8");

    {
        char *args[] = { "stdin", "42" };
        int r;

        config.argc = 0;
        config.argv = NULL;
        config.str = "";
        output_len = 0;
        r = interpret(config);
        assert(1 == r);

        config.argc = 27;
        config.argv = args;
        config.str = "";
        output_len = 0;
        r = interpret(config);
        assert(1 == r);

        config.argc = 1;
        config.argv = args;
        config.str = "";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);

        config.argc = 1;
        config.argv = args;
        config.str = "\xc3";  // First character of multibyte UTF-8 sequence.
        output_len = 0;
        r = interpret(config);
        assert(1 == r);

        config.argc = 1;
        config.argv = args;
        config.str = "1 0/";
        output_len = 0;
        r = interpret(config);
        assert(1 == r);

        config.argc = 1;
        config.argv = args;
        config.str = "1a:a;.";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(1 == output_len);
        assert('1' == output[0]);

        config.argc = sizeof(args) / sizeof(*args);
        config.argv = args;
        config.str = "a;.':,b;.':,c;.";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(6 == output_len);
        assert('1' == output[0]);
        assert(':' == output[1]);
        assert('4' == output[2]);
        assert('2' == output[3]);
        assert(':' == output[4]);
        assert('0' == output[5]);

        config.str = "[3+]b: 4 b;! a: a; 2 - .";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(1 == output_len);
        assert('5' == output[0]);

        config.str = "[3+]b: 4 b;! a: a; 2 - $ 1+ 1 O 2+ ...";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(3 == output_len);
        assert('7' == output[0]);
        assert('6' == output[1]);
        assert('5' == output[2]);

        config.str = "0~['T,]?";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(1 == output_len);
        assert('T' == output[0]);

        config.str = "0$['F,]?~['T,]?";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(1 == output_len);
        assert('T' == output[0]);

        config.str = "1[$100\\>][1+]#.";
        output_len = 0;
        r = interpret(config);
        assert(0 == r);
        assert(3 == output_len);
        assert('1' == output[0]);
        assert('0' == output[1]);
        assert('0' == output[2]);
    }

    /* Ad-hoc fuzz test of all supported symbols.
     * The iterations and range variables were chosen to get good code coverage. */
    {
        char *args[] = { "stdin" };

        config.argc = sizeof(args) / sizeof(*args);
        config.argv = args;

        config.fatal       = nop_fatal;
        config.emit_number = nop_emit_number;
        config.emit_char   = nop_emit_char;
        config.input       = nop_input;

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
        };

        const size_t iterations = 10000;
        const int range = 500;

        size_t ws_size = 1;
        size_t digit_size = 10;
        size_t letter_size = 26;
        size_t operator_size = sizeof(operator_array) / sizeof(*operator_array);
        size_t invalid_size = 1;
        size_t alphabet_size = ws_size + digit_size + letter_size + operator_size + invalid_size;

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
    }

    return 0;
}
