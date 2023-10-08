
#include <iostream>
#include "C8View.h"

// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

int main(int argc, char* argv[])
{
    
    if (argc != 2 || (argc == 2 && strcmp(argv[1], "-help") == 0))
    {
        std::cout << "Syntax: Chip8 [rom file]\n";
        return 0;
    }

    C8View c8;
    if (!c8.loadROM(argv[1]))
    {
        std::cout << "Error opening ROM file!";
        return -1;
    }

    c8.eventLoop();

    return 0;
}