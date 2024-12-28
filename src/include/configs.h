#ifndef CONFIG_H
#define CONFIG_H

/**
 * this file contains the configuration that the QBeKERN will be built with
 * these configurations should ideally come at buildtime from the make command
 * since I don't want to spend time on this, here is another TODO: move this
 * to be supported by make flags enabled during the build time
*/

// if set, disables the initialization of PIC and masks all interrupts from it
#define DISABLE_PIC false 

// if set, disables the initialization of the IOAPIC and masks all interrupts
// from it
#define DISABLE_APIC true 

// if set, QBeKern is built as a 32 bit kernel
#define ARCH_X86_32 true

// if set, QBeKern is built as a 64 bit kernel
#define ARCH_X86_64 false

// if set, QBeKern is built as an arm 32 bit kernel
#define ARCH_ARM32 false

// if set, QBeKern is built as an arm 64 bit kernel
#define ARCH_ARM64 false

#define DEBUG false

#endif


