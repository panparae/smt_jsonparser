#pragma once

#include "smt_jsonparser_json.h"

std::string json::WriteLiterals(smt_jsonparser::Valuetype _type, const uint32_t& _depth, jvalue* _value, const std::string& _key = "") {
	using valuetype = smt_jsonparser::Valuetype;
	std::string result, space;
	auto literals = static_cast<jliterals*>(_value);

	for (uint32_t i = 0; i < _depth; i++) {
		space += json::kSpaceAmount;
	}

	if (_type == valuetype::kArray) {
		switch (_value->m_type) {
		case valuetype::kNumber:
		case valuetype::kBoolean:
		case valuetype::kNull:
			result = space + literals->m_value;
			break;
		case valuetype::kString:
			result = space + "\"" + literals->m_value + "\"";
			break;
		}
	}
	else if (_type == valuetype::kObject) {
		if (_key.size() == 0) {
			return result;
		}

		switch (_value->m_type) {
		case valuetype::kNumber:
		case valuetype::kBoolean:
		case valuetype::kNull:
			result = space + "\"" + _key + "\" : " + literals->m_value;
			break;
		case valuetype::kString:
			result = space + "\"" + _key + "\" : \"" + literals->m_value + "\"";
			break;
		}
	}

	return result;
}

std::string json::WriteArray(jvalue* _root, const uint32_t& _depth) {
	using valuetype = smt_jsonparser::Valuetype;
	std::string result, space;
	auto arr = static_cast<jarray*>(_root);

	for (uint32_t i = 0; i < _depth; i++) {
		space += kSpaceAmount;
	}

	uint32_t loopCounter = 0;
	size_t size = arr->m_jvalueVec.size();
	for (auto value : arr->m_jvalueVec) {
		++loopCounter;

		std::string temp;
		switch (value->m_type) {
		case valuetype::kArray:
			temp = WriteArray(value, _depth + 1);
			temp = space + "[\n" + temp + "\n" + space + "]";
			result += temp;
			break;

		case valuetype::kObject:
			temp = WriteObject(value, _depth + 1);
			temp = space + "{\n" + temp + "\n" + space + "}";
			result += temp;
			break;

		case valuetype::kNumber:
		case valuetype::kBoolean:
		case valuetype::kNull:
		case valuetype::kString:
			result += WriteLiterals(valuetype::kArray, _depth, value);
			break;
		}

		if (loopCounter < size) {
			result += ",\n";
		}
	}

	return result;
}

std::string json::WriteObject(jvalue* _root, const uint32_t& _depth) {
	using valuetype = smt_jsonparser::Valuetype;
	std::string result, space;
	auto obj = static_cast<jobject*>(_root);

	for (uint32_t i = 0; i < _depth; i++) {
		space += kSpaceAmount;
	}

	uint32_t loopCounter = 0;
	size_t size = obj->m_jvalueMap.size();
	for (auto& elem : obj->m_jvalueMap) {
		++loopCounter;

		std::string temp;
		auto& key = elem.first;
		auto value = elem.second;
		switch (value->m_type) {
		case valuetype::kArray:
			temp = WriteArray(value, _depth + 1);
			temp = space + "\"" + key + "\"" + " : [\n" + temp + "\n" + space + "]";
			result += temp;
			break;

		case valuetype::kObject:
			temp = WriteObject(value, _depth + 1);
			temp = space + "\"" + key + "\"" + " : {\n" + temp + "\n" + space + "}";
			result += temp;
			break;

		case valuetype::kNumber:
		case valuetype::kBoolean:
		case valuetype::kNull:
		case valuetype::kString:
			result += WriteLiterals(valuetype::kObject, _depth, value, key);
			break;
		}

		if (loopCounter < size) {
			result += ",\n";
		}
	}

	return result;
}

std::string json::Write() {
	std::string result;

	std::string log;
	switch (m_value->m_type) {
	case smt_jsonparser::Valuetype::kArray:
		result = WriteArray(m_value, 1);
		result = "[\n" + result + "\n]";
		break;

	case smt_jsonparser::Valuetype::kObject:
		result = WriteObject(m_value, 1);
		result = "{\n" + result + "\n}";
		break;
	}
	
	return result;
}