#pragma once

#include <wrl/client.h>
#include "SimpleMath.h"
#include <memory>

__interface IRenderer;

class ModelMesh;
class VertexShader;
class PixelShader;
class GeometryShader;
struct AnimationClipData;
struct GraphicMaterial;
struct ID3D11ShaderResourceView;
struct SpriteData;
struct QuestBlock;
struct DialogueBlock;

class NaviMeshSet;

__interface IResourceManager
{
	// ¿£Áø
	virtual void Initialize(std::shared_ptr<IRenderer> pRenderer) abstract;

	virtual void LoadBinaryFile(std::string& path) abstract;
	virtual void LoadAnimationFile(std::string& path) abstract;
	virtual void LoadTextureFile(std::string& fileName) abstract;
	virtual void LoadShaderFile(std::string& fileName) abstract;
	virtual void LoadMatFile(std::string& fileName) abstract;
	virtual void LoadIBLFile(std::string& fileName) abstract;
	virtual void LoadQuestTextFile(std::string& fileName) abstract;
	virtual void LoadDialogueTextFile(std::string& fileName) abstract;

	virtual void SaveMatFile(unsigned int, int idx) abstract;
	virtual void LoadMatFile(unsigned int, int idx) abstract;

	virtual std::shared_ptr<NaviMeshSet> MakeNeviMesh(std::string& fileName) abstract;

	virtual std::vector<std::pair<std::string, unsigned int>>& GetModelMeshNames() abstract;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetAnimationNames() abstract;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetMaterialNames() abstract;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetTextureNames() abstract;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetVertexShaderNames() abstract;
	virtual std::vector<std::pair<std::string, unsigned int>>& GetPixelShaderNames() abstract;

	// ·»´õ·¯
	virtual std::shared_ptr<ModelMesh> GetModelMesh(const std::string& name) abstract;
	virtual std::shared_ptr<AnimationClipData> GetAnimationClipData(const std::string& name) abstract;
	virtual std::shared_ptr<GraphicMaterial> GetMaterial(const std::string& name) abstract;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const std::string& name) abstract;
	virtual std::shared_ptr<SpriteData> GetSpriteData(const std::string& name) abstract;

	virtual std::shared_ptr<VertexShader> GetVertexShader(const std::string& name) abstract;
	virtual std::shared_ptr<PixelShader> GetPixelShader(const std::string& name) abstract;
	virtual std::shared_ptr<GeometryShader> GetGeometryShader(const std::string& name) abstract;

	virtual std::shared_ptr<ModelMesh> GetModelMesh(unsigned int idx) abstract;
	virtual std::shared_ptr<AnimationClipData> GetAnimationClipData(unsigned int idx) abstract;
	virtual std::shared_ptr<GraphicMaterial> GetMaterial(unsigned int idx) abstract;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(unsigned int idx) abstract;
	virtual std::shared_ptr<SpriteData> GetSpriteData(unsigned int idx) abstract;

	virtual std::shared_ptr<VertexShader> GetVertexShader(unsigned int idx) abstract;
	virtual std::shared_ptr<PixelShader> GetPixelShader(unsigned int idx) abstract;
	virtual std::shared_ptr<GeometryShader> GetGeometryShader(unsigned int idx) abstract;

	virtual std::vector<QuestBlock> GetQuestText(const std::string& name) abstract;
	virtual std::vector<DialogueBlock> GetDialogueText(const std::string& name) abstract;
	
	virtual std::unordered_map<std::string, std::vector<struct QuestBlock>> GetWholeQuestText() abstract;
	virtual std::unordered_map<std::string, std::vector<struct DialogueBlock>> GetWholeDialogueText() abstract;

	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIBLImage(const std::string& name) abstract;
	virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetIBLImage(unsigned int idx) abstract;

	virtual const std::string& GetMaterialName(
		unsigned int modelIndex, unsigned int matIndex) abstract;

	virtual const std::string& GetMaterialTextureName(
		unsigned int modelIndex, unsigned int matIndex, unsigned int attIndex) abstract;
	virtual void SetMaterialTextureName(
		unsigned int modelIndex,
		unsigned int matIndex, unsigned int attIndex, const std::string& name) abstract;

	virtual float& GetMaterialCalibrateFactor(unsigned int modelIndex, unsigned int matIndex, int att) abstract;
	virtual bool& GetMaterialTransparencyFactor(unsigned int modelIndex, unsigned int matIndex) abstract;

	virtual RECT GetTextureRect(unsigned int idx) abstract;

	virtual void GetSpriteScale(unsigned int idx, DirectX::SimpleMath::Vector3& _scale) abstract;

	virtual unsigned int GetModelMeshIndex(const std::string& name) abstract;
	virtual unsigned int GetAnimationClipDataIdnex(const std::string& name) abstract;
	virtual unsigned int GetMaterialIndex(const std::string& name) abstract;
	virtual unsigned int GetTextureIndex(const std::string& name) abstract;
	virtual unsigned int GetSpriteDataIndex(const std::string& name) abstract;
	virtual unsigned int GetVertexShaderIndex(const std::string& name) abstract;
	virtual unsigned int GetPixelShaderIndex(const std::string& name) abstract;
	virtual unsigned int GetGeometryShaderIndex(const std::string& name) abstract;

	virtual unsigned int GetIBLImageIndex(const std::string& name) abstract;

	virtual unsigned int GetTotalKeyFrame(unsigned int animationIndex) abstract;

	virtual const std::vector<std::shared_ptr<GraphicMaterial>>& GetMaterialOfMesh(unsigned int idx) abstract;

	virtual void* GetNodeDataWorldTM(unsigned int idx) abstract;
	virtual void* GetBoneDataWorldTM(unsigned int idx, const std::string& name) abstract;
	virtual void* GetBoneDataWorldTM(unsigned int idx, unsigned int boneIndex)abstract;

};