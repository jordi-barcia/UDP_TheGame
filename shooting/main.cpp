#include "game.h"
#include "Servidor.h"
#include "Cliente.h"
#include "entities.h"

int main()
{
	bool exit = true;
	int mode;
	std::string mode_str;
	std::cout << "Select a mode: (1) server, (2) cliente" << std::endl;
	std::cin >> mode_str;
	mode = std::stoi(mode_str);
	

	if (mode == 1) // SERVER
	{
		Servidor s;
		s.StartServer();
	}
	else if (mode == 2){
		
		Cliente c; // Cliente c = new Cliente(..., ..., ...) 
		c.ClientMain();
	}


}