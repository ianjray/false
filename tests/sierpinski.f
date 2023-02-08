{ usage: sierpinski.f order }

{ print spaces; in:num }
[
 [$0>]
 [" " 1-]#%
]w:

{ left shift; in:x,y out:x<<y }
[
 [$0>]
 [\2*\ 1-]#%
]l:

1b;l;!        { SIZE }
$             { y = SIZE }
[$0>]         { y > 0 }
[
 1-           { y-- }
 $w;!         { w(y) }
 1O           { x = SIZE }
 [$0>]        { x > 0 }
 [
  1-          { x-- }
  1O$2O\-&0=  { !((x - y) & y) }
  $ ["* "]?
  ~ ["  "]?
 ]#
 %            { x }
 10,          { newline }
]#
%             { y }
%             { SIZE }
