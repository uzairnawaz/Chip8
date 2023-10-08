#pragma once

#include <stack>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

class Chip8
{
private:
	unsigned __int16 idx_register;
	unsigned __int16 program_counter;
	unsigned __int8 delay_timer;
	unsigned __int8 sound_timer;
	unsigned __int8 registers[16];
	std::stack<unsigned __int16> stack;
	unsigned __int8 memory[4096];
	bool display[32][64];
	bool keyPressed[16];

	void initializeFonts();
	unsigned __int16 fetch(); // fetch the current instruction from memory
	void clearScreen();
	unsigned __int8 genRandomNumber(); 
	unsigned __int8 getHalfByte(unsigned __int16 instruction, int idx);
	void storeMemory(int lastRegister);
	void loadMemory(int lastRegister);
	void displaySprite(unsigned __int16 instruction);

public:
	Chip8();
	void tick(); // Must be called 60 times per second to update timers
	void execute(); // Executes 1 instruction
	bool loadROM(std::string path);
	bool getPixel(int x, int y);
	void setKey(int k, bool pressed);
	bool playingSound();
};

