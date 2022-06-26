#include "pch.h"
#include "MeshFilter.h"
#include "CATrace.h"
#include "IRenderer.h"
#include "IResourceManager.h"
#include "EngineDLL.h"


MeshFilter::MeshFilter()
	: ComponentBase(ComponentType::MeshFilter)
	, m_pMeshName_V(nullptr)
	, m_Index(0)
	, m_SaveData(new MeshFilter_Save())
{
}

MeshFilter::~MeshFilter()
{

}

void MeshFilter::Start()
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_pCurrMaterials = m_pResourceManager->GetMaterialOfMesh(m_Index);
}

void MeshFilter::Update(float dTime)
{
	// 오브젝트로 함께 회전하는 것이 문제다.
	m_pRenderer->BindPose(m_Index, m_Transform->GetWorld());
}

void MeshFilter::OnRender()
{

}

void MeshFilter::SetNames(std::vector<std::string>* pNames)
{
	m_pMeshName_V = pNames;

	SetMesh();
}

void MeshFilter::SetNames(std::vector<std::pair<std::string, unsigned int>>& names)
{
	m_MeshNames_V = names;

	SetMesh();
}

_DLL size_t MeshFilter::GetGraphicMaterialCount()
{
	return m_pCurrMaterials.size();
}

_DLL size_t MeshFilter::GetGraphicMaterialTextureCount()
{
	return m_pResourceManager->GetTextureNames().size();
}

const std::string& MeshFilter::GetGraphicMaterialName(unsigned int idx)
{
	return m_pResourceManager->GetMaterialName(m_Index, idx);
}

_DLL const char* MeshFilter::GetGraphicMaterialName_char(unsigned int idx)
{
	return GetGraphicMaterialName(idx).c_str();
}

const std::string& MeshFilter::GetGraphicMaterialTextureName(unsigned int idx, unsigned int texnum)
{
	return m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum);
}

_DLL const char* MeshFilter::GetGraphicMaterialTextureName_char(unsigned int idx, unsigned int texnum)
{
	/// <summary>
	/// "None"==""? 
	/// </summary>
	std::string a = m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum);
	bool b = m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum)[0] == '\0';


	if ( m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum).compare("") == 0
		|| m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum)[0]=='\0')
	{
		return "None";
	}
	else
	{
		return m_pResourceManager->GetMaterialTextureName(m_Index, idx, texnum).c_str();
	}
}

const std::vector<std::pair<std::string, unsigned int>>& MeshFilter::GetTextureNames()
{
	return m_pResourceManager->GetTextureNames();
}

_DLL const char* MeshFilter::GetTextureName(unsigned int index)
{
	return m_pResourceManager->GetTextureNames()[index].first.c_str();
}


void MeshFilter::SetGraphicMaterialTextureName(unsigned int idx, unsigned int texnum, const std::string& texname)
{
	m_pResourceManager->SetMaterialTextureName(m_Index, idx, texnum, texname);
}

_DLL void MeshFilter::SetGraphicMaterialTextureName_char(unsigned int idx, unsigned int texnum, const char* texname)
{
	SetGraphicMaterialTextureName(idx, texnum, std::string(texname));
}

float& MeshFilter::GetMaterialFactor(unsigned int idx, int att)
{
	switch (att)
	{
	case METALLIC:
		return m_pResourceManager->GetMaterialCalibrateFactor(m_Index, idx, METALLIC);
	case SHINENESS:
		return m_pResourceManager->GetMaterialCalibrateFactor(m_Index, idx, SHINENESS);
	case NORMALFACFOR:
		return m_pResourceManager->GetMaterialCalibrateFactor(m_Index, idx, NORMALFACFOR);
	case EMISSIVEFACTOR:
		return m_pResourceManager->GetMaterialCalibrateFactor(m_Index, idx, EMISSIVEFACTOR);
	default:
		break;
	}
}
bool& MeshFilter::GetMaterialIsTransparent(unsigned int idx)
{
	return m_pResourceManager->GetMaterialTransparencyFactor(m_Index, idx);
}

_DLL float MeshFilter::GetMaterialFactor_(unsigned int idx, int att)
{
	return GetMaterialFactor(idx, att);
}

_DLL void MeshFilter::SetMaterialFactor(unsigned int idx, int att, float value)
{
	GetMaterialFactor(idx, att) = value;
}

size_t MeshFilter::GetMeshCount()
{
	///Imgui에서 필요해서 임시로 만듭니다.
	//return m_pMeshName_V->size();
	return m_MeshNames_V.size();
}

_DLL void MeshFilter::SaveMaterial(int idx)
{
	//어떤 오브젝트의 + 어떤 메터리얼인지
	m_pResourceManager->SaveMatFile(m_Index, idx);
}

void MeshFilter::LoadMaterial(int idx)
{
	m_pResourceManager->LoadMatFile(m_Index, idx);
}


