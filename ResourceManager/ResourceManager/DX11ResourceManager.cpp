#include "pch.h"

#include "DirectXTex.h"
#include "IRenderer.h"
#include "DX11ResourceManager.h"
#include "DX11MeshFactory.h"

#include "NodeData.h"
#include "NaviMeshFace.h"
#include "MaterialData.h"
#include "InputLayout.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

#include "MatSerializer.h"
#include "MatDeserializer.h"

#include "EffectUIEnum.h"
#include "TextLoader.h"

#include "COMException.h"
#include "ErrorLogger.h"

DX11ResourceManager::DX11ResourceManager()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
	, m_pMatSerializer(nullptr)
	, m_pMatDeserializer(nullptr)
{

}

DX11ResourceManager::~DX11ResourceManager()
{

}

void DX11ResourceManager::Initialize(std::shared_ptr<IRenderer> pRenderer)
{
	pRenderer->GetDeviceAndDeviceContext((void**)&m_pDevice, (void**)&m_pDeviceContext);
	m_pRenderer = pRenderer;

	m_Deserializer = std::make_shared<BinaryDeserializer>();

	m_pMatSerializer = std::make_shared<MatSerializer>();
	m_pMatDeserializer = std::make_shared<MatDeserializer>();

	m_pMeshFactory = std::make_shared<DX11MeshFactory>();
	m_pTextLoader = std::make_shared<TextLoader>();

	// 기본이 되는 머트리얼을 하나 넣어준다
	// mat가 추가되면 그리로 빼야지
	std::shared_ptr<GraphicMaterial> _basic = std::make_shared<GraphicMaterial>();
	_basic->materialName = "Basic";
	_basic->albedoColor = DirectX::SimpleMath::Vector4(0.5f, 0.5f, 0.5f, 1.0f);
	_basic->ambientColor = DirectX::SimpleMath::Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	_basic->fresnelR0 = DirectX::SimpleMath::Vector3(0.5f, 0.5f, 0.5f);
	_basic->normalFactor = 1.0f; _basic->emissiveFactor = 1.0f;

	m_Material_V.push_back(_basic);
	m_Material_UM.insert({ _basic->materialName, _basic });
	m_MaterialNames_V.push_back({ _basic->materialName, m_Material_V.size() - 1 });


	std::shared_ptr<SpriteData> _nowSpriteData = std::make_shared<SpriteData>();

	_nowSpriteData->textureName = "None";
	_nowSpriteData->width = 0.0f;
	_nowSpriteData->height = 0.0f;
	_nowSpriteData->depth = 0.0f;
	_nowSpriteData->shaderResource = nullptr;

	{
		m_Texture_V.push_back(nullptr);
		m_Texture_UM.insert({ "None" ,nullptr });
		m_TextureInfo_V.push_back(_nowSpriteData);
		m_TextureInfo_UM.insert({ "None", _nowSpriteData });
		m_TextureNames_V.push_back({ "None", m_Texture_V.size() - 1 });
	}

	{
		// null 포인터를 넣어두고 시작한다
		// unsigned int에서 0일떄 nullptr을 활용할 수 있게끔
		m_IBL_V.push_back(nullptr);
		m_IBL_UM.insert({ "None" ,nullptr });
		m_IBLNames_V.push_back({ "None", m_Texture_V.size() - 1 });
	}


}


