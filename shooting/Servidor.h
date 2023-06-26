#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <thread>
#include <mutex>
#include <functional>
#include <map>
#include "Timer.h"

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

	Client con;

	struct Game {
	};

	struct Packet{
		int packetID;
		std::string action;
		std::string clientName;
	};

	std::string action, content;
	int packetID;
	std::map<int,Game>games; // Mapa de juegos
	sf::UdpSocket socket;
	
	bool create = true;
	std::vector<bool> hasCreatedGame;
	std::map<std::string, int>clientToGames;
	int nextGameId = 0;
	int GetClosestClient(unsigned short remotePort);
	Client GetClientFromName(std::string name);
	//void Ping(std::atomic_bool* stopThread);

	void CriticalReceive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content, int* packetID);
	void CriticalSend(Client* con, sf::UdpSocket* sock, std::string message, int packetID);
	void SavePacketContent(int pId, std::string action, std::string cName);
	Packet pack;
	std::vector<Packet> packets;

	//Timer
	Timer timer;
	std::vector<Timer> timers; // Timer para gestionar la desconexion de cada uno de los clientes. 
	int time = 10;
	int pings = 2;
	int pingCounter = -1;


	std::vector<Timer> timersCritic;
	
public:
	std::vector<Client> clients;
	std::vector<Client> NoConnectedClients;

	int clientID;

	void ShutdownServer(std::string* mssg, bool* exit);
	void Hello(Client* con, sf::UdpSocket* sock);
	void StartServer();
	void Send(Client* con, sf::UdpSocket* sock, std::string message);
	void PingPong();
	void PacketChecker();

};

