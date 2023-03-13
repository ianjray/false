#pragma once

#include <stdbool.h>
#include <wchar.h>

/*
 https://strlen.com/files/lang/false/false.txt

 A Forth type language.
 https://forth-standard.org/standard/core

 Extensions (marked "Ext" below) are inspired by:
 https://esolangs.org/wiki/DUP
 https://esolangs.org/wiki/Falsish
 https://wiki.laptop.org/go/Forth_stack_operators

     Stack Diagram                          Token  Description
     =====================================  =====  ===========

     (            num -- )                  .      print-number
                                            "STR"  print string
     (             ch -- )                  ,      print-character
     (                -- ch )               ^      character from stdin
     (                -- )                  ß      flush (nop)               (macOS Option-ß)
     (                -- )                  B      flush (nop)

     (                -- )                  {..}   comment
     (                -- func )             [..]   lambda
     (                -- num )              0-9    number
     (                -- var )              a-z    variable
     (                -- ch )               'C     character

 Ext (           bool -- )                  ∫      assert true               (macOS Option-B)
     (          x var -- )                  :      assign
     (            var -- x )                ;      dereference
     (           func -- )                  !      call

     (              x -- -x)                _      negate
     (      term term -- sum )              +      add
     (      term term -- diff )             -      subtract
     (  factor factor -- product )          *      multiply
     (    numer demon -- quot )             /      divide
 Ext (    numer denom -- rem quot )         ÷      (Forth) /MOD              (macOS Option-/)

     (            x y -- bool )             =      equal-to
 Ext (            x y -- bool )             ≠      not-equal-to              (macOS Option-=)
 Ext (            x y -- bool )             <      less-than
     (            x y -- bool )             >      greater-than
 Ext (            x y -- bool )             ≤      less-than-or-equal-to     (macOS Option-<)
 Ext (            x y -- bool )             ≥      greater-than-or-equal-to  (macOS Option->)

     (            x y -- num )              &      and (bitwise)
     (            x y -- num )              |      or (bitwise)
     (              x -- num )              ~      invert (bitwise)
 Ext (            x y -- num )              ⊻      xor (bitwise)             (U+22BB)
 Ext (            x y -- num )              «      lshift                    (macOS Option-\)
 Ext (            x y -- num )              »      rshift                    (macOS Shift-Option-\)

 Ext (                -- num )              §      stack depth
     (              a -- a a )              $      dup
     (              a -- )                  %      drop
     (            a b -- b a )              \      swap
     (          a b c -- b c a )            @      rot
 Ext (            a b -- a b a )            £      over                      (macOS Option-3)
 Ext (            a b -- b )                ‰      nip                       (macOS Shift-Option-r)
 Ext (            a b -- b a b )            €      tuck                      (macOS Shift-Option-2)
 Ext (            a b -- a b a b )          Ø      2dup                      (macOS Shift-Option-o)
     ( xu ... x1 x0 u -- xu ... x1 x0 xu )  ø      pick                      (macOS Option-o)
     ( xu ... x1 x0 u -- xu ... x1 x0 xu )  O      pick

     (      bool func -- )                  ?      if-then
 Ext ( bool func func -- )                  ¿      if-then-else              (macOS Option-?)
     (      func func -- )                  #      while

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

    /// Emit wide character.
    /// @note Strings are emitted according to the source encoding.
    void (*emit_wchar)(wchar_t);

    /// Emit character.
    /// @note Octets read from stdin are emitted as-is.
    void (*emit_char)(char c);

    /// Get input.
    int (*input)(void);

    /// Read until newline.
    void (*flush)(void);
};

/// @return int Zero on success, one otherwise.
int interpret(struct config config);
