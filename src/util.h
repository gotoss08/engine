/*
 * util.h
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <chrono>
#include <string>
#include <iostream>

bool is_number(const std::string&);

class Timer {
	std::chrono::high_resolution_clock clock;
	std::chrono::high_resolution_clock::time_point start_tp;
public:
	void start();
	int stop();
};

#endif /* UTIL_H_ */
