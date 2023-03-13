#include "false.h"

#include "storage.h"

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

static const char *buffered_input;

static int input(void)
{
    if (buffered_input) {
        if (*buffered_input) {
            return *buffered_input++;
        }
        buffered_input = NULL;
    }

    return -1;
}

static void nop_flush(void)
{
}

static char output[1024];
static size_t output_len;

static void capture_emit_number(int x)
{
    output_len += (size_t)snprintf(&output[output_len], sizeof(output) - output_len, "%d", x);
}

static void capture_emit_wchar(wchar_t wc)
{
    output_len += wcrtomb(&output[output_len], wc, NULL);
    output[output_len] = 0;
}

static void capture_emit_char(char c)
{
    output[output_len++] = c;
    output[output_len] = 0;
}

static int testcase(struct config config, const char *input)
{
    int r;

    output_len = 0;
    output[output_len] = 0;

    config.str = input;
    r = interpret(config);

    buffered_input = NULL;

    return r;
}

static void test_multibyte(struct config config)
{
    char *args[] = { "stdin" };
    int r;

    config.argc = 1;
    config.argv = args;

    r = testcase(config, "\xc3");  // First character of multibyte UTF-8 sequence.
    assert(1 == r);

    buffered_input = "oö";
    r = testcase(config, "\"aä\" [^$1_=~][,]#%");
    assert(!strcmp(output, "aäoö"));
}

