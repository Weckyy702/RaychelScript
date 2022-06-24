[[config]]
input a b
output c

[[body]]

fn twelve()
    return 12
endfn

fn eleven() = 11

fn f(x, y) = x*y

#overloading should be a thing
fn f(x) = f(x, 2)

fn g(x, y)
    if y < 0
        return -x
    endif
    return x
endfn

c = f(g(a, b), f(12))
c = eleven() * twelve() + f(g(b, a))
