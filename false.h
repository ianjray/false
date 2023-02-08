#pragma once

#include <wchar.h>

/*
 https://strlen.com/files/lang/false/false.txt

     (        , )             {..}   comment
     (        , func )        [..]   lambda

     (        , num )         0-9    number
     (        , var )         a-z    variable
     (        , ch )          'C     character

     (  x var , )             :      assign
     (    var , x )           ;      dereference
     (   func , )             !      call

     (    x y , sum )         +      add
     (    x y , diff )        -      subtract
     (    x y , product )     *      multiply
     (    x y , quot )        /      divide
     (      x , -x)           _      negate

     (    x y , bool )        =      equal-to
     (    x y , bool )        >      greater-than

     (    x y , v )           &      bitwise-and
     (    x y , v )           |      bitwise-or
     (      x , v )           ~      bitwise-not

     (      a , a a )         $      (Forth) dup
     (      a , )             %      (Forth) drop
     (    a b , b a )         \      (Forth) swap/exch
     (  a b c , b c a )       @      (Forth) rot/roll
     (      n , v )           ø      pick                             (macOS Option-o)
     (      n , v )           O      pick

     (      bool func , )     ?      if-then ``[true]?`` or ``$[true]?~[false]?``
     (      func func , )     #      while

     (    num , )             .      print-number
                              "STR"  print string
     (     ch , )             ,      print-character
     (        , ch )          ^      push character from stdin
     (        , )             ß      flush (nop)                      (macOS Option-ß)
     (        , )             B      flush (nop)

 Unused ASCII characters

                              `      unused (Amiga emitword)
                              <      unused (note: one example is documented upstream)
                              A      unused
                              C-N    unused
                              P-Z    unused
                              (      unused
                              )      unused

 */

struct config {
    /// Command line argument count.
    /// @note: Minimum one, since file name is the first element.
    int argc;

    /// Command line arguments.
    /// @note: The first element is the source file name.
    char **argv;

    /// Source file contents.
    const char *str;

    /// Report fatal error.
    /// @param arg Points to the current symbol in the source file contents @c str.
    void (*fatal)(const struct config config, const char *pos, const char *msg);

    /// Trace execution.
    /// @param arg Points to the current symbol in the source file contents @c str.
    void (*log_trace)(const struct config config, wchar_t wc, const char *pos);

    /// Log stack operations.
    /// @param op Describes the stack operation, for example @c "push".
    /// @param dump Contains a stack dump.
    void (*log_stack)(const struct config config, const char *op, const char *dump);

    /// Emit number.
    void (*emit_number)(int);

    /// Emit character.
    void (*emit_char)(wchar_t);

    /// Get input.
    int (*input)(void);
};

/// @return int Zero on success, one otherwise.
int interpret(struct config config);