void MeshFilter::SetIndex(unsigned int idx)
{
	m_Index = idx;
	m_MeshName = GetName(idx);

	m_pCurrMaterials = m_pResourceManager->GetMaterialOfMesh(m_Index);
	
	auto _nodeWorldTM = reinterpret_cast<DirectX::XMMATRIX*>(m_pResourceManager->GetNodeDataWorldTM(m_Index));
	DirectX::SimpleMath::Matrix _nowWorldTM = *_nodeWorldTM;

	DirectX::SimpleMath::Vector3 _nowScale;
	DirectX::SimpleMath::Quaternion _nowRotation;
	DirectX::SimpleMath::Vector3 _nowTranslation;
	_nowWorldTM.Decompose(_nowScale, _nowRotation, _nowTranslation);
	
	//m_Transform->SetPosition(_nowTranslation);
	//m_Transform->SetRotation(_nowRotation);
	//m_Transform->SetScale(_nowScale);
	
}

std::string& MeshFilter::GetName()
{
	return m_MeshNames_V.at(m_Index).first;
}

std::string& MeshFilter::GetName(const std::string& name)
{

	for (auto& _nowName : m_MeshNames_V)
	{
		if (_nowName.first == name)
		{
			return _nowName.first;
		}
	}

	CA_TRACE("%s 라는 메쉬는 없음", name.c_str());
	return m_MeshNames_V.at(m_Index).first;
}

std::string& MeshFilter::GetName(unsigned int idx)
{
	/*
	if (m_pMeshName_V->size() <= idx)
	{
		CA_TRACE("인덱스를 넘어갔다");
		m_Index = m_pMeshName_V->size() - 1;
		return m_pMeshName_V->at(m_Index);
	}

	m_Index = idx;

	CA_TRACE("현재 메쉬필터 %s", m_pMeshName_V->at(m_Index).c_str());
	return m_pMeshName_V->at(idx);
	*/

	if (m_MeshNames_V.size() <= idx)
	{
		CA_TRACE("인덱스를 넘어갔다");
		m_Index = m_MeshNames_V.size() - 1;
		return m_MeshNames_V.at(m_Index).first;
	}

	m_Index = idx;

	CA_TRACE("현재 메쉬필터 %s", m_MeshNames_V.at(m_Index).first.c_str());
	return m_MeshNames_V.at(idx).first;

}

_DLL const char* MeshFilter::GetName_char(unsigned int idx)
{
	return GetName(idx).c_str();
}

_DLL const char* MeshFilter::GetName_char(const char* name)
{
	return GetName(name).c_str();
}

_DLL const char* MeshFilter::GetName_char()
{
	return GetName().c_str();
}

_DLL void MeshFilter::SetMesh(const std::string name)
{
	m_MeshName = name;

	SetMesh();
}

void MeshFilter::SetMesh()
{
	// 인덱스 초기화
	unsigned int i = 0;

	// 벡터를 순회하면서 name에 해당하는 mesh의 인덱스를 계산
	for (auto MeshName : m_MeshNames_V)
	{
		// 일치하는 메쉬를 찾으면
		if (MeshName.first == m_MeshName)
		{
			// 현재 인덱스로 설정
			SetIndex(i);

			return;
		}

		// 인덱스 증가
		i++;
	}

	CA_TRACE("[MeshFilter] Can not find mesh '%s'", m_MeshName);

	return;
}

_DLL void MeshFilter::SetFbxTransform()
{
	auto _nodeWorldTM = reinterpret_cast<DirectX::XMMATRIX*>(m_pResourceManager->GetNodeDataWorldTM(m_Index));
	DirectX::SimpleMath::Matrix _nowWorldTM = *_nodeWorldTM;

	DirectX::SimpleMath::Vector3 _nowScale;
	DirectX::SimpleMath::Quaternion _nowRotation;
	DirectX::SimpleMath::Vector3 _nowTranslation;
	_nowWorldTM.Decompose(_nowScale, _nowRotation, _nowTranslation);

	m_Transform->SetPosition(_nowTranslation);
	m_Transform->SetRotation(_nowRotation);
	m_Transform->SetScale(_nowScale);
}

_DLL void* MeshFilter::GetBoneDataWorldTM(const std::string& name)
{
	return m_pResourceManager->GetBoneDataWorldTM(m_Index, name);
}

std::string& MeshFilter::GetMeshName(unsigned int idx)
{
	/*
	if (m_pMeshName_V->size() <= idx)
	{
		CA_TRACE("인덱스를 넘어갔다");
		m_Index = m_pMeshName_V->size() - 1;
		return m_pMeshName_V->at(m_Index);
	}

	return m_pMeshName_V->at(idx);
	*/


	if (m_MeshNames_V.size() <= idx)
	{
		CA_TRACE("인덱스를 넘어갔다");
		m_Index = m_MeshNames_V.size() - 1;
		return m_MeshNames_V.at(m_Index).first;
	}

	return m_MeshNames_V.at(idx).first;
}

const std::string& MeshFilter::GetCurrentMeshName()
{
	return m_MeshName;
}

_DLL const char* MeshFilter::GetMeshName_char(unsigned int idx)
{
	return GetMeshName(idx).c_str();
}

unsigned int MeshFilter::GetCurrentIndex()
{
	return m_Index;
}

unsigned int MeshFilter::GetModelMeshIndex()
{
	return m_MeshNames_V.at(m_Index).second;
}

void MeshFilter::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_pResourceManager = pReosureManager;

	m_Index = pReosureManager->GetModelMeshIndex(m_MeshName);
	SetNames(pReosureManager->GetModelMeshNames());
}

void MeshFilter::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_MeshName = m_MeshName;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void MeshFilter::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
	m_MeshName = m_SaveData->m_MeshName;
}
