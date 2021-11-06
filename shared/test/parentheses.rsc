[[config]]
name parentheses
input
output

[[body]]
#these should all be equally valid
[a + b] * c
(a + b) * c
{a + b} * c

#unfortunately, these are now also valid. I don't think they should
[a + b) * c
{a + b) * c
[a + b} * c