#pragma once
#include "JsonLoader_GE.h"

class JsonLoader_QT : public JsonLoader_GE
{
public:
	JsonLoader_QT();
	~JsonLoader_QT();

protected:
	virtual bool AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value);

};

