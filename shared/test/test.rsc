[[config]]
name alsdkjfsdaklj
input x, y, z, r
output d
precision single

#this is a comment

[[body]]
d = (x^2 + y^2 + z^2)^0.5 - r

if d < 0 
    d = 0
endif