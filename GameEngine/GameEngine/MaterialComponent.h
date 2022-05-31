#pragma once
#include "DLLDefine.h"

#include "ComponentBase.h"
#include "ResourceObserver.h"

struct GraphicMaterial;
struct MaterialComponent_Save;
class MeshFilter;

class MaterialComponent : public ComponentBase, public ResourceObserver
{
public:
	
	_DLL MaterialComponent();
	virtual ~MaterialComponent();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override; 

	_DLL void SetVertexShaderName(const std::string& name);
	_DLL void SetPixelShaderName(const std::string& name);
	_DLL std::string GetVertexShaderName();
	_DLL std::string GetPixelShaderName();

	std::shared_ptr<GraphicMaterial> GetMaterial();
	const std::vector< std::shared_ptr<GraphicMaterial>>& GetMaterials();

	void SetNames(std::vector<std::pair<std::string, unsigned int>>& names);

	void UpdateMaterial();

	const std::string& GetNowAlbedoName();
	const std::string& GetNowNormalName();
	const std::string& GetNowEmissiveName();
	const std::string& GetNowMetalRoughName();

	std::string m_VertexShader;
	std::string m_PixelShader;

	std::string m_AlbedoMapName;
	std::string m_NormalMapName;
	std::string m_EmissiveMapName;
	std::string m_MetalRoughMapName;

	unsigned int m_AlbedoMapIndex;
	unsigned int m_NormalMapIndex;
	unsigned int m_EmissiveMapIndex;
	unsigned int m_MetalRoughMapIndex;

	DirectX::SimpleMath::Vector4 m_AlbedoColor;
	DirectX::SimpleMath::Vector4 m_AmbientColor;
	DirectX::SimpleMath::Vector3 m_FresnelR0Color;

	float m_TransparentFactor;
	float m_SpecularPower;
	float m_Metallic;
	float m_Shineness;								// 블렌더 상 Roughness를 의미 
	float m_ReflactionFactor;

	std::vector<std::pair<std::string, unsigned int>> m_TextureName_V;


private:
	std::vector<std::shared_ptr<GraphicMaterial>> m_Material_V;
	MeshFilter* m_MyMeshFilter;
	unsigned int m_NowMeshIndex;	// 메쉬의 인덱스를 기억하고 있다가 바뀌었을 때 수정해줄 친구

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	MaterialComponent_Save* m_SaveData;
};

struct MaterialComponent_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	std::string			m_VertexShader;
	std::string			m_PixelShader;
};

BOOST_DESCRIBE_STRUCT(MaterialComponent_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_VertexShader,	
	m_PixelShader
	))