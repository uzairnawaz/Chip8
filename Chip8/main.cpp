#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include "Chip8.h"

// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

void handleInputs(Chip8* c8)
{
    c8->setKey(0x1, sf::Keyboard::isKeyPressed(sf::Keyboard::Num1));
    c8->setKey(0x2, sf::Keyboard::isKeyPressed(sf::Keyboard::Num2));
    c8->setKey(0x3, sf::Keyboard::isKeyPressed(sf::Keyboard::Num3));
    c8->setKey(0xC, sf::Keyboard::isKeyPressed(sf::Keyboard::Num4));
    c8->setKey(0x4, sf::Keyboard::isKeyPressed(sf::Keyboard::Q));
    c8->setKey(0x5, sf::Keyboard::isKeyPressed(sf::Keyboard::W));
    c8->setKey(0x6, sf::Keyboard::isKeyPressed(sf::Keyboard::E));
    c8->setKey(0xD, sf::Keyboard::isKeyPressed(sf::Keyboard::R));
    c8->setKey(0x7, sf::Keyboard::isKeyPressed(sf::Keyboard::A));
    c8->setKey(0x8, sf::Keyboard::isKeyPressed(sf::Keyboard::S));
    c8->setKey(0x9, sf::Keyboard::isKeyPressed(sf::Keyboard::D));
    c8->setKey(0xE, sf::Keyboard::isKeyPressed(sf::Keyboard::F));
    c8->setKey(0xA, sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
    c8->setKey(0x0, sf::Keyboard::isKeyPressed(sf::Keyboard::X));
    c8->setKey(0xB, sf::Keyboard::isKeyPressed(sf::Keyboard::C));
    c8->setKey(0xF, sf::Keyboard::isKeyPressed(sf::Keyboard::V));
}

int main(int argc, char* argv[])
{
    int pixel_size = 20;
    sf::RenderWindow window(sf::VideoMode(64 * pixel_size, 32 * pixel_size), "Chip8 Emulator");

    Chip8* c8 = new Chip8();
    if (argc != 2 || (argc == 2 && strcmp(argv[1], "-help") == 0))
    {
        std::cout << "Syntax: Chip8 [rom file]\n";
        return 0;
    }
    
    if (!c8->loadROM(argv[1])) {
        
        std::cout << "Error opening ROM file!";
        return -1;
    }


    sf::VertexArray verts(sf::Quads, 4 * 32 * 64);

    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            int idx = 4 * (y * 64 + x);
            verts[idx    ].position = sf::Vector2f((x    ) * pixel_size, (y    ) * pixel_size);
            verts[idx + 1].position = sf::Vector2f((x + 1) * pixel_size, (y    ) * pixel_size);
            verts[idx + 2].position = sf::Vector2f((x + 1) * pixel_size, (y + 1) * pixel_size);
            verts[idx + 3].position = sf::Vector2f((x    ) * pixel_size, (y + 1) * pixel_size);
        }
    }

    sf::SoundBuffer buffer;
    
    buffer.loadFromFile("beep.wav");
    sf::Sound beep;
    beep.setBuffer(buffer);

    sf::Clock execClock; 
    sf::Clock timerClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        handleInputs(c8);

        if (execClock.getElapsedTime().asMicroseconds() > 1428) // ~700 instructions per second
        {
            c8->execute();
            for (int i = 0; i < 16; i++)
            {
                c8->setKey(i, false);
            }
            
            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 64; x++)
                {
                    sf::Color c = c8->getPixel(x, y) ? sf::Color::White : sf::Color::Black;
                    int idx = 4 * (y * 64 + x);
                    verts[idx].color = c;
                    verts[idx + 1].color = c;
                    verts[idx + 2].color = c;
                    verts[idx + 3].color = c;
                }
            }
            
            execClock.restart();
        }

        if (timerClock.getElapsedTime().asMilliseconds() > 16) // ~60 times per second
        {
            c8->tick();
            timerClock.restart();
        }

        if (c8->playingSound())
        {
            beep.play();
        }
        else
        {
            beep.stop();
        }

        window.clear();
        window.draw(verts);
        window.display();
    }

    return 0;
}