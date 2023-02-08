{ number of lines to show }
3l:

{ characters }
0c:

{ newlines }
0n:

{ read stdin to stack }
{ count characters "c" and newlines "n" }
[^$1_=~]
[
 c;1+c:
 $ 10= _n;+n:
]#%

{ determine how many lines to drop "d" }
{ d = n - l;
  if (0 > d) d = 0 }
n;l;-$d: 0\> [ 0d: ]?

c;x:

[c;0=~ c;1-c:]
[
 { pick character at index c }
 c;O

 { if (d == 0) emit character else if (character == \n) d-- }
 d;0=
 $ [ \ , ]?
 ~ [ 10= d;+d: ]?
]#

{ discard stack }
[x;0> $x;+x:][%]#
