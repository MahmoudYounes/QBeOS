#ifndef ASM_H
#define ASM_H

#define OutputPort(port, val) asm("outb " ## port ## "," ## val)


#endif /* ASM_H */
