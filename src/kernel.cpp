/*
    currently, this code gets loaded correctly to the memory all we need to do is have this code work
*/
void kmain()
{
    char *videoRamAdress = (char *)0xb800;
    char *welcomeMessage = "Welcome To BeOS\0";
    while( *welcomeMessage != '\0')
    {
        *videoRamAdress++ = *welcomeMessage++;
        *videoRamAdress++ = 0x70;
    }
    asm("hlt");
}
