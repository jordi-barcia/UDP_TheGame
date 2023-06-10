#include "GameConnections.h"

void GameConnections::ClientSetup()
{
	// Windows initialization
	window.create(sf::VideoMode(850, 600), "Game");

	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);

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

void GameConnections::ChallengeSetUp()
{
	// Windows initialization
	window.create(sf::VideoMode(850, 600), "Game");

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

void GameConnections::RunConnections()
{
	//ClientSetUp();
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close(); // Close windows if X is pressed 
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape)
				window.close(); // Close windows if ESC is pressed 
			if (playing) { // Manage events when playing
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
			}
			if (!playing) {
				// Manage events when no playing
				if ((event.key.code == sf::Keyboard::Delete || event.key.code == sf::Keyboard::BackSpace) && input.getSize() > 0) {
					input.erase(input.getSize() - 1, input.getSize());
				}
				else if (event.key.code == sf::Keyboard::Return && input.getSize() > 0) {

					//playing = true;
					if (!challenge && !chooseSetUp) {
						challenge = true;

						//Servidor deberia pasar el challenge al cliente
						name = nameText.getString();
						ChallengeSetUp();
					}
					else if (challenge && nameText.getString() == challengeWord) {
						chooseGame = true;
						challenge = false;
						chooseSetUp = true;
						ChooseGameSetUp();
						//enviar al servidor que el challenge es correcto para escoger la partida
					}
					else if(challenge){
						ChallengeSetUp();
					}

					if (chooseSetUp && nameText.getString() == "1")
					{
						//Join Game
						joined = true;
					}

					if(chooseSetUp && nameText.getString() == "2")
					{
						//Create Game
						std::cout << "creating game..." << std::endl;
						created = true;
					}

					if (chooseSetUp && (!joined && !created)) {
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
	if (chooseSetUp) {
		// When no playing
		window.draw(nameRectangle);
		nameText.setString(input);
		window.draw(nameText);
		window.draw(text);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	window.display();
}

void GameConnections::ChooseGameSetUp()
{
	// Windows initialization
	window.create(sf::VideoMode(850, 600), "Game");

	if (!font.loadFromFile("resources/fonts/courbd.ttf"))
	{
		std::cout << "Can't load the font file" << std::endl;
	}

	bg.loadFromFile("resources/bg.png");
	sprite.setTexture(bg);

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
