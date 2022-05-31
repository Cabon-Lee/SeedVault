#pragma once
#include "DLLDefine.h"

#include "ComponentBase.h"

class MaterialData;

class Material : public ComponentBase
{
public:
	_DLL Material();
	_DLL virtual ~Material();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void Render() override; 

public:

	//void SetDiffuseMap(std::string name);
	//void SetVertexShader(std::string name);
	//void SetPixelShader(std::string name);

	void SetMaterialData(MaterialData* materialData);
	MaterialData* GetMaterialData();

	std::string m_Diffuse;
	std::string m_Vertex;
	std::string m_Pixel;

private:
	
	MaterialData* m_MaterialData;

};

