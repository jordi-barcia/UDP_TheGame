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
	return closestClient; // Devuelve la posicion en el vector del cliente mas cercano
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
	
	int random = rand() % packetLostProb; // Crea un numero random con el numero proporcionado para forzar la perdida de paquetes. 
	if (random == 0) // Envia el mensaje solo si el valor es 0.
	{
		sock->send(outPacket, con->ip, con->port);
		std::cout << "Critical Sending: " << message << " " << con->name << " " << packetID << std::endl;
		check = "SENDED";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
		if (*action == "PONG") // Si el contenido del mensaje es PONG lo trata como paquete no critico. (Faltaria añadir los mensajes de action = MOV)
		{
			std::cout << "Receive: " << *action << " " << *content << std::endl;
		}
		else { // Si el mensaje es de otro tipo lo trata como critico.
			std::cout << "Critical Receive: " << *action << " " << *content << " " << *packetID << std::endl;
			packIDreceived = *packetID;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Servidor::Hello(Client* con, sf::UdpSocket* sock)
{
	//SEND
	if (action == "HELLO") // Si la action es un HELLO le envia CH_SYN
	{
		CriticalSend(con, sock, "CH_SYN", 0);
	}

	else if (action == "CH_ACK") // Si la action es un CH_ACK lo elimina de la tabla de clientes no conectados y lo pone en la tabla de conectados.
	{
		std::cout << "CONNECTED" << std::endl;

		clients.push_back(*con);
		
		for (int i = 0; i < NoConnectedClients.size(); i++)
		{
			NoConnectedClients.erase(NoConnectedClients.begin() + i);
		}
		
		create = false;
		hasCreatedGame.push_back(create);

		timer.init(initialTime);
		timers.push_back(timer);
		action = "";
		hasHello = false;
	}
}

void Servidor::RTTChanger() 
{
	while (true)
	{
		std::cin >> key;
		if (key == '9') // Sube la probabilidad de perdida de mensaje.
		{
			packetLostProb++;
			std::cout << "Message failure prob: " << 100/packetLostProb << "%" << std::endl;
		}
		else if (key == '8')
		{
			if (packetLostProb > 1)// Baja la probabilidad de perdida de mensaje.
			{
				packetLostProb--;
				std::cout << "Message failure prob: " << 100/packetLostProb << "%" << std::endl;
			}			
		}
	}
}

void Servidor::RTTCalculation()
{
	while (true)
	{
		double aux = 0.0f;
		for (int i = 0; i < rttContainer.size(); i++)
		{
			if (i < 10)
			{
				aux = aux + rttContainer[i];
			}
		}
		aux /= 10.0f;
		
		std::cout << "RTT: " << std::setprecision(10) << aux << std::endl; // Imprime por pantalla el RTT de los ultimos 10 paquetes criticos.
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void Servidor::PacketChecker() {
	
	while (true)
	{
		if (clients.size() > 0) { // Entra si hay algun cliente conectado
			for (int j = 0; j < clients.size(); j++)
			{
				for (int i = 0; i < packets.size(); i++)// Revisa por cada cliente los paquetes que tiene sin confirmar
				{
					if (packets.size() > 0) // Revisamos si hay algun paquete.
					{
						if (timersCritic.size() > 0)
						{
							timersCritic[i].update();
							rttxPacket += expected_frametime; // Aumentamos el tiempo del RTT del paquete.
							if (clients[j].name == packets[i].clientName)
							{
								if (action == "CREATE" || action == "JOINED") { // Los clientes conectados revisan si han recibido uno de estos dos mensajes.
									if (timersCritic[i].temp <= 0) {// Si el tiempo de reenvio del paquete es inferior a 0 se hace el reenvio. 
										if (clients[j].name == content)
										{
											timers[j].init(initialTime); // Reiniciamos el tiempo del PING PONG.
											pingCounter = -1;
										}
										CriticalSend(&con, &socket, packets[i].action, pack.packetID);
										timersCritic[i].init(0.5f);
										std::this_thread::sleep_for(std::chrono::milliseconds(500));
									}
								}
								if(check == "SENDED") // Al confirmar un envio de un paquete critico revisamos que la id del paquete recibido sea la misma que el enviado y lo borramos de la tabla de paquetes.
								{
									if (packIDreceived == packets[i].packetID) {
										std::cout << "Erased Packet: " << packets[i].action << std::endl;

										rttContainer.insert(rttContainer.begin(), rttxPacket);
										rttxPacket = 0.0f;

										packets.erase(packets.begin() + i);
										timersCritic.erase(timersCritic.begin() + i);
										check = "";
										action = "";
										selectGame = false;
									}
								}								
							}
						}
					}
				}
			}
		}

		if (NoConnectedClients.size() > 0) { // Entra si hay algun cliente no conectado
			for (int j = 0; j < NoConnectedClients.size(); j++)
			{
				for (int i = 0; i < packets.size(); i++)// Revisa por cada cliente los paquetes que tiene sin confirmar
				{
					if (packets.size() > 0)// Revisamos si hay algun paquete.
					{
						if (timersCritic.size() > 0)
						{
							timersCritic[i].update();
							rttxPacket += expected_frametime; // Aumentamos el tiempo del RTT del paquete.
							if (NoConnectedClients[j].name == packets[i].clientName)
							{
								if (packIDreceived == packets[i].packetID)// Revisamos que la id del paquete recibido sea la misma que el enviado y lo borramos de la tabla de paquetes.
								{
									std::cout << "Erased Packet: " << packets[i].action << std::endl;

									rttContainer.insert(rttContainer.begin(), rttxPacket);
									rttxPacket = 0.0f;

									packets.erase(packets.begin() + i);
									timersCritic.erase(timersCritic.begin() + i);
								}
								else if (timersCritic[i].temp <= 0) {// Si el tiempo de reenvio del paquete es inferior a 0 se hace el reenvio. 
									if (action != "CH_ACK") {
										Hello(&con, &socket);
										std::this_thread::sleep_for(std::chrono::milliseconds(500));
									}
									timersCritic[i].init(0.5f);
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
	timer.init(0.5f);
	timersCritic.push_back(timer);
}

void Servidor::PingPong()
{
	if (timers.size() > 0) {
		for (int i = 0; i < timers.size(); i++) {
			timers[i].update();
			if (timers[i].temp <= 0) { // Cada 10 segundos enviara un PING
				if (pingCounter == -1)
				{
					Send(&clients[i], &socket, "PING");
					pingCounter++;
					timers[i].init(2);
				}
				else if (pingCounter < 4 && pingCounter >= 0) // Cada 2 segundos se enviara PING (Hasta 5)
				{
					Send(&clients[i], &socket, "PING");
					pingCounter++;
					timers[i].init(2);
				}
				else if (pingCounter > 4) // Si no responde a los PINGS se desconecta al cliente
				{
					action == "EXIT_CL";
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Servidor::ShutdownServer(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);

		if (line.size() > 0)
		{
			if (line == "exit") // Si se escribe "exit" por consola el servidor se cierra vaciando todas las tablas. 
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
				clients.clear();
				NoConnectedClients.clear();
				timersCritic.clear();
				timers.clear();
				packets.clear();
				*exit = false;
				std::terminate();
			}
		}
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
	unsigned int port = 0;

	// Application loop
	sf::IpAddress remoteIP;
	unsigned short remotePort = 0;

	//Threads
	
	//Thread para cerrar el servidor
	std::thread shutdownServerThread(&Servidor::ShutdownServer, this, &sendMessage, &exit);
	shutdownServerThread.detach();
	
	//Thread para recibir los mensajes de los clientes
	std::thread criticalReceiveFromClients(&Servidor::CriticalReceive, this, &socket, &inPacket, &remotePort, &remoteIP, &action, &content, &IDpack);
	criticalReceiveFromClients.detach();

	//Thread para revisar los paquetes de los clientes
	std::thread packetCheckerThread(&Servidor::PacketChecker,this);
	packetCheckerThread.detach();

	//Thread para ir calculando el RTT
	std::thread rttCalculationThread(&Servidor::RTTCalculation, this);
	rttCalculationThread.detach();
	
	//Thread para leer el % de fallo del envio de paquetes
	std::thread rttChangerThread(&Servidor::RTTChanger, this);
	rttChangerThread.detach();

	srand(time(NULL));


	while (exit) {
		// Logic for receiving
		//Paquete << Accion << Contenido
		if (inPacket.getDataSize() > 0)
		{
			if (action == "HELLO" && !hasHello)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				//Guarda la configuracion del cliente
				con.name = content;
				con.port = remotePort;
				con.ip = remoteIP;
				//Guarda la informacion en la tabla de clientes no conectados
				NoConnectedClients.push_back(con);
				std::cout << con.name << " " << con.port << " " << con.ip << std::endl;
				SavePacketContent(1, "CH_SYN", con.name);
				Hello(&con, &socket);
				hasHello = true;
			}
			else if (action == "CH_ACK" && hasHello)
			{
				Hello(&con, &socket);
				hasHello = false;
			}
			else if (action == "CREATE" && !selectGame) {// Si recibe CREATED empieza el proceso de creacion.
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
						IDpack = 3;
						SavePacketContent(IDpack, "CREATE_ACK", clients[j].name);
						CriticalSend(&con, &socket, "CREATE_ACK", IDpack);
					}
				}
				inPacket.clear();
				selectGame = true;
			}
			else if (action == "JOINED" && !selectGame) { // Si recibe JOINED empieza el proceso de conexion del cliente.
				std::cout << "joining game..." << std::endl;
				if (games.size() % 2 == 0 || games.size() == 0) // Si no hay juegos creados para que el se una envia un NO_GAME al cliente para decirle que mande un create. 
				{
					Send(&con, &socket, "NO_GAME");
				}
				else { // Conecta al cliente que busca partida con el que tenga la primera letra del nombre mas cercana a la suya 
					int indexClosestClient = GetClosestClient(remotePort);
					int gameId = clientToGames[clients[indexClosestClient].name];
					clientToGames[con.name] = gameId; // Content = client.name(El que pide el join)
					Client ClientName = GetClientFromName(con.name);
					IDpack = 3;
					SavePacketContent(IDpack, "JOIN_ACK", con.name);
					CriticalSend(&ClientName, &socket, "JOIN_ACK", IDpack);
					selectGame = true;
				}
				inPacket.clear();
			}
			else if (action == "EXIT_CL") // Si recibe EXIT_CL por parte del cliente empieza la desconnexion
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (clients[i].name == content) // SI encuentra al cliente en cuestion lo borra de la tabla junto a su timer. 
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
				for (int i = 0; i < packets.size(); i++)  // Borra los paquetes y los Timers de cada paquete
				{
					if (packets[i].clientName == content)
					{
						timersCritic.erase(timersCritic.begin() + i);
						packets.erase(packets.begin() + i);
					}
				}
			}

			if (action == "PONG") // Si recibe un PONG se resetean los valores del PING
			{
				for (int i = 0; i < clients.size(); i++)
				{
					if (clients[i].name == content)
					{
						timers[i].init(initialTime);
						pingCounter = -1;
						action = "";
					}
				}
			}

			if (action == "MOV")
			{
				//Validar movimiento
				//Comunicar a los otros clientes el movimiento del jugador
			}
		}
		PingPong();
	}
}

