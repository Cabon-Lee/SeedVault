#include "pch.h"
#include "MeshRenderer.h"

#include "IRenderer.h"

#include "Camera.h"
#include "Transform.h"
#include "MeshFilter.h"
#include "Animation.h"
#include "MaterialComponent.h" 
#include "EngineDLL.h"

#include "CATrace.h"

MeshRenderer::MeshRenderer()
	: ComponentBase(ComponentType::Rendering)
	, m_IsGizmoOn(true)
	, m_pMyMeshFilter(nullptr)
	, m_pMyMaterial(nullptr)
	, m_eRasterizer(eRasterizerState::SOLID)
	, m_eTopology(eTopology::TRIANGLELIST)
	, m_ReflectionProbeIndex(REFLECTION_PROBE_MAXNUM)
	, m_SaveData(new MeshRenderer_Save())
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Start()
{
	m_pMyTransform = m_pMyObject->GetComponent<Transform>();
	m_pMyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
	m_pMyMaterial = m_pMyObject->GetComponent<MaterialComponent>();
	m_pMyAnimation = m_pMyObject->GetComponent<Animation>();

	m_gameObjectId = m_pMyObject->GetGameObjectId();

	if (m_pMyAnimation == nullptr)
	{
		m_pMyAnimation = m_pMyObject->GetComponent<Animator>();
	}

	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
		
	// 일단 렌더러에 요청해서 리플렉션 프로브의 위치를 얻어온다
	// 이 방법을 피해 게임 엔진내에서 끝내는 방법을 찾아 


	
}

void MeshRenderer::Update(float dTime)
{

}

void MeshRenderer::OnPreRender()
{
	if (m_RenderFlag & CASTSHADOW_FLAG)
	{
		m_pRenderer->PreRenderPath(m_pMyMeshFilter->GetModelMeshIndex(), m_pMyTransform->GetWorld());
	}
}

void MeshRenderer::OnRender()
{
	m_pCamera = Managers::GetInstance()->GetCameraManager()->GetNowCamera();

	switch (Managers::GetInstance()->GetNowRenderMode())
	{
	case  eRENDER_MODE::REFLECTION_PROBE:
	{
		// 여기서의 viewTM과 projTM은 리플렉션 프로브의 카메라에 있는 그것이다
		// MeshRenderer에서 이 작업을 하는 이유는 ReflectionProbe가 모든 오브젝트에 접근하는게 비합리적으로 보였기 때문
		m_pRenderer->RenderToReflectionProbe(m_pMyMeshFilter->GetCurrentIndex(),
			m_pMyTransform->GetWorld(),
			Managers::GetInstance()->GetCameraManager()->GetViewTM(),
			Managers::GetInstance()->GetCameraManager()->GetProjTM());
		break;
	}
	case eRENDER_MODE::DEFAULT:
	default:
	{

		if (this->GetIsEnabled() == true)
		{
			DirectX::XMMATRIX* _nowBoneOffset = nullptr;
			if (m_pMyAnimation != nullptr)
				_nowBoneOffset = m_pMyAnimation->GetBoneOffSetTM();

			m_pRenderer->MeshDraw(
				m_pMyMeshFilter->GetCurrentIndex(), 
				m_ReflectionProbeIndex,
				m_RenderFlag,
				m_gameObjectId,
				m_pMyTransform->GetWorld(),
				_nowBoneOffset);
		}

	}
	}

}

void MeshRenderer::OnPostRender()
{
	DirectX::XMMATRIX* _nowBoneOffset = nullptr;
	if (m_pMyAnimation != nullptr)
		_nowBoneOffset = m_pMyAnimation->GetBoneOffSetTM();

	m_pRenderer->DebugDraw(
		static_cast<int>(m_eRasterizer), 
		m_gameObjectId,
		m_pMyMeshFilter->GetCurrentIndex(), 
		m_pMyTransform->GetWorld(), _nowBoneOffset);
}

_DLL void MeshRenderer::SetRasterizerState(eRasterizerState state)
{
	m_eRasterizer = state;
}

_DLL void MeshRenderer::SetGizmo(bool val)
{
	m_IsGizmoOn = val;
}

const bool& MeshRenderer::GetIsWireframeMode()
{
	return m_eRasterizer == eRasterizerState::WIREFRAME ? true : false;
}

const bool& MeshRenderer::GetIsGizmoOn()
{
	return m_IsGizmoOn;
}

const unsigned int MeshRenderer::GetReflectionProbeIndex()
{
	return m_ReflectionProbeIndex;
}

void MeshRenderer::SetReflectionProbeIndex(unsigned int probeIdx)
{
	m_ReflectionProbeIndex = probeIdx;
}

_DLL void MeshRenderer::SetSelected(bool val)
{
	val ? m_RenderFlag |= SELECTED_FLAG : m_RenderFlag &= ~SELECTED_FLAG;
}

_DLL unsigned int MeshRenderer::GetRenderFlag()
{
	return m_RenderFlag;
}

_DLL bool MeshRenderer::GetCastShadow()
{
	return (m_RenderFlag & CASTSHADOW_FLAG);
}

_DLL bool MeshRenderer::GetReflection()
{
	return (m_RenderFlag & REFLECTION_FLAG) ?  true : false;
}

_DLL void MeshRenderer::SetReflection(bool val)
{
	val ? m_RenderFlag |= REFLECTION_FLAG : m_RenderFlag &= ~REFLECTION_FLAG;
}

_DLL void MeshRenderer::SetCastShadow(bool val)
{
	val ? m_RenderFlag |= CASTSHADOW_FLAG : m_RenderFlag &= ~CASTSHADOW_FLAG;
}

void MeshRenderer::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_RenderFlag = m_RenderFlag;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void MeshRenderer::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}

