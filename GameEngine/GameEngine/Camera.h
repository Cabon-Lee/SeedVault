#pragma once
#include "DLLDefine.h"

#include "ComponentBase.h"
#include "ResourceObserver.h"

__interface IRenderer;

class Managers;
class Transform;

struct Camera_Save;

class Camera : public ComponentBase, public ResourceObserver
{
public:
	_DLL Camera(std::string name);
	_DLL ~Camera();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	DirectX::SimpleMath::Vector3 GetLook() const;
	DirectX::SimpleMath::Vector3 GetPosition() const;

	// get near & far view 공간 좌표
	float GetNearWndWidth() const;
	float GetNearWndHeight() const;
	float GetFarWndWidth() const;
	float GetFarWndHeight() const;

	// Frustum 
	float GetNearZ() const;
	float GetFarZ() const;

	_DLL float GetAspect() const;
	_DLL void SetAspect(float aspect);

	_DLL float GetFovY() const;
	_DLL void SetFovY(float fovY);

	float GetFovX() const;

	// set frustum
	_DLL void SetLens(float fovY, float aspect, float zn, float zf);

	// 카메라 시야/공간 세팅
	_DLL void LookAtTarget(DirectX::SimpleMath::Vector3 target);
	_DLL void SetSkyBoxTextureName(std::string name);
	_DLL void SetDrawSkyBox(bool val);
	_DLL const DirectX::SimpleMath::Matrix& GetWorldTM();

	void LookAtVector(DirectX::SimpleMath::Vector3 vector);	/// 수정 필요

	DirectX::SimpleMath::Matrix View() const;
	DirectX::SimpleMath::Matrix Proj() const;
	DirectX::SimpleMath::Matrix ViewProj() const;

	void UpdateViewMatrix();

	const std::string& GetName();

	unsigned int GetSkyBoxTextrueIndex();
	bool GetIsSkyBox();

	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

private:
	std::string m_Name;
	std::string m_SkyBoxTextureName;
	unsigned int m_SkyBoxTextureIndex;

	bool m_IsSkyBox;

	// 카메라의 상하좌우
	DirectX::SimpleMath::Vector3 m_Right;
	DirectX::SimpleMath::Vector3 m_Up;
	DirectX::SimpleMath::Vector3 m_Look;

	// for SetLens
	float m_NearZ;
	float m_FarZ;
	float m_Aspect;
	float m_FovY; // field of view
	float m_NearWndHeight;
	float m_FarWndHeight;

	// for View matrix
	DirectX::SimpleMath::Matrix m_View;
	DirectX::SimpleMath::Matrix m_Proj;

	std::shared_ptr<IRenderer> m_pRenderer;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Camera_Save* m_SaveData;
};

struct Camera_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;

	std::string			m_Name;
	DirectX::SimpleMath::Vector3* m_Look;
	float				m_NearZ;
	float				m_FarZ;
	float				m_Aspect;
	float				m_FovY; // field of view

};

BOOST_DESCRIBE_STRUCT(Camera_Save, (), (
	m_bEnable,
	m_ComponentId,
	m_Name,
	m_Look,
	m_NearZ,
	m_FarZ,
	m_Aspect,
	m_FovY
	))