void DX11ResourceManager::LoadBinaryFile(std::string& fileName)
{
	// 아래 함수를 거치면 fileName(path)의 파일을 읽을 준비가 됐다
	std::shared_ptr<FBXModelBinaryFile> _binData = m_Deserializer->LoadBinaryFileFBX(fileName.c_str());

	// Bin 파일 내부에서 메쉬에 대한 정보를 가져올 포인터
	std::shared_ptr<ModelMesh> _newModelMesh = std::make_shared<ModelMesh>();

	// Bin 파일 내부에서 머트리얼 정보를 꺼내올 자료구조, 미리 힙에 할당해둔다
	std::vector<std::shared_ptr<MaterialDesc>> _pMaterial_V;
	_pMaterial_V.resize(_binData->binHeader.materialCount);
	for (int i = 0; i < _binData->binHeader.materialCount; ++i)
	{
		_pMaterial_V[i] = std::make_shared<MaterialDesc>();
	}


	// 내부에서 메쉬 정보와 머트리얼 정보를 가져온다
	m_pMeshFactory->GetModelMeshBinary(m_pDevice, _binData, _newModelMesh, _pMaterial_V);

	std::string _nowFileName = StringHelper::GetFileName(fileName, true);

	// 정리한다
	// 메쉬 정보
	m_ModelMesh_V.push_back(_newModelMesh);
	m_ModelMesh_UM.insert({ _nowFileName , _newModelMesh });
	m_ModelMeshNames_V.push_back({ _nowFileName, m_ModelMesh_V.size() - 1 });

	// 머트리얼 정보
	for (int i = 0; i < _pMaterial_V.size(); ++i)
	{
		// MaterialDesc와 일치하는 이름이 GraphicMaterial 중에 있는지 찾는다.
		// 있으면 안만들어도 되니까
		bool _exist = false;
		for (auto& _nowMatNames : m_MaterialNames_V)
		{
			if (_nowMatNames.first.compare(_pMaterial_V[i]->materialName) == 0)
			{
				_exist = true;
				break;
			}
		}

		if (_exist == false)
		{
			std::shared_ptr<GraphicMaterial> _nowGraphicMat = CreateGraphicMaterial(_pMaterial_V[i]);

			m_Material_V.push_back(_nowGraphicMat);
			m_Material_UM.insert({ _nowGraphicMat->materialName, _nowGraphicMat });
			m_MaterialNames_V.push_back({ _nowGraphicMat->materialName, m_Material_V.size() - 1 });
		}
		else { continue; }
	}

	// 여기서 머트리얼과 노드를 먼저 엮어준다
	LinkMaterialToNode(_newModelMesh);
}

std::shared_ptr<GraphicMaterial> DX11ResourceManager::CreateGraphicMaterial(std::shared_ptr<MaterialDesc> matDesc)
{
	std::shared_ptr<GraphicMaterial> _nowGraphicMat = std::make_shared<GraphicMaterial>();

	_nowGraphicMat->materialName = matDesc->materialName.c_str();

	_nowGraphicMat->albedoMap = matDesc->textureName_V[DIFFUSE_TEXUTRE];
	_nowGraphicMat->normalMap = matDesc->textureName_V[BUMP_TEXUTRE];
	_nowGraphicMat->metalRoughMap = matDesc->textureName_V[SHININESS_TEXUTRE];
	_nowGraphicMat->emissiveMap = matDesc->textureName_V[EMISSIVE_TEXUTRE];

	_nowGraphicMat->textureArray[DIFFUSE_TEXUTRE] = GetTextureIndex(_nowGraphicMat->albedoMap);
	_nowGraphicMat->textureArray[BUMP_TEXUTRE] = GetTextureIndex(_nowGraphicMat->normalMap);
	_nowGraphicMat->textureArray[SHININESS_TEXUTRE] = GetTextureIndex(_nowGraphicMat->metalRoughMap);
	_nowGraphicMat->textureArray[EMISSIVE_TEXUTRE] = GetTextureIndex(_nowGraphicMat->emissiveMap);

	_nowGraphicMat->reflactionFactor = matDesc->reflactionFactor;
	_nowGraphicMat->transparentFactor = matDesc->transparentFactor;
	_nowGraphicMat->specularPower = matDesc->specularPower;
	_nowGraphicMat->smoothness = matDesc->shineness;


	_nowGraphicMat->fresnelR0 = matDesc->fresnelR0;
	_nowGraphicMat->albedoColor = DirectX::SimpleMath::Vector4(matDesc->diffuse);
	_nowGraphicMat->albedoColor.w = 1.0f;
	_nowGraphicMat->ambientColor = DirectX::SimpleMath::Vector4(matDesc->ambient);
	_nowGraphicMat->ambientColor.w = 1.0f;

	return 	_nowGraphicMat;
}

