#pragma once

#include "DLLDefine.h"

#include <vector>
#include <string>
#include "ComponentBase.h"
#include "ResourceObserver.h"

__interface IRenderer;
struct MeshFilter_Save;

struct GraphicMaterial;

#define ALBEDO		0
#define NORMAL		1
#define METALROUGH	2
#define EMISSIVE	3

#define METALLIC		0
#define SHINENESS		1
#define NORMALFACFOR	2
#define EMISSIVEFACTOR	3

class MeshFilter : public ComponentBase, public ResourceObserver
{
public:
	_DLL MeshFilter();
	_DLL virtual ~MeshFilter();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void SetIndex(unsigned int idx);

	_DLL std::string& GetName(unsigned int idx);
	_DLL std::string& GetName(const std::string& name);
	_DLL std::string& GetName();

	_DLL const char* GetName_char(unsigned int idx);
	_DLL const char* GetName_char(const char* name);
	_DLL const char* GetName_char();

	// Added by 최 요 환
	// string name에 해당하는 메쉬를 찾아서 현재 메쉬를 설정함
	_DLL void SetMesh(const std::string name);
	_DLL void SetMesh();

	_DLL void SetFbxTransform();
	_DLL void* GetBoneDataWorldTM(const std::string& name);

	_DLL std::string& GetMeshName(unsigned int idx);		//m_Index수정안하는 버젼
	_DLL const char* GetMeshName_char(unsigned int idx);		//m_Index수정안하는 버젼
	const std::string& GetCurrentMeshName();

	_DLL unsigned int GetCurrentIndex();
	unsigned int GetModelMeshIndex();
	_DLL size_t GetMeshCount();

	void SetNames(std::vector<std::string>* pNames);
	void SetNames(std::vector<std::pair<std::string, unsigned int>>& names);

	const std::string& GetGraphicMaterialName(unsigned int idx);
	_DLL const char* GetGraphicMaterialName_char(unsigned int idx);
	_DLL size_t GetGraphicMaterialCount();
	_DLL size_t GetGraphicMaterialTextureCount();
	const std::string& GetGraphicMaterialTextureName(unsigned int idx , unsigned int texnum);
	_DLL const char*  GetGraphicMaterialTextureName_char(unsigned int idx , unsigned int texnum);
	void SetGraphicMaterialTextureName(unsigned int idx , unsigned int texnum, const std::string& texname);
	_DLL void SetGraphicMaterialTextureName_char(unsigned int idx , unsigned int texnum, const char* texname);
	float& GetMaterialFactor(unsigned int idx, int att);
	bool& GetMaterialIsTransparent(unsigned int idx);

	_DLL float GetMaterialFactor_(unsigned int idx, int att);	//단순 값 반환 버젼
	_DLL void SetMaterialFactor(unsigned int idx, int att, float value);

	const std::vector<std::pair<std::string, unsigned int>>& GetTextureNames();
	_DLL const char* GetTextureName(unsigned int index);
	
	_DLL void SaveMaterial(int idx);
	void LoadMaterial(int idx);

private:
	std::vector<std::string>* m_pMeshName_V;
	std::vector<std::pair<std::string, unsigned int>> m_MeshNames_V;


	std::shared_ptr<IRenderer> m_pRenderer;
	std::shared_ptr<IResourceManager> m_pResourceManager;

	std::vector<std::shared_ptr<GraphicMaterial>> m_pCurrMaterials;

	unsigned int m_Index;
	std::string m_MeshName;


public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

public:
	virtual void SaveData();
	virtual void LoadData();
	
protected:
	MeshFilter_Save* m_SaveData;

};

struct MeshFilter_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	unsigned int		m_Index;
	std::string			m_MeshName;

};

BOOST_DESCRIBE_STRUCT(MeshFilter_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_Index,
	m_MeshName
	))