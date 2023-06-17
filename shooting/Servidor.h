#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include "entities.h"
#include "Cliente.h"
#include "game.h"
#include "Timer.h"
#include <functional>

class Servidor
{
	struct newConnection {
		sf::IpAddress ip;
		unsigned short port;
		std::string name;
		int id = -1;
		//int ch_ts; // Timeout para el challenge
		std::string challenge; // Pregunta del challenge
		std::string solution; // Respuesta al challenge
	};
	std::string action, content;
	int countdownSeconds = 30;
	Game gc;
	std::vector<Game>games;
	Timer timer;
	std::vector<Timer> timers;
	bool ping = false;
	sf::UdpSocket socket;
	newConnection con;
	bool create = true;
	std::vector<bool> hasCreatedGame;

	
public:
	std::vector<newConnection> clients;
	int clientID;

	void Hello(newConnection* con, sf::UdpSocket* sock, sf::Packet* inPacket);
	void StartServer();
	void Countdown();
	void Ping(newConnection* con, sf::UdpSocket* sock);
};

