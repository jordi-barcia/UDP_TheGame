#include "Servidor.h"

Servidor::Client Servidor::GetClosestClient(unsigned short remotePort)
{
	int nameChar, diff, lastDiff;
	for (int j = 0; j < clients.size(); j++)
	{
		if (clients[j].port == remotePort)
		{
			nameChar = int(clients[j].name[0]);
			diff = nameChar;
			lastDiff = diff;
			for (int i = 0; i < clients.size(); i++)
			{
				if (i != j)
				{
					diff = nameChar - int(clients[i].name[0]);
					if (lastDiff <= diff)
					{
						if (hasCreatedGame[i])
						{
							lastDiff = diff;
							return clients[i];
						}
					}
				}
			}
		}
	}
	
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

void Servidor::GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}


void Servidor::Ping(Client* con, sf::UdpSocket* sock) {
	sf::Packet outPacket;
	action = "PING";
	outPacket << action << con->name;
	sock->send(outPacket, con->ip, con->port);
}
void Servidor::Send(Client* con, sf::UdpSocket* sock, std::string message)
{
	sf::Packet outPacket;
	outPacket << action << con->name;
	sock->send(outPacket, con->ip, con->port);
}
void Servidor::Hello(Client* con, sf::UdpSocket* sock, sf::Packet* inPacket)
{
	sf::Packet outPacket;
	//SEND

	std::cout << "Debug Action/Content " << action << " " << content << std::endl;
	if (action == "HELLO")
	{
		//con->id = clients.size();
		outPacket << "CH_SYN" << con->name;
		sock->send(outPacket, con->ip, con->port);

	}

	else if (action == "CH_ACK" && content == con->name)
	{
		std::cout << "CONNECTED" << std::endl;

		clients.push_back(*con);
		create = false;
		hasCreatedGame.push_back(create);
	}
}

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
	//Client con;

	//Threads
	std::thread read_console_t(&Servidor::GetLineFromCin_t, this, &sendMessage, &exit);
	read_console_t.detach();


	while (true) {
		// Logic for receiving
		socket.receive(inPacket, remoteIP, remotePort);

		//Paquete << Accion << Contenido
		inPacket >> action >> content;

		//std::cout << action << " " << content << " " << remotePort << std::endl;

		if (action == "HELLO")
		{
			std::cout << "ENTRO" << std::endl;
			//Saving client config

			con.name = content;
			con.port = remotePort;
			con.ip = remoteIP;
			Hello(&con, &socket, &inPacket);
		}
		else if (action == "MSG")
		{

		}
		else if (action == "CH_SYN")
		{
			//Aqui se tiene que tratar, Conexion con otros servidores
		}
		else if (action == "CH_ACK")
		{
			Hello(&con, &socket, &inPacket);
			pongTimer.init(timeTillPingPong);
		}
		else if (action == "CREATE") {
			std::cout << "creating game..." << std::endl;
			for (int j = 0; j < clients.size(); j++)
			{
				if (clients[j].port == remotePort)
				{
					hasCreatedGame[j] = true;
					games[nextGameId] = Game();
					clientToGames[clients[j].name] = nextGameId;
					nextGameId++;
					std::cout << "Numero de Games creados: " + games.size() << std::endl;
				}
			}
			inPacket.clear();
		}
		else if (action == "JOINED") {
			std::cout << "joining game..." << std::endl;
			std::cout << games.size() << std::endl;
			if (games.size() % 2 != 0 || games.size() == 0) // Moverlo a una funcion de create o copiar lo que esta en CREATE
			{
				//for (int j = 0; j < clients.size(); j++)
				//{
				//	if (clients[j].port == remotePort)
				//	{
				//		std::cout << "Hola juancarlos" << std::endl;
				//		hasCreatedGame[j] = true;
				//		Send(&clients[j], &socket, "NO_GAME");
				//		//gc.isFirstGame = true;
				//	}
				//}
			}
			else
			{
				Client closestClient = GetClosestClient(remotePort);
				int gameId = clientToGames[closestClient.name];
				clientToGames[content] = gameId; // Content = client.name(El que pide el join)
				Send(&GetClientFromName(content), &socket, "JOIN_ACK");
			}
			inPacket.clear();
		}

		else if (action == "EXIT_CL")
		{
			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].port == remotePort)
				{
					std::cout << "DISCONNECTED: " << clients[i].name + " " << clients[i].port << std::endl;
					clients.erase(clients.begin() + i);
				}
			}
		}

		else if (action == "MOV")
		{
			//Validar movimiento
			//Comunicar a los otros clientes el movimiento del jugador
		}

		if (pongTimer.temp < 0)
		{
			std::cout << "EMPIEZA EL PING" << std::endl;
			for (int i = 0; i < clients.size(); i++)
			{
				if (clients[i].port == remotePort)
				{
					if (action == "PONG")
					{
						if (pongCounter >= 5)
						{
							pongTimer.temp = timeTillPingPong;
							std::cout << "Termina el PING" << std::endl;
						}
						pongCounter++;
					}
					Ping(&clients[i], &socket);
				}
			}
		}
		else
		{
			pongTimer.update();
			std::cout << pongTimer.temp << std::endl;
		}

		std::cout << sendMessage << std::endl;

		if (sendMessage.size() > 0)
		{
			if (sendMessage == "exit")
			{
				std::cout << "DISCONNECTED" << std::endl;
				for (int i = 0; i < clients.size(); i++)
				{
					Send(&clients[i], &socket, "EXIT");
				}
				clients.erase(clients.begin(), clients.begin() + clients.size());
				exit = false;
				break;
			}
		}
	}
}
