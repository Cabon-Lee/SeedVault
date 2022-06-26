#pragma once

/// DLL로 만들어진 리소스 매니저
/// 렌더러한테 모든 것을 맡기다보니, 렌더러가 전용 리소스 매니저가 되어버린 느낌
/// 리소스를 관리할 주체를 따로 만드는 것을 생각하다보니 리쇠스 매니저가 등장
/// 단, 리소스들은 DX등에 종속적이므로 인터페이스가 있고, 내부 구현은 렌더러에 따라 나뉘는 걸로 생각
/// 이 매니저는 DX11에 종속적으로 만들것

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


	// 메쉬, 머트리얼, 애니메이션 정보를 모두 가져온다
	// 단, 하나의 클래스에 몰아서 저장하는게 아니라 각각의 요소를 따로 분류해서 저장한다
	// 엔진에서 사용하는 인터페이스
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

	// 렌더러에서 사용하는 인터페이스
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
	// 자원 생성용 디바이스
	std::shared_ptr<IRenderer> m_pRenderer;
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	
	std::shared_ptr<BinaryDeserializer> m_Deserializer;
	
	std::shared_ptr<MatSerializer> m_pMatSerializer;
	std::shared_ptr<MatDeserializer> m_pMatDeserializer;

	std::shared_ptr<DX11MeshFactory> m_pMeshFactory;

	std::shared_ptr<class TextLoader> m_pTextLoader;


	// 각 정보들을 인덱싱하는데 성공하면 벡터로 접근하고자 한다
	// 메쉬 정보
	std::vector<std::shared_ptr<ModelMesh>> m_ModelMesh_V;
	std::unordered_map<std::string, std::shared_ptr<ModelMesh>> m_ModelMesh_UM;
	std::vector<std::pair<std::string, unsigned int>> m_ModelMeshNames_V;		// unsigned int가 인덱스 정보

	// 애니메이션 정보
	std::vector<std::shared_ptr<AnimationClipData>> m_AnimationClipData_V;
	std::unordered_map<std::string, std::shared_ptr<AnimationClipData>> m_AnimationClipData_UM;
	std::vector<std::pair<std::string, unsigned int>> m_AnimationClipDataNames_V;

	// 머트리얼 정보
	std::vector<std::shared_ptr<GraphicMaterial>> m_Material_V;
	std::unordered_map<std::string, std::shared_ptr<GraphicMaterial>> m_Material_UM;
	std::vector<std::pair<std::string, unsigned int>> m_MaterialNames_V;

	// 텍스쳐 정보
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_Texture_V;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_Texture_UM;
	std::vector<std::pair<std::string, unsigned int>> m_TextureNames_V;

	std::vector<std::shared_ptr<SpriteData>> m_TextureInfo_V;
	std::unordered_map<std::string, std::shared_ptr<SpriteData>> m_TextureInfo_UM;
	std::vector<std::pair<std::string, unsigned int>> m_TextureInfoNames_V;

	// IBL 데이터 정보
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_IBL_V;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_IBL_UM;
	std::vector<std::pair<std::string, unsigned int>> m_IBLNames_V;

	// 쉐이더 정보
	std::vector<std::shared_ptr<VertexShader>> m_VertexShader_V;
	std::vector<std::shared_ptr<PixelShader>> m_PixelShader_V;
	std::vector<std::shared_ptr<GeometryShader>> m_GeometryShader_V;

	std::unordered_map<std::string, std::shared_ptr<VertexShader>> m_VertexShader_UM;
	std::unordered_map<std::string, std::shared_ptr<PixelShader>> m_PixelShader_UM;
	std::unordered_map<std::string, std::shared_ptr<GeometryShader>> m_GeometryShader_UM;

	std::vector<std::pair<std::string, unsigned int>> m_VertexShaderNames_V;
	std::vector<std::pair<std::string, unsigned int>> m_PixelShaderNames_V;
	std::vector<std::pair<std::string, unsigned int>> m_GeometryShaderNames_V;

	// 퀘스트 텍스트 정보
	std::unordered_map<std::string, std::vector<struct QuestBlock>> m_QuestText_UM;
	// 대화 텍스트 정보
	std::unordered_map<std::string, std::vector<struct DialogueBlock>> m_DialogueText_UM;


private:
	std::shared_ptr<GraphicMaterial> CreateGraphicMaterial(std::shared_ptr<MaterialDesc> matDesc);
	void LinkMaterialToNode(std::shared_ptr<ModelMesh> pMesh);

	HRESULT FindIndexByName(
		std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result);
	HRESULT FindTextureIndexByName(
		std::vector<std::pair<std::string, unsigned int>> list, const std::string& name, unsigned int& __out result);

	// 쉐이더 로드를 위한 템플릿 함수
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

