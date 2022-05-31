#include "pch.h"
#include "EngineDLL.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Input.h"

CameraManager::CameraManager()
	: m_pNowCamera(nullptr)
	, m_PrevMousePos({ 0, 0 })
	, m_CurrMousePos({ 0, 0 })
	, m_hWnd(nullptr)
{

}

CameraManager::~CameraManager()
{

}

void CameraManager::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

void CameraManager::SetNowCamera(string name)
{
	if (m_Camera_UM[name]!=0)
	{
		m_pNowCamera = m_Camera_UM.find(name)->second;
	}

	//else
	//{
	//	m_pNowCamera = m_Camera_UM.begin()->second;
	//}
}

void CameraManager::SetNowCamera(unsigned int index)
{
	unsigned int _index = 0;
	std::unordered_map<std::string, Camera*>::iterator _cameraIter = m_Camera_UM.begin();
	for (; _cameraIter != m_Camera_UM.end(); _cameraIter++)
	{
		if (index == _index)
		{
			m_pNowCamera = _cameraIter->second;
			return;
		}
		_index++;
	}


	m_pNowCamera = m_Camera_UM.begin()->second;
	CA_TRACE("Camera Manager : Camera Set Fail - %d Index over", index);
}

Camera* CameraManager::GetNowCamera()
{
	if (m_pNowCamera == nullptr)
	{
		if (!m_Camera_UM.empty())
		{
			SetNowCamera(0);
		}
		else
		{
			//땜빵용 카메라도 없고 현재 카메라도 없다면?
			GameObject* _EngineCamra = DLLEngine::CreateObject(Managers::GetInstance()->GetSceneManager()->GetNowScene());
			_EngineCamra->SetObjectName("EngineCamera");
			_EngineCamra->AddComponent<Camera>(new Camera("EngineCamera"));

			//러닝중이 아니라면 동작한다.
			m_pNowCamera = m_Camera_UM.begin()->second;
		}
	}

	return m_pNowCamera;
}

Camera* CameraManager::GetCamera(unsigned int index)
{
	unsigned int _index = 0;
	std::unordered_map<std::string, Camera*>::iterator _cameraIter = m_Camera_UM.begin();
	for (; _cameraIter != m_Camera_UM.end(); _cameraIter++)
	{
		if (index == _index)
		{
			return _cameraIter->second;
		}
		_index++;
	}

	CA_TRACE("Camera Manager : %d Index over", index);
	return nullptr;
}

void CameraManager::AddCamera(Camera* pCamera, string name)
{
	m_Camera_UM.insert({ name, pCamera });
}

void CameraManager::RemoveAllCamera()
{
	m_Camera_UM.clear();
	m_pNowCamera = nullptr;
}

DirectX::SimpleMath::Matrix& CameraManager::GetViewTM()
{
	return m_ViewTM;
}

DirectX::SimpleMath::Matrix& CameraManager::GetProjTM()
{
	return m_ProjTM;
}

void CameraManager::SetLensOnResize(int width, int height)
{
	if (m_pNowCamera != nullptr)
	{
		m_pNowCamera->SetLens(0.25f * 3.1415, static_cast<float>(width) / static_cast<float>(height), 1.0f, 10000.0f);
	}
}

unsigned int CameraManager::GetCameraCount()
{
	return m_Camera_UM.size();
}

//void CameraManager::OnMouseDown(HWND hWnd, int x, int y)
//{
//   SetCapture(hWnd);
//
//   float _dx = XMConvertToRadians(0.25f * static_cast<float>(x - m_PrevMousePos.x));
//   float _dy = XMConvertToRadians(0.25f * static_cast<float>(y - m_PrevMousePos.y));
//
//   //m_pNowCamera->Pitch(_dy);
//   //m_pNowCamera->RotateY(_dx);
//}
//
//void CameraManager::OnMouseUp()
//{
//   ReleaseCapture();
//}