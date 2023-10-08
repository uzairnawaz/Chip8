#include "Chip8.h"

Chip8::Chip8()
{
    initializeFonts();

    // initialize random seed
    srand((unsigned)std::time(NULL));

    program_counter = 512;
}

void Chip8::initializeFonts()
{
    int fonts[] = { 0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
                    0x20, 0x60, 0x20, 0x20, 0x70,   // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F

    for (int i = 0; i < 80; i++)
    {
        memory[i + 0x50] = fonts[i];
    }
}

void Chip8::tick()
{
    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) sound_timer--;
}

void Chip8::execute()
{
    unsigned __int16 instruction = fetch();
    program_counter += 2; // move pointer to the next instruction

    switch (getHalfByte(instruction, 0))
    {
    case 0:
        if (instruction == 0x00E0)
        {
            clearScreen();
        }
        else if (instruction == 0x00EE)
        {
            program_counter = stack.top();
            stack.pop();
        }
        break;
    case 1:
        program_counter = instruction & 0x0FFF;
        break;
    case 2:
        stack.push(program_counter);
        program_counter = instruction & 0x0FFF;
        break;
    case 3:
    {
        int reg = getHalfByte(instruction, 1);
        if (registers[reg] == (instruction & 0x00FF))
        {
            program_counter += 2;
        }
        break;
    }
    case 4:
    {
        int reg = getHalfByte(instruction, 1);
        if (registers[reg] != (instruction & 0x00FF))
        {
            program_counter += 2;
        }
        break;
    }
    case 5:
    {
        int reg1 = getHalfByte(instruction, 1);
        int reg2 = getHalfByte(instruction, 2);
        if (registers[reg1] == registers[reg2])
        {
            program_counter += 2;
        }
        break;
    }
    case 9:
    {
        int reg1 = getHalfByte(instruction, 1);
        int reg2 = getHalfByte(instruction, 2);
        if (registers[reg1] != registers[reg2])
        {
            program_counter += 2;
        }
        break;
    }
    case 6:
    {
        int reg = getHalfByte(instruction, 1);
        registers[reg] = instruction & 0x00FF;
        break;
    }
    case 7:
    {
        int reg = getHalfByte(instruction, 1);
        registers[reg] += instruction & 0x00FF;
        break;
    }
    case 8:
    {
        int mode = getHalfByte(instruction, 3);
        int reg1 = getHalfByte(instruction, 1);
        int reg2 = getHalfByte(instruction, 2);
        switch (mode)
        {
        case 0:
            registers[reg1] = registers[reg2];
            break;
        case 1:
            registers[reg1] = registers[reg1] | registers[reg2];
            break;
        case 2:
            registers[reg1] = registers[reg1] & registers[reg2];
            break;
        case 3:
            registers[reg1] = registers[reg1] ^ registers[reg2];
            break;
        case 4:
            if (registers[reg1] > 255 - registers[reg2])
            {
                registers[15] = 1;
            }
            registers[reg1] = registers[reg1] + registers[reg2];
            break;
        case 5:
            registers[15] = (registers[reg1] >= registers[reg2]) ? 1 : 0;
            registers[reg1] = registers[reg1] - registers[reg2];
            break;
        case 7:
            registers[15] = (registers[reg1] <= registers[reg2]) ? 1 : 0;
            registers[reg1] = registers[reg2] - registers[reg1];
            break;
        case 6:
            registers[15] = registers[reg1] & 1;
            registers[reg1] >>= 1;
            break;
        case 0xE:
            registers[15] = (registers[reg1] & (1 << 7)) >> 7;
            registers[reg1] <<= 1;
            break;
        }
        break;
    }
    case 0xA:
        idx_register = instruction & 0x0FFF;
        break;
    case 0xB:
        program_counter = (instruction & 0x0FFF) + registers[0];
        break;
    case 0xC:
    {
        int reg = getHalfByte(instruction, 1);
        registers[reg] = genRandomNumber() & (instruction & 0x00FF);
        break;
    }
    case 0xD:
        displaySprite(instruction);
        break;
    case 0xE:
    {
        int key = registers[getHalfByte(instruction, 1)];
        switch (instruction & 0x00FF)
        {
        case 0x9E:
            if (keyPressed[key])
            {
                program_counter += 2;
            }
            break;
        case 0xA1:
            if (!keyPressed[key])
            {
                program_counter += 2;
            }
            break;
        }
        break;
    }
    case 0xF:
    {
        int reg = getHalfByte(instruction, 1);
        switch (instruction & 0x00FF)
        {
        case 0x07:
            registers[reg] = delay_timer;
            break;
        case 0x15:
            delay_timer = registers[reg];
            break;
        case 0x18:
            sound_timer = registers[reg];
            break;
        case 0x1E:
            idx_register += registers[reg];
            if (idx_register > 0x0FFF)
            {
                registers[15] = 1;
            }
            break;
        case 0x0A:
            program_counter -= 2;
            for (int i = 0; i < 16; i++)
            {
                if (keyPressed[i])
                {
                    program_counter += 2;
                    registers[reg] = i;
                    break;
                }
            }
            break;
        case 0x29:
            idx_register = 0x50 + (registers[reg] & 0x0F) * 5;
            break;
        case 0x33:
        {
            int num = registers[reg];
            memory[idx_register] = num / 100;
            num %= 100;
            memory[idx_register + 1] = num / 10;
            num %= 10;
            memory[idx_register + 2] = num;
            break;
        }
        case 0x55:
            storeMemory(reg);
            break;
        case 0x65:
            loadMemory(reg);
            break;
        }
        break;
    }
    }
}