void DX11ResourceManager::LoadMatFile(std::string& fileName)
{
	std::shared_ptr<MatFile> _nowMatFile = m_pMatDeserializer->LoadMatFile(fileName.c_str());

	std::shared_ptr<GraphicMaterial> _nowGraphicMat = std::make_shared<GraphicMaterial>();

	_nowGraphicMat->materialName = _nowMatFile->matData.materialName.c_str();

	_nowGraphicMat->albedoMap = _nowMatFile->matData.albedoMap.c_str();
	_nowGraphicMat->normalMap = _nowMatFile->matData.normalMap.c_str();
	_nowGraphicMat->metalRoughMap = _nowMatFile->matData.metalRoughMap.c_str();
	_nowGraphicMat->emissiveMap = _nowMatFile->matData.emissiveMap.c_str();

	_nowGraphicMat->textureArray[0] = this->GetTextureIndex(_nowGraphicMat->albedoMap);
	_nowGraphicMat->textureArray[1] = this->GetTextureIndex(_nowGraphicMat->normalMap);
	_nowGraphicMat->textureArray[2] = this->GetTextureIndex(_nowGraphicMat->metalRoughMap);
	_nowGraphicMat->textureArray[3] = this->GetTextureIndex(_nowGraphicMat->emissiveMap);

	_nowGraphicMat->metallic = _nowMatFile->matData.metallic;
	_nowGraphicMat->smoothness = _nowMatFile->matData.shineness;
	_nowGraphicMat->normalFactor = _nowMatFile->matData.normalFactor;
	_nowGraphicMat->emissiveFactor = _nowMatFile->matData.emissiveFactor;

	_nowGraphicMat->isTransparency = _nowMatFile->matData.isTransparency;

	// 디시리얼라이즈를 한다
	// 디시리얼라이즈된 정보를 GraphicMaterial 형식으로 만든다
	// 그 정보를 여기에 넣는다
	m_Material_V.push_back(_nowGraphicMat);
	m_Material_UM.insert({ _nowGraphicMat->materialName, _nowGraphicMat });
	m_MaterialNames_V.push_back({ _nowGraphicMat->materialName, m_Material_V.size() - 1 });
}


std::shared_ptr<NaviMeshSet> DX11ResourceManager::MakeNeviMesh(std::string& fileName)
{
	// fileName에 path를 더해준다
	std::string _nowPath;
	_nowPath.append(_BINPATH);
	_nowPath.append(fileName);

	std::shared_ptr<FBXModelBinaryFile> _binData = m_Deserializer->LoadBinaryFileFBX(_nowPath.c_str());
	std::shared_ptr<NaviMeshSet> _newNeviMeshSet = std::make_shared<NaviMeshSet>();

	// 내부에서 메쉬 정보와 머트리얼 정보를 가져온다
	m_pMeshFactory->GetModelMeshBinaryAsNeviMesh(m_pDevice, _binData, _newNeviMeshSet);

	return _newNeviMeshSet;
}

