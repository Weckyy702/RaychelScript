[[config]]
name conditionals
input _a b
output c

[[body]]
var a = _a
if a < 0
    a *= -1
else
    if b > 10
        a *= b
    else
        a /= b
    endif
endif

c = a
