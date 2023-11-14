#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

#include "jsoncpp_module.h"
#include "nlohmann_module.h"
#include "smt_jsonparser_module.h"

const char* JSON_SMALL_PATH = "C:/Workspace/smt_jsonparser/demo/test/files_in/test_file_small.json";
const char* JSON_MEDIUM_PATH = "C:/Workspace/smt_jsonparser/demo/test/files_in/test_file_medium.json";
const char* JSON_LARGE_PATH = "C:/Workspace/smt_jsonparser/demo/test/files_in/test_file_large.json";

enum class color : int {
	red = 4,
	green = 10,
	yellow = 14,
	white = 15
};

void ColorOut(const std::string& _str, color _color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO Info;
	GetConsoleScreenBufferInfo(hConsole, &Info);
	SetConsoleTextAttribute(hConsole, static_cast<int>(_color));
	std::cout << _str;
	SetConsoleTextAttribute(hConsole, Info.wAttributes);
}

void BoxOut(const int& _size) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO Info;
	GetConsoleScreenBufferInfo(hConsole, &Info);
	SetConsoleTextAttribute(hConsole, 240);
	for (int i = 0; i < _size / 2; i++) {
		std::cout << " ";
	}
	SetConsoleTextAttribute(hConsole, Info.wAttributes);
}

void RemoveBackFloat(std::string* _str) {
	for (int i = 0; i < 4; i++) {
		_str->pop_back();
	}
}

