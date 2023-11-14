#pragma once

#include <chrono>

class timecheck {
	std::chrono::system_clock::time_point m_start;

public:
	timecheck() {
		m_start = std::chrono::system_clock::now();
	}

	double end() {
		std::chrono::duration<double>sec = std::chrono::system_clock::now() - m_start;
		return sec.count() * 1000;
	}
};