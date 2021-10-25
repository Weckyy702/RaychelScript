[[config]]
input a b
output c

[[body]]
let d = a + b
var d2 = 2 * d
d2 = d2 * d
d2 = d2 * 3.5
c = d

#these should be equivalent in the AST
a + b * c
c * b + a

#these shouldn't
a - b * c
b * c - a