int main(int argc, char* argv[]) {
	// Raw String
	//{
	//	std::string str = R"(
	//	{
	//		"myObject" : {
	//			"myNumber" : 10
	//		}
	//	}
	//	)";

	std::string usageStr = "Usage: smt_jsonparser_demo.exe [-F] [-T]\n\n";
	usageStr = usageStr + "  -F" + "\t\t" + "Data size of json file.\n";
	usageStr = usageStr + "  attributes" + "\t " + "0 : Small\n";
	usageStr = usageStr + "\t\t 1 : Medium\n";
	usageStr = usageStr + "\t\t 2 : Large\n" + "\n";
	usageStr = usageStr + "  -T" + "\t\t" + "Number of test iteration.\n";
	usageStr = usageStr + "  attributes" + "\t " + "1~99 : 1~99 number of test allowed.\n";

	std::string dataSize = "Small";
	std::string dataPath = JSON_SMALL_PATH;

	uint32_t testNum = 1;

	if (argc != 3) {
		std::cout << usageStr << "\n";
		return 0;
	}

	std::string arg1 = argv[1];
	bool condA_1 = arg1.size() == 3;
	bool condA_2 = arg1[0] == '-';
	bool condA_3 = arg1[1] == 'F' || arg1[1] == 'f';
	if (condA_1 && condA_2 && condA_3) {
		switch (arg1[2]) {
		case '0':
			dataSize = "Small";
			dataPath = JSON_SMALL_PATH;
			break;
		case '1':
			dataSize = "Medium";
			dataPath = JSON_MEDIUM_PATH;
			break;
		case '2':
			dataSize = "Large";
			dataPath = JSON_LARGE_PATH;
			break;
		}
	}

	std::string arg2 = argv[2];
	bool condB_1 = arg2.size() == 3 || arg2.size() == 4;
	bool condB_2 = arg2[0] == '-';
	bool condB_3 = arg2[1] == 'T' || arg2[1] == 't';
	if (condB_1 && condB_2 && condB_3) {
		arg2.erase(arg2.begin());
		arg2.erase(arg2.begin());
		testNum = std::stoi(arg2) < 1 ? 1 : std::stoi(arg2);
	}

	std::cout << "\n";
	std::cout << "==============================================================================" << "\n";
	ColorOut("+ Demo Running Count: ", color::yellow);
	ColorOut(std::to_string(testNum), color::white);
	std::cout << "\n";
	ColorOut("+ Parsing Json Data Size: ", color::yellow);
	ColorOut(dataSize, color::white);
	std::cout << "\n\n";
	std::cout << "------------------------------------------------------------------------------" << "\n";
	ColorOut("[smt_jsonparser]", color::white);
	std::cout << "\t" << "[JsonCpp]" <<"\t\t" << "[nlohmann-json]" << "\n\n";

	std::vector<std::vector<double>> container;
	for (uint32_t i = 0; i < testNum; i++) {
		std::vector<double> vec;
		
		vec.push_back(smt::Run(dataPath.c_str()));
		{
			std::string str = std::to_string(floor(vec[0] * 100) / 100);
			std::string str2 = std::to_string(floor(vec[0] / vec[0] * 10000) / 100);
			RemoveBackFloat(&str);			
			RemoveBackFloat(&str2);
			str += "ms";
			std::cout << str;
			std::cout << " (" << str2 << "%)";
			std::cout << "\t";
		}

		vec.push_back(jcpp::Run(dataPath.c_str()));
		{
			std::string str = std::to_string(floor(vec[1] * 100) / 100);
			std::string str2 = std::to_string(floor(vec[0] / vec[1] * 10000) / 100);
			RemoveBackFloat(&str);
			RemoveBackFloat(&str2);
			str += "ms";
			std::cout << str;
			std::cout << " (" << str2 << "%)";
			if (dataSize == "Small") {
				std::cout << "\t\t";
			}
			else {
				std::cout << "\t";
			}
		}

		vec.push_back(nloh::Run(dataPath.c_str()));
		{
			std::string str = std::to_string(floor(vec[2] * 100) / 100);
			std::string str2 = std::to_string(floor(vec[0] / vec[2] * 10000) / 100);
			RemoveBackFloat(&str);
			RemoveBackFloat(&str2);
			str += "ms";
			std::cout << str;
			std::cout << " (" << str2 << "%)";
		}

		std::cout << "\r                                                                         \r";

		int min = 0;
		for (int i = 0; i < vec.size(); i++) {
			if (vec[i] < vec[min]) min = i;
		}

		for (int i = 0; i < vec.size(); i++) {
			std::string str = std::to_string(floor(vec[i] * 100) / 100);
			RemoveBackFloat(&str);
			str += "ms";

			if (i == min) {
				ColorOut(str, color::green);
			}
			else {
				std::cout << str;
			}

			std::string str2 = std::to_string(floor(vec[0] / vec[i] * 10000) / 100);
			RemoveBackFloat(&str2);
			std::cout << " (" << str2 << "%)";

			if (i != vec.size() - 1) {
				std::cout << "\t";
				if (i == 1 && dataSize == "Small") {
					std::cout << "\t";
				}
			}
		}

		container.push_back(vec);
		std::cout << "\n";
	}
	
	std::cout << "\n\n";
	std::cout << "==============================================================================" << "\n";
	ColorOut("+ Average ", color::yellow);
	std::cout << "(";
	ColorOut(std::to_string(testNum) + " iteration)", color::white);
	std::cout << "\n\n";

	double smt_avg = 0;
	double jcpp_avg = 0;
	double nloh_avg = 0;

	for (auto& elem : container) {
		smt_avg += elem[0];
		jcpp_avg += elem[1];
		nloh_avg += elem[2];
	}

	smt_avg = smt_avg / container.size();
	jcpp_avg = jcpp_avg / container.size();
	nloh_avg = nloh_avg / container.size();

	ColorOut("[smt_jsonparser]", color::white);
	std::cout << "\t" << "[JsonCpp]" << "\t\t" << "[nlohmann-json]" << "\n\n";


	{
		std::string str = std::to_string(floor(smt_avg * 100) / 100);
		RemoveBackFloat(&str);
		str += "ms";

		if (smt_avg < jcpp_avg && smt_avg < nloh_avg) {
			ColorOut(str, color::green);
		}
		else {
			std::cout << str;
		}

		std::cout << " (100.00%)" <<"\t";
	}
	{
		std::string str = std::to_string(floor(jcpp_avg * 100) / 100);
		std::string str2 = std::to_string(floor(smt_avg / jcpp_avg * 10000) / 100);
		RemoveBackFloat(&str);
		RemoveBackFloat(&str2);
		str += "ms";

		if (jcpp_avg < smt_avg && jcpp_avg < nloh_avg) {
			ColorOut(str, color::green);
		}
		else {
			std::cout << str;
		}

		std::cout << " (" << str2 << "%)";
		if (dataSize == "Small") {
			std::cout << "\t\t";
		}
		else {
			std::cout << "\t";
		}
	}
	{
		std::string str = std::to_string(floor(nloh_avg * 100) / 100);
		std::string str2 = std::to_string(floor(smt_avg / nloh_avg * 10000) / 100);
		RemoveBackFloat(&str);
		RemoveBackFloat(&str2);
		str += "ms";

		if (nloh_avg < smt_avg && nloh_avg < jcpp_avg) {
			ColorOut(str, color::green);
		}
		else {
			std::cout << str;
		}

		std::cout << " (" << str2 << "%)";
	}
	std::cout << "\n\n\n";
	std::cout << "==============================================================================" << "\n";
	ColorOut("+ Overall", color::yellow);
	std::cout << "\n\n";

	ColorOut("smt_jsonparser | ", color::white);
	BoxOut(100);
	std::cout << " (100.00)%";
	std::cout << "\n\n";
	std::cout << "JsonCpp        | ";
	BoxOut(int(smt_avg / jcpp_avg * 100));
	std::cout << " (" << floor(smt_avg / jcpp_avg * 10000) / 100 <<")%";
	std::cout << "\n\n";
	std::cout << "nlohmann-json  | ";
	BoxOut(int(smt_avg / nloh_avg * 100));
	std::cout << " (" << floor(smt_avg / nloh_avg * 10000) / 100 << ")%" << "\n";

	return 0;
}
