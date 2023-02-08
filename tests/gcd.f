{
 while y != 0
  if x > y
   x := x - y
  else
   y := y - x
  return x
}

[
  { parameters }
  y:
  x:

  [y;0=~]
  [
    x;y;>
    $ [ x;y;-x: ]?
    ~ [ y;x;-y: ]?
  ]#
]g:

{ Expect 7 }

49 21 g;!
x;."
"
