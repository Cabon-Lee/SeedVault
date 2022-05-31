#include "pch.h"
#include "Material.h"
#include "MaterialData.h"

Material::Material() 
	: ComponentBase(ComponentType::Rendering)
	, m_MaterialData(nullptr)
{
	m_MaterialData = new MaterialData();
}

Material::~Material()
{

}

void Material::Start()
{

}

void Material::Update(float dTime)
{

}


void Material::Render()
{

}

/*
void Material::SetDiffuseMap(std::string name)
{
	m_MaterialData->SetDiffuseMap(name);
}

void Material::SetVertexShader(std::string name)
{
	m_MaterialData->SetVertexShader(name);

}

void Material::SetPixelShader(std::string name)
{
	m_MaterialData->SetPixelShader(name);
}
*/

void Material::SetMaterialData(MaterialData* materialData)
{
	m_MaterialData = materialData;
}

MaterialData* Material::GetMaterialData()
{
	return m_MaterialData;
}
