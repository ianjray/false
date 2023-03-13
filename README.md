# false

[FALSE](https://strlen.com/false-language/) interpreter.

`FALSE` is a small, stack-based, esoteric programming language created by Wouter van Oortmerssen.

## Usage

```
false_int [OPTIONS...] FILE [ARGUMENTS...]

Interpret a 'FALSE' program.

Options:
  -e, --extensions      Enable extensions.
  -h, --help            Print this message and exit.
  -i, --input STRING    Input string.
  -v, --verbose         Print debug messages.

Upto 25 numeric arguments may be given.  These are passed to the program
using the variables `b..z' while `a' holds the count of arguments.  This
feature is documented in the False1.2b portable interpreter.

Input may be given on the command line (in a similar manner as the Amiga
implementation) with a single `--input' string, which is read before any
further input from stdin.

Extended (Unicode) characters are processed according to current locale.
However, 'B' and 'O' are supported for 'flush' and 'pick' operations, as
implemented in the False1.2b portable interpreter.
```

## Examples

Several examples are included in the [tests](tests) directory.

```shell
$ false_int tests/sierpinski.f 3
       *
      * *
     *   *
    * * * *
   *       *
  * *     * *
 *   *   *   *
* * * * * * * *
```


# Tutorials

[The FALSE Programming Language](https://strlen.com/files/lang/false/false.txt) introduces the language and syntax.

[Rosetta Code](https://rosettacode.org/wiki/Category:FALSE) is a great source of interesting programs.

## String Reverse

There is a nice example at [https://morphett.info/false/false.html](https://morphett.info/false/false.html) which reverses a string by pushing lambdas onto the stack.
Each lambda pops the previous.

```
[][^$1_=~][[,!]]#%!
```

## Greatest Common Divisor

[Rosetta Code](https://rosettacode.org/wiki/Greatest_common_divisor#FALSE) presents an expression of the [*division-based* greatest common divisor algorithm](https://en.wikipedia.org/wiki/Euclidean_algorithm#Implementations).

```
// https://en.wikipedia.org/wiki/Euclidean_algorithm#Implementations
function gcd(a, b)
    while b ≠ 0
        t := b
        b := a mod b
        a := t
    return a
```

```
10 15$ [0=~][$@$@$@\/*-$]#%. { gcd(10,15)=5 }
```

Note that the implementation may be shortened by one character, by moving `dup` into the while-loop condition.

```
10 15 [$0=~][$@$@$@\/*-]#%.
```

Expanded for explanation, with stack contents shown in comments:

```
10 15   { a b }
[$0=~]  { a b b }
[
 $@     { t b a }
 $@     { t a a b }
 $@     { t a b b a }
 \      { t a b a b }
 /      { t a b N      N = a / b }
 *      { t a N        N = b * N }
 -      { t N          N = a - N or N = a mod b }
]#
%       { a }
.
```

Note that the sequence `$@$@$@\` is equivalent to (the equal length) `\1ø1ø1ø`.


# Extensions

Extensions are available with the ``--extensions`` flag.

The various extensions can make `FALSE` programs **slightly** more succinct (at the cost of compatibility).
For example, the *division-based* greatest common divisor algorithm may be expressed as:

```
10 15 [$0=~][$@$@$@\/*-]#%  5=∫
10 15 [$0≠][$@$@$@\/*-]#%   5=∫
10 15 [$0≠][€Ø/*-]#%        5=∫  { tuck, 2dup }
10 15 [$0≠][€÷%]#%          5=∫  { tuck, /MOD }
```


# Alternatives and Variants

* [DUP](https://esolangs.org/wiki/DUP)

* [False!](http://www.nsl.com/papers/false.htm)

* [Falsish](https://esolangs.org/wiki/Falsish)

## Future Directions

In alternate implementations, the emit-word operator `` ` `` is typically ignored (or re-used for language extensions).

There seems to be some consensus that the omission of the *less-than* operator `<` was needlessly restrictive.
Supporting this would be a backwards-compatible change.

The sequence `;!` could be simplified to `!` *if* the interpreter were designed to automatially dereference a variable in this context.
This would be backwards compatible.

Per `Falsish` there are a number of *extended* punctuation and mathematical characters that are easily typed on macOS.
Some of these are used to provide extended functionality (see the ``--extensions`` flag).

The *flush* operator is seldom implemented, but is needed to support some *interactive* programs (including False1.2b `contrib/Marcel_van_Kervinck/tic.f`).

