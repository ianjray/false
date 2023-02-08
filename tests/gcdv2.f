{
 while y != 0
  if x > y
   x := x - y
  else
   y := y - x
  return x
}

[
  { dup y }
  [ $ 0=~ ]
  [
    1O1O
    { x y x y }
    >
    { x y T }

    $ [ 
        { x y T }
        @
        { y T x }
        @
        { T x y }
        \
        { T y x }
        1O
        { T y x y }
        -
        { T y x' }
        \
        { T x' y }
        @
        { x' y T }
    ]?

    ~ [
        { x y }
        1O
        { x y x }
        -
        { x y' }
    ]?

  ]#

  { dump y }
  %

]g:

{ Expect 7 }
49 21 g;!

."
"
