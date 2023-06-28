#include "Cliente.h"


void Cliente::SavePacketContent(int pId, std::string action, std::string cName)
{
	pack.packetID = pId;
	pack.action = action;
	pack.clientName = cName;
	packets.push_back(pack);

	timer.init(0.5f);
	timersCritic.push_back(timer);
}

void Cliente::HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket)
{
	//SEND
	if (action == "CH_SYN") // Programar reenvio de paquete CH_SYN;
	{
		//packetCounter++;
		SavePacketContent(packetCounter, "CH_ACK", gc.name);
		SendCritPacket(sock, "CH_ACK", gc.name, 1);
		action = "";
	}
	else if (!hasHello) // Programar reenvio de paquete HELLO
	{
		//packetCounter++;
		SavePacketContent(packetCounter, "HELLO", gc.name);
		SendCritPacket(sock, "HELLO", gc.name, packetCounter);
		hasHello = true;
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

}


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
	std::cout << "Critical Sending: " + actionMssg << " " << contentMssg << " " << packetID << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void Cliente::ReceiveCriticalPacket(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg, int* packetID)
{
	while (true)
	{
		sf::Packet inPacket;
		sock->receive(inPacket, serverIp, serverPort);
		inPacket >> *actionMssg >> *contentMssg >> *packetID;
		if (*actionMssg == "PING")
		{
			std::cout << "Receive: " << *actionMssg << " " << *contentMssg << std::endl;
		}
		else {

			std::cout << "Critical Receive: " << *actionMssg << " " << *contentMssg << " " << *packetID << std::endl;
			packIDreceived = *packetID;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
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

	std::thread criticalReceive(&Cliente::ReceiveCriticalPacket, this, &socket, &action, &content, &packetCounter);
	criticalReceive.detach();

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
			//std::this_thread::sleep_for(std::chrono::milliseconds(100));
			packetCounter = 2;
			packIDreceived = -1;
			SavePacketContent(3, "CREATE", gc.name);
			SendCritPacket(&socket, "CREATE", content, packetCounter);
			gc.created = false;
			action = "";
			packetCounter = 3;
		}

		gc.updateGame();

		if (gc.hasExit || action == "EXIT")
		{
			content = gc.name;
			SendPacket(&socket, "EXIT_CL", content);
			break;
		}

		if (action == "PING")
		{
			SendPacket(&socket, "PONG", gc.name);
			action = "";
		}

		if (action == "CREATE_ACK" || action == "JOIN_ACK") {
			gc.startPlaying = true;
		}
	}
	// When the application loop is broken, we have to release resources
}

void Cliente::GameSelected(sf::UdpSocket* sock)
{
	sf::Packet outPacket;

	//SEND
	if (gc.created) {
		content = gc.name;
		packetCounter = 2;
		SavePacketContent(3, "CREATE", gc.name);
		SendCritPacket(sock, "CREATE", content, packetCounter);
		gc.created = false;
		packetCounter = 3;
	}
	else if (gc.joined) {
		content = gc.name;
		packetCounter = 2;
		SavePacketContent(3, "JOINED", gc.name);
		SendCritPacket(sock, "JOINED", content, packetCounter);
		gc.joined = false;
		packetCounter = 3;
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
					//std::cout << IDpack << "." << packets[i].packetID << std::endl;
					if (packIDreceived == packets[i].packetID)
					{
						std::cout << "Erased Packet: " << packets[i].action << std::endl;
						packets.erase(packets.begin() + i);
						timersCritic.erase(timersCritic.begin() + i);
					}
					else if (timersCritic[i].temp <= 0) {
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




