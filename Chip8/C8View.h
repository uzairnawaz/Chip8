#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Chip8.h"

class C8View
{
public:
	C8View();

	/**
	 * Loads ROM from a given path
	 * Params
	 *   std::string path - path to ROM file
	 * 
	 * Returns true if successful, false otherwise
	 */
	bool loadROM(std::string path);

	/**
	 * Begin the event loop that processes inputs,
	 * updates display, and executes instructions
	 */
	void eventLoop();

private:
	Chip8 c8;
	sf::RenderWindow window;
	sf::VertexArray verts;
	sf::Sound beep;
	sf::Clock execClock;
	sf::Clock timerClock;

	/**
	 * Passes inputs to the Chip8 object
	 */
	void handleInputs();
};

