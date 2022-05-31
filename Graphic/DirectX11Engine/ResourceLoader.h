#pragma once

/// 지금은 DX12를 사용하지 않을 것을 생각하고 만들었다.
/// 만약 DX12을 추가하게 된다면, 필수 구현과 컨테이너를 인터페이스로 만들어 상속 받은 후
/// Initiailze 등을 각자 구현하여 사용하는 구조가 될 것 같다.
/// 그렇게하면 나중에 Renderer나 Engine에서 ResourceManager를 사용할때 크게 문제 되지 않을 것.

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

