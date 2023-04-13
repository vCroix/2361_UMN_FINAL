.include "xc.inc"

.text                       ;BP (put the following data in ROM(program memory))

; This is a library, thus it can *not* contain a _main function: the C file will
; define main().  However, we will need a .global statement to make available ASM
; functions to C code.
; All functions utilized outside of this file will need to have a leading 
; underscore (_) and be included in a comment delimited list below.
.global _wait100us, _wait1ms, _wait4cycle

    
_wait100us: ; 2 (total needed is 1600)
    repeat #1593 ; 1
    nop ; 1593 + 1
    return ;3

_wait1ms: ; 2 (total is 16000)
    repeat #15993 ; 1
    nop ; 15993 +1
    return ; 3
 
_wait4cycle:
    repeat #1
    nop
    return



