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
#include "Timer.h"
#include "game.h"

class Cliente
{
	//Client
	std::string action, content;
	sf::Time t1 = sf::seconds(1.0f);
	Timer timer;
	bool check = false;
	Game game;
	bool noGame = false;

public:
	Cliente();
	Game gc;
	std::string name;
	sf::IpAddress ip;
	unsigned short port = 5000;
	int clientID = -1;


	void HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket);
	void SendPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg);
	void ClientMain();
	void GameSelected(sf::UdpSocket* sock);
	void Pong();
	void Check();
	void CreateGame();
	void JoinGame(std::vector<Game> games);
};

