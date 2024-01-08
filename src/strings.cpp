#include "strings.h"

// TODO: resolve this issue. This implementation is copy pasted from glibc BYTE_COPY_FWD macro implementation.
// the original glibc memcpy is more optimized. TODO: either port glibc to this OS or remove this implementation,
// and provide an implementation more optimzed.
extern "C" void memcpy(void *dstpp, void *srcpp, size_t count){
   unsigned long int dst_bp = (long int) dstpp;
   unsigned long int src_bp = (long int) srcpp;
   unsigned long int nbytes = count;

   int __d0;
   __asm__ __volatile__(
         /* Clear the direction flag, so copying goes forward. */
        "cld\n"
        /* Copy bytes.  */
        "rep\n"
        "movsb" :
        "=D" (dst_bp), "=S" (src_bp), "=c" (__d0) :
        "0" (dst_bp), "1" (src_bp), "2" (nbytes) :
        "memory");
}

extern "C" void memset(void *dstpp, uint8_t val, size_t count) {
   uintptr_t dst_bp = (uintptr_t) dstpp;
   __asm__ __volatile__(
      "cld\n\t"
      "mov al, %1\n\t"
      "rep\n\t" // repeat with the value in (e)cx
      "stosb\n\t"
      :
      : "D" (dst_bp), "a"(val), "c"(count)
   );
}
