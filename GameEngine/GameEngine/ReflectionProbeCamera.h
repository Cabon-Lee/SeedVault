#pragma once

#include "ComponentBase.h"
#include "ResourceObserver.h"

__interface IRenderer;

const unsigned int CAMERA_FACE = 6;

const unsigned int ENVIRONMENT_MAP = 0;
const unsigned int IRRADIANCE_MAP = 1;
const unsigned int PREFILTER_MAP = 2;

const std::string ENV_NAME = "_EnvironmentMap_";
const std::string IRR_NAME = "_IrradianceMap_";
const std::string FILTER_NAME = "_PreFilterMap_";

class CameraProbe
{
public:
	CameraProbe() {};
	~CameraProbe() {};

	void LookAT(const DirectX::SimpleMath::Vector3& pos,  
		const DirectX::SimpleMath::Vector3& target, 
		const DirectX::SimpleMath::Vector3& worldUp);
	void SetLens(float fovY, float aspect, float zn, float zf);
	void UpdateViewMatrix(const DirectX::SimpleMath::Matrix& world);

	// for View matrix
	DirectX::SimpleMath::Matrix m_View;
	DirectX::SimpleMath::Matrix m_Proj;

	DirectX::SimpleMath::Vector3 m_Position;
	DirectX::SimpleMath::Vector3 m_Right;
	DirectX::SimpleMath::Vector3 m_Up;
	DirectX::SimpleMath::Vector3 m_Look;

};

// 컴포넌트니까 Transform이 있을 것을 전재로 한다
struct  ReflectionProbeCamera_Save;

class ReflectionProbeCamera : public ComponentBase, public ResourceObserver
{
public:
	_DLL ReflectionProbeCamera();
	_DLL virtual ~ReflectionProbeCamera();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	// 임시로 Boolean 값을 True로 바꾸어 렌더 패스를 돌게하는 방식
	_DLL void BakeReflectionProbe();
	_DLL void SetSceneIndex(const unsigned int idx);	// 씬에서 지역변수를 줄 것을 상정하여 그냥 받음
	_DLL void SetReferenceDistance(float dis);
	_DLL void SetIsRender(bool val);
	_DLL bool GetIsRender();

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

private:
	void BakingEnvironment();
	void CheckTargetDistance();

	unsigned int m_SceneIndex;
	std::string m_SceneName;
	unsigned int m_ProbeIndex;
	bool m_IsBaking;
	bool m_IsRender;

	float m_ReferenceDistance;
	// 맵들이 0이면 이전에 구워둔 DDS는 없다는 의미
	unsigned int m_Maps[3];
	std::string m_MapNames[3];

	std::shared_ptr<IRenderer> m_pRenderer;

	DirectX::SimpleMath::Vector3 m_ProbePos;

	CameraProbe m_CameraPorbe[6];

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	ReflectionProbeCamera_Save* m_SaveData;
};

struct ReflectionProbeCamera_Save
{
	bool m_bEnable;
	unsigned int  m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(ReflectionProbeCamera_Save, (), (
	m_bEnable,
	m_ComponentId
	))

