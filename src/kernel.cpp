/*
    currently, this code gets loaded correctly to the memory all we need to do is have this code work
*/
void kmain()
{
    // volatile char *videoRamAdress = (volatile char *)0xb8000;
    // const char *welcomeMessage = "Welcome To BeOS\0";
    // while(*welcomeMessage != 0)
    // {
    //     *videoRamAdress++ = *welcomeMessage++;
    //     *videoRamAdress++ = 0x07;
    // }
    *((int*)0xb8000)=0x07690748;
    asm("hlt");
    // asm("mov ax, 0xb8000;"
    //     "mov %ax, %es;"
    //     "xor %bx, %bx;"
    //     "mov byte (%es,%bx), $0x57;"
    //     "inc %bx;"
    //     "mov byte (%es,%bx), $0x70;"
    //     "hlt");
}
