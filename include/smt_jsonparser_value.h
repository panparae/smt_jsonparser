#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "smt_jsonparser_malloc.h"

namespace smt_jsonparser {
	enum class Valuetype {
		kArray,
		kObject,
		kNumber,
		kString,
		kBoolean,
		kNull,
		kNone
	};
}
class jarray;
class jobject;
class jliterals;

class jvalue {
	friend class json;

protected:
	smt_jsonparser::Valuetype m_type{ smt_jsonparser::Valuetype::kNone };

public:
	jvalue() = default;
	~jvalue() = default;
	jvalue& operator=(const jvalue&) = delete;

	virtual jvalue& operator[](const std::string& _path) {
		// Never called.
		return *this;
	}

	virtual void operator=(const int& _value) {}
	virtual void operator=(const char* _value) {}
	virtual void operator=(bool _value) {}

	// Calling destructor recursively to free memory.
	virtual void Destroy() {
		this->~jvalue();
	}

	// Cast as jarry, jobject, jliterals.
	template <typename T, typename std::enable_if<std::is_same<T, jarray>::value || std::is_same<T, jobject>::value || std::is_same<T, jliterals>::value, T>::type* = nullptr>
	T& As() {
		if (typeid(T) == typeid(jarray) && this->m_type == smt_jsonparser::Valuetype::kNone) {
			auto obj = dynamic_cast<jobject*>(this);
			if (obj) {
				jarray* arr = new jarray;
				obj->m_parentValue->m_jvalueMap[obj->m_parentKey] = arr;
				this->Destroy();

				return *dynamic_cast<T*>(arr);
			}
		}

		return *static_cast<T*>(this);
	}

	// Type check jarray, jobject, jliterals or not.
	template <typename T, typename std::enable_if<std::is_same<T, jarray>::value || std::is_same<T, jobject>::value || std::is_same<T, jliterals>::value, T>::type* = nullptr>
	bool Is() {
		return typeid(*this) == typeid(T);
	}
};

class jliterals : public jvalue {
	friend class json;

private:
	std::string m_value{ "" };

private:
	void Destroy() override {
		this->~jliterals();
	}

public:
	void operator=(const int& _value) override {
		if (m_type == smt_jsonparser::Valuetype::kNumber) {
			this->m_value = std::to_string(_value);
		}
		else if (m_type == smt_jsonparser::Valuetype::kNone) {
			this->m_value = std::to_string(_value);
			this->m_type = smt_jsonparser::Valuetype::kNumber;
		}
	}

	void operator=(const char* _value) override {
		if (m_type == smt_jsonparser::Valuetype::kString) {
			this->m_value = _value;
		}
		else if (m_type == smt_jsonparser::Valuetype::kNone) {
			this->m_value = _value;
			this->m_type = smt_jsonparser::Valuetype::kString;
		}
	}

	void operator=(bool _value) override {
		if (m_type == smt_jsonparser::Valuetype::kBoolean) {
			this->m_value = _value ? "true" : "false";
		}
		else if (m_type == smt_jsonparser::Valuetype::kNone) {
			this->m_value = _value ? "true" : "false";
			this->m_type = smt_jsonparser::Valuetype::kBoolean;
		}
	}

	int to_int() {
		return atoi(m_value.c_str());
	}

	std::string to_string() {
		return m_value;
	}
};

class jarrayIter {
private:
	std::vector<jvalue*>* m_vec;
	size_t m_pos{ 0 };

public:
	jarrayIter(std::vector<jvalue*>* _vec, const size_t& _size) : m_vec(_vec), m_pos(_size) {}
	jarrayIter& operator++() {
		m_pos++;
		return *this;
	}
	jarrayIter& operator++(int) {
		m_pos++;
		return *this;
	}
	jvalue& operator*() {
		return *m_vec->at(m_pos);
	}
	bool operator==(const jarrayIter& _iter) {
		return m_pos == _iter.m_pos;
	}
	bool operator!=(const jarrayIter& _iter) {
		return m_pos != _iter.m_pos;
	}
};

class jarray : public jvalue {
	friend class json;

private:
	std::vector<jvalue*> m_jvalueVec;

private:
	void Destroy() override {
		for (auto& value : m_jvalueVec) {
			value->Destroy();
		}
		this->~jarray();
	}

public:
	jarray() {
		m_type = smt_jsonparser::Valuetype::kArray;
	}

	size_t size() const {
		return m_jvalueVec.size();
	}

	jvalue& at(const size_t& _pos) {
		auto pos = _pos >= size() ? size() - 1 : _pos;
		return (*m_jvalueVec.at(pos));
	}

	jvalue& operator[](const size_t& _pos) {
		return (*m_jvalueVec[_pos]);
	}

	void push_back(const int& _value) {
		jliterals* literals = new jliterals;
		*literals = _value;

		m_jvalueVec.push_back(literals);
	}

	void push_back(jvalue& _value) {
		m_jvalueVec.push_back(&_value);
	}

	void pop_back() {
		if (m_jvalueVec.size() <= 0) {
			return;
		}

		auto pop = m_jvalueVec.back();
		m_jvalueVec.pop_back();
		delete pop;
	}

	void clear() {
		auto size = m_jvalueVec.size();
		for (int i = 0; i < size; i++) {
			pop_back();
		}
	}

	typedef jarrayIter iterator;
	iterator begin() {
		return iterator(&m_jvalueVec, 0);
	}
	iterator end() {
		return iterator(&m_jvalueVec, size());
	}
};

class jobject : public jvalue {
	friend class json;
	friend class jvalue;

private:
	std::unordered_map<std::string, jvalue*> m_jvalueMap;
	std::string m_parentKey;
	jobject* m_parentValue;

private:
	void Destroy() override {
		for (auto& [key, value] : m_jvalueMap) {
			value->Destroy();
		}
		this->~jobject();
	}

	void SetParent(const std::string& _parentKey,  jobject* _parentValue) {
		m_parentKey = _parentKey;
		m_parentValue = _parentValue;
	}

public:
	jobject() {
		m_type = smt_jsonparser::Valuetype::kObject;
	}

	jvalue& operator[](const std::string& _path) override {
		jvalue* result = this;

		if (m_jvalueMap[_path]) {
			result = m_jvalueMap[_path];
		}
		else {
			// Set Valuetype::kNone.
			// For distinguish temp jobject or not.
			jobject* obj = new jobject;
			obj->m_type = smt_jsonparser::Valuetype::kNone;

			m_jvalueMap[_path] = obj;
			result = obj;

			obj->SetParent(_path, this);

			if (m_type == smt_jsonparser::Valuetype::kNone) {
				m_type = smt_jsonparser::Valuetype::kObject;
			}
		}

		return *result;
	}

	void operator=(const int& _value) override {
		if (m_type == smt_jsonparser::Valuetype::kNone) {
			jliterals* literals = new jliterals;
			*literals = _value;

			m_parentValue->m_jvalueMap[m_parentKey] = literals;
			delete this;
		}
	}

	void operator=(const char* _value) override {
		if (m_type == smt_jsonparser::Valuetype::kNone) {
			jliterals* literals = new jliterals;
			*literals = _value;

			m_parentValue->m_jvalueMap[m_parentKey] = literals;
			delete this;
		}
	}

	void operator=(bool _value) override {
		if (m_type == smt_jsonparser::Valuetype::kNone) {
			jliterals* literals = new jliterals;
			*literals = _value;

			m_parentValue->m_jvalueMap[m_parentKey] = literals;
			delete this;
		}
	}
};