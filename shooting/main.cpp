#include "game.h"
#include "Servidor.h"
#include "Cliente.h"
#include "entities.h"

// Funcion para leer la consola
void GetLineFromCin_t(std::string* mssg, bool* exit) {
	while (*exit) {
		std::string line;
		std::getline(std::cin, line);
		mssg->assign(line);
	}
}

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
		
		Cliente c;
		c.ClientMain();
	}


}