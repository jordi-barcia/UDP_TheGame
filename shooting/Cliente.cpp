#include "Cliente.h"

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
	//SEND
	//*inPacket >> action >> content;
	if (action == "CH_SYN")
	{
		SendPacket(sock, "CH_ACK", gc.name);
		packetCounter++;
	}
	else
	{
		SendPacket(sock, "HELLO", gc.name);
		packetCounter++;
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
	std::thread receive(&Cliente::RecieveMessage, this, &socket, &action, &content);
	receive.detach();
	//std::thread printScreen(&Cliente::printSomething, this);
	//printScreen.detach();

	bool hasHello = false;

	gc.ClientSetup();
	while (true) {

		if (gc.chooseGame && !hasHello)
		{
			HelloClient(&socket, &inPacket);

			if (action == "CH_SYN")
			{
				HelloClient(&socket, &inPacket);
				hasHello = true;
			}
		}

		if (action == "JOIN_ACK")
		{
			std::cout << "JOINED GAME" << std::endl;
			action = "";
		}

		/*if (action == "PING") {
			std::cout << "PING RECIBIDO" << std::endl;
			SendPacket(&socket, "PONG", name);
		}*/

		if (gc.created || gc.joined) {
			GameSelected(&socket);
		}

		/*if (action == "NO_GAME") //Para que el cliente le envie CREATE al server en el caso de que no haya games creados
		{
			noGame = true;
			GameSelected(&socket);
			SendPacket(&socket, "CREATE", content);
		}*/

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
		content = gc.name;
		/*outPacket << "CREATE" << content;
		sock->send(outPacket, "127.0.0.1", 5000); */
		SendPacket(sock, "CREATE", content);
		gc.created = false;
		packetCounter++;
		//CreateGame();
	}
	else if (gc.joined) {
		content = gc.name;
		/*outPacket << "JOINED" << content;
		sock->send(outPacket, "127.0.0.1", 5000);*/
		SendPacket(sock, "JOINED", content);
		gc.joined = false;
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

void Cliente::RecieveMessage(sf::UdpSocket* sock, std::string* actionMssg, std::string* contentMssg)
{
	while (true)
	{
		sf::Packet inPacket;
		sock->receive(inPacket, serverIp, serverPort);
		inPacket >> *actionMssg >> *contentMssg;
		std::cout << "Receive: " + *actionMssg << " " + *contentMssg << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Cliente::printSomething()
{
	while (true)
	{
		std::cout << "Threads activos" << std::endl;
	}
}

