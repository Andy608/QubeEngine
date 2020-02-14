#include <iostream>

extern int QubeEngineMain(void);

int main(int argc, char* argv[])
{
	std::cout << "Common Main Entrance." << std::endl;
	return QubeEngineMain();
}