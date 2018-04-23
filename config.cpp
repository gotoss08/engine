/*
 * config.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "config.h"

int Config::Load() {
	pugi::xml_document config_doc;
	pugi::xml_parse_result result = config_doc.load_file("config.xml");
	std::cout << "config loading status: " << result.description() << std::endl;

	pugi::xml_node config_node = config_doc.child("config");

	for (const pugi::xml_node& option : config_node.children()) {
		options_map[option.name()] = option.text().as_string();
	}


	for (const auto& kv : options_map) {
		std::cout << kv.first << ": " << kv.second << std::endl;
	}

	return 0;
}

std::string Config::Get(std::string key) {
	return (options_map[key.c_str()]);
}

void Config::Set(std::string key, std::string value) {
	options_map[key.c_str()] = value;
}
