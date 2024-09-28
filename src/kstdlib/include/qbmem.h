#ifndef QBEMEM_H
#define QBEMEM_H


/**
  * this is left here intentionally
  * this class should manage memory on top of VMM. all VMM does is allocate and
  * free pages of memory. this is a memory waste.
  * how other OSes manage it:
  * - linux: Buddy system allocator & slab
  * - FreeBSD: Mach mm
  * for now keep all locations going through new and delete operators. once this
  * module is ready, you will allocate lesser memory without anything extra.
  *
  */

class QBMem{
};

#endif
