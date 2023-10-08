#pragma once

#include <stack>
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>

class Chip8
{
public:
	static const int SCREEN_WIDTH = 64;
	static const int SCREEN_HEIGHT = 32;

	Chip8();

	/**
	 * Updates internal timers. Should be executed 60 times per second to accurately maintain timers.
	 */
	void tick();

	/**
	 * Executes the next instruction in memory
	 */
	void execute();

	/**
	 * Load ROM into memory from an input file
	 * Params
	 *   std::string path - file path for ROM
	 *
	 * Returns a boolean if loading was successful
	 */
	bool loadROM(std::string path);

	/**
	 * Get the value of a pixel on the screen
	 * Params
	 *   int x - screen x value (0 <= x < SCREEN_WIDTH)
	 *   int y - screen y value (0 <= y < SCREEN_HEIGHT)
	 *
	 * Returns a boolean representing whether the pixel is on or off at that location
	 */
	bool getPixel(int x, int y);

	/**
	 * Update the state of a key on the Chip8 system.
	 * Params
	 *   int k - key that is being updated (0 <= k <= 0xF since Chip8 has 16 keys)
	 *   bool pressed - should be true if the key is being pressed and false otherwise.
	 */
	void setKey(int k, bool pressed);

	/**
	 * Returns true if sound should be playing, false otherwise.
	 */
	bool playingSound();

private:
	unsigned __int16 idx_register;
	unsigned __int16 program_counter;
	unsigned __int8 delay_timer;
	unsigned __int8 sound_timer;
	unsigned __int8 registers[16];
	std::stack<unsigned __int16> stack;
	unsigned __int8 memory[4096];
	bool display[SCREEN_HEIGHT][SCREEN_WIDTH];
	bool keyPressed[16];

	/**
	 * Load font data into memory
	 */
	void initializeFonts();

	/**
	 * Fetch the next instruction and return it as a 16 bit integer
	 */
	unsigned __int16 fetch(); 

	/**
	 * Clear the internal display array
	 */
	void clearScreen();

	/**
	 * Returns a randomly generated 8 bit integer
	 */
	unsigned __int8 genRandomNumber(); 

	/**
	 * Returns a specific 4 bits from a 16 bit instruction
	 * EX:
	 *   if instruction is: 00010010 00110100 
	 *   getHalfByte(instruction, 0) -> 0b0001 // first four bits
	 *   getHalfByte(instruction, 1) -> 0b0010 // second set of bits
	 *   getHalfByte(instruction, 2) -> 0b0011 // third set of bits
	 *   getHalfByte(instruction, 3) -> 0b0100 // fourth set of bits
	 * 
	 * Params
	 *   unsigned __int16 instruction - the instruction to extract the half byte from
	 *   int idx - the index of the half byte to extract (0 <= idx < 4)
	 * 
	 * Returns the value of the half byte (range of [0x0, 0xF])
	 */
	unsigned __int8 getHalfByte(unsigned __int16 instruction, int idx);

	/**
	 * Stores the value of all registers from [0, lastRegister] in memory starting
	 * at memory address stored in idx_register
	 * Params
	 *   int lastRegister - index of the last register that will be stored in memory (0 < lastRegister < 0xF)
	 */
	void storeMemory(int lastRegister);

	/**
	 * Loads the value of all registers from [0, lastRegister] from memory starting
	 * at memory address stored in idx_register
	 * Params
	 *   int lastRegister - index of the last register that will loaded from memory (0 < lastRegister < 0xF)
	 */
	void loadMemory(int lastRegister);

	/**
	 * Process the display instruction
	 * Params
	 *   unsigned __int16 instruction - the display instruction
	 */
	void displaySprite(unsigned __int16 instruction);
};

