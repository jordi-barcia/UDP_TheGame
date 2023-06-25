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
				clients.erase(clients.begin(), clients.begin() + clients.size());
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
}

void Servidor::Hello(Client* con, sf::UdpSocket* sock)
{
	//SEND
	if (action == "HELLO")
	{
		Send(con, sock, "CH_SYN");
	}

	else if (action == "CH_ACK" && content == con->name)
	{
		std::cout << "CONNECTED" << std::endl;

		clients.push_back(*con);
		create = false;
		hasCreatedGame.push_back(create);
	}
}

void Receive(sf::UdpSocket* socket, sf::Packet* inPacket, unsigned short* remotePort, sf::IpAddress* remoteIp, std::string* action, std::string* content)
{
	while (true)
	{
		if (socket->receive(*inPacket, *remoteIp, *remotePort) != socket->Done)
		{
			std::cout << "Receive Error: " << socket->Error << std::endl;
		}
		*inPacket >> *action >> *content;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
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
	unsigned int port;

	// Application loop
	sf::IpAddress remoteIP;
	unsigned short remotePort;
	
	//Time
	auto startTime = std::chrono::steady_clock::now();
	auto duration = std::chrono::steady_clock::now() - startTime;
	int pingCounter = -1;

	//Threads
	std::atomic_bool stopThreadTimer;
	std::thread read_console_t(&Servidor::ShutdownServer, this, &sendMessage, &exit);
	read_console_t.detach();
	std::thread receiveFromClients(Receive, &socket, &inPacket, &remotePort, &remoteIP, &action, &content);
	receiveFromClients.detach();


	while (exit) {
		// Logic for receiving
		//Paquete << Accion << Contenido
		if (inPacket.getDataSize() > 0)
		{
			std::cout << "Data size: " << inPacket.getDataSize() << std::endl;
			std::cout << "Receive: " << action << " " << content << " " << remotePort << std::endl;
			
			startTime = std::chrono::steady_clock::now();
			pingCounter = -1;
			
			//std::cout << action << " " << content << " " << remotePort << std::endl;

			if (action == "HELLO")
			{
				//Saving client config
				con.name = content;
				con.port = remotePort;
				con.ip = remoteIP;
				std::cout << con.name << " " << con.port << " " << con.ip << std::endl;
				Hello(&con, &socket);
			}
			else if (action == "CH_ACK")
			{
				Hello(&con, &socket);
				
				//EMPEZAR PROCESO DE PING PONG
				
			}
			else if (action == "CREATE") {
				std::cout << "creating game..." << std::endl;
				for (int j = 0; j < clients.size(); j++)
				{
					if (clients[j].port == remotePort)
					{
						hasCreatedGame[j] = true;
						games[nextGameId] = Game{};
						clientToGames[clients[j].name] = nextGameId;
						nextGameId++;
						std::cout << "Numero de Games creados: " << games.size() << std::endl;
					}
				}
				inPacket.clear();
			}
			else if (action == "JOINED") {
				std::cout << "joining game..." << std::endl;
				if (games.size() % 2 == 0 || games.size() == 0)
				{
					// Moverlo a una funcion de create o copiar lo que esta en CREATE
				}
				int indexClosestClient = GetClosestClient(remotePort);
				int gameId = clientToGames[clients[indexClosestClient].name];
				clientToGames[con.name] = gameId; // Content = client.name(El que pide el join)
				Client ClientName = GetClientFromName(con.name);
				Send(&ClientName, &socket, "JOIN_ACK");
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
					}
				}
			}

			else if (action == "PONG")
			{
				// Es fa algo?
			}

			else if (action == "MOV")
			{
				//Validar movimiento
				//Comunicar a los otros clientes el movimiento del jugador
			}

			inPacket.clear();
		}
		else
		{
			//std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << std::endl;
			duration = std::chrono::steady_clock::now() - startTime;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 10000)
			{
				Send(&con, &socket, "PING");
				startTime = std::chrono::steady_clock::now();
				pingCounter++;
			}
			else if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 2000 && pingCounter <= 4 && pingCounter >= 0)
			{
				Send(&con, &socket, "PING");
				startTime = std::chrono::steady_clock::now();
				pingCounter++;
			}
			else if (std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() >= 10000 && pingCounter >= 4)
			{
				action == "EXIT_CL";
			}
		}

	}
}
