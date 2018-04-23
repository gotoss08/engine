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

class Config {
private:
	std::map<std::string, std::string> options_map;
public:
	int Load();
	std::string Get(std::string);
	void Set(std::string, std::string);
};

#endif /* CONFIG_H_ */
