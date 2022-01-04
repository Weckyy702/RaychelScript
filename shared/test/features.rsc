[[config]]
input a b c
output d

[[body]]
let i       #constant, initialized to 0
var j = 1   #variable, initialized 1

let e = (j + i) ^ 2 #assignment expression, rhs is arithmetic expression
e *= 2 #update expresssion
e = |e| #absolute value
e = e! #factorial expression

if e < 0 #conditional construct / comparison
    e = -1 #unary minus
endif

while false #loop / literal true there is also false
    j += 1
endwhile