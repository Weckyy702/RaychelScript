[[config]]
input n
output fib

[[body]]
fn fib(n)
    if n < 2
        return n
    endif
    return fib(n-1) + fib(n-2)
endfn

fn sum(n)
    if n == 1
        return 1
    endif
    return sum(n-1) + n
endfn

fib = fib(n)
