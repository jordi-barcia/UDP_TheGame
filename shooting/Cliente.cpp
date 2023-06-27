#include "Cliente.h"

void Cliente::DeleteCriticalPacket() 
{
	while (true) {
		if (packets.size() > 0)
		{
			for (int i = 0; i < packets.size(); i++)
			{
				if (packetCounter == packets[i].packetID)
				{
					std::cout << "Erased Packet: " << packets[i].action << std::endl;
					packets.erase(packets.begin() + i);
				}
			}
		}
	}
}

void Cliente::SafePacketContent(int pId, std::string action, std::string cName)
{
	pack.packetID = pId;
	pack.action = action;
	pack.clientName = cName;
	packets.push_back(pack);
}

void Cliente::HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket)
{
	//SEND
	if (action == "CH_SYN") // Programar reenvio de paquete CH_SYN;
	{
		//packetCounter++;
		SafePacketContent(packetCounter, "CH_ACK", gc.name);
		SendCritPacket(sock, "CH_ACK", gc.name, packetCounter);
		action = "";
	}
	else if(!hasHello) // Programar reenvio de paquete HELLO
	{
		//packetCounter++;
		SafePacketContent(packetCounter, "HELLO", gc.name);
		SendCritPacket(sock, "HELLO", gc.name,packetCounter);
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
	std::cout << "Critical Sending: " + actionMssg << " " << contentMssg << packetID << std::endl;
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
			std::cout << "Receive: " << *actionMssg << " " << *contentMssg << " " << std::endl;
		}
		else {
			
			std::cout << "Critical Receive: " << *actionMssg << " " << *contentMssg << " " << *packetID << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

	//Threads
	/*std::thread receive(&Cliente::RecieveMessage, this, &socket, &action, &content);
	receive.detach();*/

	std::thread criticalReceive(&Cliente::ReceiveCriticalPacket, this, &socket, &action, &content, &packetCounter);
	criticalReceive.detach();


	std::thread deleteConfirmedPackets(&Cliente::DeleteCriticalPacket, this);
	deleteConfirmedPackets.detach();

	

	gc.ClientSetup();
	while (true) {

		if (gc.chooseGame && !hasHello)
		{			
			if (action == "CH_SYN")
			{
				hasHello = true;
			}
			HelloClient(&socket, &inPacket);
			
		}

		if (action == "JOIN_ACK")
		{
			std::cout << "JOINED GAME" << std::endl;
			action = "";
		}


		if (gc.created || gc.joined) {
			GameSelected(&socket);
		}

		if (action == "NO_GAME") //Para que el cliente le envie CREATE al server en el caso de que no haya games creados
		{
			//noGame = true;
			//GameSelected(&socket);
			gc.isFirstGame = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			SendPacket(&socket, "CREATE", content);
			SafePacketContent(packetCounter++, action, gc.name);
			action = "";
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
	}
	// When the application loop is broken, we have to release resources
}

void Cliente::GameSelected(sf::UdpSocket* sock)
{
	sf::Packet outPacket;

	//SEND
	if (gc.created) {
		content = gc.name;
		SendPacket(sock, "CREATE", content);
		SafePacketContent(packetCounter++, action, gc.name);
		gc.created = false;
	}
	else if (gc.joined) {
		content = gc.name;
		SendPacket(sock, "JOINED", content);
		SafePacketContent(packetCounter++, action, gc.name);
		gc.joined = false;
	}
}


/*void Cliente::RecieveMessage(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg)
{
	while (true)
	{
		sf::Packet inPacket;
		sock->receive(inPacket, serverIp, serverPort);
		inPacket >> *actionMssg >> *contentMssg;
		std::cout << "Receive: " + *actionMssg << " " + *contentMssg << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}*/




