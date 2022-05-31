#pragma once

/// ������ DX12�� ������� ���� ���� �����ϰ� �������.
/// ���� DX12�� �߰��ϰ� �ȴٸ�, �ʼ� ������ �����̳ʸ� �������̽��� ����� ��� ���� ��
/// Initiailze ���� ���� �����Ͽ� ����ϴ� ������ �� �� ����.
/// �׷����ϸ� ���߿� Renderer�� Engine���� ResourceManager�� ����Ҷ� ũ�� ���� ���� ���� ��.

#include <d3d11.h>
#include <wrl/client.h>

#include <unordered_map>
#include <memory>
#include <map>
#include <queue>
#include <vector>
#include "StringHelper.h"

class JMFBXParser;
class DX11MeshFactory;

class Shaders;
class MaterialData;
class VertexShader;
class PixelShader;
class ModelMesh;
class BinaryDeserializer;

struct MaterialDesc;
struct NodeData;
struct FBXModel;
struct AnimationClipData;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);


	void Destroy();

	void LoadResource(std::string& path);
	
	// ���� �� ���ҽ� �Ŵ����� �����ϱ� ����
	std::vector< std::shared_ptr<NodeData>> GetNodeData(const std::string& name);
	std::shared_ptr<ModelMesh>  GetModelMesh(const std::string& name);
	ID3D11ShaderResourceView* GetTextrue(const std::string& name);
	VertexShader* GetVertexShader(const std::string& name);
	PixelShader* GetPixelShader(const std::string& name);
	MaterialDesc* GetMaterialDesc(const std::string& name);
	std::shared_ptr<AnimationClipData> GetAnimationData(int idx) 
	{ return m_AnimationClipData_V[idx]; };


	void LoadFBXFile(std::string& fileName);
	void LoadBinaryFile(std::string& fileName);
	void LoadTextureFile(std::string& fileName);
	void LoadShaderFile(std::string& fileName);
	void AddAnimationClipData(std::shared_ptr<AnimationClipData> anim);
	void AddAnimationClipData(std::string fileName, std::shared_ptr<AnimationClipData> anim);


	void CreateInputLayout();

private:
	template<typename T>
	T* LoadShaderFile(std::string& fileName);

	void* GetMaterialDataVoid();

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

	DX11MeshFactory* m_pMeshFactory;
	Shaders* m_pShader;

	std::shared_ptr<BinaryDeserializer> m_Deserializer;

	std::unordered_map<std::string, std::vector<NodeData*>> m_NodeData_UM;		
	std::unordered_map<std::string, std::shared_ptr<ModelMesh>> m_ModelMesh_UM;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_Textrue_UM;
	std::unordered_map<std::string, VertexShader*> m_VertexShader_UM;
	std::unordered_map<std::string, PixelShader*> m_PixelShader_UM;
	std::unordered_map<std::string, MaterialDesc*> m_MaterialDesc_UM;
	std::unordered_map<std::string, std::shared_ptr<AnimationClipData>> m_AnimationClipData_UM;
	std::map<std::string, unsigned int> m_AnimationIndex_M;

	std::vector<std::shared_ptr<AnimationClipData>> m_AnimationClipData_V;

};

template<typename T>
T* ResourceManager::LoadShaderFile(std::string& path)
{
	T* _shader = new T;
	_shader->Initialize(m_pDevice, StringHelper::StringToWide(path));

	return _shader;
}

