#include "Cliente.h"

void Cliente::SendPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg)
{
	sf::Packet outPacket;
	outPacket << actionMssg << contentMssg;
	if (sock->send(outPacket, "127.0.0.1", 5000) != sock->Done)
	{
		std::cout << "Sending error: " << sock->Error << std::endl;
	}
	std::cout << "Sending: " + actionMssg << " " + contentMssg << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Cliente::SendCritPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg, int packetID)
{
	sf::Packet outPacket;
	outPacket << actionMssg << contentMssg << packetID;
	if (sock->send(outPacket, "127.0.0.1", 5000) != sock->Done)
	{
		std::cout << "Sending error: " << sock->Error << std::endl;
	}
	std::cout << "Critical Sending: " + actionMssg << " " << contentMssg << " " << packetID << std::endl; // Envia el paquete pero con la ID
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Cliente::ReceiveCriticalPacket(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg, int* packetID)
{
	while (true)
	{
		sf::Packet inPacket;
		sock->receive(inPacket, serverIp, serverPort);
		inPacket >> *actionMssg >> *contentMssg >> *packetID;
		if (*actionMssg == "PING") // Si el paquete es un PING lo trata como no critico.
		{
			std::cout << "Receive: " << *actionMssg << " " << *contentMssg << std::endl;
		}
		else { // Los demas paquetes los trata como criticos.

			std::cout << "Critical Receive: " << *actionMssg << " " << *contentMssg << " " << *packetID << std::endl;
			packIDreceived = *packetID;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Cliente::GameSelected(sf::UdpSocket* sock)
{
	sf::Packet outPacket;

	//SEND
	if (gc.created) { // Si el cliente ha escogido crear una partida le envia al servidor CREATE
		content = gc.name;
		packetCounter = 3;
		SavePacketContent(3, "CREATE", gc.name);
		SendCritPacket(sock, "CREATE", content, packetCounter);
		gc.created = false;
	}
	else if (gc.joined) { // Si el cliente ha escogido unirse una partida le envia al servidor JOINED
		content = gc.name;
		packetCounter = 3;
		SavePacketContent(3, "JOINED", gc.name);
		SendCritPacket(sock, "JOINED", content, packetCounter);
		gc.joined = false;
	}
}

void Cliente::HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket)
{
	//SEND
	if (action == "CH_SYN") // Programar reenvio de paquete CH_SYN;
	{
		SavePacketContent(packetCounter, "CH_ACK", gc.name);
		SendCritPacket(sock, "CH_ACK", gc.name, 1);
		gc.startMenu = true;
		action = "";

		//Simula que se pulsa la tecla enter para poder pasar a la seleccion del modo de juego.
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		gc.event.type = sf::Event::KeyPressed;
		gc.event.key.code = sf::Keyboard::Return;
		gc.input = "Return";
	}
	else if (!hasHello) // Programar reenvio de paquete HELLO
	{
		SavePacketContent(packetCounter, "HELLO", gc.name);
		SendCritPacket(sock, "HELLO", gc.name, packetCounter);
		hasHello = true;
	}

}

void Cliente::PacketChecker(sf::UdpSocket* sock)
{
	while (true)
	{
		for (int i = 0; i < packets.size(); i++)
		{
			if (packets.size() > 0)
			{
				if (timersCritic.size() > 0)
				{
					timersCritic[i].update();
					if (packIDreceived == packets[i].packetID) // Si la ID del paquete local coincide con la del paquete remoto lo borra de la tabla de paquetes.
					{
						std::cout << "Erased Packet: " << packets[i].action << std::endl;
						packets.erase(packets.begin() + i);
						timersCritic.erase(timersCritic.begin() + i);
					}
					else if (timersCritic[i].temp <= 0) { //Si el tiempo de reenvio es inferior a 0 se reenvia el paquete mientras no sea un PING.
						if (action != "PING")
						{
							SendCritPacket(sock, packets[i].action, gc.name, packetCounter);
							std::this_thread::sleep_for(std::chrono::milliseconds(500));
						}
						timersCritic[i].init(0.5f);
					}
				}
			}
		}
	}
}

void Cliente::SavePacketContent(int pId, std::string action, std::string cName)
{
	pack.packetID = pId;
	pack.action = action;
	pack.clientName = cName;
	packets.push_back(pack);

	timer.init(0.5f);
	timersCritic.push_back(timer);
}

void Cliente::ClientMain()
{
	// Aplication init
	bool exit = true;
	sf::UdpSocket socket;

	// Client mode
	std::cout << "Client mode running" << std::endl;

	socket.bind(socket.getLocalPort());

	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;

	sf::IpAddress serverIP = "127.0.0.1";
	unsigned short serverPort = 5000;

	//Threads

	//Thread para recibir los paquetes.
	std::thread criticalReceive(&Cliente::ReceiveCriticalPacket, this, &socket, &action, &content, &packetCounter);
	criticalReceive.detach();

	//Thread para revisar la tabla de paquetes.
	std::thread packetCheckerThread(&Cliente::PacketChecker, this, &socket);
	packetCheckerThread.detach();

	gc.ClientSetup();
	while (true) {

		if (gc.chooseGame)
		{
			if (action == "CH_SYN")
			{
				HelloClient(&socket, &inPacket);
			}
			else if (!hasHello) {
				HelloClient(&socket, &inPacket);
				hasHello = true;
			}
		}

		if (action == "JOIN_ACK")
		{
			std::cout << "JOINED GAME" << std::endl;
			action = "";
		}

		if (action == "CREATE_ACK")
		{
			std::cout << "CREATED GAME" << std::endl;
			action = "";
		}

		if (gc.created || gc.joined) {
			GameSelected(&socket);
		}

		if (action == "NO_GAME") //Para que el cliente le envie CREATE al server en el caso de que no haya games creados
		{
			content = gc.name;
			gc.isFirstGame = true;
			packets.erase(packets.begin());
			timersCritic.erase(timersCritic.begin());
			packetCounter = 3;
			packIDreceived = -1;
			SavePacketContent(packetCounter, "CREATE", gc.name);
			SendCritPacket(&socket, "CREATE", content, packetCounter);
			gc.created = false;
			action = "";
		}

		gc.updateGame();

		if (gc.hasExit || action == "EXIT") // Si el juego se cierra o el servidor se apaga el cliente se desconnecta. 
		{
			content = gc.name;
			SendPacket(&socket, "EXIT_CL", content);
			break;
		}

		if (action == "PING") // Si recibe PING le envia un PONG al servidor
		{
			SendPacket(&socket, "PONG", gc.name);
			action = "";
		}

		if (action == "CREATE_ACK" || action == "JOIN_ACK") { // Si recibe un CREATE_ACK o un JOIN_ACK empieza la partida
			gc.startPlaying = true;
		}
	}
}
