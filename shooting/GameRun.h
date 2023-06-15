#pragma once
#include "entities.h"
#include "utils.h"
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>

class GameRun
{
	// GUI VARIABLES
	sf::Texture characterTex;
	sf::Texture bg;
	sf::Sprite sprite;
	sf::Event event;
	sf::String input;


	// GAME VARIABLES
	sf::Vector2f cDir;
	Character character = Character(sf::Vector2f(10, 10), 1);
	std::vector<Bullet> bullets;  // Bullet container to manage them
	Character character2 = Character(sf::Vector2f(40, 40), 2);
	std::vector<Bullet> bullets2; // Bullet container to manage them

public:
	sf::RenderWindow window;
	std::string host = "a";
	std::string guest = "a";
	void updateGame();
	void setupGame();
};

