#pragma once
#include "JsonLoader_GE.h"

class JsonLoader_CL : public JsonLoader_GE
{
public:
	JsonLoader_CL();
	~JsonLoader_CL();

protected:
	virtual bool AddComponentToGameObject(GameObject* object, const char* compName, Json::Value* value) override;

};

