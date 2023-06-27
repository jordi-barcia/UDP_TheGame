#include "Servidor.h"

int Servidor::GetClosestClient(unsigned short remotePort) {
	int nameChar, diff, minDiff = INT_MAX;
	int closestClient = -1;

	for (int j = 0; j < clients.size(); j++) {
		if (clients[j].port == remotePort) {
			nameChar = static_cast<int>(clients[j].name[0]);

			for (int i = 0; i < clients.size(); i++) {
				if (i != j && hasCreatedGame[i]) {
					diff = std::abs(nameChar - static_cast<int>(clients[i].name[0]));

					if (diff < minDiff) {
						minDiff = diff;
						closestClient = i;
					}
				}
			}
			break;
		}
	}
	return closestClient;
}

Servidor::Client Servidor::GetClientFromName(std::string name)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i].name == name)
		{
			return clients[i];
		}
	}
}

void Servidor::ShutdownServer(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);

		if (line.size() > 0)
		{
			if (line == "exit")
			{
				std::cout << "DISCONNECTED" << std::endl;
				for (int i = 0; i < clients.size(); i++)
				{
					Send(&clients[i], &socket, "EXIT");
				}
				for (int i = 0; i < NoConnectedClients.size(); i++)
				{
					Send(&NoConnectedClients[i], &socket, "EXIT");
				}
				clients.erase(clients.begin(), clients.begin() + clients.size());
				NoConnectedClients.erase(NoConnectedClients.begin(), NoConnectedClients.begin() + NoConnectedClients.size());
				*exit = false;
				std::terminate();
			}
		}
	}
}

