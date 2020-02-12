#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <nanogui/nanogui.h>

#include <qubeengine/core/QubeEngine.h>

#include <matrix.hpp>
#include <spdlog/logger.h>

int QubeEngineMain()
{
	std::cout << "Hello, Qube Engine." << std::endl;
	std::cin.get();

	qe::QubeEngine::constructEngine();

	glm::mat4 m;

	return 0;
}