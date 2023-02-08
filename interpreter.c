#include "false.h"

#include "utils/file.h"

#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct position {
    size_t line;
    size_t ch;
    const char *bol;
    const char *eol;
};

/// Determine line and character for @c pos.
static struct position position_of(const struct config config, const char *pos)
{
    struct position position;
    bool advance;

    position.line = 0;
    position.ch = 0;
    position.bol = NULL;
    position.eol = NULL;

    if (!pos) {
        return position;
    }

    // line n: ".....X....."
    //          ^    ^    ^
    //         BOL  POS  EOL/EOF

    advance = true;

    for (const char *p = config.str; p <= pos; ++p) {
        if (advance) {
            advance = false;
            position.line++;
            position.ch = 0;
            // Beginning of line that contains symbol at position "pos".
            position.bol = p;
        }
        advance = *p == '\n';
        position.ch++;
    }

    // End of line that contains symbol at position "pos".
    for (position.eol = pos; position.eol < strchr(config.str, 0) && *position.eol != '\n'; ++position.eol) {
    }

    return position;
}

static void fatal(const struct config config, const char *pos, const char *msg)
{
    if (!pos) {
        // Usage error.
        fprintf(stderr
                , "%s: error: %s\n"
                , config.argv[0]
                , msg);
    } else {
        struct position position = position_of(config, pos);

        int len = (int)(position.eol - position.bol);
        int off = (int)(pos - position.bol + 1);

        fprintf(stderr
                , "%s:%zu:%zu: error: %s\n"
                "%*.*s\n"
                "%*.*s\n"
                , config.argv[0]
                , position.line
                , position.ch
                , msg
                , len, len, position.bol
                , off, off, "^");
    }
}

static void log_trace(const struct config config, wchar_t wc, const char *pos)
{
    (void)config;
    (void)pos;
    printf("# symbol %lc\n", wc);
}

static void log_stack(const struct config config, const char *op, const char *dump)
{
    (void)config;
    printf("# %6s %s\n", op, dump);
}

static void emit_number(int number)
{
    printf("%d", number);
}

static void emit_wchar(wchar_t wc)
{
    putwchar(wc);
}

static void emit_char(char c)
{
    putchar(c);
}

static const char *buffered_input;

static int input(void)
{
    if (buffered_input) {
        if (*buffered_input) {
            return *buffered_input++;
        }
        buffered_input = NULL;
        return '\n';
    }

    return getchar();
}

static void flush(void)
{
    if (buffered_input) {
        buffered_input = NULL;
    } else {
        fflush(stdin);
    }
}

/// Skip magic header.
static const char * skip_magic(const char * str)
{
    if (str[0] == '#' && str[1] == '!') {
        const char *nl = strchr(str, '\n');
        if (nl) {
            return nl;
        }
    }
    return str;
}

static void usage(void)
{
    printf(
            "false_int [OPTIONS...] FILE [ARGUMENTS...]\n"
            "\n"
            "Interpret a 'FALSE' program.\n"
            "\n"
            "Options:\n"
            "  -h, --help            Print this message and exit.\n"
            "  -i, --input STRING    Input string.\n"
            "  -v, --verbose         Print debug messages.\n"
            "\n"
            "Upto 25 numeric arguments may be given.  These are passed to the program\n"
            "using the variables `b..z' while `a' holds the count of arguments.  This\n"
            "feature is documented in the False1.2b portable interpreter.\n"
            "\n"
            "Input may be given on the command line (in a similar manner as the Amiga\n"
            "implementation) with a single `--input' string, which is read before any\n"
            "further input from stdin.\n"
            "\n"
            "Extended (Unicode) characters are processed according to current locale.\n"
            "However, 'B' and 'O' are supported for 'flush' and 'pick' operations, as\n"
            "implemented in the False1.2b portable interpreter.\n"
    );
}

static int drop(int i, int argc, char **argv)
{
    argc--;
    memmove(&argv[i], &argv[i + 1], (size_t)(argc - i) * sizeof(char *));
    return argc;
}

int main(int argc, char **argv)
{
    const char *filename = NULL;
    struct config config;
    char *buf;
    int r;

    config.fatal       = fatal;
    config.log_trace   = NULL;
    config.log_stack   = NULL;
    config.emit_number = emit_number;
    config.emit_wchar  = emit_wchar;
    config.emit_char   = emit_char;
    config.input       = input;
    config.flush       = flush;

    // Skip this executable name.
    argc--;
    argv++;

    // Filter out arguments.
    for (int i = 0; i < argc; ) {
        const char *arg = argv[i];

        if (!strcmp(arg, "--")) {
            argc = drop(i, argc, argv);
            break;

        } else if (!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
            usage();
            return EXIT_SUCCESS;

        } else if (!strcmp(arg, "-i") || !strcmp(arg, "--input")) {
            argc = drop(i, argc, argv);
            if (!buffered_input && i < argc) {
                buffered_input = argv[i];
                argc = drop(i, argc, argv);

            } else {
                usage();
                return EXIT_FAILURE;
            }

        } else if (!strcmp(arg, "-v") || !strcmp(arg, "--verbose")) {
            argc = drop(i, argc, argv);
            config.log_trace = log_trace;
            config.log_stack = log_stack;

        } else if (arg[0] == '-') {
            usage();
            return EXIT_FAILURE;

        } else if (filename) {
            break;

        } else {
            filename = arg;
            i++;
        }
    }

    if (!filename) {
        usage();
        return EXIT_FAILURE;
    }

    r = file_read_fully(filename, &buf);
    if (r < 0) {
        errno = -r;
        perror(filename);
        return EXIT_FAILURE;
    }

    // Use locales specified in the environment.
    setlocale(LC_ALL, "");

    config.argc = argc;
    config.argv = argv;
    config.str = skip_magic(buf);

    r = interpret(config);

    free(buf);

    if (r) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
