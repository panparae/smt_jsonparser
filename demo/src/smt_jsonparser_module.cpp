#include <fstream>
#include <smt_jsonparser/smt_jsonparser.h>

#include "smt_jsonparser_module.h"
#include "timecheck.h"


bool ReadFile(const char* _path, std::string* _out) {
	std::ifstream file(_path);
	std::string strFile;

	if (file.is_open()) {
		file.seekg(0, std::ios::end);
		auto size = file.tellg();
		strFile.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(&strFile[0], size);
		*_out = strFile;

		return true;
	}

	return false;
}

namespace smt {
	double Run(const char* _file) {
		std::string str;
		ReadFile(_file, &str);
		timecheck t;
		json myjson(str);
		myjson.Parse();
		return t.end();
	}
}