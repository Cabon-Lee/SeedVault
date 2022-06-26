#pragma once

/// DLL�� ������� ���ҽ� �Ŵ���
/// ���������� ��� ���� �ñ�ٺ���, �������� ���� ���ҽ� �Ŵ����� �Ǿ���� ����
/// ���ҽ��� ������ ��ü�� ���� ����� ���� �����ϴٺ��� ���轺 �Ŵ����� ����
/// ��, ���ҽ����� DX� �������̹Ƿ� �������̽��� �ְ�, ���� ������ �������� ���� ������ �ɷ� ����
/// �� �Ŵ����� DX11�� ���������� �����

#include <d3d11.h>
#include <memory>
#include <unordered_map>
#include <map>

#include "Shaders.h"

#include "IResourceManager.h"

#ifdef _DEBUG
const std::string _PATH = "../../SeedVault/Data/mat/";
const std::string _BINPATH = "../../SeedVault/Data/bin/";
#else
const std::string _PATH = "../Data/mat/";
const std::string _BINPATH = "../Data/bin/";
#endif

class ModelMesh;
class BinaryDeserializer;
class DX11MeshFactory;
class VertexShader;
class PixelShader;
class GeometryShader;
struct AnimationClipData;
struct SpriteData;
class MatSerializer;
class MatDeserializer;
struct QuestBlock;
struct DialogueBlock;


class DX11ResourceManager : public IResourceManager
{
public:
	DX11ResourceManager();
	~DX11ResourceManager();

	virtual void Initialize(std::shared_ptr<IRenderer> pRenderer) override;


	// �޽�, ��Ʈ����, �ִϸ��̼� ������ ��� �����´�
	// ��, �ϳ��� Ŭ������ ���Ƽ� �����ϴ°� �ƴ϶� ������ ��Ҹ� ���� �з��ؼ� �����Ѵ�
	// �������� ����ϴ� �������̽�
	virtual void LoadBinaryFile(std::string& fileName) override;
	virtual void LoadAnimationFile(std::string& path) override;
	virtual void LoadTextureFile(std::string& fileName) override;
	virtual void LoadShaderFile(std::string& fileName) override;
	virtual void LoadMatFile(std::string& fileName) override;
	virtual void LoadIBLFile(std::string& fileName) override;
	virtual void LoadQuestTextFile(std::string& fileName) override;
	virtual void LoadDialogueTextFile(std::string& fileName) override;

	virtual void SaveMatFile(unsigned int, int idx) override;
	virtual void LoadMatFile(unsigned int, int idx) override;

	virtual std::shared_ptr<NaviMeshSet> MakeNeviMesh(std::string& fileName) override;

	virtual std::vector<std::pair<std::string, unsigned int>>& GetModelMeshNames() override;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetAnimationNames() override;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetMaterialNames() override;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetTextureNames() override;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetVertexShaderNames() override;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetPixelShaderNames() override;

	// ���������� ����ϴ� �������̽�
	virtual std::shared_ptr<ModelMesh> GetModelMesh(const std::string& name) override;
	virtual std::shared_ptr<AnimationClipData> GetAnimationClipData(const std::string& name) override;
	virtual std::shared_ptr<GraphicMaterial> GetMaterial(const std::string& name) override;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const std::string& name) override;
	virtual std::shared_ptr<SpriteData> GetSpriteData(const std::string& name) override;

	virtual std::shared_ptr<VertexShader> GetVertexShader(const std::string& name) override;
	virtual std::shared_ptr<PixelShader> GetPixelShader(const std::string& name) override;
	virtual std::shared_ptr<GeometryShader> GetGeometryShader(const std::string& name) override;

	virtual std::shared_ptr<ModelMesh> GetModelMesh(unsigned int idx) override;
	virtual std::shared_ptr<AnimationClipData> GetAnimationClipData(unsigned int idx) override;
	virtual std::shared_ptr<GraphicMaterial> GetMaterial(unsigned int idx) override;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(unsigned int idx) override;
	virtual std::shared_ptr<SpriteData> GetSpriteData(unsigned int idx) override;

	virtual std::shared_ptr<VertexShader> GetVertexShader(unsigned int idx) override;
	virtual std::shared_ptr<PixelShader> GetPixelShader(unsigned int idx) override;
	virtual std::shared_ptr<GeometryShader> GetGeometryShader(unsigned int idx) override;

	virtual std::vector<QuestBlock> GetQuestText(const std::string& name) override;
	virtual std::vector<DialogueBlock> GetDialogueText(const std::string& name) override;

	virtual std::unordered_map<std::string, std::vector<struct QuestBlock>> GetWholeQuestText() override;
	virtual std::unordered_map<std::string, std::vector<struct DialogueBlock>> GetWholeDialogueText() override;

	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIBLImage(const std::string& name) override;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIBLImage(unsigned int idx) override;

	virtual const std::string& GetMaterialName(
		unsigned int modelIndex, unsigned int matIndex) override;

	virtual const std::string& GetMaterialTextureName(
		unsigned int modelIndex,
		unsigned int matIndex, unsigned int attIndex) override;

	virtual void SetMaterialTextureName(
		unsigned int modelIndex,
		unsigned int matIndex, unsigned int attIndex, const std::string& name) override;

	virtual float& GetMaterialCalibrateFactor(unsigned int modelIndex, unsigned int matIndex, int att) override;
	virtual bool& GetMaterialTransparencyFactor(unsigned int modelIndex, unsigned int matIndex) override;

	virtual RECT GetTextureRect(unsigned int idx) override;

	virtual void GetSpriteScale(unsigned int idx, DirectX::SimpleMath::Vector3& _scale) override;

	virtual unsigned int GetModelMeshIndex(const std::string& name) override;
	virtual unsigned int GetAnimationClipDataIdnex(const std::string& name) override;
	virtual unsigned int GetMaterialIndex(const std::string& name) override;
	virtual unsigned int GetTextureIndex(const std::string& name) override;
	virtual unsigned int GetSpriteDataIndex(const std::string& name) override;
	virtual unsigned int GetVertexShaderIndex(const std::string& name) override;
	virtual unsigned int GetPixelShaderIndex(const std::string& name) override;
	virtual unsigned int GetGeometryShaderIndex(const std::string& name) override;


	virtual unsigned int GetIBLImageIndex(const std::string& name) override;

	virtual unsigned int GetTotalKeyFrame(unsigned int animationIndex) override;

	virtual const std::vector<std::shared_ptr<GraphicMaterial>>& GetMaterialOfMesh(unsigned int idx) override;

	virtual void* GetNodeDataWorldTM(unsigned int idx) override;
	virtual void* GetBoneDataWorldTM(unsigned int idx, const std::string& name) override;
	virtual void* GetBoneDataWorldTM(unsigned int idx, unsigned int boneIndex)override;