void DX11ResourceManager::LoadIBLFile(std::string& fileName)
{
	if (fileName.empty() == true)
	{
		return;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _tempBitmap;
	Microsoft::WRL::ComPtr<ID3D11Resource> _texResource;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC desc;

	std::wstring _path;
	_path.assign(fileName.begin(), fileName.end());

	try
	{
		HRESULT _hr;

		// DDS로 먼저 텍스쳐 로드를 시도하고, 그 후 WIC로 시도한다.
		// DDS 확장자를 검사해서 실행하는 방법도 있지만 단순하게 실행시켰다.
		_hr = CreateDDSTextureFromFile(m_pDevice, _path.c_str(), &_texResource, &_tempBitmap);

		COM_ERROR_IF_FAILED(_hr, "CreateWICTextureFromFile Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return;
	}

	std::string _nowTextureName = StringHelper::GetFileName(fileName, false);

	m_IBL_V.push_back(_tempBitmap);
	m_IBL_UM.insert({ _nowTextureName  ,_tempBitmap });
	m_IBLNames_V.push_back({ _nowTextureName, m_IBL_V.size() - 1 });
}

void DX11ResourceManager::LoadQuestTextFile(std::string& fileName)
{
	std::vector<QuestBlock> _QuestBlock_V;
	m_pTextLoader->LoadQuestText(fileName, _QuestBlock_V);

	std::string _nowQuestTextName = StringHelper::GetFileName(fileName, false);
	m_QuestText_UM.insert({ _nowQuestTextName ,_QuestBlock_V });
}

void DX11ResourceManager::LoadDialogueTextFile(std::string& fileName)
{
	std::vector<DialogueBlock> _DialogueBlock_V;
	m_pTextLoader->LoadDialogueText(fileName, _DialogueBlock_V);

	std::string _nowDialogueTextName = StringHelper::GetFileName(fileName, false);
	m_DialogueText_UM.insert({ _nowDialogueTextName ,_DialogueBlock_V });
}

void DX11ResourceManager::LinkMaterialToNode(std::shared_ptr<ModelMesh> pMesh)
{
	std::vector<std::shared_ptr<GraphicMaterial>> _nowMat;

	for (auto& _nowNode : *pMesh->m_pNodeData_V)
	{
		// 비어있다면 일단 베이직을 넣어줘야지
		if (_nowNode->materialName.empty() == true)
		{
			_nowNode->pMaterial = GetMaterial("Basic");
			_nowMat.push_back(_nowNode->pMaterial);
		}
		else
		{
			_nowNode->pMaterial = GetMaterial(_nowNode->materialName);
			_nowMat.push_back(_nowNode->pMaterial);
		}
	}

	pMesh->m_pGraphicMaterial_V.swap(_nowMat);
}

void DX11ResourceManager::LoadAnimationFile(std::string& fileName)
{
	// 아래 함수를 거치면 fileName(path)의 파일을 읽을 준비가 됐다
	std::shared_ptr<AnimFileForRead> _binData = m_Deserializer->LoadAnimFile(fileName.c_str());

	std::shared_ptr<AnimationClipData> _nowAnimData = std::make_shared<AnimationClipData>();

	// 내부에서 메쉬 정보와 머트리얼 정보를 가져온다
	m_pMeshFactory->GetAnimationData(_binData, _nowAnimData);

	std::string _nowFileName = StringHelper::GetFileName(fileName, true);

	_nowAnimData->m_ModelName = _nowFileName;
	m_AnimationClipData_V.push_back(_nowAnimData);
	m_AnimationClipData_UM.insert({ _nowFileName , _nowAnimData });
	m_AnimationClipDataNames_V.push_back({ _nowFileName, m_AnimationClipData_V.size() - 1 });

}

void DX11ResourceManager::LoadTextureFile(std::string& fileName)
{
	if (fileName.empty() == true)
	{
		return;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _tempBitmap;
	Microsoft::WRL::ComPtr<ID3D11Resource> _texResource;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	D3D11_TEXTURE2D_DESC desc;

	std::wstring _path;
	_path.assign(fileName.begin(), fileName.end());


	try
	{
		HRESULT _hr;

		auto _extens = StringHelper::GetFileExtension(fileName);

		if (_extens == "hdr")
		{
			DirectX::ScratchImage _scratchImage;

			_hr = LoadFromHDRFile(_path.c_str(), nullptr, _scratchImage);

			COM_ERROR_IF_FAILED(_hr, "LoadFromHDRFile Fail");

			if (_hr == S_OK)
			{
				_hr = CreateShaderResourceView(m_pDevice,
					_scratchImage.GetImages(), _scratchImage.GetImageCount(), _scratchImage.GetMetadata(),
					_tempBitmap.GetAddressOf());

				COM_ERROR_IF_FAILED(_hr, "CreateShaderResourceView Fail");
			}
		}
		else
		{
			// DeviceContext를 사용 중이라면 기다린다
			while (m_pRenderer->ContextInUse(TRUE) == TRUE)
				Sleep(0);

			if (_extens == "dds")
			{
				_hr = CreateDDSTextureFromFile(m_pDevice, m_pDeviceContext, _path.c_str(), &_texResource, &_tempBitmap);
			}
			else
			{
				_hr = CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, _path.c_str(), &_texResource, &_tempBitmap);
			}

			// 사용이 끝났다면 다시 써라
			m_pRenderer->ContextInUse(FALSE);


			if (_texResource != nullptr)
			{
				_hr = _texResource.As(&tex);
				tex->GetDesc(&desc);
			}
		}

		COM_ERROR_IF_FAILED(_hr, "CreateWICTextureFromFile Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return;
	}

	std::string _nowTextureName = StringHelper::GetFileName(fileName, true);
	std::shared_ptr<SpriteData> _nowSpriteData = std::make_shared<SpriteData>(fileName, desc.Width, desc.Height, 0, _tempBitmap);

	m_Texture_V.push_back(_tempBitmap);
	m_Texture_UM.insert({ _nowTextureName  ,_tempBitmap });

	m_TextureInfo_V.push_back(_nowSpriteData);
	m_TextureInfo_UM.insert({ _nowTextureName, _nowSpriteData });

	m_TextureNames_V.push_back({ _nowTextureName, m_Texture_V.size() - 1 });
	m_TextureInfoNames_V.push_back({ _nowTextureName, m_TextureInfo_V.size() - 1 });

	/*
	while (m_pRenderer->ContextInUse(TRUE) == TRUE)
		Sleep(0);

	m_pRenderer->ExecuteCommandLine();

	m_pRenderer->ContextInUse(FALSE);
	*/
}

void DX11ResourceManager::CreateInputLayout()
{
	for (auto& _nowVertexShader : m_VertexShader_UM)
	{
		HRESULT _hr;

		try
		{
			_hr = InputLayout::CrateInputLayoutFromVertexShader(
				m_pDevice,
				_nowVertexShader.second->GetVertexShaderBuffer(),
				&_nowVertexShader.second->inputLayout);

			COM_ERROR_IF_FAILED(_hr, "CreateInputLayout Fail");

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}
	}
}


HRESULT DX11ResourceManager::FindIndexByName(std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result)
{
	for (std::pair<std::string, unsigned int>& _nowElem : list)
	{
		if (_nowElem.first == name)
		{
			result = _nowElem.second;
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT DX11ResourceManager::FindTextureIndexByName(std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result)
{
	if (name.empty() == true || name.size() == 1)
	{
		return E_INVALIDARG;
	}

	for (std::pair<std::string, unsigned int>& _nowElem : list)
	{
		if (_nowElem.first.compare(name.c_str()) == 0)
		{
			result = _nowElem.second;
			return S_OK;
		}
	}

	return E_FAIL;
}

void DX11ResourceManager::LoadShaderFile(std::string& path)
{
	// 이름만 남긴다
	std::string _fileName = StringHelper::GetFileName(path, true);
	std::string _temp;

	for (size_t i = 0; i < 2; i++)
	{
		_temp += _fileName[i];
	}

	// 접두가 vs라면 버텍스 쉐이더
	if (_temp == "vs")
	{
		std::shared_ptr<VertexShader> _newShader = LoadShaderFile<VertexShader>(path);
		m_VertexShader_V.push_back(_newShader);
		m_VertexShader_UM.insert({ _fileName, _newShader });
		m_VertexShaderNames_V.push_back({ _fileName , m_VertexShader_V.size() - 1 });

		try
		{
			HRESULT _hr;
			_hr = InputLayout::CrateInputLayoutFromVertexShader(
				m_pDevice,
				_newShader->GetVertexShaderBuffer(),
				&_newShader->inputLayout);

			COM_ERROR_IF_FAILED(_hr, "CreateInputLayout Fail");

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}

	}
	else if (_temp == "ps")
	{
		std::shared_ptr<PixelShader> _newShader = LoadShaderFile<PixelShader>(path);
		m_PixelShader_V.push_back(_newShader);
		//m_PixelShader_UM.insert({ _fileName, LoadShaderFile<PixelShader>(path) });
		m_PixelShader_UM.insert({ _fileName, _newShader });
		m_PixelShaderNames_V.push_back({ _fileName , m_PixelShader_V.size() - 1 });
	}
	else if (_temp == "gs")
	{
		std::shared_ptr<GeometryShader> _newShader = LoadShaderFile<GeometryShader>(path);
		m_GeometryShader_V.push_back(_newShader);
		m_GeometryShader_UM.insert({ _fileName, _newShader });
		m_GeometryShaderNames_V.push_back({ _fileName , m_GeometryShader_V.size() - 1 });
	}
}

void DX11ResourceManager::SaveMatFile(unsigned int modelIdx, int idx)
{

	std::shared_ptr<GraphicMaterial> _graphicMat = m_ModelMesh_V[modelIdx]->m_pGraphicMaterial_V[idx];

	m_pMatSerializer->SaveMatFile(_graphicMat, _PATH.c_str());
	//std::shared_ptr<GraphicMaterial> p = m_Material_UM.find(m_MaterialNames_V[modelIdx].first)->second;

}

void DX11ResourceManager::LoadMatFile(unsigned int, int idx)
{
	//m_pMatDeserializer->LoadMatFile();
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetModelMeshNames()
{
	return m_ModelMeshNames_V;
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetAnimationNames()
{
	return m_AnimationClipDataNames_V;
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetMaterialNames()
{
	return m_MaterialNames_V;
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetTextureNames()
{
	return m_TextureNames_V;
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetVertexShaderNames()
{
	return m_VertexShaderNames_V;
}

std::vector<std::pair<std::string, unsigned int>>& DX11ResourceManager::GetPixelShaderNames()
{
	return m_PixelShaderNames_V;
}

std::shared_ptr<ModelMesh> DX11ResourceManager::GetModelMesh(const std::string& name)
{
	return m_ModelMesh_UM.at(name);
}

std::shared_ptr<ModelMesh> DX11ResourceManager::GetModelMesh(unsigned int idx)
{
	return m_ModelMesh_V[idx];
}

std::shared_ptr<AnimationClipData> DX11ResourceManager::GetAnimationClipData(const std::string& name)
{
	return m_AnimationClipData_UM.at(name);
}

std::shared_ptr<AnimationClipData> DX11ResourceManager::GetAnimationClipData(unsigned int idx)
{
	return m_AnimationClipData_V[idx];
}

std::shared_ptr<GraphicMaterial> DX11ResourceManager::GetMaterial(const std::string& name)
{
	return m_Material_UM.at(name);
}

std::shared_ptr<GraphicMaterial> DX11ResourceManager::GetMaterial(unsigned int idx)
{
	return m_Material_V[idx];
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DX11ResourceManager::GetTexture(const std::string& name)
{
	return m_Texture_UM.at(name.c_str());
}

std::shared_ptr<SpriteData> DX11ResourceManager::GetSpriteData(const std::string& name)
{
	return m_TextureInfo_UM.at(name.c_str());
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DX11ResourceManager::GetTexture(unsigned int idx)
{
	if (idx < 0) { return nullptr; }
	if (idx >= m_Texture_V.size())
	{
		idx = 0;
	}

	return m_Texture_V[idx];
}

std::shared_ptr<SpriteData> DX11ResourceManager::GetSpriteData(unsigned int idx)
{
	return m_TextureInfo_V[idx];
}

std::shared_ptr<VertexShader> DX11ResourceManager::GetVertexShader(const std::string& name)
{
	return m_VertexShader_UM.at(name.c_str());
}

std::shared_ptr<VertexShader> DX11ResourceManager::GetVertexShader(unsigned int idx)
{
	return m_VertexShader_V[idx];
}

std::shared_ptr<PixelShader> DX11ResourceManager::GetPixelShader(const std::string& name)
{
	return m_PixelShader_UM.at(name.c_str());
}

std::shared_ptr<PixelShader> DX11ResourceManager::GetPixelShader(unsigned int idx)
{
	return m_PixelShader_V[idx];
}

std::shared_ptr<GeometryShader> DX11ResourceManager::GetGeometryShader(const std::string& name)
{
	return m_GeometryShader_UM.at(name.c_str());
}

std::shared_ptr<GeometryShader> DX11ResourceManager::GetGeometryShader(unsigned int idx)
{
	return m_GeometryShader_V[idx];
}

std::vector<QuestBlock> DX11ResourceManager::GetQuestText(const std::string& name)
{
	return m_QuestText_UM.at(name.c_str());
}

std::vector<DialogueBlock> DX11ResourceManager::GetDialogueText(const std::string& name)
{
	return m_DialogueText_UM.at(name.c_str());
}

std::unordered_map<std::string, std::vector<struct QuestBlock>> DX11ResourceManager::GetWholeQuestText()
{
	return m_QuestText_UM;
}

std::unordered_map<std::string, std::vector<struct DialogueBlock>> DX11ResourceManager::GetWholeDialogueText()
{
	return m_DialogueText_UM;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DX11ResourceManager::GetIBLImage(unsigned int idx)
{
	return m_IBL_V[idx];
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> DX11ResourceManager::GetIBLImage(const std::string& name)
{
	return m_IBL_UM.at(name.c_str());
}

const std::string& DX11ResourceManager::GetMaterialName(
	unsigned int modelIndex, unsigned int matIndex)
{
	return m_ModelMesh_V[modelIndex]->m_pGraphicMaterial_V[matIndex]->materialName;
}

const std::string& DX11ResourceManager::GetMaterialTextureName(
	unsigned int modelIndex, unsigned int matIndex, unsigned int attIndex)
{

	std::shared_ptr<ModelMesh> _nowMesh = m_ModelMesh_V[modelIndex];

	switch (attIndex)
	{
	case DIFFUSE_TEXUTRE: return _nowMesh->m_pGraphicMaterial_V[matIndex]->albedoMap;
	case BUMP_TEXUTRE: return _nowMesh->m_pGraphicMaterial_V[matIndex]->normalMap;
	case SHININESS_TEXUTRE: return _nowMesh->m_pGraphicMaterial_V[matIndex]->metalRoughMap;
	case EMISSIVE_TEXUTRE: return _nowMesh->m_pGraphicMaterial_V[matIndex]->emissiveMap;
	default:
		return "";
	}
}

void DX11ResourceManager::SetMaterialTextureName(
	unsigned int modelIndex,
	unsigned int matIndex, unsigned int attIndex, const std::string& name)
{
	std::shared_ptr<ModelMesh> _nowMesh = m_ModelMesh_V[modelIndex];

	switch (attIndex)
	{
	case DIFFUSE_TEXUTRE:
	{
		_nowMesh->m_pGraphicMaterial_V[matIndex]->albedoMap = name;
		_nowMesh->m_pGraphicMaterial_V[matIndex]->textureArray[DIFFUSE_TEXUTRE] = GetTextureIndex(name);
		break;
	}
	case BUMP_TEXUTRE:
	{
		_nowMesh->m_pGraphicMaterial_V[matIndex]->normalMap = name;
		_nowMesh->m_pGraphicMaterial_V[matIndex]->textureArray[BUMP_TEXUTRE] = GetTextureIndex(name);
		break;
	}
	case SHININESS_TEXUTRE:
	{
		_nowMesh->m_pGraphicMaterial_V[matIndex]->metalRoughMap = name;
		_nowMesh->m_pGraphicMaterial_V[matIndex]->textureArray[SHININESS_TEXUTRE] = GetTextureIndex(name);
		break;
	}
	case EMISSIVE_TEXUTRE:
	{
		_nowMesh->m_pGraphicMaterial_V[matIndex]->emissiveMap = name;
		_nowMesh->m_pGraphicMaterial_V[matIndex]->textureArray[EMISSIVE_TEXUTRE] = GetTextureIndex(name);
		break;
	}
	}
}

float& DX11ResourceManager::GetMaterialCalibrateFactor(
	unsigned int modelIndex, unsigned int matIndex, int att)
{
	std::shared_ptr<ModelMesh> _nowMesh = m_ModelMesh_V[modelIndex];

	// att를 Shininess와 Fresnel로 나눌 예정
	switch (att)
	{
	case 0: return _nowMesh->m_pGraphicMaterial_V[matIndex]->metallic;
	case 1: return _nowMesh->m_pGraphicMaterial_V[matIndex]->smoothness;
	case 2: return _nowMesh->m_pGraphicMaterial_V[matIndex]->normalFactor;
	case 3: return _nowMesh->m_pGraphicMaterial_V[matIndex]->emissiveFactor;
	default:
		break;
	}
}

bool& DX11ResourceManager::GetMaterialTransparencyFactor(
	unsigned int modelIndex, unsigned int matIndex)
{
	std::shared_ptr<ModelMesh> _nowMesh = m_ModelMesh_V[modelIndex];

	return _nowMesh->m_pGraphicMaterial_V[matIndex]->isTransparency;
}

RECT DX11ResourceManager::GetTextureRect(unsigned int idx)
{
	RECT _newRect;

	_newRect.left = 0;
	_newRect.top = 0;
	_newRect.right = static_cast<LONG>(m_TextureInfo_V[idx]->width);
	_newRect.bottom = static_cast<LONG>(m_TextureInfo_V[idx]->height);

	return _newRect;
}

void DX11ResourceManager::GetSpriteScale(unsigned int idx, DirectX::SimpleMath::Vector3& _scale)
{
	_scale.x = m_TextureInfo_V[idx]->width;
	_scale.y = m_TextureInfo_V[idx]->height;
	_scale.z = m_TextureInfo_V[idx]->depth;
}

unsigned int DX11ResourceManager::GetModelMeshIndex(const std::string& name)
{
	unsigned int _result;

	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_ModelMeshNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;

}

unsigned int DX11ResourceManager::GetAnimationClipDataIdnex(const std::string& name)
{
	unsigned int _result;

	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_AnimationClipDataNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;

}

unsigned int DX11ResourceManager::GetMaterialIndex(const std::string& name)
{
	unsigned int _result;

	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_MaterialNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;

}

unsigned int  DX11ResourceManager::GetTextureIndex(const std::string& name)
{
	unsigned int _result;

	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_TextureNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;
}

unsigned int DX11ResourceManager::GetSpriteDataIndex(const std::string& name)
{
	unsigned int _result;
	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_TextureInfoNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;
}

unsigned int DX11ResourceManager::GetVertexShaderIndex(const std::string& name)
{
	unsigned int _result;
	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_VertexShaderNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;

}

unsigned int DX11ResourceManager::GetPixelShaderIndex(const std::string& name)
{
	unsigned int _result;
	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_PixelShaderNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;
}

unsigned int DX11ResourceManager::GetGeometryShaderIndex(const std::string& name)
{
	unsigned int _result;
	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_GeometryShaderNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;
}

unsigned int DX11ResourceManager::GetIBLImageIndex(const std::string& name)
{
	unsigned int _result;
	try
	{
		HRESULT _hr;
		_hr = FindTextureIndexByName(m_IBLNames_V, name, _result);

		RESOURCE_MISSING(_hr, std::string(name) + " Resource is Missing");
		RESOURCE_NULLNAME(_hr, "Null Name Error");
	}
	catch (RESOURCE_MISSING_Exception& e)
	{
		_result = 0;
	}
	catch (NULLNameException& e)
	{
		_result = 0;
	}

	return _result;

}

unsigned int DX11ResourceManager::GetTotalKeyFrame(unsigned int animationIndex)
{
	return m_AnimationClipData_V[animationIndex]->m_TotalKeyFrames;
}

const std::vector<std::shared_ptr<GraphicMaterial>>& DX11ResourceManager::GetMaterialOfMesh(unsigned int idx)
{
	return m_ModelMesh_V[idx]->m_pGraphicMaterial_V;
}

void* DX11ResourceManager::GetNodeDataWorldTM(unsigned int idx)
{
	return reinterpret_cast<void*>(&m_ModelMesh_V[idx]->m_pNodeData_V->at(0)->meshWorldTM);
}

void* DX11ResourceManager::GetBoneDataWorldTM(unsigned int idx, const std::string& name)
{
	auto _nowModel = m_ModelMesh_V[idx];
	if (_nowModel->m_pSkeleton != nullptr)
		return reinterpret_cast<void*>(m_ModelMesh_V[idx]->GetBoneWorldPostion(name));
	else
		return nullptr;
}

void* DX11ResourceManager::GetBoneDataWorldTM(unsigned int idx, unsigned int boneIndex)
{
	auto _nowModel = m_ModelMesh_V[idx];
	if (_nowModel->m_pSkeleton != nullptr)
		return reinterpret_cast<void*>(m_ModelMesh_V[idx]->GetBoneWorldPostion(boneIndex));
	else
		return nullptr;
}