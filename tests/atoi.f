{ atoi }

{ isspace }
[
 $ 9=
 £10=|
 £11=|
 £12=|
 £13=|
 \32=|
]w:

{ accumulator }
0

{ skip whitespace }
[^$w;!][]#

{ accept 0..9 }
{ multiply accumulator }
{ add character }
[$'0≥ £'9≤ &][\10*\'0-+ ^]#%

$1-.',,
$  .',,
 1+.
10,
