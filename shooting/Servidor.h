#pragma once
#include <SFML\Network.hpp>
#include <iostream> 
#include <future>
#include <thread>
#include <mutex>
#include <functional>
#include <map>
#include "Timer.h"
#include <iomanip>
#include <time.h>
#include <stdlib.h>

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
	int IDpack;
	int packIDreceived = -1;
	bool selectGame = false;
	double rttxPacket = 0.0f;
	std::vector<double> rttContainer;
	Packet pack;
	std::vector<Packet> packets;
	
	std::map<int,Game>games; // Mapa de juegos
	sf::UdpSocket socket;
	
	bool create = true;
	std::vector<bool> hasCreatedGame;
	std::map<std::string, int>clientToGames;
	int nextGameId = 0;
	int GetClosestClient(unsigned short remotePort);
	Client GetClientFromName(std::string name);
	//void Ping(std::atomic_bool* stopThread);

	char key;

	void CriticalReceive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content, int* packetID);
	void CriticalSend(Client* con, sf::UdpSocket* sock, std::string message, int packetID);
	void SavePacketContent(int pId, std::string action, std::string cName);
	void RTTCalculation();
	void RTTChanger();

	//Timer
	Timer timer;
	std::vector<Timer> timers; // Timer para gestionar la desconexion de cada uno de los clientes. 
	int initialTime = 10;
	int pings = 2;
	int pingCounter = -1;
	int expected_fps = 30;
	double expected_frametime = 1.0 / expected_fps;
	int packetLostProb = 1;

	bool hasHello = false;
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
