#include "include/strings.h"

void memcpy(void *dstpp, void *srcpp, size_t count) {
  unsigned long int dst_bp = (long int)dstpp;
  unsigned long int src_bp = (long int)srcpp;
  unsigned long int nbytes = count;

  int __d0;
  __asm__ __volatile__(
      /* Clear the direction flag, so copying goes forward. */
      "cld\n"
      /* Copy bytes.  */
      "rep\n"
      "movsb"
      : "=D"(dst_bp), "=S"(src_bp), "=c"(__d0)
      : "0"(dst_bp), "1"(src_bp), "2"(nbytes)
      : "memory");
}

