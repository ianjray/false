{
 func factorial(x int) -> (int)
  if x == 1
   return 1
  else
   return x * factorial(x - 1)

 fac
  dup one equ not
  dup one sub f() mul
  tst
}

[
  $1=~
  [
    $1-
    f;!
    *
  ]?
]f:

{ Expect 24 }

4f;!."
"
