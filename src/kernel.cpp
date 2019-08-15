void kmain()
{
    *((int*)0xb8000)=0x07690748;
    asm("hlt");
}
