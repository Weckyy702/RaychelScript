[[config]]
name conditionals
input a b
output c

[[body]]
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
