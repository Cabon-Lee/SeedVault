#pragma once

#include "DLLDefine.h"

#include "ComponentBase.h"
#include "ResourceObserver.h"

__interface IRenderer;
class NaviMeshSet;
struct NaviMeshFace;
class MeshFilter;

class NaviMeshController :
    public ComponentBase, public ResourceObserver
{
public:
	_DLL NaviMeshController();
	_DLL virtual ~NaviMeshController();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;
	virtual void OnHelperRender() override;

	_DLL void MakeNaviMesh();
	_DLL bool GetDebugMode();
	_DLL void SetDebugMode(bool val);

	_DLL float GetOffset();
	_DLL void SetOffset(float val);


public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;


private:
	std::shared_ptr<IRenderer> m_pIRenderer;
	std::shared_ptr<IResourceManager> m_pResourceManager;
	std::shared_ptr<NaviMeshSet> m_MyNaviMesh;

	DirectX::SimpleMath::Vector3 m_OffsetVertex[3];

	MeshFilter* m_MyMeshFilter;
	bool m_IsDebug;
	bool m_IsWire;
	float m_Offset;
};

