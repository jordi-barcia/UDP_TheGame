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

void Servidor::GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}


//void Servidor::Ping(Client* con, sf::UdpSocket* sock) {
//	sf::Packet outPacket;
//	action = "PING";
//	outPacket << action << con->name;
//	sock->send(outPacket, con->ip, con->port);
//}

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
		if (socket.receive(inPacket, remoteIP, remotePort) != socket.Done) 
		{
			std::cout << "Receive Error: " << socket.Error << std::endl;
		}

		//Paquete << Accion << Contenido
		inPacket >> action >> content;
		std::cout << "Receive: " << action << " " << content << " " << remotePort << std::endl;

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
		else if (action == "MSG")
		{

		}
		else if (action == "CH_SYN")
		{
			//Aqui se tiene que tratar, Conexion con otros servidores
		}
		else if (action == "CH_ACK")
		{
			Hello(&con, &socket);
			//pingPong.init(timeTillPingPong);
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

		/*if (pingPong.temp < 0)
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
							pingPong.temp = timeTillPingPong;
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
			pingPong.update();
			std::cout << "Tiempo restante (PING): "<< pingPong.temp << std::endl;
		}*/


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
