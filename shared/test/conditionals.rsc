[[config]]
name conditionals
input _a b
output c

[[body]]
var a = _a
if a < 0
    a *= -1
endif


#uncomment for satisfying AST
#if true
#    if false
#        if true
#            if false
#                if true
#                    if false
#                        if true
#                            if false
#                                if true
#                                    if false
#                                        if true
#                                            if false
#                                                if true
#                                                    if false
#                                                        if true
#                                                            if false
#                                                                if true
#                                                                    if false
#                                                                        if true
#                                                                            if false
#                                                                            endif
#                                                                        endif
#                                                                    endif
#                                                                endif
#                                                            endif
#                                                        endif
#                                                    endif
#                                                endif
#                                            endif
#                                        endif
#                                    endif
#                                endif
#                            endif
#                        endif
#                    endif
#                endif
#            endif
#        endif
#    endif
#endif