void Servidor::Send(Client* con, sf::UdpSocket* sock, std::string message)
{
	sf::Packet outPacket;
	outPacket << message << con->name;
	sock->send(outPacket, con->ip, con->port);
	std::cout << "Sending: " + message << " " + con->name << " " + con->port << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Servidor::CriticalSend(Client* con, sf::UdpSocket* sock, std::string message, int packetID)
{
	sf::Packet outPacket;
	outPacket << message << con->name << packetID;
	sock->send(outPacket, con->ip, con->port);
	//SavePacketContent(packetID, message, con->name);
	std::cout << "Critical Sending: " << message << " " << con->name << " " << packetID << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Servidor::PingPong()
{
	if (timers.size() > 0) {
		for (int i = 0; i < timers.size(); i++) {
			timers[i].update();
			if (timers[i].temp <= 0) {
				//mtx.lock();
				if (pingCounter == -1)
				{
					std::cout << "entro PING" << std::endl;
					Send(&clients[i], &socket, "PING");
					pingCounter++;
					timers[i].init(2);
				}
				else if (pingCounter <= 4 && pingCounter >= 0)
				{
					Send(&clients[i], &socket, "PING");
					pingCounter++;
					timers[i].init(2);
				}
				else if (pingCounter >= 4)
				{
					action == "EXIT_CL";
				}
				//mtx.unlock();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Servidor::Hello(Client* con, sf::UdpSocket* sock)
{
	//SEND
	if (action == "HELLO" && !hasHello)
	{
		SavePacketContent(0, "CH_SYN", con->name);
		CriticalSend(con, sock, "CH_SYN", 0);
	}

	else if (action == "CH_ACK")
	{
		std::cout << "CONNECTED" << std::endl;

		clients.push_back(*con);
		
		for (int i = 0; i < NoConnectedClients.size(); i++)
		{
			NoConnectedClients.erase(NoConnectedClients.begin() + i);
		}
		
		create = false;
		hasCreatedGame.push_back(create);

		timer.init(time);
		timers.push_back(timer);
		std::cout << timers.size() << std::endl;
		action = "";
		hasHello = false;
	}
}

void Servidor::CriticalReceive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content, int* packetID)
{
	while (true)
	{
		if (socket->receive(*inPacket, *remoteIp, *remotePort) != socket->Done)
		{
			std::cout << "Receive Error: " << socket->Error << std::endl;
		}
		*inPacket >> *action >> *content >> *packetID;
		if (*action == "PONG")
		{
			std::cout << "Receive: " << *action << " " << *content << std::endl;
		}
		else {
			std::cout << "Critical Receive: " << *action << " " << *content << " " << *packetID << std::endl;
			IDpack = *packetID;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

//void Receive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content)
//{
//	while (true)
//	{
//		if (socket->receive(*inPacket, *remoteIp, *remotePort) != socket->Done)
//		{
//			std::cout << "Receive Error: " << socket->Error << std::endl;
//		}
//		*inPacket >> *action >> *content;
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//}

void Servidor::PacketChecker() {
	
	while (true)
	{
		//std::cout << "Revisando tabla de paquetes" << std::endl;
		if (clients.size() > 0) {
			for (int j = 0; j < clients.size(); j++)
			{
				for (int i = 0; i < packets.size(); i++)
				{
					if (packets.size() > 0)
					{
						if (timersCritic.size() > 0)
						{
							timersCritic[i].update();
							if (clients[j].name == packets[i].clientName)
							{
								//std::cout << IDpack << "." << packets[i].packetID << std::endl;
								if (IDpack == packets[i].packetID)
								{
									std::cout << "Erased Packet: " << packets[i].action << std::endl;
									packets.erase(packets.begin() + i);
									timersCritic.erase(timersCritic.begin() + i);
								}
								else if (timersCritic[i].temp <= 0) {
									if (action != "HELLO" && action != "CH_ACK") {
										CriticalSend(&con, &socket, packets[i].action, pack.packetID);
									}
									timersCritic[i].init(.5f);
								}
							}
						}
					}
				}
			}
		}

		if (NoConnectedClients.size() > 0) {
			for (int j = 0; j < NoConnectedClients.size(); j++)
			{
				for (int i = 0; i < packets.size(); i++)
				{
					if (packets.size() > 0)
					{
						if (timersCritic.size() > 0)
						{
							timersCritic[i].update();
							if (NoConnectedClients[j].name == packets[i].clientName)
							{
								//std::cout << IDpack << "." << packets[i].packetID << std::endl;
								if (IDpack == packets[i].packetID)
								{
									std::cout << "Erased Packet: " << packets[i].action << std::endl;
									packets.erase(packets.begin() + i);
									timersCritic.erase(timersCritic.begin() + i);
								}
								else if (timersCritic[i].temp <= 0) {
									if (action != "HELLO" && action != "CH_ACK") {

									}
									else {
										Hello(&con, &socket);
									}
									timersCritic[i].init(.5f);
								}
							}
						}
					}
				}
			}
		}
	}
}


void Servidor::SavePacketContent(int pId, std::string action, std::string cName) 
{
	pack.packetID = pId;
	pack.action = action;
	pack.clientName = cName;
	packets.push_back(pack);
	timer.init(.5f);
	timersCritic.push_back(timer);
}

//void Servidor::Ping(std::atomic_bool* stopThread)
//{
//    std::cout << "Ping Thread ID: " << std::this_thread::get_id() << std::endl;
//    auto startTime = std::chrono::steady_clock::now();
//    auto duration = std::chrono::steady_clock::now() - startTime;
//    while (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < 10000)
//    {
//        duration = std::chrono::steady_clock::now() - startTime;
//        std::cout << "Ping Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << std::endl;
//        if (stopThread)
//        {
//            return;
//        }
//    }
//
//    std::cout << "Ping Duration Out Bucle: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << std::endl;
//    Send(&con, &socket, "PING");
//}

std::thread timer;

void Servidor::StartServer()
{
	// Aplication init
	bool exit = true;

	//Server mode
	std::cout << "Server mode running" << std::endl;

	//We open the port 5000 manage TCP connection
	socket.bind(5000);

	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;
	unsigned int port = 0;

	// Application loop
	sf::IpAddress remoteIP;
	unsigned short remotePort = 0;
	
	//Time
	//auto startTime = std::chrono::steady_clock::now();
	//auto duration = std::chrono::steady_clock::now() - startTime;
	//int pingCounter = -1;

	//Threads
	std::atomic_bool stopThreadTimer;
	
	std::thread read_console_t(&Servidor::ShutdownServer, this, &sendMessage, &exit);
	read_console_t.detach();
	
	/*std::thread receiveFromClients(Receive, &socket, &inPacket, &remotePort, &remoteIP, &action, &content);
	receiveFromClients.detach();*/
	
	std::thread criticalReceiveFromClients(&Servidor::CriticalReceive, this, &socket, &inPacket, &remotePort, &remoteIP, &action, &content, &IDpack);
	criticalReceiveFromClients.detach();

	std::thread packetCheckerThread(&Servidor::PacketChecker,this);
	packetCheckerThread.detach();



	while (exit) {
		// Logic for receiving
		//Paquete << Accion << Contenido
		if (inPacket.getDataSize() > 0)
		{
			//std::cout << "Data size: " << inPacket.getDataSize() << std::endl;
			//std::cout << "Receive: " << action << " " << content << " " << remotePort << std::endl;
			
			
			//startTime = std::chrono::steady_clock::now();
			//std::cout << action << " " << content << " " << remotePort << std::endl;


			if (action == "HELLO" && !hasHello)
			{
				//Saving client config
				con.name = content;
				con.port = remotePort;
				con.ip = remoteIP;
				NoConnectedClients.push_back(con);
				std::cout << con.name << " " << con.port << " " << con.ip << std::endl;
				Hello(&con, &socket);
				hasHello = true;
			}
			else if (action == "CH_ACK" && hasHello)
			{
				Hello(&con, &socket);
				hasHello = false;
				//EMPEZAR PROCESO DE PING PONG
			}
			else if (action == "CREATE") {
				std::cout << "creating game..." << std::endl;
				for (int j = 0; j < clients.size(); j++)
				{
					if (clients[j].port == con.port)
					{
						hasCreatedGame[j] = true;
						games[nextGameId] = Game{};
						clientToGames[clients[j].name] = nextGameId;
						nextGameId++;
						std::cout << "Numero de Games creados: " << games.size() << std::endl;
						IDpack = 1;
						SavePacketContent(IDpack, "CREATE_ACK", con.name);
						CriticalSend(&con, &socket, "CREATE_ACK", IDpack);
					}
				}
				inPacket.clear();
			}
			else if (action == "JOINED") {
				std::cout << "joining game..." << std::endl;
				if (games.size() % 2 == 0 || games.size() == 0)
				{
					// Moverlo a una funcion de create o copiar lo que esta en CREATE
					Send(&con, &socket, "NO_GAME");
				}
				else {
					int indexClosestClient = GetClosestClient(remotePort);
					int gameId = clientToGames[clients[indexClosestClient].name];
					clientToGames[con.name] = gameId; // Content = client.name(El que pide el join)
					Client ClientName = GetClientFromName(con.name);
					IDpack = 1;
					SavePacketContent(IDpack, "JOIN_ACK", con.name);
					CriticalSend(&ClientName, &socket, "JOIN_ACK", IDpack);
				}
				inPacket.clear();
			}
			else if (action == "EXIT_CL")
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (clients[i].name == content)
					{
						std::cout << "DISCONNECTED: " << clients[i].name + " " << clients[i].port << std::endl;
						clients.erase(clients.begin() + i);
						timers.erase(timers.begin() + i);
						if (games.size() != 0) {
							if (i < games.size()) {
								games.erase(i);
							}
							else {
								games.erase(i - 1);
							}
						}
					}
				}
			}

			else if (action == "PONG")
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (clients[i].name == content)
					{
						timers[i].init(time);
						pingCounter = -1;
						content = "";
					}
				}
			}

			else if (action == "MOV")
			{
				//Validar movimiento
				//Comunicar a los otros clientes el movimiento del jugador
			}
		}
		else
		{
			//std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << std::endl;
			//duration = std::chrono::steady_clock::now() - startTime;
			//if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 10000)
			//{
			//	Send(&con, &socket, "PING");
			//	startTime = std::chrono::steady_clock::now();
			//	pingCounter++;
			//}
			//else if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 2000 && pingCounter <= 4 && pingCounter >= 0)
			//{
			//	Send(&con, &socket, "PING");
			//	startTime = std::chrono::steady_clock::now();
			//	pingCounter++;
			//}
			//else if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 10000 && pingCounter >= 4)
			//{
			//	action == "EXIT_CL";
			//}
		}
		PingPong();
	}
}
