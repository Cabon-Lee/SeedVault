#include "pch.h"
#include "NaviMeshController.h"
#include "MeshFilter.h"
#include "NaviMeshFace.h"

#include "CATrace.h"
#include "IRenderer.h"
#include "IResourceManager.h"
#include "EngineDLL.h"


_DLL NaviMeshController::NaviMeshController()
	: ComponentBase(ComponentType::Rendering)
	, m_pIRenderer(nullptr)
	, m_pResourceManager(nullptr)
	, m_MyNaviMesh(nullptr)
	, m_MyMeshFilter(nullptr)
	, m_IsDebug(false)
	, m_IsWire(true)
	, m_Offset(0.0f)
{
	
}

_DLL NaviMeshController::~NaviMeshController()
{

}

void NaviMeshController::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();

	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
}

void NaviMeshController::Update(float dTime)
{

}

void NaviMeshController::OnRender()
{

}

void NaviMeshController::OnHelperRender()
{
	if (m_IsDebug == true)
	{

		if (m_MyNaviMesh == nullptr) return;
		for (size_t i = 0; i < m_MyNaviMesh->GetIndexSize(); ++i)
		{
			auto _nowFace = m_MyNaviMesh->GetNeviMeshFace(i);

			// 디버깅을 위해 오프셋 만큼 띄우는 로직을 추가
			m_OffsetVertex[0] = _nowFace->m_Vertex_Arr[0];
			m_OffsetVertex[1] = _nowFace->m_Vertex_Arr[1];
			m_OffsetVertex[2] = _nowFace->m_Vertex_Arr[2];
			m_OffsetVertex[0].y += m_Offset;
			m_OffsetVertex[1].y += m_Offset;
			m_OffsetVertex[2].y += m_Offset;

			m_pIRenderer->RenderFace(
				m_IsWire,
				m_OffsetVertex[0],
				m_OffsetVertex[1],
				m_OffsetVertex[2],
				_nowFace->m_FaceNormal,
				m_pMyObject->GetTransform()->GetWorld(), 2);
		}
	}
}

_DLL void NaviMeshController::MakeNaviMesh()
{
	m_MyNaviMesh.reset();
	m_MyNaviMesh = m_pResourceManager->MakeNeviMesh(const_cast<std::string&>(m_MyMeshFilter->GetCurrentMeshName()));

	// Added by 최 요 환
	// 내비메쉬 만들어지면 매니져에 등록해준다.
	Managers::GetInstance()->GetNavMeshManager()->AddNavMesh(m_MyMeshFilter->GetCurrentMeshName(), m_MyNaviMesh);
}

_DLL bool NaviMeshController::GetDebugMode()
{
	return m_IsDebug;
}

_DLL void NaviMeshController::SetDebugMode(bool val)
{
	m_IsDebug = val;
}

_DLL float NaviMeshController::GetOffset()
{
	return  m_Offset;
}

_DLL void NaviMeshController::SetOffset(float val)
{
	m_Offset = val;
}

void NaviMeshController::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_pResourceManager = pReosureManager;
}
