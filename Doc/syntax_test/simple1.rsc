[[config]]
input: x, y, z, r
output: d

[[body]]
d = sqrt(x**2 + y**2 + z**2) - r

#this is a comment
#A script consists of at least two BLOCKs: [[config]] and [[body]]
#The [[config]] block contains metadata:
# -input: List of input variables for the script. Must be set by the runtime
# -output: List of output variables for the script. Will be zero if not set during execution of the script
# All metadata is available to the calling context as a Key-Value store
# Possible default metadata: name, description, comparison specification
