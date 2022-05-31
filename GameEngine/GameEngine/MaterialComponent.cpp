#include "pch.h"
#include "MaterialComponent.h"
#include "MaterialData.h"
#include "MeshFilter.h"

#include "IResourceManager.h"

MaterialComponent::MaterialComponent()
	: ComponentBase(ComponentType::Etc)
	, m_VertexShader()
	, m_PixelShader()
	, m_AlbedoMapIndex(0)
	, m_NormalMapIndex(0)
	, m_EmissiveMapIndex(0)
	, m_MetalRoughMapIndex(0)
	, m_SaveData(new MaterialComponent_Save())

{

}

MaterialComponent::~MaterialComponent()
{

}

void MaterialComponent::Start()
{
	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
	m_NowMeshIndex = m_MyMeshFilter->GetCurrentIndex();
	
}

void MaterialComponent::Update(float dTime)
{

}


void MaterialComponent::OnRender()
{

}

std::string MaterialComponent::GetVertexShaderName()
{
	return m_VertexShader;
}

std::string MaterialComponent::GetPixelShaderName()
{
	return m_PixelShader;
}

std::shared_ptr<GraphicMaterial> MaterialComponent::GetMaterial()
{
	return nullptr;
}

const std::vector< std::shared_ptr<GraphicMaterial>>& MaterialComponent::GetMaterials()
{
	return m_Material_V;
}

void MaterialComponent::SetNames(std::vector<std::pair<std::string, unsigned int>>& names)
{
	m_TextureName_V = names;
}

void MaterialComponent::UpdateMaterial()
{

}

const std::string& MaterialComponent::GetNowAlbedoName()
{
	return m_TextureName_V[m_AlbedoMapIndex].first;
}

const std::string& MaterialComponent::GetNowNormalName()
{
	return m_TextureName_V[m_NormalMapIndex].first;
}

const std::string& MaterialComponent::GetNowEmissiveName()
{
	return m_TextureName_V[m_EmissiveMapIndex].first;
}

const std::string& MaterialComponent::GetNowMetalRoughName()
{
	return m_TextureName_V[m_MetalRoughMapIndex].first;
}

void MaterialComponent::SetVertexShaderName(const std::string& name)
{
	m_VertexShader = name;
}

void MaterialComponent::SetPixelShaderName(const std::string& name)
{
	m_PixelShader = name;
}

void MaterialComponent::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	// 이름을 모두 집어넣는다
	SetNames(pReosureManager->GetTextureNames());



}

void MaterialComponent::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_VertexShader = m_VertexShader;
	m_SaveData->m_PixelShader = m_PixelShader;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void MaterialComponent::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);

	m_VertexShader = m_SaveData->m_VertexShader;
	m_PixelShader = m_SaveData->m_PixelShader;
}