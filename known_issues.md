List of known issues in QBeOS that needs handling:
* BIOS is deprecated. Need to switch to UEFI.
-> this switch involves using the new ROM image in Bochs and arriving to the kernel again. The switch will be done after I have a decent implementation of working drivers for disk and mmu and so on.
   expected changes:
   * QBeOS ISO image structure
   * QBeOS BL as a whole.. the one nice thing is that the BL can be written in C/C++ since gcc supports generating a PE file.
   materials for reading:
   * https://uefi.org/sites/default/files/resources/ACPI_Spec_6_5_Aug29.pdf
   * https://wiki.osdev.org/UEFI_Bare_Bones
* Loading the kernel when it spans more than 1 sector.
-> Currently, there is code to calculate how many sectors does the kernel span, but it does not act upon the results :D. currently the kernel is 1.2Kb. once we move beyond the 2kb limit which is soon
   I need to resolve this issue. the issue is resolved by loading consecutive sectors and taking care of the buffer addresses.
* a decent ISO 9660 fs implementation
