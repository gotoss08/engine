/*
 * util.h
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <chrono>

using namespace std::chrono;

bool is_number(const std::string&);

class Timer {
	high_resolution_clock clock;
	high_resolution_clock::time_point start_tp;
public:
	void start();
	int stop();
};

#endif /* UTIL_H_ */
