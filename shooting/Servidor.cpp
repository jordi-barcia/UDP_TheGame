#include "Servidor.h"

void Servidor::Countdown() {
	
}

void Servidor::Ping(newConnection* con, sf::UdpSocket* sock) {
	sf::Packet outPacket;
	std::string ping = "PING";
	outPacket << "PING" << ping;
	sock->send(outPacket, con->ip, con->port);
	std::cout << "AAAAAA" << std::endl;
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
			
		}
		else if (action == "JOINED") {
			std::cout << "joining game..." << std::endl;
		}
		//if (ping) {
		//	Ping(&con, &socket);
		//	ping = false;
		//}
	}
}
