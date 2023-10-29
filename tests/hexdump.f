{ hexdump -C }

{ format %x }
[$9> [10-'a+ ,] ['0+ ,] ¿] d:


[
 a;16≤ ["00000000"] ?

 { reverse }
 0i: [i;n;<] [i;™ i;1+i:] #

 { duplicate }
 0i: [i;n;<] [n;1-ø i;1+i:] #

 { asciihex section }
 "  "
 0i: [i;16<]
 [
  i;n;< [16÷ d;! d;! " "] ["   "] ¿
  i;1+$i:
  8= [" "] ?
 ] #

 { character section }
 " |"
 [n;0≠ $n;+n:] [$32<£126>| [%'.] ? ,] #
 "|" 10,

 { beginning of line: format address %08x }
 a;
 16 16 16 16 16 16 16 ****** ÷ d;!
    16 16 16 16 16 16  ***** ÷ d;!
       16 16 16 16 16   **** ÷ d;!
          16 16 16 16    *** ÷ d;!
             16 16 16     ** ÷ d;!
                16 16      * ÷ d;!
                   16        ÷ d;!
                               d;!
]o:


0a:
0n:

{ fragments of 16 characters }
[^$1_=~] [a;1+a: n;1+n: n;16= [o;!] ?] # %

{ final fragment }
n;0> [o;!] ?

a;0> [10,] ?
