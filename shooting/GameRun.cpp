#include "GameRun.h"

void GameRun::updateGame()
{
	setupGame();
	// App loop
	while (windowGame.isOpen())
	{
		while (windowGame.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				windowGame.close(); // Close windowGames if X is pressed 
				break;
			case sf::Event::KeyPressed:

				if (event.key.code == sf::Keyboard::Escape)
					windowGame.close(); // Close windowGames if ESC is pressed 
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

				windowGame.clear();
				windowGame.draw(sprite);

				// When playing
				windowGame.draw(character.GetSprite());
				windowGame.draw(character2.GetSprite());

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
					windowGame.draw((*it2).GetShape());
					it2++;
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		windowGame.display();
	}
}

void GameRun::setupGame() 
{
	// windowGames initialization
	windowGame.create(sf::VideoMode(850, 600), "Partida");

	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);
}
