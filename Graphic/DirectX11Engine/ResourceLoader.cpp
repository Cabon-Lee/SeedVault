#include "ResourceLoader.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include "JMFBXParser.h"
#include "DX11MeshFactory.h"
#include "Shaders.h"


#include "NodeData.h"
#include "MaterialData.h"

#include "DirectoryReader.h"
#include "CATrace.h"

ResourceLoader::ResourceLoader() :
	m_pDevice(nullptr),
	m_pDeviceContext(nullptr),
	m_pMeshFactory(nullptr),
	m_pParser(nullptr),
	m_pShader(nullptr)
{

}

ResourceLoader::~ResourceLoader()
{
}


void ResourceLoader::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;

	m_pParser = new JMFBXParser();
	m_pParser->Initalize();

	m_pMeshFactory = new DX11MeshFactory();
	m_pMeshFactory->Initialize(pDevice, pDeviceContext);

	m_pShader = new Shaders();
	m_pShader->Initialize(pDevice);
}

void ResourceLoader::Destroy()
{

}


std::vector<void*>& ResourceLoader::LoadFBXFile(std::string& pathName)
{
	// ���� ��θ� ������ ���� �ε��Ѵ�.
	FBXModel* _newmodel = m_pParser->LoadScene(pathName);

	// �߰��� NodeData�� ��ȯ���Ѽ� �����ش�
	return m_pMeshFactory->GetNodeDataVoid(_newmodel);
}

void* ResourceLoader::LoadTextureFile(std::string& fileName)
{
	if (fileName.empty() == true)
	{
		return nullptr;
	}
		

	ID3D11ShaderResourceView* _tempBitmap = nullptr;
	ID3D11Resource* texResource = nullptr;

	std::wstring _path;
	_path.assign(fileName.begin(), fileName.end());

	// ���� DDS�� �ε带 �õ��Ѵ�.
	CreateDDSTextureFromFile(m_pDevice.Get(), _path.c_str(), &texResource, &_tempBitmap);

	// �����ߴٸ� WIC�� �ε带 �õ��Ѵ�.
	if (_tempBitmap == nullptr)
	{
		CreateWICTextureFromFile(m_pDevice.Get(), _path.c_str(), &texResource, &_tempBitmap);
	}

	if (_tempBitmap == nullptr)
	{
		return nullptr;
	}

	// DDS, WIC ��� ����
	CA_TRACE("�ؽ��� �ε� ���� : %s", _path.c_str());
	return _tempBitmap;

}

void* ResourceLoader::LoadShaderFile(std::string& fileName)
{
	return nullptr;
}

void* ResourceLoader::GetMaterialDataVoid()
{
	MaterialData* _materialData = new MaterialData();
	_materialData->SetPixelShader(&m_pShader->colorShader);

	return _materialData;
}
