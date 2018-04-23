/*
 * main.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#include <SDL2/SDL.h>
#define LOGURU_IMPLEMENTATION 1
#include "libs/loguru.hpp"
#include "engine.h"

void print_log(void* userdata, int category, SDL_LogPriority priority, const char* message) {
	std::cout << "log: " << message << std::endl;
}

int main(int argc, char* argv[]) {
	loguru::init(argc, argv);
	SDL_LogSetOutputFunction(print_log, NULL);
//	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

	Engine* engine = Engine::getInstance();

	if (engine->Init("game") != 0) {
		SDL_Log("Unable to initialize engine.");
		return -1;
	}

	engine->Loop(5,60);

	delete engine;

    system("PAUSE");
	return 0;
}
