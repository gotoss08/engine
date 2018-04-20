/*
 * config.cpp
 *
 *  Created on: Apr 18, 2018
 *      Author: gotoss08
 */

#include "config.h"

int Config::Load() {
	xml_document config_doc;
	xml_parse_result result = config_doc.load_file("config.xml");
	cout << "config loading status: " << result.description() << endl;

	xml_node config_node = config_doc.child("config");

	for (const xml_node& option : config_node.children()) {
		options_map[option.name()] = option.text().as_string();
	}


	for (const auto& kv : options_map) {
		cout << kv.first << ": " << kv.second << endl;
	}

	return 0;
}

string Config::operator [](string key) {
	return (options_map[key.c_str()]);
}
