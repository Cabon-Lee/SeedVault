#pragma once

/// �������� �׸��� ��� �������� �����ؾ߰ڴ�
/// Debugging������ �ƴ���?

#include "DLLDefine.h"
#include "ComponentBase.h"

__interface IRenderer;

class Camera;
class MeshFilter;
class MaterialComponent;
class Transform;
class Animation;
struct MeshRenderer_Save;

// �ִ�ġ���� �����ѹ�
const unsigned int REFLECTION_PROBE_MAXNUM = 9999;

enum class eRasterizerState
{
	SOLID = 0,
	WIREFRAME,
};

enum class eTopology
{
	TRIANGLELIST = 0,
	LINELIST
};

const char SELECTED_FLAG	= 1;	// 0b0001
const char REFLECTION_FLAG	= 2;	// 0b0010
const char CASTSHADOW_FLAG	= 4;	// 0b0100

class MeshRenderer : public ComponentBase
{
public:
	_DLL MeshRenderer();
	_DLL virtual ~MeshRenderer();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	
	virtual void OnPreRender() override;
	virtual void OnRender() override;
	virtual void OnPostRender() override;
	
	_DLL void SetRasterizerState(eRasterizerState state);
	_DLL void SetGizmo(bool val);

	_DLL const bool& GetIsWireframeMode();
	_DLL const bool& GetIsGizmoOn();

	const unsigned int GetReflectionProbeIndex();
	void SetReflectionProbeIndex(unsigned int probeIdx);

	_DLL void SetReflection(bool val);
	_DLL void SetCastShadow(bool val);
	_DLL void SetSelected(bool val);
	_DLL unsigned int GetRenderFlag();
	_DLL bool GetCastShadow();
	_DLL bool GetReflection();

private:
	bool m_IsGizmoOn;
	eRasterizerState m_eRasterizer;
	eTopology m_eTopology;

	std::shared_ptr<IRenderer> m_pRenderer;
	Camera* m_pCamera;

	// ���� ������Ʈ�κ��� ������ ��
	Transform* m_pMyTransform;
	MeshFilter* m_pMyMeshFilter;
	Animation* m_pMyAnimation;
	MaterialComponent* m_pMyMaterial;

	unsigned int m_RenderFlag;					// �׸���, �ݻ�
	unsigned int m_ReflectionProbeIndex;	// �� ��° ���÷��� ���κ�����, ������ ���� 

	unsigned int m_gameObjectId;
	

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	MeshRenderer_Save* m_SaveData;
};


struct MeshRenderer_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	unsigned int			m_RenderFlag;
};

BOOST_DESCRIBE_STRUCT(MeshRenderer_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_RenderFlag
	))
	