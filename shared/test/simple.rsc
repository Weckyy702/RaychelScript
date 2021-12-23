[[config]]
name simple
input a b c
output

[[body]]
let d = (a^2 + b^2 + c^2)^0.5 - 2
var d2 = 4
let d1 = 12
d2 = 7 + 2


if d2 < 0   
    d2 = -d2
endif

while d2 < 0
    d2 = -d2
endwhile