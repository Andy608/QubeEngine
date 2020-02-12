#include <iostream>

extern int QubeEngineMain(void);

int main(int argc, char* argv[])
{
	std::cout << "Common Main Entrance." << std::endl;
	std::string* s = new std::string("hello, world!");
	delete s;
	std::cin.get();

	return QubeEngineMain();
}