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
	//Server
	sf::UdpSocket socket;
	char key;
	
	//Client
	struct Client {
		sf::IpAddress ip;
		unsigned short port;
		std::string name;
		int id = -1;
	};
	Client con;
	std::vector<Client> clients; // Tabla de clientes conectados
	std::vector<Client> NoConnectedClients; // Tabla de clientes no conectados. 
	int clientID;

	//Game
	struct Game {
	};
	std::map<int,Game>games; // Mapa de juegos
	std::vector<bool> hasCreatedGame;
	std::map<std::string, int>clientToGames; // Mapa para identificar clientes con el juego en el que estan. 
	int nextGameId = 0;

	//Packet
	struct Packet{
		int packetID;
		std::string action;
		std::string clientName;
	};
	Packet pack;
	std::vector<Packet> packets; // Tabla de paquetes criticos
	std::string action, content, check;
	int IDpack;
	int packIDreceived = -1;
	bool selectGame = false;
	double rttxPacket = 0.0f;
	std::vector<double> rttContainer;
	bool create = true;
	int packetLostProb = 1;
	bool hasHello = false;

	//Timer
	Timer timer;
	std::vector<Timer> timers; // Timer para gestionar la desconexion de cada uno de los clientes. 
	std::vector<Timer> timersCritic; // Timers de los paquetes criticos
	int initialTime = 10;
	int pings = 2;
	int pingCounter = -1;
	int expected_fps = 30;
	double expected_frametime = 1.0 / expected_fps;

	//Functions
	int GetClosestClient(unsigned short remotePort); // Devuelve la posicion del cliente en el vector con la primera letra del nombre mas cercana al del usuario local.
	Client GetClientFromName(std::string name); // Devuelve el cliente que se esta buscando. 
	void Send(Client* con, sf::UdpSocket* sock, std::string message); // Envia mensajes no criticos al cliente. 
	void CriticalSend(Client* con, sf::UdpSocket* sock, std::string message, int packetID); // Envia mensajes criticos al cliente.
	void CriticalReceive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content, int* packetID); // Recibe los mensajes del cliente tanto criticos como no criticos.
	void Hello(Client* con, sf::UdpSocket* sock); // Gestion del proceso de conexion con el cliente.
	void RTTChanger();// Cambiar la probabilidad de perdida de mensaje.
	void RTTCalculation();// Calcula el RTT de los ultimos 10 mensajes y lo imprime cada 2 segundos. 
	void PacketChecker(); // Revisa que paquetes estan todavia por confirmar en la tabla de paquetes criticos
	void SavePacketContent(int pId, std::string action, std::string cName); // Guarda el contenido de los paquetes recibidos.
	void PingPong(); // Proceso de desconexion por ping pong
	void ShutdownServer(std::string* mssg, bool* exit); // Proceso de apagado del Servidor
	
public:
	void StartServer(); // Bucle principal del servidor. 
};
