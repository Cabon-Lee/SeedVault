#include "ResourceManager.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include "Shaders.h"

#include "NodeData.h"
#include "MaterialData.h"

#include "InputLayout.h"

#include "ErrorLogger.h"
#include "COMException.h"
#include "CATrace.h"


ResourceManager::ResourceManager() :
	m_pDevice(nullptr),
	m_pDeviceContext(nullptr),
	m_pMeshFactory(nullptr),
	m_pShader(nullptr)
{

}

ResourceManager::~ResourceManager()
{
}


void ResourceManager::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;


	//m_Deserializer = std::make_shared<BinaryDeserializer>();

}

void ResourceManager::Destroy()
{

}


void ResourceManager::LoadResource(std::string& path)
{


}


std::vector< std::shared_ptr<NodeData>> ResourceManager::GetNodeData(const std::string& name)
{

	std::shared_ptr<ModelMesh>  _nowModel = m_ModelMesh_UM.at(name);
	return *(_nowModel->m_pNodeData_V);
}

std::shared_ptr<ModelMesh> ResourceManager::GetModelMesh(const std::string& name)
{
	return m_ModelMesh_UM.at(name);
}

ID3D11ShaderResourceView* ResourceManager::GetTextrue(const std::string& name)
{
	return m_Textrue_UM.at(name.c_str());
}

VertexShader* ResourceManager::GetVertexShader(const std::string& name)
{
	return m_VertexShader_UM.at(name);

}

PixelShader* ResourceManager::GetPixelShader(const std::string& name)
{
	return m_PixelShader_UM.at(name);
}

MaterialDesc* ResourceManager::GetMaterialDesc(const std::string& name)
{
	return m_MaterialDesc_UM.at(name);
}

void ResourceManager::LoadFBXFile(std::string& pathName)
{
	/*
	std::vector<MaterialDesc*> _material_V;

	BinaryDeserializer* deser = new BinaryDeserializer();
	FBXModelBinaryFile* _binData = deser->LoadBinaryFileFBX(pathName.c_str());
	
	_material_V.resize(_binData->binHeader.materialCount);
	for (int i = 0; i < _binData->binHeader.materialCount; ++i)
	{
		_material_V[i] = new MaterialDesc();
	}

	std::shared_ptr<ModelMesh>_newModelMesh = m_pMeshFactory->GetModelMeshBinary(_binData, _material_V);
	for (int i = 0; i < _material_V.size(); ++i)
	{
		m_MaterialDesc_UM.insert({ _material_V[i]->materialName, _material_V[i] });
	}
	//std::shared_ptr<ModelMesh> _newModelMesh = m_pMeshFactory->GetModelMesh(_newModel);


	m_ModelMesh_UM.insert({ StringHelper::GetFileName(pathName, true) , _newModelMesh });
	*/

	/// 데시리얼라이저 Test(임시)
	///-----------------------------------------
	
	//std::shared_ptr<ModelMesh> _newModelMesh;
	//m_ModelMesh_UM.insert({ StringHelper::GetFileName(pathName, true) , _newModelMesh });
}

void ResourceManager::LoadBinaryFile(std::string& fileName)
{
	/*
	std::vector<MaterialDesc*> _material_V;

	FBXModelBinaryFile* _binData = m_Deserializer->LoadBinaryFileFBX(fileName.c_str());

	_material_V.resize(_binData->binHeader.materialCount);
	for (int i = 0; i < _binData->binHeader.materialCount; ++i)
	{
		_material_V[i] = new MaterialDesc();
	}


	std::shared_ptr<ModelMesh>_newModelMesh = m_pMeshFactory->GetModelMeshBinary(_binData, _material_V);
	for (int i = 0; i < _material_V.size(); ++i)
	{
		m_MaterialDesc_UM.insert({ _material_V[i]->materialName, _material_V[i] });
	}

	m_ModelMesh_UM.insert({ StringHelper::GetFileName(fileName, true) , _newModelMesh });
	*/
}

void ResourceManager::LoadTextureFile(std::string& fileName)
{
	if (fileName.empty() == true)
	{
		return;
	}

	ID3D11ShaderResourceView* _tempBitmap = nullptr;
	ID3D11Resource* texResource = nullptr;

	std::wstring _path;
	_path.assign(fileName.begin(), fileName.end());

	try
	{
		HRESULT _hr;

		// DDS로 먼저 텍스쳐 로드를 시도하고, 그 후 WIC로 시도한다.
		// DDS 확장자를 검사해서 실행하는 방법도 있지만 단순하게 실행시켰다.
		_hr = CreateDDSTextureFromFile(m_pDevice.Get(), _path.c_str(), &texResource, &_tempBitmap);
			
		if (_hr != S_OK)
		{
			_hr = CreateWICTextureFromFile(m_pDevice.Get(), _path.c_str(), &texResource, &_tempBitmap);
		}

		COM_ERROR_IF_FAILED(_hr, "CreateWICTextureFromFile Fail");
	
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return;
	}
	

	m_Textrue_UM.insert({ StringHelper::GetFileName(fileName, true) ,_tempBitmap });
}


void ResourceManager::LoadShaderFile(std::string& path)
{
	// 이름만 남긴다
	
	std::string _fileName = StringHelper::GetFileName(path, true);

	std::string _temp;
	for (size_t i = 0; i < 2; i++)
	{
		_temp += _fileName[i];
	}

	if (_temp == "vs")
	{
		m_VertexShader_UM.insert(
			{ _fileName, LoadShaderFile<VertexShader>(path) });
	}
	else if (_temp == "ps")
	{
		m_PixelShader_UM.insert(
			{ _fileName, LoadShaderFile<PixelShader>(path) });
	}
}

void ResourceManager::AddAnimationClipData(std::shared_ptr<AnimationClipData> anim)
{
	m_AnimationClipData_V.push_back(anim);
}

void ResourceManager::AddAnimationClipData(std::string fileName, std::shared_ptr<AnimationClipData> anim)
{
	m_AnimationClipData_UM.insert({fileName, anim});
	m_AnimationIndex_M.insert({fileName, m_AnimationClipData_V.size()});
	m_AnimationClipData_V.push_back(anim);
}

void ResourceManager::CreateInputLayout()
{

	for (auto& _nowVertexShader : m_VertexShader_UM)
	{

		// 일단은 되게끔만 해둔 상태
		// 이제부터는 Layout을 만들때 동적으로 알 수 있게 하면 좋겠다

		HRESULT _hr;

		try
		{
			InputLayout::CrateInputLayoutFromVertexShader(
				m_pDevice.Get(), 
				_nowVertexShader.second->GetVertexShaderBuffer(),
				&_nowVertexShader.second->inputLayout);
		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}
	}
}

void* ResourceManager::GetMaterialDataVoid()
{
	MaterialData* _materialData = new MaterialData();

	return _materialData;
}
