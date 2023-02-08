{ assert(x) }
[0= ["assertion failed" 10, } ]?]t:


{ comment "string" [lambda] }
[1]a:
2[{[lambda]in first comment}a;!+]! 3= t;!
3[[{]}2]a:{]}{[lambda]in second comment}a;!+]! 5= t;!


{ not false equals -1 }
0~1_= t;!


{ a>2 && a<5 }

2a:
a;2>5a;> & ~ t;!
3a:
a;2>5a;> &   t;!
4a:
a;2>5a;> &   t;!
5a:
a;2>5a;> & ~ t;!


{ increment }
{
 a
 aa 
 aN
 aN1
 aN1+
 aN
 Na
 Na:
}
[$;1+\:]i:

2 a: ai;!  3  a; = t;!
2_b: bi;!  1_ b; = t;!


{ max(x, y) }
[
 1ø1ø { xyxy }
 >    { xyT }
 $ [ 
  @@  { Txy }
  %   { Tx }
  \   { xT }
 ]?
 ~ [
  \   { yx }
  %   { y }
 ]?
]m:

2_ 3_ m;! 2_= t;!
3_ 2_ m;! 2_= t;!
1_ 0  m;! 0 = t;!
0  1_ m;! 0 = t;!
2  3  m;! 3 = t;!
3  2  m;! 3 = t;!


{ XOR(a,b) = AND(OR(a,b), NOT(AND(a,b))) }

{
  x y
  x y y
  y y x
  y y x x
  y x x y
  y x |
  | y x
  | &
  | ~
  &
}

[$@$@|@@&~&]x:

0 0 x;! 0 = t;!
0 1 x;! 1 = t;!
1 0 x;! 1 = t;!
1 1 x;! 0 = t;!


