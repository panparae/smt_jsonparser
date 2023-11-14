#pragma once

#include <future>

#include "smt_jsonparser_json.h"

jliterals* json::ParseLiterals(smt_jsonparser::tokenContainer* _tokenContainer, smt_jsonparser::MemoryPool* _pool) {
	auto token = _tokenContainer->Get();

	jliterals* result = (jliterals*)_pool->Malloc(sizeof(jliterals));
	new(result) jliterals();
	result->m_value = token.GetValue();

	using tokentype = smt_jsonparser::Tokentype;
	using valuetype = smt_jsonparser::Valuetype;
	auto type = token.GetType();
	switch (type) {
	case tokentype::kNumber:
		result->m_type = valuetype::kNumber;
		break;
	case tokentype::kString:
		result->m_type = valuetype::kString;
		break;
	case tokentype::kBoolean:
		result->m_type = valuetype::kBoolean;
		break;
	case tokentype::kNull:
		result->m_type = valuetype::kNull;
		break;
	}

	return result;
}

void json::ParseArray(smt_jsonparser::tokenContainer* _tokenContainer, jvalue* _root, smt_jsonparser::MemoryPool* _pool) {
	using tokentype = smt_jsonparser::Tokentype;
	using valuetype = smt_jsonparser::Valuetype;
	auto token = _tokenContainer->Get();
	auto type = token.GetType();

	if (type == tokentype::kClose_Bracket) {
		// Array is empty.
		return;
	}

	// Start of checking 'value' type.
	auto rootArr = static_cast<jarray*>(_root);
	switch (type) {
		// For 'Array' and 'Object' need to create new jvalue.
		// Sending a parameter new jvalue to parsing method recursively,
		// Making structure as tree node.
	case tokentype::kOpen_Bracket:
	{
		jarray* newRoot = (jarray*)_pool->Malloc(sizeof(jarray));
		new(newRoot) jarray();
		rootArr->m_jvalueVec.emplace_back(newRoot);
		ParseArray(_tokenContainer, newRoot, _pool);
		break;
	}
	case tokentype::kOpen_CulryBracket:
	{
		jobject* newRoot = (jobject*)_pool->Malloc(sizeof(jobject));
		new(newRoot) jobject();
		rootArr->m_jvalueVec.emplace_back(newRoot);
		ParseObject(_tokenContainer, newRoot, _pool);
		break;
	}
	case tokentype::kNumber:
	case tokentype::kString:
	case tokentype::kBoolean:
	case tokentype::kNull:
	{
		_tokenContainer->RewindToken();
		jliterals* value = ParseLiterals(_tokenContainer, _pool);
		rootArr->m_jvalueVec.emplace_back(value);
		break;
	}
	}

	// Checking next data exist or not.
	auto nextToken = _tokenContainer->Get();
	auto nextType = nextToken.GetType();
	if (nextType == smt_jsonparser::Tokentype::kComma) {
		ParseArray(_tokenContainer, _root, _pool);
	}
}

void json::ParseObject(smt_jsonparser::tokenContainer* _tokenContainer, jvalue* _root, smt_jsonparser::MemoryPool* _pool) {
	using tokentype = smt_jsonparser::Tokentype;
	using valuetype = smt_jsonparser::Valuetype;
	std::string log;
	auto token = _tokenContainer->Get();
	auto type = token.GetType();

	if (type == tokentype::kClose_CulryBracket) {
		// Object is empty.
		return;
	}
	else if (type != tokentype::kString) {
		log = "Line " + std::to_string(token.GetLineNumber()) + ": Expected string, but not.\n";
		throw log;
	}

	std::string key = token.GetValue();
	auto nextToken = _tokenContainer->Get();
	auto nextType = nextToken.GetType();
	if (nextType != tokentype::kColon) {
		log = "Line " + std::to_string(nextToken.GetLineNumber()) + ": Expected colon, but not.\n";
		throw log;
	}

	// Start of checking 'value' type.
	auto rootObj = static_cast<jobject*>(_root);
	nextToken = _tokenContainer->Get();
	nextType = nextToken.GetType();
	switch (nextType) {
		// For 'Array' and 'Object' need to create new jvalue.
		// Sending a parameter new jvalue to parsing method recursively,
		// Making structure as tree node.
	case tokentype::kOpen_Bracket:
	{
		jarray* newRoot = (jarray*)_pool->Malloc(sizeof(jarray));
		new(newRoot) jarray();
		rootObj->m_jvalueMap[key] = newRoot;
		ParseArray(_tokenContainer, newRoot, _pool);
		break;
	}
	case tokentype::kOpen_CulryBracket:
	{
		jobject* newRoot = (jobject*)_pool->Malloc(sizeof(jobject));
		new(newRoot) jobject();
		rootObj->m_jvalueMap[key] = newRoot;
		ParseObject(_tokenContainer, newRoot, _pool);
		break;
	}
	// Parsing other values.
	case tokentype::kNumber:
	case tokentype::kString:
	case tokentype::kBoolean:
	case tokentype::kNull:
	{
		_tokenContainer->RewindToken();
		jliterals* value = ParseLiterals(_tokenContainer, _pool);
		if (rootObj) {
			rootObj->m_jvalueMap[key] = value;
		}
		break;
	}
	}

	// Checking next data exist or not.
	nextToken = _tokenContainer->Get();
	nextType = nextToken.GetType();
	if (nextType == tokentype::kComma) {
		ParseObject(_tokenContainer, _root, _pool);
	}
	else if (nextType == tokentype::kClose_CulryBracket) {
		// Parsing object done.
		return;
	}
	else {
		log = "Line " + std::to_string(nextToken.GetLineNumber()) + ": Expected comma or closing culry bracket, but not.\n";
		throw log;
	}
}

