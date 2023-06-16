#include "GameRun.h"

void GameRun::updateGame()
{
	setupGame();
	// App loop
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close(); // Close windowGames if X is pressed 
				break;
			case sf::Event::KeyPressed:

				if (event.key.code == sf::Keyboard::Escape)
					window.close(); // Close windowGames if ESC is pressed 
				// Manage events when playing
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
				}

				break;

				window.clear();
				window.draw(sprite);

				// When playing
				window.draw(character.GetSprite());
				window.draw(character2.GetSprite());

				// Bullets update

				auto it2 = bullets.begin();
				while (it2 != bullets.end()) {

					if (character2.CheckShoot(*it2)) {
						it2 = bullets.erase(it2);
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
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		window.display();
	}
}

void GameRun::setupGame() 
{
	// windowGames initialization
	window.create(sf::VideoMode(850, 600), "Game");

	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);
}
