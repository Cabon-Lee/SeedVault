#include "pch.h"
#include "Animation.h"

#include "EngineDLL.h"
#include "IRenderer.h"
#include "IResourceManager.h"


Animation::Animation()
	: ComponentBase(ComponentType::Animation)
	, m_AnimationFrameRate(eANIM_FRAMERATE::FPS60)
	, m_NowKeyFrame(0)
	, m_OffsetAnlge(0.0f)
{
	m_BoneOffsetTM = 
		static_cast<DirectX::XMMATRIX*>(malloc(sizeof(DirectX::XMMATRIX) * 96));
	memset(m_BoneOffsetTM, 0, sizeof(DirectX::XMMATRIX) * 96);

	m_TargetBoneTM = new DirectX::XMMATRIX();

	m_pPrevAnimatonTM = static_cast<DirectX::SimpleMath::Matrix*>(malloc(sizeof(DirectX::SimpleMath::Matrix) * 96));
	memset(m_pPrevAnimatonTM, 0, sizeof(DirectX::SimpleMath::Matrix) * 96);
	m_pInterpolateAnimationTM = static_cast<DirectX::SimpleMath::Matrix*>(malloc(sizeof(DirectX::SimpleMath::Matrix) * 96));
	memset(m_pInterpolateAnimationTM, 0, sizeof(DirectX::SimpleMath::Matrix) * 96);
}

Animation::~Animation()
{
	delete(m_BoneOffsetTM);
	delete(m_TargetBoneTM);
	delete(m_pPrevAnimatonTM);
	delete(m_pInterpolateAnimationTM);
}

void Animation::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
}

void Animation::Update(float dTime)
{
	if (TimeCheck(dTime))
	{
		m_NowKeyFrame++;
		m_TimeStack = 0.0f;
	}

	// 애니메이션 키프레임을 넘겨준다
	// 만약 넘어갔을 때는 내부에서 firstKeyFrame으로 바꿔서 반환한다
	// AnimationClipData를 숨기기 위해 이런식으로 고안했다
	m_pIRenderer->AnimationProcess(
		m_MyMeshFilter->GetModelMeshIndex(),
		m_AnimationClipIndex,
		m_NowKeyFrame,
		m_pMyObject->m_Transform->GetWorld(),
		m_pPrevAnimatonTM,
		m_OffsetAnlge);

	m_pIRenderer->EndAnimation(
		m_MyMeshFilter->GetModelMeshIndex(), m_BoneOffsetTM);

	*m_TargetBoneTM = *reinterpret_cast<DirectX::XMMATRIX*>(m_pIResourceManager->GetBoneDataWorldTM(
		m_MyMeshFilter->GetModelMeshIndex(), m_TargetBoneIndex));
}

void Animation::OnRender()
{

}

_DLL void Animation::SetTagetBone(unsigned int boneIndex)
{
	m_TargetBoneIndex = boneIndex;
}

DirectX::XMMATRIX* Animation::GetBoneOffSetTM()
{
	return m_BoneOffsetTM;
}

DirectX::XMMATRIX* Animation::GetTargetBoneTM()
{
	return m_TargetBoneTM;
}

bool Animation::TimeCheck(float dTime)
{
	m_TimeStack += dTime;

	switch (m_AnimationFrameRate)
	{
	case eANIM_FRAMERATE::FPS60:
		return (m_TimeStack >= FRAME_60);
	case eANIM_FRAMERATE::FPS30:
		return (m_TimeStack >= FRAME_30);
	case eANIM_FRAMERATE::FPS24:
		return (m_TimeStack >= FRAME_24);
	default:
		break;
	}

	return false;
}

void Animation::CopyPrevAnimation()
{
	for (unsigned int i = 0; i < 96; i++)
		m_pInterpolateAnimationTM[i] = m_pPrevAnimatonTM[i];
}

void Animation::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_pIResourceManager = pReosureManager;
	m_AnimationClipIndex = pReosureManager->GetAnimationClipDataIdnex(m_AnimationClipName);
}
