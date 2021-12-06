[[config]]
name optimizable
input a, b
output c

[[body]]
var d = a + b #the output variable does not depend on this, so we can remove it

(a + b) * 2 #this statement does not affect the global state, so we remove it (done)

if false    #this conditional will never be executed, remove it
    let d2 = ((a!) + (b!))!

    (a + b) * 2 #although this statement is hiding in another scope, we can still remove it (done)
endif

if true #this conditional will always be executed, inline it
    let d2 = a! + b!

    if false
        (a + b) * 2
    endif
endif

c = a + b