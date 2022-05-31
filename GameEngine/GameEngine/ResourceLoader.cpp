#include "pch.h"
#include "ResourceLoader.h"

#include "IResourceManager.h"
#include "DirectoryReader.h"

#include "CATrace.h"

std::vector<std::string>* ResourceLoader::m_pModelName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pAnimName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pTexture_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pShader_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pMatName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pIBLName_V = nullptr;
std::vector<std::string>* ResourceLoader::m_pBankName_V = nullptr;


ResourceLoader::ResourceLoader()
	: m_pResourceManager(nullptr)
{

}

ResourceLoader::~ResourceLoader()
{

}

void ResourceLoader::Initialize(std::shared_ptr<IResourceManager> pResource)
{
	m_pResourceManager = pResource;

	m_pModelName_V = new std::vector<std::string>;
	m_pTexture_V = new std::vector<std::string>;
	m_pShader_V = new std::vector<std::string>;
	m_pAnimName_V = new std::vector<std::string>;
	m_pMatName_V = new std::vector<std::string>;
	m_pIBLName_V = new std::vector<std::string>;
	m_pBankName_V = new std::vector<std::string>;
}

void ResourceLoader::LoadResource(std::string& name)
{

	CA_TRACE("Load Resource");

	DirectoryReader::InputEntryDir(name);

	HRESULT _hr = S_OK;

	try
	{

		if ((DirectoryReader::GetBinaryNames().size() +
			DirectoryReader::GetTextureNames().size() +
			DirectoryReader::GetShaderNames().size() +
			DirectoryReader::GetMatNames().size() +
			DirectoryReader::GetIBLNames().size() +
			DirectoryReader::GetAnimNames().size()) == 0)
		{
			throw std::string("��ΰ� Ʋ��");
		}

		// GraphicMaterial���� ȿ���� ���ؼ� Texture ���� ó���Ѵ�.
		for (std::string& _nowTextureName : DirectoryReader::GetTextureNames())
		{

#if _DEBUG
			CA_TRACE("TextureName = %s", _nowTextureName.c_str());
#endif


			m_pResourceManager->LoadTextureFile(_nowTextureName);
			m_pTexture_V->push_back(DirectoryReader::GetFileName(_nowTextureName, true));

		}
		CA_TRACE("ResourceManager Texture Load Complete");

		for (std::string& _nowIBLName : DirectoryReader::GetIBLNames())
		{
			m_pResourceManager->LoadIBLFile(_nowIBLName);
		}
		CA_TRACE("ResourceManager LIB Image Load Complete");

		// Binary���� �ݵ�� ���� ����Ǿ����
		for (std::string& _nowMatName : DirectoryReader::GetMatNames())
		{
			m_pResourceManager->LoadMatFile(_nowMatName);

			m_pMatName_V->push_back(DirectoryReader::GetFileName(_nowMatName, true));
		}
		CA_TRACE("ResourceManager mat Load Complete");

		for (std::string& _nowBinFileName : DirectoryReader::GetBinaryNames())
		{
			std::string _nowName = DirectoryReader::GetFileName(_nowBinFileName, true);
			m_pResourceManager->LoadBinaryFile(_nowBinFileName);
			m_pModelName_V->push_back(_nowName);
		}
		CA_TRACE("ResourceManager bin Load Complete");

		for (std::string& _nowAnimFileName : DirectoryReader::GetAnimNames())
		{
			m_pResourceManager->LoadAnimationFile(_nowAnimFileName);
			m_pAnimName_V = new std::vector<std::string>();
			m_pAnimName_V->push_back(DirectoryReader::GetFileName(_nowAnimFileName, true));
		}
		CA_TRACE("ResourceManager Anim Load Complete");

		for (std::string& _nowShaderName : DirectoryReader::GetShaderNames())
		{
			m_pResourceManager->LoadShaderFile(_nowShaderName);
			m_pShader_V->push_back(DirectoryReader::GetFileName(_nowShaderName, true));
		}
		CA_TRACE("ResourceManager Shader Load Complete");

		for (std::string& _nowBankName : DirectoryReader::GetBankNames())
		{
			m_pBankName_V->push_back(_nowBankName);
		}
		CA_TRACE("ResourceManager Bank Load Complete");

	}
	catch (std::string exception)
	{
		//CA_TRACE("������ �Է� �߸��� ��� : %s", exception.c_str());
		//PostQuitMessage(-1);
	}

	CA_TRACE("ResourceManager Resource Load Complete");

}

std::vector<std::string>* ResourceLoader::GetNodeDataNames()
{
	return m_pModelName_V;
}

std::vector<std::string>* ResourceLoader::GetAnimNames()
{
	return m_pAnimName_V;
}

std::vector<std::string>* ResourceLoader::GetTextureNames()
{
	return m_pTexture_V;
}

std::vector<std::string>* ResourceLoader::GetShaderNames()
{
	return m_pShader_V;
}

std::vector<std::string>* ResourceLoader::GetMatNames()
{
	return m_pMatName_V;
}

std::vector<std::string>* ResourceLoader::GetIBLNames()
{
	return m_pIBLName_V;
}

std::vector<std::string>* ResourceLoader::GetBankNames()
{
	return m_pBankName_V;
}
