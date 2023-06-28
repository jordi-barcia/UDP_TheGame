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
	//Timers
	Timer timer;
	std::vector<Timer> timersCritic;
	
	//Game
	Game gc;

	//Packet Variables
	struct Packet {
		int packetID;
		std::string action;
		std::string clientName;
	};
	Packet pack;
	std::string action, content;
	std::vector<Packet> packets; // Tabla de paquetes criticos.
	int packIDreceived = -1;
	int packetCounter = 0;
	
	//Server Variables
	unsigned short serverPort = 5000;
	sf::IpAddress serverIp = "127.0.0.1";
	
	//Client Variables
	std::string name;
	sf::IpAddress ip;
	unsigned short port = 5000;
	int clientID = -1;
	bool hasHello = false;

	//Functions
	void SavePacketContent(int pId, std::string action, std::string cName); // Guarda el contenido del paquete critico en la tabla de paquetes.
	void SendCritPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg, int packetID); // Envia paquetes criticos al servidor.
	void PacketChecker(sf::UdpSocket* sock); // Revisa que paquetes faltan por confirmar.
	void HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket); // Proceso de conexion con el servidor.
	void SendPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg); // Envia paquetes no criticos al servidor.
	void GameSelected(sf::UdpSocket* sock); // Deriva al jugador enviar o CREATED o JOINED al Servidor en funcion de que modo a escogido e
	void ReceiveCriticalPacket(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg, int* packetID); // Recibe los paquetes del servidor tanto criticos como no criticos

public:
	//Constructor
	Cliente() = default;
	void ClientMain(); // Bucle principal del Cliente

};

