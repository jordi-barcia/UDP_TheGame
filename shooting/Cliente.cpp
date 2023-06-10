#include "Cliente.h"

Cliente::Cliente()
{
	this->ip = "127.0.0.1";
	this->port = 5000;
	this->name = gc.name;
}
void Cliente::Pong() {
	std::cout << "PING RECIBIDO" << std::endl;
}

void Cliente::Check() {
	//timer.start(std::chrono::milliseconds(1000), [] {
	//	std::puts("Checking...");
	//	});
	//std::this_thread::sleep_for(std::chrono::seconds(1));
	//timer.stop();
	check = false;
}

void Cliente::HelloClient(sf::UdpSocket* sock, sf::Packet* inPacket)
{
	sf::Packet outPacket;

	std::cout << "Debug Action/Content " << action << " " << content << std::endl;
	//SEND
	//*inPacket >> action >> content;
	if (action == "CH_SYN")
	{
		gc.name = content;
		outPacket << "CH_ACK" << gc.name;
		sock->send(outPacket, "127.0.0.1", 5000);
		std::cout << "ENVIO" << std::endl;
	}
	else
	{
		outPacket << "HELLO" << gc.name;
		if (sock->send(outPacket, "127.0.0.1", 5000) != sf::Socket::Done)
		{
			//This is an error
		}
	}
	sf::sleep(t1);
}

void Cliente::ClientMain()
{
	// Aplication init
	bool exit = true;
	sf::UdpSocket socket;
	

	// Client mode
	std::cout << "Client mode running" << std::endl;


	socket.bind(socket.getLocalPort());
	//std::cin >> name;

	sf::Packet inPacket, outPacket;
	std::string sendMessage, rcvMessage;

	sf::IpAddress serverIP = "127.0.0.1";
	unsigned short serverPort = 5000;


	gc.ClientSetup();
	while (gc.window.isOpen()) {
		if (!gc.pingPong && gc.chooseGame) {
			HelloClient(&socket, &inPacket);
			// Logic for receiving
			socket.receive(inPacket, serverIP, serverPort);
			inPacket >> action >> content;

			std::cout << action << " " << content << " " << serverPort << std::endl;

			if (action == "HELLO" || action == "CH_ACK")
			{
				//Aqui se tiene que tratar, Conexion con otros clientes
			}
			else if (action == "CH_SYN")
			{
				HelloClient(&socket, &inPacket);
			}
			gc.pingPong = true;
		}		
		else if (!check && gc.pingPong) {
			// Logic for receiving
			//socket.receive(inPacket, serverIP, serverPort);
			//inPacket >> action >> content;
			check = true;
			//Check();
		}
		std::cout << check << std::endl;
		if (action == "MSG")
		{

		}
		if (action == "PING") {
			Pong();
			std::cout << "PING RECIBIDO" << std::endl;
		}
		if (gc.created || gc.joined) {
			GameSelected(&socket);
		}

		gc.RunConnections();
	}
	// When the application loop is broken, we have to release resources
}

void Cliente::GameSelected(sf::UdpSocket* sock)
{
	sf::Packet outPacket;

	//SEND
	if (gc.created) {
		content = gc.created;
		outPacket << "CREATED" << content;
		sock->send(outPacket, "127.0.0.1", 5000); 
	}
	else if (gc.joined) {
		content = gc.joined;
		outPacket << "JOINED" << content;
		sock->send(outPacket, "127.0.0.1", 5000);
	}
	sf::sleep(t1);
}


