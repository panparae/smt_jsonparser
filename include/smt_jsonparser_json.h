#pragma once

#include "smt_jsonparser_token.h"
#include "smt_jsonparser_value.h"

class json {
private:
	std::string* m_jsonString;
	jvalue* m_value;
	smt_jsonparser::tokenContainer m_tokenContainerA;
	smt_jsonparser::tokenContainer m_tokenContainerB;
	smt_jsonparser::MemoryPool* m_poolA;
	smt_jsonparser::MemoryPool* m_poolB;
	smt_jsonparser::MemoryPool* m_poolShared;
	
	const std::string kSpaceAmount = "  ";

private:
	// Parsing 'Number', 'String', 'Boolean', 'NULL'
	jliterals* ParseLiterals(smt_jsonparser::tokenContainer* _tokenContainer, smt_jsonparser::MemoryPool* _pool);

	// Parsing 'Array'
	void ParseArray(smt_jsonparser::tokenContainer* _tokenContainer, jvalue* _root, smt_jsonparser::MemoryPool* _pool);

	// Parsing 'Object'
	void ParseObject(smt_jsonparser::tokenContainer* _tokenContainer, jvalue* _root, smt_jsonparser::MemoryPool* _pool);

	// Writting 'Number', 'String', 'Boolean', 'NULL'
	std::string WriteLiterals(smt_jsonparser::Valuetype _type, const uint32_t& _depth, jvalue* _value, const std::string& _key);

	// Writting 'Array'
	std::string WriteArray(jvalue* _root, const uint32_t& _depth);

	// Writting 'Object'
	std::string WriteObject(jvalue* _root, const uint32_t& _depth);

public:
	json() {
		m_poolA = new smt_jsonparser::MemoryPool(65'536);
		m_poolB = new smt_jsonparser::MemoryPool(65'536);
		m_poolShared = new smt_jsonparser::MemoryPool(65'536);

		m_value = (jobject*)m_poolShared->Malloc(sizeof(jobject));
		new(m_value) jobject();
		m_value->m_type= smt_jsonparser::Valuetype::kNone;
		
		auto obj = static_cast<jobject*>(m_value);
		obj->m_parentValue = obj;
	}

	~json() {
		m_value->Destroy();

		delete m_poolA;
		delete m_poolB;
		delete m_poolShared;
	}

	json& operator=(const json& _json) = delete;
	json(const json& _json) = delete;

	json(std::string& _str) : m_jsonString(&_str) {
		m_poolA = new smt_jsonparser::MemoryPool;
		m_poolB = new smt_jsonparser::MemoryPool;
		m_poolShared = new smt_jsonparser::MemoryPool;

		m_value = (jobject*)m_poolShared->Malloc(sizeof(jobject));
		new(m_value) jobject();
		m_value->m_type = smt_jsonparser::Valuetype::kNone;

		auto obj = static_cast<jobject*>(m_value);
		obj->m_parentValue = obj;
	}

	void operator<<(std::string& _str) {
		m_jsonString = &_str;
	}

	jvalue& Get() {
		return *m_value;
	}

	jvalue& Parse();
	std::string Write();
};