#pragma once

#include <vector>

namespace smt_jsonparser {
	class MemoryPool {
	private:
		std::vector<void*> m_pageVec;
		void* m_addr;
		size_t m_size{ 4096 };
		size_t m_allocNum{ 0 };

	public:
		void* Malloc(const size_t& _size) {
			if (m_allocNum + _size > m_size) {
				m_allocNum = 0;
				void* page = malloc(m_size);
				m_addr = page;
				m_pageVec.emplace_back(page);
			}

			unsigned char* cAddr = reinterpret_cast<unsigned char*>(m_addr);

			cAddr += _size;
			m_allocNum += _size;

			void* result = m_addr;
			m_addr = (void*)cAddr;

			return result;
		}

	public:
		MemoryPool() {
			void* page = malloc(m_size);
			m_addr = page;
			m_pageVec.emplace_back(page);
		}
		MemoryPool(const size_t& _size) : m_size(_size) {
			void* page = malloc(_size);
			m_addr = page;
			m_pageVec.emplace_back(page);
		}
		~MemoryPool() {
			for (auto elem : m_pageVec) {
				if (elem) {
					free(elem);
				}
			}
		}
	};
}