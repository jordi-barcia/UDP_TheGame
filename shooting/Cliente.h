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
#include "GameConnections.h"
#include "Timer.h"

class Cliente
{
	//Client
	std::string action, content;
	sf::Time t1 = sf::seconds(1.0f);
	Timer timer;
	bool check = false;
public:
	Cliente();
	GameConnections gc;
	std::string name;
	sf::IpAddress ip;
	unsigned short port = 5000;
	int clientID = -1;


	void HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket);
	void ClientMain();
	void GameSelected(sf::UdpSocket* sock);
	void Pong();
	void Check();
};

