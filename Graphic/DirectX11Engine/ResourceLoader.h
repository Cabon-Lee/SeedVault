#pragma once

/// ������ DX12�� ������� ���� ���� �����ϰ� �������.
/// ���� DX12�� �߰��ϰ� �ȴٸ�, �ʼ� ������ �����̳ʸ� �������̽��� ����� ��� ���� ��
/// Initiailze ���� ���� �����Ͽ� ����ϴ� ������ �� �� ����.
/// �׷����ϸ� ���߿� Renderer�� Engine���� ResourceManager�� ����Ҷ� ũ�� ���� ���� ���� ��.

#include <unordered_map>
#include <queue>
#include <vector>
#include "JMParsingDataClass.h"

#include <d3d11.h>
#include <wrl/client.h>


class JMFBXParser;
class DX11MeshFactory;

class Shaders;
class MaterialData;

struct NodeData;
struct FBXModel;

class ResourceLoader
{
public:
	ResourceLoader();
	~ResourceLoader();

	void Initialize(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext);
	void Destroy();

	std::vector<void*>& LoadFBXFile(std::string& fileName);
	void* LoadTextureFile(std::string& fileName);
	void* LoadShaderFile(std::string& fileName);
	void* GetMaterialDataVoid();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

	JMFBXParser* m_pParser;
	DX11MeshFactory* m_pMeshFactory;
	Shaders* m_pShader;

};

