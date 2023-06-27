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
	Timer timer;
	std::vector<Timer> timersCritic;
	bool check = false;
	bool noGame = false;
	int packetCounter = 0;

	struct Packet {
		int packetID;
		std::string action;
		std::string clientName;
	};

	unsigned short serverPort = 5000;
	sf::IpAddress serverIp = "127.0.0.1";

	Game gc;
	std::string name;
	sf::IpAddress ip;
	unsigned short port = 5000;
	int clientID = -1;

	Packet pack;
	std::vector<Packet> packets;
	bool hasHello = false;
	
	void SavePacketContent(int pId, std::string action, std::string cName);
	void SendCritPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg, int packetID);
	void PacketChecker(sf::UdpSocket* sock);

public:
	Cliente() = default;

	void HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket);
	void SendPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg);
	void ClientMain();
	void GameSelected(sf::UdpSocket* sock);
	void CreateGame();
	void JoinGame(std::vector<Game> games);
	void RecieveMessage(sf::UdpSocket* sock, std::string *actionMssg, std::string *contentMssg);
	void ReceiveCriticalPacket(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg, int* packetID);
};