unsigned __int16 Chip8::fetch()
{
    // combine two bytes from memory into a single 16-bit integer
    return ((unsigned __int16)(memory[program_counter]) << 8) + memory[program_counter + 1];
}

unsigned __int8 Chip8::getHalfByte(unsigned __int16 instruction, int idx)
{
    return (instruction & (0xF << ((3 - idx) * 4))) >> ((3 - idx) * 4);
}

void Chip8::storeMemory(int lastRegister)
{
    for (int i = 0; i <= lastRegister; i++)
    {
        memory[idx_register + i] = registers[i];
    }
}

void Chip8::loadMemory(int lastRegister)
{
    for (int i = 0; i <= lastRegister; i++)
    {
        registers[i] = memory[idx_register + i];
    }
}

void Chip8::displaySprite(unsigned __int16 instruction)
{
    // read sprite x, y positions from the specified registers
    int posx = registers[getHalfByte(instruction, 1)] % SCREEN_WIDTH;
    int posy = registers[getHalfByte(instruction, 2)] % SCREEN_HEIGHT;

    int numRows = getHalfByte(instruction, 3);
    // stores whether any pixels have been turned OFF as a result of displaying this sprite
    bool flag = false; 
    for (int y = 0; y < numRows; y++)
    {
        if (posy + y >= SCREEN_HEIGHT)
        {
            break;
        }
        // read one row of sprite data from memory
        unsigned __int8 sprite_row = memory[idx_register + y];
        for (int x = 0; x < 8; x++)
        {
            if (posx + x >= SCREEN_WIDTH)
            {
                break;
            }
            // get the value of the specific pixel from the sprite row data
            bool sprite_pixel = (sprite_row & (1 << (7 - x))) >> (7 - x);

            // if this sprite will cause the pixel to go from on -> off, set flag to true
            flag |= (sprite_pixel == true && display[posy + y][posx + x] == true);
            display[posy + y][posx + x] ^= sprite_pixel;
        }
    }
    registers[15] = flag;
}

void Chip8::clearScreen()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            display[y][x] = false;
        }
    }
}

unsigned __int8 Chip8::genRandomNumber()
{
    return rand() % 256;
}

bool Chip8::loadROM(std::string path)
{
    std::ifstream rom(path, std::ios::binary);
    if (!rom.is_open())
    {
        return false;
    }
    
    // load ROM into memory
    int idx = 512;
    while (rom)
    {
        memory[idx] = rom.get();
        idx++;
    }

    rom.close();
    return true;
}

bool Chip8::getPixel(int x, int y)
{
    return display[y][x];
}

void Chip8::setKey(int k, bool pressed)
{
    keyPressed[k] = pressed;
}

bool Chip8::playingSound()
{
    return sound_timer > 0;
}