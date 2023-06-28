#include "game.h"

Game::Game()
{
	window.pollEvent(event);

	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}
	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);
}

Game& Game::operator=(const Game& other)
{
	// Verificar la autoasignación
	if (this == &other) {
		return *this;
	}

	characterTex = other.characterTex;
	bg = other.bg;
	sprite = other.sprite;
	message = other.message;
	text = other.text;
	nameText = other.nameText;
	font = other.font;
	event = other.event;
	nameRectangle = other.nameRectangle;
	input = other.input;

	cDir = other.cDir;
	character = other.character;
	bullets = other.bullets;
	character2 = other.character2;
	bullets2 = other.bullets2;

	return *this;
}

void Game::ClientSetup()
{
	window.create(sf::VideoMode(850, 600), "Game");
	message = "Type in your name to start playing";

	text = sf::Text(message, font, 18);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setStyle(sf::Text::Bold);
	text.setPosition(200, 100);

	input = "";

	nameText = sf::Text(input, font, 18);
	nameText.setFillColor(sf::Color(0, 0, 0));
	nameText.setStyle(sf::Text::Bold);
	nameText.setPosition(220, 130);

	nameRectangle = sf::RectangleShape(sf::Vector2f(400, 30));
	nameRectangle.setFillColor(sf::Color(255, 255, 255, 150));
	nameRectangle.setPosition(200, 130);
}

void Game::ChallengeSetUp()
{
	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);
	message = "Type " + challengeWord + " to complete the challenge";

	text = sf::Text(message, font, 18);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setStyle(sf::Text::Bold);
	text.setPosition(200, 100);

	input = "";

	nameText = sf::Text(input, font, 18);
	nameText.setFillColor(sf::Color(0, 0, 0));
	nameText.setStyle(sf::Text::Bold);
	nameText.setPosition(220, 130);

	nameRectangle = sf::RectangleShape(sf::Vector2f(400, 30));
	nameRectangle.setFillColor(sf::Color(255, 255, 255, 150));
	nameRectangle.setPosition(200, 130);
}

void Game::updateGame()
{
	if (window.pollEvent(event) || startMenu)
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			hasExit = true;
			window.close(); // Close windows if X is pressed 
			break;
		case sf::Event::KeyPressed:
			// Close windows if ESC is pressed
			if (event.key.code == sf::Keyboard::Escape) {
				hasExit = true;
				window.close();  
			}

			// Manage events when playing
			if (playing) { 
				// Checking Movement
				cDir.x = 0;
				cDir.y = 0;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
					cDir.y--;
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					cDir.y++;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					cDir.x--;
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					cDir.x++;
				character.Move(cDir);
				character2.Move(cDir);
				// Managing Shooting
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					cDir.x = 1; // Default shoot direction
					cDir.y = 0; // Default shoot direction

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
						cDir.y = -1;
						cDir.x = 0;
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
						cDir.y = 1;
						cDir.x = 0;
					}
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
						cDir.x = -1;
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
						cDir.x = 1;
					bullets.push_back(Bullet(character.GetPos(), cDir));
					bullets2.push_back(Bullet(character2.GetPos(), cDir));
				}
				break;
			}
			if (!playing) {
				// Manage events when no playing
				if ((event.key.code == sf::Keyboard::Delete || event.key.code == sf::Keyboard::BackSpace) && input.getSize() > 0) {
					input.erase(input.getSize() - 1, input.getSize());
				}
				else if (event.key.code == sf::Keyboard::Return && input.getSize() > 0) {
					if (!challenge && !chooseSetUp) {
						challenge = true;
						name = nameText.getString();
						ChallengeSetUp();
					}
					else if (challenge && nameText.getString() == challengeWord) {
						chooseGame = true;
						challenge = false;
						chooseSetUp = true;
					}
					else if (challenge) {
						ChallengeSetUp();
					}

					if (chooseSetUp && nameText.getString() == "1") // Si pulsa 1 sera para unirse a un juego
					{
						if (!isFirstGame) // Si hay un juego creado lo une 
						{
							//Join Game
							joined = true;
							isP1 = false;
							playing = true;
							chooseSetUp = false;
						}
						else // Sino lo crea 
						{
							//Create Game
							created = true;
							playing = true;
							chooseSetUp = false;
							isP1 = true;
							isFirstGame = false;
						}
					}

					if (chooseSetUp && nameText.getString() == "2") // Si pulsa 2 sera para crear un juego
					{
						//Create Game
						created = true;
						playing = true;
						chooseSetUp = false;
						isP1 = true; // Lo pone como primer jugador
					}
					if (chooseSetUp && (!joined && !created) && startMenu) {
						ChooseGameSetUp();
					}
				}
				else { input += key2str(event.key.code); }
			}
		}
	}
	
	window.clear();
	window.draw(sprite);

	// GIU draw when no playing
	if (!playing) {
		// When no playing
		window.draw(nameRectangle);
		nameText.setString(input);
		window.draw(nameText);
		window.draw(text);
	}
	if (challenge) {
		// When no playing
		window.draw(nameRectangle);
		nameText.setString(input);
		window.draw(nameText);
		window.draw(text);
	}
	if (chooseSetUp && startMenu) {
		// When no playing
		window.draw(nameRectangle);
		nameText.setString(input);
		window.draw(nameText);
		window.draw(text);
	}
	if (playing && startPlaying)
	{
		if (isFirstGame)
		{
			isP1 = true;
			isFirstGame = false;
		}
		// When playing
		if (isP1)
		{
			window.draw(character.GetSprite());
		}
		else
		{
			window.draw(character2.GetSprite());
		}

		// Bullets update
		auto it2 = bullets.begin();
		while (it2 != bullets.end()) {

			if (character2.CheckShoot(*it2)) {
				it2 = bullets2.erase(it2);
				continue;
			}
			// If out-of-bounds, the bullet is erased from the list
			if ((*it2).OutOfBounds()) {
				it2 = bullets.erase(it2);
				continue;
			}
			(*it2).Move();
			window.draw((*it2).GetShape());
			it2++;
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	window.display();
}

void Game::ChooseGameSetUp()
{
	message = "Join Game (1) or Create Game (2)";

	text = sf::Text(message, font, 18);
	text.setFillColor(sf::Color(255, 255, 255));
	text.setStyle(sf::Text::Bold);
	text.setPosition(200, 100);

	input = "";

	nameText = sf::Text(input, font, 18);
	nameText.setFillColor(sf::Color(0, 0, 0));
	nameText.setStyle(sf::Text::Bold);
	nameText.setPosition(220, 130);


	nameRectangle = sf::RectangleShape(sf::Vector2f(400, 30));
	nameRectangle.setFillColor(sf::Color(255, 255, 255, 150));
	nameRectangle.setPosition(200, 130);
}
