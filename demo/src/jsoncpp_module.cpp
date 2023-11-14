#include <fstream>
#include <json/json.h>

#include "jsoncpp_module.h"
#include "timecheck.h"

namespace jcpp {
	double Run(const char* _file) {
		Json::Value root;
		std::ifstream config_doc(_file, std::ifstream::binary);
		timecheck t;
		config_doc >> root;
		return t.end();
	}
}