{ Strip redundant whitespace and comments from False programs }

{ isspace }
[ $9= £10=| £11=| £12=| £13=| \32=| ]w:

{ isdigit }
[ $'0≥ \'9≤& ]d:

{ test data: 1<SPC>2 shall not be reduced to 12 }
{ test data: 2<COMMENT>3 shall not be reduced to 23 }
1 2{}3 + + 6 = ∫

{ state: 0:idle 1:digit 2:digit+space }
0

[^$1_≠]
[
 $''=[
  { single-quote; emit next character if not EOF }
  ,
  %0
  ^$1_≠[,]?
 ][
  $d;![
   { emit whitespace before digit in state 2 }
   £2=[' ,]?
   ,
   %1
  ][
   $w;![
    { filter out whitespace }
    %
    $1=[%2]?
   ][
    $'{=[
     { filter out comments }
     %
     $1=[%2]?
     [^$1_≠ \'}≠ &][]#
    ][
     ,
     %0
    ]¿
   ]¿
  ]¿
 ]¿
]#%%
