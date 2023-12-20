#include "strings.h"

// TODO: resolve this issue. This implementation is copy pasted from glibc BYTE_COPY_FWD macro implementation.
// the original glibc memcpy is more optimized. TODO: either port glibc to this OS or remove this implementation,
// and provide an implementation more optimzed.
extern "C" void memcpy(void *dstpp, void *srcpp, size_t count){
    unsigned long int dst_bp = (long int) dstpp;
    unsigned long int src_bp = (long int) srcpp;
    unsigned long int nbytes = count;

    int __d0;
    asm volatile(
         /* Clear the direction flag, so copying goes forward.  */
         "cld\n"
         /* Copy bytes.  */
         "rep\n"
         "movsb" :
         "=D" (dst_bp), "=S" (src_bp), "=c" (__d0) :
         "0" (dst_bp), "1" (src_bp), "2" (nbytes) :
         "memory");
}
