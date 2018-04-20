/*
 * main.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#include <SDL2/SDL.h>
#include "engine.h"

void print_log(void* userdata, int category, SDL_LogPriority priority, const char* message) {
	cout << "log: " << message << endl;
}

int main(int argc, char* argv[]) {
	SDL_LogSetOutputFunction(print_log, NULL);
//	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

	Engine* engine = Engine::getInstance();

	if (engine->Init("game") != 0) {
		SDL_Log("Unable to initialize engine.");
		return -1;
	}

	engine->Loop(60,60);

	return 0;
}
