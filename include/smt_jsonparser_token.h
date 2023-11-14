#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace smt_jsonparser {
	enum class Tokentype {
		kOpen_Bracket,
		kClose_Bracket,
		kOpen_CulryBracket,
		kClose_CulryBracket,
		kColon,
		kComma,
		kNumber,
		kString,
		kBoolean,
		kNull
	};

	class token {
	private:
		Tokentype m_type;
		std::string m_value;
		uint32_t m_lineNumber;

	public:
		Tokentype GetType() {
			return m_type;
		}

		std::string GetValue() {
			return m_value;
		}

		uint32_t GetLineNumber() {
			return m_lineNumber;
		}

	public:
		token() = default;
		~token() = default;
		token(Tokentype _type, const std::string& _value, const uint32_t& _lineNumber) : m_type(_type), m_value(_value), m_lineNumber(_lineNumber) {}
	};

	class alignas(64) tokenContainer {
	private:
		std::vector<token> m_tokenVec;
		uint32_t m_pos{ 0 };

	private:
		static void Add(const token& _token, tokenContainer* _tokenVec) {
			_tokenVec->m_tokenVec.emplace_back(_token);
		}

	public:
		tokenContainer() = default;
		~tokenContainer() = default;

		static void Init(std::string* _stringJson, tokenContainer* _tokenVecA, tokenContainer* _tokenVecB) {
			_tokenVecA->Clear();
			_tokenVecB->Clear();

			_tokenVecA->m_tokenVec.reserve(_stringJson->size() / 20);
			_tokenVecB->m_tokenVec.reserve(_stringJson->size() / 20);

			std::string& stringJson = *_stringJson;
			uint32_t depth = 0;
			uint32_t lineNumber = 1;
			uint32_t s_pos = 0;
			size_t half = stringJson.size() / 2;
			bool isNextCon = false;

			tokenContainer* curTokenCon = _tokenVecA;

			while (stringJson[s_pos] != '\0') {
				// Always remove blank space and line break.
				if (stringJson[s_pos] == ' ') {
					++s_pos;
					continue;
				}
				else if (stringJson[s_pos] == '\t') {
					++s_pos;
					continue;
				}
				else if (stringJson[s_pos] == '\n') {
					++s_pos;
					++lineNumber;
					continue;
				}

				if (stringJson[s_pos] == '[') {
					Add(token(Tokentype::kOpen_Bracket, "[", lineNumber), curTokenCon);
					++s_pos;
					++depth;
				}
				else if (stringJson[s_pos] == ']') {
					Add(token(Tokentype::kClose_Bracket, "]", lineNumber), curTokenCon);
					++s_pos;
					--depth;
				}
				else if (stringJson[s_pos] == '{') {
					Add(token(Tokentype::kOpen_CulryBracket, "{", lineNumber), curTokenCon);
					++s_pos;
					++depth;
				}
				else if (stringJson[s_pos] == '}') {
					Add(token(Tokentype::kClose_CulryBracket, "}", lineNumber), curTokenCon);
					++s_pos;
					--depth;
				}
				else if (stringJson[s_pos] == ':') {
					Add(token(Tokentype::kColon, ":", lineNumber), curTokenCon);
					++s_pos;
				}
				else if (stringJson[s_pos] == ',') {
					Add(token(Tokentype::kComma, ",", lineNumber), curTokenCon);
					++s_pos;
					if (!isNextCon && depth == 1 && s_pos > half) {
						isNextCon = true;
						curTokenCon = _tokenVecB;
					}
				}
				else if (stringJson[s_pos] == '\"') {
					++s_pos;

					bool bEsacpe = false;
					size_t pos = 0;
					size_t findPos = 0;
					while (!bEsacpe) {
						pos = stringJson.find("\"", s_pos + findPos);
						if (pos == std::string::npos) {
							std::string log = "Line " + std::to_string(lineNumber) + ": Expected double quotes, but not.\n";
							throw(log);
						}

						// Checking backslash escape double quotes.
						if (pos > s_pos && stringJson[pos - 1] == '\\') {
							findPos = pos - s_pos + 1;
							continue;
						}

						// Closing double quotes found.
						// Escape the loop.
						bEsacpe = true;
					}

					std::string value = stringJson.substr(s_pos, pos - s_pos);
					Add(token(Tokentype::kString, value, lineNumber), curTokenCon);
					s_pos = (uint32_t)pos + 1;
				}
				else if (stringJson[s_pos] == 't' || stringJson[s_pos] == 'T') {
					std::string str = stringJson.substr(s_pos, 4);
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);

					if (str == "true") {
						Add(token(Tokentype::kBoolean, str, lineNumber), curTokenCon);
						s_pos += 4;
					}
					else {
						std::string log = "Line " + std::to_string(lineNumber) + ": Expected boolean, but not.\n";
						throw(log);
					}
				}
				else if (stringJson[s_pos] == 'f' || stringJson[s_pos] == 'F') {
					std::string str = stringJson.substr(s_pos, 5);
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);

					if (str == "false") {
						Add(token(Tokentype::kBoolean, str, lineNumber), curTokenCon);
						s_pos += 5;
					}
					else {
						std::string log = "Line " + std::to_string(lineNumber) + ": Expected boolean, but not.\n";
						throw(log);
					}
				}
				else if (stringJson[s_pos] == 'n' || stringJson[s_pos] == 'N') {
					std::string str = stringJson.substr(s_pos, 4);
					std::transform(str.begin(), str.end(), str.begin(), ::tolower);

					if (str == "null") {
						Add(token(Tokentype::kNull, str, lineNumber), curTokenCon);
						s_pos += 4;
					}
					else {
						std::string log = "Line " + std::to_string(lineNumber) + ": Expected NULL, but not.\n";
						throw(log);
					}
				}
				else if (static_cast<int>(stringJson[s_pos]) >= 48 && static_cast<int>(stringJson[s_pos]) <= 57) {
					std::string value;
					bool bEscape = false;
					while (!bEscape) {
						// Checking ascii 48 ~ 57 range which is 0 ~ 9.
						int ascii = static_cast<int>(stringJson[s_pos]);
						if (ascii >= 48 && ascii <= 57) {
							value += stringJson[s_pos];
							++s_pos;
						}
						else {
							bEscape = true;
						}
					}

					if (value != "") {
						Add(token(Tokentype::kNumber, value, lineNumber), curTokenCon);
					}
				}
				else {
					std::string log = "Line " + std::to_string(lineNumber) + ": Unexpected token.\n";
					throw(log);
				}
			}
		}

		token Get() {
			return m_tokenVec.at(m_pos++);
		}

		token GetEnd() {
			return m_tokenVec.back();
		}

		uint32_t GetSize() {
			return (uint32_t)m_tokenVec.size();
		}

		std::vector<smt_jsonparser::token>* GetTokenVec() {
			return &m_tokenVec;
		}
		
		void RewindToken() {
			m_pos = m_pos > 0 ? m_pos - 1 : 0;
		}

		void Clear() {
			m_pos = 0;
			m_tokenVec.clear();
		}
	};
}