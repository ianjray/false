# false

[False](https://strlen.com/false-language/) interpreter.

`False` is a small, stack-based, esoteric programming language created by Wouter van Oortmerssen.

## Usage

```
false_int [OPTIONS...] FILE [ARGUMENTS...]

Interpret a 'false' program.

Options:
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

Several small examples are included in the `tests` directory.

```
$ ./false_int tests/sierpinski.false 3
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

[Rosetta Code](https://rosettacode.org/wiki/Category:FALSE) is a great source of examples.

## Greatest Common Divisor

The [Euclidean](https://en.wikipedia.org/wiki/Euclidean_algorithm) greatest common divisor algorithm (subtraction-based version).

* Using variables.

```
[y:x:
 [y;0=~]
  [x;y;>             { x > y }
   $ [ x;y;-x: ]?    { x = x - y }
   ~ [ y;x;-y: ]?    { y = y - x }
  ]#
]g:
49 21 g;!
x;.
```

* Using the stack.

```
[
 [$0=~]
  [1ø1ø>             { xyxy -> xyT }
   $ [ @@\1ø-\@ ]?   { yTx -> Txy -> Tyx -> Tyxy -> Tyx' -> Tx'y -> x'yT }
   ~ [    1ø-   ]?   { xy -> xyx -> xy' }
  ]#
 %
]g:
49 21 g;!
.
```

[Rosetta Code](https://rosettacode.org/wiki/Greatest_common_divisor#FALSE) presents a very succinct version.
This is an expression of the *division-based* algorithm.

```
10 15$ [0=~][$@$@$@\/*-$]#%. { gcd(10,15)=5 }
```

Expanded for explanation, with stack contents of first round of execution in comments:

```
10 15$
[0=~]
 [
  $@  { yyx   }
  $@  { yxxy  }
  $@  { yxyyx }
  \   { yxyxy }
  /   { yxyN    N = x / y }
  *   { yxN     N = y * N }
  -   { yN      N = x - N or N = x mod y }
  $   { yNN   }
 ]#%
.
```

## String Reverse

There is a nice example at [https://morphett.info/false/false.html](https://morphett.info/false/false.html) which reverses a string by pushing lambdas onto the stack.
Each lambda pops the previous.

```
[][^$1_=~][[,!]]#%!
```


# Alternatives and Variants

* [DUP](https://esolangs.org/wiki/DUP)

* [False!](http://www.nsl.com/papers/false.htm)

* [Falsish](https://esolangs.org/wiki/Falsish)

## Future Directions

In alternate implementations, the emit-word operator `` ` `` is typically ignored (or re-used for language extensions).

There seems to be some consensus that the omission of the *less-than* operator `<` was needlessly restrictive.
Supporting this would be a backwards-compatible change.

The use of the dereference operator `;` *may* have been a compromise to make the original implementation as small as possible.
Making it optional would be an opportunity for more concise code (and would be backwards-compatible).

Per *Falsish* there are a number of *extended* punctuation and mathematical characters that are easily typed on macOS.
These may be used to add new functionality.

