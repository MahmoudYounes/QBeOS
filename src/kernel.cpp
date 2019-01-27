int main()
{
    int* videoRamAdress = (int *)0xb800;
    const char *welcomeMessage = "Welcome To BeOS\0";

    while( *welcomeMessage != 0)
    {
        *videoRamAdress++ = *welcomeMessage++;
        *videoRamAdress++ = 0x70;
    }
    return 0;
}
