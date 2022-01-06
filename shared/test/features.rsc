[[config]]
input a b c
output d
#TODO: there can be any number of identifiers in a config block
name feature_test

[[body]]
let i       #constant, implicitly initialized to 0
var j = 1   #variable, explicitly initialized to 1
let _j2 = 12 #constant, identifier contains a number and an underscore

let e = (j + i) ^ 2 #assignment expression, rhs is arithmetic expression also illegal identifier
e *= 2 #update expresssion
e = |e| #absolute value
e = e! #factorial expression

if e < 0 #conditional construct / comparison
    e /= -1 #unary minus
endif

while false #loop / literal false there is also true
    j += 1
endwhile