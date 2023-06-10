#pragma once
#include <SFML\Network.hpp>
#include "entities.h"
#include "utils.h"
#include <iostream> 
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

class GameConnections
{
	// GUI VARIABLES
	sf::Texture characterTex;
	sf::Texture bg;
	sf::Sprite sprite;
	sf::String message;
	sf::Text text;
	sf::Text nameText;
	sf::Font font;
	sf::Event event;
	sf::RectangleShape nameRectangle;
	sf::String input;

	// GAME VARIABLES
	sf::Vector2f cDir;
	Character character = Character(sf::Vector2f(10, 10), 1);
	std::vector<Bullet> bullets;  // Bullet container to manage them
	Character character2 = Character(sf::Vector2f(40, 40), 2);
	std::vector<Bullet> bullets2; // Bullet container to manage them
	std::string challengeWord = "OSADFWE";


public:
	sf::RenderWindow window;
	std::string name;
	bool playing = false;
	bool challenge = false;
	bool shooting = false;
	bool pingPong = false;
	bool chooseGame = false;
	bool chooseSetUp = false;
	bool joined = false;
	bool created = false;
	
	void ClientSetup();
	void ChallengeSetUp();
	void RunConnections();
	void ChooseGameSetUp();
};

