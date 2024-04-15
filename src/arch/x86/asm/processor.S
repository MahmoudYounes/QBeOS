    .file "processor.S"
    .text
    .global readIP
    .type readIP,@function
    .intel_syntax noprefix
    .code32
readIP:
    push ebp
    mov ebp, esp
    call catchIP
catchIP:
    pop eax  # return in eax

    pop ebp
    ret
