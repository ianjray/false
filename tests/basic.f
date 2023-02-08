{ assert(x) }
[0=["assertion failed"10,}]?]t:


{ https://rosettacode.org/wiki/Logical_operations#FALSE }
1 3=~    t;!
1 1= 1_= t;!
0~       t;!
'm$'a>'z@>& t;!


{ xor(a,b) = and(or(x,y), not(and(x,y))) }
[
 $  { x y y }
 @  { y y x }
 $  { y y x x }
 @  { y x x y }
 |  { y x A }
 @  { x A y }
 @  { A y x }
 &  { A B }
 ~  { A C }
 &  { D }
]x:

0 0 x;! 0 = t;!
0 1 x;! 1 = t;!
1 0 x;! 1 = t;!
1 1 x;! 0 = t;!


{ max(x,y) }
[
 1ø1ø  { x y x y }
 >     { x y T }
 $ [
  @@   { T x y }
  %    { T x }
  \    { x T }
 ]?
 ~ [
  \    { y x }
  %    { y }
 ]?
]m:

2_ 3_ m;! 2_= t;!
3_ 2_ m;! 2_= t;!
1_ 0  m;! 0 = t;!
0  1_ m;! 0 = t;!
2  3  m;! 3 = t;!
3  2  m;! 3 = t;!

