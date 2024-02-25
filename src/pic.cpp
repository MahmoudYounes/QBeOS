#include "pic.h"

PIC::PIC(){
    // intel chips normally come with 2 pics to act as master and slave. this is called legace interrupt delivery mechanism.
    // this is deprecated in favor of the more modern APIC interrupt delivery mechanism. QBeOS is NOT BACKWARDS COMPATIBLE and
    // will not be. hence, we are disabling the PIC intentionally.
    //
    // The PIC has 4 serial addresses 0x20 0x21 for commands for master and slave and 0xA0 0xA1 for data for master and slave
    kprint("Disabling PIC...");
    outb(0x21, 0xff); // mask all interrupts on the master chip
    outb(0xA1, 0xff); // mask all interrupts on the slave chip
}

PIC pic;