private:
	void CreateInputLayout();

private:
	// �ڿ� ������ ����̽�
	std::shared_ptr<IRenderer> m_pRenderer;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	
	std::shared_ptr<BinaryDeserializer> m_Deserializer;
	
	std::shared_ptr<MatSerializer> m_pMatSerializer;
	std::shared_ptr<MatDeserializer> m_pMatDeserializer;

	std::shared_ptr<DX11MeshFactory> m_pMeshFactory;

	std::shared_ptr<class TextLoader> m_pTextLoader;


	// �� �������� �ε����ϴµ� �����ϸ� ���ͷ� �����ϰ��� �Ѵ�
	// �޽� ����
	std::vector<std::shared_ptr<ModelMesh>> m_ModelMesh_V;
	std::unordered_map<std::string, std::shared_ptr<ModelMesh>> m_ModelMesh_UM;
	std::vector<std::pair<std::string, unsigned int>> m_ModelMeshNames_V;		// unsigned int�� �ε��� ����

	// �ִϸ��̼� ����
	std::vector<std::shared_ptr<AnimationClipData>> m_AnimationClipData_V;
	std::unordered_map<std::string, std::shared_ptr<AnimationClipData>> m_AnimationClipData_UM;
	std::vector<std::pair<std::string, unsigned int>> m_AnimationClipDataNames_V;

	// ��Ʈ���� ����
	std::vector<std::shared_ptr<GraphicMaterial>> m_Material_V;
	std::unordered_map<std::string, std::shared_ptr<GraphicMaterial>> m_Material_UM;
	std::vector<std::pair<std::string, unsigned int>> m_MaterialNames_V;

	// �ؽ��� ����
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_Texture_V;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_Texture_UM;
	std::vector<std::pair<std::string, unsigned int>> m_TextureNames_V;

	std::vector<std::shared_ptr<SpriteData>> m_TextureInfo_V;
	std::unordered_map<std::string, std::shared_ptr<SpriteData>> m_TextureInfo_UM;
	std::vector<std::pair<std::string, unsigned int>> m_TextureInfoNames_V;

	// IBL ������ ����
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_IBL_V;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_IBL_UM;
	std::vector<std::pair<std::string, unsigned int>> m_IBLNames_V;

	// ���̴� ����
	std::vector<std::shared_ptr<VertexShader>> m_VertexShader_V;
	std::vector<std::shared_ptr<PixelShader>> m_PixelShader_V;
	std::vector<std::shared_ptr<GeometryShader>> m_GeometryShader_V;

	std::unordered_map<std::string, std::shared_ptr<VertexShader>> m_VertexShader_UM;
	std::unordered_map<std::string, std::shared_ptr<PixelShader>> m_PixelShader_UM;
	std::unordered_map<std::string, std::shared_ptr<GeometryShader>> m_GeometryShader_UM;

	std::vector<std::pair<std::string, unsigned int>> m_VertexShaderNames_V;
	std::vector<std::pair<std::string, unsigned int>> m_PixelShaderNames_V;
	std::vector<std::pair<std::string, unsigned int>> m_GeometryShaderNames_V;

	// ����Ʈ �ؽ�Ʈ ����
	std::unordered_map<std::string, std::vector<struct QuestBlock>> m_QuestText_UM;
	// ��ȭ �ؽ�Ʈ ����
	std::unordered_map<std::string, std::vector<struct DialogueBlock>> m_DialogueText_UM;


private:
	std::shared_ptr<GraphicMaterial> CreateGraphicMaterial(std::shared_ptr<MaterialDesc> matDesc);
	void LinkMaterialToNode(std::shared_ptr<ModelMesh> pMesh);

	HRESULT FindIndexByName(
		std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result);
	HRESULT FindTextureIndexByName(
		std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result);

	// ���̴� �ε带 ���� ���ø� �Լ�
	template<typename T>
	std::shared_ptr<T> LoadShaderFile(std::string& path);
};


template<typename T>
std::shared_ptr<T> DX11ResourceManager::LoadShaderFile(std::string& path)
{
	std::shared_ptr<T> _shader = std::make_shared<T>();
	_shader->Initialize(m_pDevice, StringHelper::StringToWide(path));

	return _shader;
}

