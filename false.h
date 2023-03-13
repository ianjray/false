#pragma once

#include <stdbool.h>
#include <wchar.h>

/*
 https://strlen.com/files/lang/false/false.txt

 Extensions inspired by:
 https://esolangs.org/wiki/DUP
 https://esolangs.org/wiki/Falsish
 https://wiki.laptop.org/go/Forth_stack_operators
 https://en.wikipedia.org/wiki/List_of_Java_bytecode_instructions

     (        , )             {..}   comment
     (        , func )        [..]   lambda

 Ext (   bool , )             ∫      assert stack top is not false    (macOS Option-B)

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

 Ext (    x y , rem quot )    ÷      (Forth) /MOD                     (macOS Option-/)

     (    x y , bool )        =      equal-to
     (    x y , bool )        >      greater-than

 Ext (    x y , bool )        <      less-than
 Ext (    x y , bool )        ≠      not-equal-to                     (macOS Option-=)
 Ext (    x y , bool )        ≤      less-than-or-equal-to            (macOS Option-<)
 Ext (    x y , bool )        ≥      greater-than-or-equal-to         (macOS Option->)

     (    x y , v )           &      bitwise-and
     (    x y , v )           |      bitwise-or
     (      x , v )           ~      bitwise-not

 Ext (    x y , v )           ⊻      bitwise-exclusive-or             (U+22BB)
 Ext (    x y , v )           «      binary left shift                (macOS Option-\)
 Ext (    x y , v )           »      binary right shift               (macOS Shift-Option-\)

     (      a , a a )         $      (Forth) dup
     (      a , )             %      (Forth) drop
     (    a b , b a )         \      (Forth) swap/exch
     (  a b c , b c a )       @      (Forth) rot/roll
     (      n , v )           ø      pick                             (macOS Option-o)
     (      n , v )           O      pick

 Ext (        , v )           §      push stack depth
 Ext (    a b , a b a )       £      (Forth) over                     (macOS Option-3)
 Ext (    a b , b )           ‰      (Forth) nip                      (macOS Shift-Option-r)
 Ext (    a b , b a b )       €      (Forth) tuck (Java) dup_x1       (macOS Shift-Option-2)
 Ext (    a b , a b a b )     Ø      (Forth) 2dup (Java) dup2         (macOS Shift-Option-o)

     (      bool func , )     ?      if-then ``[true]?`` or ``$[true]?~[false]?``
     (      func func , )     #      while

 Ext ( bool func func , )     ¿      if-then-else ``[true][false]¿``  (macOS Option-?)

     (    num , )             .      print-number
                              "STR"  print string
     (     ch , )             ,      print-character
     (        , ch )          ^      push character from stdin
     (        , )             ß      flush (nop)                      (macOS Option-ß)
     (        , )             B      flush (nop)

 Unused ASCII characters

                              `      unused (Amiga emitword)
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

    /// Enable extensions.
    bool extensions;

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
