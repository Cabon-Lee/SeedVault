#include "pch.h"
#include "SkinningAnimation.h"

#include "EngineDLL.h"
#include "IRenderer.h"
#include "ResourceLoader.h"
#include "AnimationSystem.h"

SkinningAnimation::SkinningAnimation()
	: ComponentBase(ComponentType::Animation)
	, m_SaveData(new SkinningAnimation_SaveData())
{

}

SkinningAnimation::~SkinningAnimation()
{

}

void SkinningAnimation::Start()
{
	Engine* engine = DLLEngine::GetEngine();
	m_pRenderer = engine->GetIRenderer();

	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();
	m_NowMeshName = m_MyMeshFilter->GetName();



	for (auto& _nowMeshData : *m_MeshMetadata)
	{
		if (_nowMeshData.meshName == m_NowMeshName)
		{
			/// <summary>
			/// 초기 MyMesh값을 지정해주어야 한다.
			/// -KEKAL
			SetMyMeshData(&_nowMeshData);

			break;
		}
	}

}

void SkinningAnimation::Update(float dTime)
{
	///매쉬필터가 변경되었을때만 데이터를 변경해주면 된다.
	if (m_NowMeshName != m_MyMeshFilter->GetName())
	{
		m_NowMeshName = m_MyMeshFilter->GetName();

		for (auto& _nowMeshData : *m_MeshMetadata)
		{
			if (_nowMeshData.meshName == m_NowMeshName)
			{
				/// <summary>
				/// Now AnimationName을 따로 지정해주지 않아서 임시로 제가 수정합니다.
				/// SetMeshAnimationIndex를 통해서 지정해 주어야 합니다.
				/// -KEKAL
				/// </summary>
				SetMyMeshData(&_nowMeshData);
				//_nowAnimName = _nowMeshData.animationName_V[0];

				if (_nowMeshData.isSkinned != true)
				{
					return;
				}

				break;
			}
		}
	}

	/// 애니메이터 없이 애니메이션 컴포넌트만 있는 경우는 어떻게 처리할것인지?
	//std::string _nowAnimClip =  m_MyAnimator->GetNowState()->m_AniamtionClip.animationName;


	//if (m_MyMeshData->hasAnimation || m_MyMeshData->isSkinned)
	{
		//m_pRenderer->Animate(m_NowMeshName, "d", m_Transform->GetWorld(), true, 1.0f, dTime);
	}
}

void SkinningAnimation::OnRender()
{

}

void SkinningAnimation::SetMeshdata(std::vector<MeshMetadata>* data)
{
	m_MeshMetadata = data;
}

const std::string& SkinningAnimation::GetNowMeshName()
{
	return m_NowMeshName;
}

const std::string& SkinningAnimation::GetNowAnimationName()
{
	return m_AnimationName;
}

const size_t SkinningAnimation::GetMeshAnimationCnt()
{
	return m_MyMeshData->animationName_V.size();
}

void SkinningAnimation::SetMyMeshData(MeshMetadata* data)
{
	/// <summary>
	/// 새로운 매쉬메타 데이터로 지정해줄때 마다
	/// </summary>
	m_MyMeshData = data;

	///0번째 애니메이션으로 초기화
	SetMeshAnimationIndex(0);
}

const bool SkinningAnimation::GetIsAniPossibleMesh()
{
	return m_MyMeshData->hasAnimation && m_MyMeshData->isSkinned;
}

const std::string SkinningAnimation::GetMeshAnimationName(size_t index)
{
	if (index >= m_MyMeshData->animationName_V.size())
	{
		return "";
	}

	return m_MyMeshData->animationName_V.at(index);
}

void SkinningAnimation::SetMeshAnimationIndex(size_t index)
{
	if (m_MyMeshData->animationName_V.empty())
	{
		m_AnimationName = "";
		return;
	}

	if (index >= m_MyMeshData->animationName_V.size())
	{
		m_AnimationName = m_MyMeshData->animationName_V.at(0);
		return;
	}

	m_AnimationName = m_MyMeshData->animationName_V.at(index);
}

void SkinningAnimation::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;
	m_SaveData->m_NowMeshName = m_NowMeshName;
	m_SaveData->m_AnimationName = m_AnimationName;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void SkinningAnimation::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
	m_NowMeshName = m_SaveData->m_NowMeshName;
	m_AnimationName = m_SaveData->m_AnimationName;
}

