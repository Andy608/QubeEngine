#include <qubeengine/core/QubeEngine.h>

#include <qubeengine/vulkan_tutorial/VulkanTutorial.h>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
using namespace qe;

int QubeEngineMain()
{
	std::cout << "Hello, Qube Engine." << std::endl;
	
	VulkanTutorial renderer;

	try
	{
		renderer.run();
	}
	catch (const std::exception & e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}