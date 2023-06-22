#include "Servidor.h"

void Servidor::Countdown() {
	
}

void Servidor::Ping(newConnection* con, sf::UdpSocket* sock) {
	sf::Packet outPacket;
	std::string ping = "PING";
	outPacket << "PING" << ping;
	sock->send(outPacket, con->ip, con->port);
}
void Servidor::Send(newConnection* con, sf::UdpSocket* sock, std::string message)
{
	sf::Packet outPacket;
	outPacket << action << con->name;
	sock->send(outPacket, con->ip, con->port);
}
void Servidor::Hello(newConnection* con, sf::UdpSocket* sock, sf::Packet* inPacket)
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

		Countdown();
	}
}

void Servidor::StartServer()
{
	// Aplication init
	bool exit = true;
	//sf::UdpSocket socket;

	// Timer to disconnect the player using ping-pong 



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
	//newConnection con;

	while (true) {
		// We control if the server sends "exit", the server disconnects all the players
		/*std::string disconnectClients;
		std::cin >> disconnectClients;

		if (disconnectClients._Equal("exit"))
		{
			for (int i = 0; i < clients.size(); i++)
			{
				clients.erase(clients.cbegin() + i);
			}
		}*/
		
		// Logic for receiving
		socket.receive(inPacket, remoteIP, remotePort);

		//Paquete << Accion << Contenido
		inPacket >> action >> content;

		std::cout << action << " " << content << " " << " " << remotePort << std::endl;

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
		}
		else if (action == "CREATE") {
			std::cout << "creating game..." << std::endl;
			for (int j = 0; j < clients.size(); j++)
			{
				if (clients[j].port == socket.getLocalPort())
				{
					hasCreatedGame[j] = true;
					std::cout << "Numero de Games creados: " + games.size() << std::endl;
				}
			}
		}
		else if (action == "JOINED") {
			std::cout << "joining game..." << std::endl;
			int nameChar, diff, lastDiff;
			std::cout << games.size() << std::endl;
			if (games.size() % 2 != 0 || games.size() == 0)
			{
				for (int j = 0; j < clients.size(); j++)
				{
					if (clients[j].port == socket.getLocalPort())
					{
						std::cout << "HOla juancarlos" << std::endl;
						hasCreatedGame[j] = true;
						Send(&clients[j], &socket, "NO_GAME");
					}
				}
			}
			else
			{
				for (int j = 0; j < clients.size(); j++)
				{
					if (clients[j].port == socket.getLocalPort())
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
										clientID = i;
									}
								}
							}
						}
					}
				}
			}
		}
		else if (action == "EXIT")
		{
			//Disconnect client
		}
		else if (action == "MOV")
		{
			//Validar movimiento
			//Comunicar a los otros clientes el movimiento del jugador
		}
		//if (ping) {
		//	Ping(&con, &socket);
		//	ping = false;
		//}
	}
}
