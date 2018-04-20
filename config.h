/*
 * config.h
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <iostream>
#include <map>
#include "pugixml.hpp"

using namespace std;
using namespace pugi;

class Config {
	map<string, string> options_map;
public:
	int Load();
	string operator[](string);
};

#endif /* CONFIG_H_ */
