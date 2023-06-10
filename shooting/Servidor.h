#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <chrono>
#include <thread>
#include <mutex>
#include "entities.h"
#include "Cliente.h"
#include "GameConnections.h"
#include "GameRun.h"
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
	int countdownSeconds = 10000;
	GameRun g;
	GameConnections gc;
	std::vector<GameRun>games;
	Timer timer;
	bool ping = false;
	sf::UdpSocket socket;
	newConnection con;

	
public:
	std::vector<newConnection> clients;
	void Hello(newConnection* con, sf::UdpSocket* sock, sf::Packet* inPacket);
	void StartServer();
	void JoinGame(std::vector<GameRun>games, newConnection client);
	void CreateGame(std::vector<GameRun>games, newConnection client);
	void runGame(std::vector<GameRun> listOfGames);
	void Countdown();
	void Ping(newConnection* con, sf::UdpSocket* sock);
};