static void test_token(struct config config)
{
    char *args[] = { "stdin" };
    int r;

    config.argc = 1;
    config.argv = args;

    // Stack not empty.
    r = testcase(config, "42");
    assert(1 == r);

    r = testcase(config, "42.");
    assert(0 == r);
    assert(!strcmp(output, "42"));

    r = testcase(config, "\"hello\"");
    assert(0 == r);
    assert(!strcmp(output, "hello"));

    r = testcase(config, "65,");
    assert(0 == r);
    assert(!strcmp(output, "A"));

    buffered_input = "hello";
    r = testcase(config, "^,^,^,");
    assert(!strcmp(output, "hel"));

    r = testcase(config, "ß");
    assert(0 == r);
    assert(!strcmp(output, ""));

    r = testcase(config, "B");
    assert(0 == r);
    assert(!strcmp(output, ""));

    r = testcase(config, "{1");
    assert(1 == r);

    r = testcase(config, "1}");
    assert(1 == r);

    r = testcase(config, "{1,}");
    assert(0 == r);
    assert(!strcmp(output, ""));

    r = testcase(config, "[1");
    assert(1 == r);

    r = testcase(config, "1]");
    assert(1 == r);

    r = testcase(config, "{65,[42]}[4{}2]!.',,.");
    assert(0 == r);
    assert(!strcmp(output, "2,4"));

    r = testcase(config, "{65,[42]}[4{}2\"!\"[6]]!!.\",\".',,.");
    assert(0 == r);
    assert(!strcmp(output, "!6,2,4"));

    r = testcase(config, "0∫");
    assert(1 == r);

    r = testcase(config, "1∫");
    assert(0 == r);
    assert(!strcmp(output, ""));

    r = testcase(config, "1a:a;.");
    assert(0 == r);
    assert(!strcmp(output, "1"));

    r = testcase(config, "'{,");
    assert(0 == r);
    assert(!strcmp(output, "{"));

    r = testcase(config, "'[,");
    assert(0 == r);
    assert(!strcmp(output, "["));

    r = testcase(config, "[1]a:a;!.");
    assert(0 == r);
    assert(!strcmp(output, "1"));

    r = testcase(config, "1_.");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    r = testcase(config, "3 a + .");
    assert(1 == r);

    r = testcase(config, "3 4 + .");
    assert(0 == r);
    assert(!strcmp(output, "7"));

    r = testcase(config, "3 4 - .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    r = testcase(config, "3 4 * .");
    assert(0 == r);
    assert(!strcmp(output, "12"));

    r = testcase(config, "20 4 / .");
    assert(0 == r);
    assert(!strcmp(output, "5"));

    // Divide-by-zero.
    r = testcase(config, "1 0 /");
    assert(1 == r);

    r = testcase(config, "22 4 ÷ .',,.");
    assert(0 == r);
    assert(!strcmp(output, "5,2"));

    r = testcase(config, "22 0 ÷");
    assert(1 == r);

    r = testcase(config, "20 4 = .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "20 4 ≠ .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    // Stack type mismatch.
    r = testcase(config, "a 0 = .");
    assert(1 == r);

    r = testcase(config, "1$a:b: a b = .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "ab: a b; = .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    // Lambda comparison is always false.
    r = testcase(config, "[3][3]=.");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "20 4 > .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    r = testcase(config, "20 4 < .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "3 3 ≥ .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    r = testcase(config, "3 3 ≤ .");
    assert(0 == r);
    assert(!strcmp(output, "-1"));

    r = testcase(config, "7 2 & .");
    assert(0 == r);
    assert(!strcmp(output, "2"));

    r = testcase(config, "4 2 | .");
    assert(0 == r);
    assert(!strcmp(output, "6"));

    r = testcase(config, "1_ ~ .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "0 0 ⊻ .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "2 2 ⊻ .");
    assert(0 == r);
    assert(!strcmp(output, "0"));

    r = testcase(config, "4 2 ⊻ .");
    assert(0 == r);
    assert(!strcmp(output, "6"));

    r = testcase(config, "2 1_ «");
    assert(1 == r);

    r = testcase(config, "2_ 1 «");
    assert(1 == r);

    r = testcase(config, "2 32 «");
    assert(1 == r);

    r = testcase(config, "2 1 « .");
    assert(0 == r);
    assert(!strcmp(output, "4"));

    r = testcase(config, "16 2 » .");
    assert(0 == r);
    assert(!strcmp(output, "4"));

    r = testcase(config, "9 8 § . %%");
    assert(0 == r);
    assert(!strcmp(output, "2"));

    r = testcase(config, "3 $ .',,.");
    assert(0 == r);
    assert(!strcmp(output, "3,3"));

    r = testcase(config, "3 4 % .");
    assert(0 == r);
    assert(!strcmp(output, "3"));

    r = testcase(config, "3 4 \\ .',,.");
    assert(0 == r);
    assert(!strcmp(output, "3,4"));

    r = testcase(config, "1 2 3 @ ...");
    assert(0 == r);
    assert(!strcmp(output, "132"));

    r = testcase(config, "1 2 £ ...");
    assert(0 == r);
    assert(!strcmp(output, "121"));

    r = testcase(config, "1 2 ‰ .");
    assert(0 == r);
    assert(!strcmp(output, "2"));

    r = testcase(config, "1 2 € ...");
    assert(0 == r);
    assert(!strcmp(output, "212"));

    r = testcase(config, "1 2 Ø ....");
    assert(0 == r);
    assert(!strcmp(output, "2121"));

    // ( xu ... x1 x0 u -- xu ... x1 x0 xu )
    r = testcase(config, "1 2 3 0O ....");
    assert(0 == r);
    assert(!strcmp(output, "3321"));

    r = testcase(config, "1 2 3 1O ....");
    assert(0 == r);
    assert(!strcmp(output, "2321"));

    r = testcase(config, "1 2 3 2ø ....");
    assert(0 == r);
    assert(!strcmp(output, "1321"));

    r = testcase(config, "1 2 3 3O ....");
    assert(1 == r);

    r = testcase(config, "['T,]t: 0~ t;?");
    assert(0 == r);
    assert(!strcmp(output, "T"));

    r = testcase(config, "['F,]f: ['T,]t: 0$f;?~t;?");
    assert(0 == r);
    assert(!strcmp(output, "T"));

    r = testcase(config, "0~ ['T,]['F,]¿");
    assert(0 == r);
    assert(!strcmp(output, "T"));

    r = testcase(config, "0  ['F,]['T,]¿");
    assert(0 == r);
    assert(!strcmp(output, "T"));

    r = testcase(config, "[$100\\>]c: [1+]l: 1 c;l;#.");
    assert(0 == r);
    assert(!strcmp(output, "100"));

    // Unsupported code injection.
    r = testcase(config, "`");
    assert(1 == r);

    // Unknown symbol.
    r = testcase(config, "A");
    assert(1 == r);
}

static void test_arguments(struct config config)
{
    char *args[] = { "stdin", "42", "string" };
    int r;

    config.argc = 0;
    config.argv = NULL;
    r = testcase(config, "");
    assert(1 == r);

    config.argc = 27;
    config.argv = args;
    r = testcase(config, "");
    assert(1 == r);

    config.argc = 1;
    config.argv = args;
    r = testcase(config, "");
    assert(0 == r);

    config.argc = 2;
    config.argv = args;
    r = testcase(config, "a;.':,b;.':,c;.");
    assert(0 == r);
    assert(!strcmp(output, "1:42:0"));

    config.argc = 3;
    config.argv = args;
    r = testcase(config, "");
    assert(1 == r);
}

int main(void)
{
    struct config config;

    config.extensions  = true;
    config.fatal       = nop_fatal;
    config.log_trace   = nop_log_trace;
    config.log_stack   = NULL;
    config.emit_number = capture_emit_number;
    config.emit_wchar  = capture_emit_wchar;
    config.emit_char   = capture_emit_char;
    config.input       = input;
    config.flush       = nop_flush;

    // Test data includes UTF-8 encoded multibyte characters.
    setlocale(LC_ALL, "en_US.UTF-8");

    {
        struct token token;
        storage_set(',', token_make_number(42));
        token = storage_get(',');
        assert(token.tok == tokNumber);
        assert(token.u.number == 0);
    }

    test_multibyte(config);

    config.log_stack   = nop_log_stack;

    test_token(config);

    test_arguments(config);

    return 0;
}