jvalue& json::Parse() {
	using tokentype = smt_jsonparser::Tokentype;
	using valuetype = smt_jsonparser::Valuetype;
	std::string log;

	// String input invalid.
	// Need to input string json data.
	if (!m_jsonString) {
		log = "Empty json file.";
		throw log;
	}

	// Gathering tokens from plain text.
	smt_jsonparser::tokenContainer::Init(m_jsonString, &m_tokenContainerA, &m_tokenContainerB);
	if (m_tokenContainerA.GetSize() == 0) {
		log = "Empty json file.";
		throw log;
	}

	auto token = m_tokenContainerA.Get();
	auto type = token.GetType();
	m_tokenContainerA.RewindToken();

	if (type == tokentype::kOpen_Bracket) {
		smt_jsonparser::tokenContainer* tokenCon = &m_tokenContainerA;
		if (m_tokenContainerB.GetSize() != 0) {
			tokenCon = &m_tokenContainerB;
		}

		if (tokenCon->GetEnd().GetType() != tokentype::kClose_Bracket) {
			log = "Expected json array or object, but not.";
			throw log;
		}

		m_value->Destroy();
		m_value = (jarray*)m_poolShared->Malloc(sizeof(jarray));
		new(m_value) jarray();
	}
	else if (type == tokentype::kOpen_CulryBracket) {
		smt_jsonparser::tokenContainer* tokenCon = &m_tokenContainerA;
		if (m_tokenContainerB.GetSize() != 0) {
			tokenCon = &m_tokenContainerB;
		}

		if (tokenCon->GetEnd().GetType() != tokentype::kClose_CulryBracket) {
			log = "Expected json array or object, but not.";
			throw log;
		}

		m_value->Destroy();
		m_value = (jobject*)m_poolShared->Malloc(sizeof(jobject));
		new(m_value) jobject();
	}
	else {
		log = "Expected json array or object, but not.";
		throw log;
	}
	
	// Thread parse.
	if (m_tokenContainerB.GetSize() != 0) {
		m_tokenContainerA.Get();
		m_tokenContainerA.GetTokenVec()->pop_back();
		if (m_value->m_type == valuetype::kArray) {
			m_tokenContainerA.GetTokenVec()->emplace_back(smt_jsonparser::token(smt_jsonparser::Tokentype::kClose_Bracket, "]", 0));
		}
		else if (m_value->m_type == valuetype::kObject) {
			m_tokenContainerA.GetTokenVec()->emplace_back(smt_jsonparser::token(smt_jsonparser::Tokentype::kClose_CulryBracket, "}", 0));
		}

		if (m_value->m_type == valuetype::kArray) {
			jvalue* value1 = (jarray*)m_poolA->Malloc(sizeof(jarray));
			new(value1) jarray();
			jvalue* value2 = (jarray*)m_poolB->Malloc(sizeof(jarray));
			new(value2) jarray();

			auto thread1 = std::async(std::launch::async, &json::ParseArray, this, &m_tokenContainerA, value1, m_poolA);
			auto thread2 = std::async(std::launch::async, &json::ParseArray, this, &m_tokenContainerB, value2, m_poolB);

			thread1.get();
			thread2.get();

			auto targetArr = static_cast<jarray*>(m_value);
			auto arr1 = static_cast<jarray*>(value1);
			auto arr2 = static_cast<jarray*>(value2);

			targetArr->m_jvalueVec.reserve(arr1->m_jvalueVec.size() + arr2->m_jvalueVec.size());
			targetArr->m_jvalueVec.insert(targetArr->m_jvalueVec.end(), arr1->m_jvalueVec.begin(), arr1->m_jvalueVec.end());
			targetArr->m_jvalueVec.insert(targetArr->m_jvalueVec.end(), arr2->m_jvalueVec.begin(), arr2->m_jvalueVec.end());
		}
		else if (m_value->m_type == valuetype::kObject) {
			jvalue* value1 = (jobject*)m_poolA->Malloc(sizeof(jobject));
			new(value1) jobject();
			jvalue* value2 = (jobject*)m_poolB->Malloc(sizeof(jobject));
			new(value2) jobject();

			auto thread1 = std::async(std::launch::async, &json::ParseObject, this, &m_tokenContainerA, value1, m_poolA);
			auto thread2 = std::async(std::launch::async, &json::ParseObject, this, &m_tokenContainerB, value2, m_poolB);
			thread1.get();
			thread2.get();

			auto targetObj = static_cast<jobject*>(m_value);
			auto obj1 = static_cast<jobject*>(value1);
			auto obj2 = static_cast<jobject*>(value2);

			targetObj->m_jvalueMap.reserve(obj1->m_jvalueMap.size() + obj2->m_jvalueMap.size());
			targetObj->m_jvalueMap.insert(obj1->m_jvalueMap.begin(), obj1->m_jvalueMap.end());
			targetObj->m_jvalueMap.insert(obj2->m_jvalueMap.begin(), obj2->m_jvalueMap.end());
		}
	}

	// Data are very small size, can't split to two vector.
	// General parse.
	else {
		m_tokenContainerA.Get();
		if (m_value->m_type == valuetype::kArray) {
			ParseArray(&m_tokenContainerA, m_value, m_poolA);
		}
		else if (m_value->m_type == valuetype::kObject) {
			ParseObject(&m_tokenContainerA, m_value, m_poolA);
		}
	}

	return *m_value;
}