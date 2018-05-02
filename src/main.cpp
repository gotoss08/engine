/*
 * main.cpp
 *
 *  Created on: Apr 16, 2018
 *      Author: gotoss08
 */

#define LOGURU_IMPLEMENTATION 1
#include "engine.h"
#include "libs/loguru.hpp"

int main(int argc, char *argv[]) {
    loguru::init(argc, argv);
    loguru::add_file("log.txt", loguru::Truncate, loguru::Verbosity_MAX);

    Engine *engine = new Engine();

    if (engine->Init("engine_x64") != 0) {
        LOG_F(ERROR, "Unable to initialize engine.");
        return -1;
    }

    engine->Loop(60, 60);

    delete engine;

    return 0;
}
