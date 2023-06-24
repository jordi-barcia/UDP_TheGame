#include "Cliente.h"

Cliente::Cliente()
{
	this->ip = "127.0.0.1";
	this->port = 5000;
	this->name = gc.name;
}

void Cliente::GetLineFromCin_t(std::string* mssg, bool* exit)
{
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}


//void Cliente::Pong() {
//	std::cout << "PING RECIBIDO" << std::endl;
//}


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
		packetCounter++;
	}
	else
	{
		outPacket << "HELLO" << gc.name;
		packetCounter++;
		if (sock->send(outPacket, "127.0.0.1", 5000) != sf::Socket::Done)
		{
			//This is an error
		}
	}
}

void Cliente::SendPacket(sf::UdpSocket* sock, std::string actionMssg, std::string contentMssg)
{
	sf::Packet outPacket;
	outPacket << actionMssg << contentMssg;
	sock->send(outPacket, "127.0.0.1", 5000);
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

	//Threads
	std::thread read_console_t(&Cliente::GetLineFromCin_t, this, &action, &exit);
	read_console_t.detach();

	gc.ClientSetup();
	while (true) {
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
		//std::cout << check << std::endl;
		if (action == "MSG")
		{

		}

		if (action == "JOIN_ACK")
		{
			std::cout << "JOINED GAME" << std::endl;
		}

		if (action == "PING") {
			std::cout << "PING RECIBIDO" << std::endl;
			SendPacket(&socket, "PONG", name);
		}

		if (gc.created || gc.joined) {
			GameSelected(&socket);
		}
		if (action == "NO_GAME")
		{
			std::cout << "ASEREGE" << std::endl;
			noGame = true;
			GameSelected(&socket);
			SendPacket(&socket, "CREATE", content);
		}
		
		gc.RunConnections();

		if (gc.hasExit || action == "EXIT")
		{
			content = gc.name;
			SendPacket(&socket, "EXIT_CL", content);
			break;
		}
	}
	// When the application loop is broken, we have to release resources
}

void Cliente::GameSelected(sf::UdpSocket* sock)
{
	sf::Packet outPacket;

	//SEND
	if (gc.created) {
		content = name;
		/*outPacket << "CREATE" << content;
		sock->send(outPacket, "127.0.0.1", 5000); */
		SendPacket(sock, "CREATE", content);
		gc.created = false;
		packetCounter++;
		//CreateGame();
	}
	else if (gc.joined) {
		content = name;
		/*outPacket << "JOINED" << content;
		sock->send(outPacket, "127.0.0.1", 5000);*/
		SendPacket(sock, "JOINED", content);
		packetCounter++;
	}
	else if (noGame) 
	{
		SendPacket(sock, "CREATE", content);
		packetCounter++;
		noGame = false;
	}
	//sf::sleep(t1);
}

void Cliente::CreateGame()
{
	//Generar distintos threads para cada partida
	//gc.window.close();
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//std::thread run_game(&GameRun::updateGame, &game);
	//run_game.detach();
	//game.updateGame();
}

void Cliente::JoinGame(std::vector<Game> games)
{
	//Generar terreno del mapa
}

