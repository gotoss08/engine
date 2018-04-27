/*
 * util.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "util.h"

bool is_number(const std::string &s)
{
	return !s.empty() && s.find_first_not_of("0123456789") == std::string::npos;
}

void Timer::start()
{
	start_tp = clock.now();
}

int Timer::stop()
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>((clock.now() - start_tp)).count();
}
