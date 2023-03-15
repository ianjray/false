{ number of lines to show }
3l:

{ newlines }
0n:

{ read stdin to stack }
[^$1_≠]
[
 { count newlines "n" }
 $10= _n;+n:

 { while (n > l) }
 [n;l;>]
 [
  { if (pop bottom of stack == newline) n-- }
  §1-™10= n;+n:
 ]#
]#%

{ tail }
®
[§0≠][,]#
