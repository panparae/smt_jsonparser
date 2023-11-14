#include <fstream>
#include <nlohmann/json.hpp>

#include "nlohmann_module.h"
#include "timecheck.h"

namespace nloh {
	double Run(const char* _file) {
		std::ifstream f(_file);
		timecheck t;
		nlohmann::json data = nlohmann::json::parse(f);
		return t.end();
	}
}