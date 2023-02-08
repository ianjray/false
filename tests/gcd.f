10 15   { a b }

[$0=~]  { a b b }
[
 $@     { t b a }
 $@     { t a a b }
 $@     { t a b b a }
 \      { t a b a b }
 /      { t a b N      N = a / b }
 *      { t a N        N = b * N }
 -      { t N          N = a - N or N = a mod b }
]#

%       { a }
.       { }
10,
