#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <thread>
#include <mutex>
#include "entities.h"
#include "Cliente.h"
#include "game.h"
#include "Timer.h"
#include <functional>
#include <map>

class Servidor
{
	struct Client {
		sf::IpAddress ip;
		unsigned short port;
		std::string name;
		int id = -1;
		//int ch_ts; // Timeout para el challenge
		std::string challenge; // Pregunta del challenge
		std::string solution; // Respuesta al challenge
	};
	std::string action, content;
	std::map<int,Game>games; // Mapa de juegos
	std::vector<Timer> timers;
	sf::UdpSocket socket;
	Client con;
	bool create = true;
	std::vector<bool> hasCreatedGame;
	std::vector<Timer> clientsTime;
	std::map<std::string, int>clientToGames;
	int nextGameId = 0;
	int GetClosestClient(unsigned short remotePort);
	Client GetClientFromName(std::string name);
	void Ping(std::atomic_bool* stopThread);
	
public:
	std::vector<Client> clients;
	int clientID;

	void GetLineFromCin_t(std::string* mssg, bool* exit);
	void Hello(Client* con, sf::UdpSocket* sock);
	void StartServer();
	void Send(Client* con, sf::UdpSocket* sock, std::string message);
	
};

