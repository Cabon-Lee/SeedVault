#include "pch.h"

#include "AnimationSystem.h"
#include "IRenderer.h"
#include "MeshFilter.h"
#include "Transform.h"
#include "EngineDLL.h"
#include "IResourceManager.h"

#include "CATrace.h"

void Transition::AddParameter(const float standard, float* parameter, const bool greater)
{
	std::shared_ptr<Parameter> _newParam = std::make_shared<FloatType>(standard, parameter, greater);
	m_Parameter_V.push_back(_newParam);
}

void Transition::AddParameter(const bool* parameter, const bool standard)
{
	std::shared_ptr<Parameter>  _newParam = std::make_shared<BooleanType>(parameter, standard);
	m_Parameter_V.push_back(_newParam);
}

void Transition::TransitionUpdate()
{
	bool _nowResult = false;

	for (auto& _nowParam : m_Parameter_V)
	{
		if (_nowParam->ParameterCheck() == false)
		{
			return;
		}
	}

	// ������� �Դٸ� ��� true
	// ���� �����Ϳ� ���� �ڽ��� Ÿ�� �����͸� �Ѱ��ش�
	*m_pPrevState = *m_pCurrState;
	*m_pCurrState = m_TargetState.get();
	*m_pPrevTransition = this;

	State* _currState = *m_pCurrState;
	if (_currState != nullptr)
	{
		_currState->m_NowKeyFrame = 0;

		for (auto& _now : *_currState->m_Transition_V)
		{
			_now->m_IsCrossFadingOver = false;
		}

		// ���̰� �Ǿ��ٴ� ���� �˷��ش�
		_currState->m_pMyAnimLayer->m_IsStateChanged = true;
	}
}


State::State()
	: m_pCurrState(nullptr)
	, m_Speed(1.0f)
	, m_StateOffsetAngle(0.0f)
	, m_NowKeyFrame(0)
	, m_AniamtionMode(eANIMATION_MODE::LOOP)
{
	m_Transition_V = new std::vector<std::shared_ptr<Transition>>();
}

void State::AddTrnasition(std::shared_ptr<State> state)
{
	std::shared_ptr<Transition> _newTransition = std::make_shared<Transition>(state, m_pCurrState, m_pPrevState, m_pPrevTransition);
	m_Transition_V->push_back(_newTransition);
}

_DLL void State::AddTrnasition(const std::string& stateName)
{
	std::shared_ptr<Transition> _newTransition =
		std::make_shared<Transition>(m_pMyAnimLayer->GetState(stateName), m_pCurrState, m_pPrevState, m_pPrevTransition);
	m_Transition_V->push_back(_newTransition);

	// ������Ʈ�� �� ��ŭ ũ�⸦ �ø��� �����Ѵ�.
	// ����ִ� ���Ͱ� ����� ���� �ƽ���
}

_DLL void State::AddEvent(std::function<void()> pEvent, float position)
{
	m_Event_V.emplace_back(std::make_pair(pEvent, position));
	m_EventPositionRatio = position;
}

void State::SetNoneAnimLayer(AnimLayer* pAnimLayer)
{
	m_pMyAnimLayer = pAnimLayer;
}

void State::Update()
{
	// Modified by �� �� ȯ
	if (m_Event_V.size() > 0)
	{
		for (const auto& event : m_Event_V)
		{
			if (m_NowKeyFrame == static_cast<unsigned int>(event.second * m_TotalKeyframe))
			{
				event.first();
			}
		}
	}

	if (m_Transition_V->empty() == true) return;
	for (auto& _nowTransition : *m_Transition_V)
	{
		_nowTransition->TransitionUpdate();
	}

}

void Locomotion::Update()
{
	if (m_Transition_V->empty() == true) return;
	for (auto& _nowTransition : *m_Transition_V)
	{
		_nowTransition->TransitionUpdate();
	}
}

_DLL void Locomotion::AddLocomotionAnimName(const std::string& name)
{
	m_LocomotionAnimName_V.push_back(name);
}

_DLL void Locomotion::SetLocomotionParameter(float* pLocoPara)
{
	m_pLocomotionParameter = pLocoPara;
}

_DLL AnimLayer::AnimLayer()
	: m_bEnabled(true)
	, m_pMyAnimation(nullptr)
{
	m_pCurrState = new State * ();
	m_pPrevState = new State * ();
	m_pPrevTransition = new Transition * ();
}

_DLL AnimLayer::AnimLayer(Animation* pAnimation)
	: m_bEnabled(true)
{
	m_pCurrState = new State * ();
	m_pPrevState = new State * ();
	m_pPrevTransition = new Transition * ();
	m_pMyAnimation = pAnimation;
}

_DLL AnimLayer::~AnimLayer()
{

}

_DLL void AnimLayer::AddStateMap(std::string name, const std::string& animationClipName)
{
	std::shared_ptr<State> _newState = std::make_shared<State>(m_pCurrState, m_pPrevState, m_pPrevTransition);
	_newState->m_StateName = name;
	_newState->m_AnimationName = animationClipName;
	_newState->SetNoneAnimLayer(this);

	m_State_UM.insert({ name, _newState });
	m_States_V.push_back(_newState);
}

_DLL void AnimLayer::AddLocomotion(std::string name, const std::string& animationClipName, float* pPara)
{
	std::shared_ptr<Locomotion> _newState = std::make_shared<Locomotion>(m_pCurrState, m_pPrevState, m_pPrevTransition);
	_newState->m_StateName = name;
	_newState->m_AnimationName = animationClipName;
	_newState->m_LocomotionAnimName = animationClipName;
	_newState->SetNoneAnimLayer(this);
	_newState->m_pLocomotionParameter = pPara;

	m_State_UM.insert({ name, _newState });
	m_States_V.push_back(_newState);
}

_DLL State* AnimLayer::GetNowState()
{
	if (*m_pCurrState != nullptr)
	{
		return *m_pCurrState;
	}
	else
	{
		return nullptr;
	}
}

std::vector<std::shared_ptr<State>>& AnimLayer::GetStates()
{
	return m_States_V;
}

_DLL std::shared_ptr<State> AnimLayer::GetState(const std::string& name)
{
	return m_State_UM.at(name);
}

void AnimLayer::LayerStart()
{
	if (m_States_V.empty() == false)
	{
		*m_pCurrState = m_States_V.front().get();
	}
}

void AnimLayer::LayerUpdate(GameObject* pGameObject, MeshFilter* pMeshFilter, std::shared_ptr<IRenderer> pRenderer)
{
	// Modified by �� �� ȯ
	// ��Ȱ��ȭ ���̾�� ������Ʈ ���� �ʴ´�.
	if (m_bEnabled == false)
	{
		return;
	}

	State* _pCurrState = nullptr;
	State* _pPrevState = nullptr;
	Transition* _pNowTransition = nullptr;

	if (m_States_V.size() == 1)
	{
		// ������Ʈ�� �ܼ��� ��
		_pCurrState = *m_pCurrState;
	}
	else
	{
		_pCurrState = *m_pCurrState;
		_pPrevState = *m_pPrevState;
		_pNowTransition = *m_pPrevTransition;
	}

	_pCurrState->Update();
	_pCurrState->m_NowKeyFrame++;

	if (m_IsStateChanged == true)
	{
		m_pMyAnimation->CopyPrevAnimation();
		m_IsStateChanged = false;
	}

	// ���ڸ���� ������ ��� �ִϸ��̼ǿ� �ش� �ִϸ��̼��� �������� ���´�
	bool _isAnimationOver = false;
	switch (m_MaskingType)
	{
	case eMASKING_TYPE::NONE:
	{

		// ���� State�� ���� �ִϸ��̼��� �Ʒ����� ���� ó���Ѵ�.
		// ũ�ν� ���̵� �����ε�
		// ũ�ν� ���̵��� ���� ������Ʈ�� �ִϸ��̼� ������ �� �ϳ��� �����ϱ� ������ �ʿ��� ��
		// �ٵ� ���� State�� ���� ��찡 �־�(Entry State), �̶� ũ�ν� ���̵��� �õ��ϸ� ������ �̾�����
		if (_pPrevState == nullptr)
		{
			_isAnimationOver =
			pRenderer->AnimationProcess( pMeshFilter->GetModelMeshIndex(),
				_pCurrState->m_AnimationIndex, _pCurrState->m_NowKeyFrame,
				pGameObject->m_Transform->GetWorld(),
				m_pMyAnimation->m_pPrevAnimatonTM,
				_pCurrState->m_StateOffsetAngle);

			break;
		}
		
		if (_pNowTransition->m_FadingPeriod > 0)
		{
			if (_pNowTransition->m_IsCrossFadingOver == false)
			{
				_isAnimationOver = 
				pRenderer->AnimationCrossFading(
					pMeshFilter->GetModelMeshIndex(),
					(*m_pCurrState)->m_AnimationIndex, (*m_pPrevState)->m_AnimationIndex,
					(*m_pCurrState)->m_NowKeyFrame, (*m_pPrevState)->m_NowKeyFrame,		
					// PrevState�� Ű�������� ��ġ ����
					_pNowTransition->m_FadingPeriod,
					_pNowTransition->m_IsCrossFadingOver,	
					// ũ�ν� ���̵��� �������� bool&�� �����´�
					pGameObject->m_Transform->GetWorld(),
					m_pMyAnimation->m_pPrevAnimatonTM,
					m_pMyAnimation->m_pInterpolateAnimationTM,
					_pCurrState->m_StateOffsetAngle);

				pRenderer->EndAnimation(
					pMeshFilter->GetModelMeshIndex(), m_pMyAnimation->GetBoneOffSetTM());

				return;
			}
		}

		_isAnimationOver =
		pRenderer->AnimationProcess(
			pMeshFilter->GetModelMeshIndex(),
			_pCurrState->m_AnimationIndex,
			_pCurrState->m_NowKeyFrame,
			pGameObject->m_Transform->GetWorld(),
			m_pMyAnimation->m_pPrevAnimatonTM,
			_pCurrState->m_StateOffsetAngle);

		break;

	}
	case eMASKING_TYPE::OVERRINDING:
	{
		if ((*m_pCurrState)->GetIsLocomotion() == true)
		{
			Locomotion* _nowLoco = static_cast<Locomotion*>((*m_pCurrState));

			pRenderer->MaskingAnimationBiLocomotoion(
				pMeshFilter->GetModelMeshIndex(),
				_nowLoco->m_LocomotionAnimIndex,	
				*_nowLoco->m_pLocomotionParameter,		// ����ġ, �������̹Ƿ� �ѱ� �� �ֽ��͸���ũ
				pGameObject->m_Transform->GetWorld());
		}
		else
		{
			pRenderer->MaskingAnimation(
				pMeshFilter->GetModelMeshIndex(),
				_pCurrState->m_AnimationIndex,
				_pCurrState->m_NowKeyFrame,
				pGameObject->m_Transform->GetWorld());
		}
		
		break;
	}
	}

	pRenderer->EndAnimation(pMeshFilter->GetModelMeshIndex(), m_pMyAnimation->GetBoneOffSetTM());

	if (_isAnimationOver == true)
	{
		switch (_pCurrState->m_AniamtionMode)
		{
			// �ִϸ��̼��� ���� State�� �����ش�
		case eANIMATION_MODE::TRIGGER:
		case eANIMATION_MODE::LOCKED:
		{
			if (_pPrevState != nullptr)
			{
				*m_pCurrState = _pPrevState;
				*m_pPrevState = _pCurrState;
			}
			break;
		}
		case eANIMATION_MODE::LOOP:
		default:
		{
			break;
		}
		}
	}
	
}

// Modified by �� �� ȯ
_DLL void AnimLayer::SetEnabled(const bool val)
{
	m_bEnabled = val;
}


Animator::Animator()
{

}

Animator::~Animator()
{

}

void Animator::Start()
{
	m_pIRenderer = DLLEngine::GetEngine()->GetIRenderer();
	m_MyMeshFilter = m_pMyObject->GetComponent<MeshFilter>();

	// ��� ���̾���� �̸� ������ �Ǿ��־����
	for (auto& _nowAnimLayer : m_AnimLayer_V)
	{
		_nowAnimLayer->LayerStart();
	}
}

void Animator::Update(float dTime)
{
	if (TimeCheck(dTime))
	{
		// Ÿ�� üũ�� ����ؾ߸� ���̾� ��ü�� ������Ʈ�ȴ�
		for (auto& _nowAnimLayer : m_AnimLayer_V)
		{
			_nowAnimLayer->LayerUpdate(m_pMyObject, m_MyMeshFilter, m_pIRenderer);
		}
		m_TimeStack = 0.0f;
	}
}

void Animator::OnRender()
{

}

_DLL void Animator::AddAnimLayer(std::string name)
{
	std::shared_ptr<AnimLayer> _newLayer = std::make_shared<AnimLayer>(this);
	_newLayer->m_LayerName = name;

	m_AnimLayer_UM.insert({ name, _newLayer });
	m_AnimLayer_V.push_back(_newLayer);
}

_DLL std::shared_ptr<AnimLayer> Animator::GetAnimLayer(const std::string& name)
{
	// Modified by �� �� ȯ
	// �ٷ� UM ���� ���� �����ϴٰ��� �߸��� string �� ������ �� ���α׷��� �ٷ� ���������� ������ ���� ����..(out of range)
	// ���� find�� ã�Ƽ� ������ nullptr �����ϰ� caller �ʿ��� ����ó���� ����.
	if (m_AnimLayer_UM.find(name) == m_AnimLayer_UM.end())
	{
		return nullptr;
	}

	// ���������� ã���� �ش� value ����
	return m_AnimLayer_UM.at(name);
}

_DLL std::vector<std::shared_ptr<AnimLayer>>& Animator::GetAnimLayer()
{
	return m_AnimLayer_V;
}

/// <summary>
/// now None Layer�� ����
/// overrideLayer �� ���� X
/// </summary>
/// <param name="layer">������ none layer</param>
/// <returns>���� ����</returns>
_DLL bool Animator::SetNoneAnimLayer(std::string noneLayer)
{
	bool _ret = false;

	// ���̾� ���͸� ��ȸ�Ѵ�
	for (auto& layer : m_AnimLayer_V)
	{
		// ���̾��� ����ŷŸ���� NONE �� ��쿡
		if (layer->m_MaskingType == eMASKING_TYPE::NONE)
		{
			// ������ Layer ��� Ȱ��ȭ
			if (layer->m_LayerName == noneLayer)
			{
				layer->m_bEnabled = true;

				_ret = true;
			}

			// �̿��� ���̾�� ��Ȱ��ȭ
			else
			{
				layer->m_bEnabled = false;
			}

		}
	}

	return _ret;
}

/// <summary>
/// now Override Layer �� ����
/// noneLayer �� ���� X
/// </summary>
/// <param name="overrideLayer">������ override layer</param>
/// /// <param name="reset">�ִϸ��̼� ������ ���� ����</param>
/// <returns>���� ����</returns>
_DLL bool Animator::SetOverrideAnimLayer(std::string overrideLayer, bool reset)
{
	bool _ret = false;

	// ���̾� ���͸� ��ȸ�Ѵ�
	for (auto& layer : m_AnimLayer_V)
	{
		// ���̾��� ����ŷŸ���� OVERRINDING �� ��쿡
		if (layer->m_MaskingType == eMASKING_TYPE::OVERRINDING)
		{
			// ������ Layer ��� Ȱ��ȭ
			if (layer->m_LayerName == overrideLayer)
			{
				layer->m_bEnabled = true;

				_ret = true;

				// ���� ��û�� ������
				if (reset == true)
				{
					// Ű�������� 0���� �ʱ�ȭ
					layer->GetNowState()->m_NowKeyFrame = 0;
				}
			}

			// �̿��� ���̾�� ��Ȱ��ȭ
			else
			{
				layer->m_bEnabled = false;
			}

		}
	}

	return _ret;
}


void Animator::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{

	for (unsigned int i = 0; i < m_AnimLayer_V.size(); i++)
	{
		for (unsigned int stateIndex = 0; stateIndex < m_AnimLayer_V[i]->GetStates().size(); stateIndex++)
		{
			// ���ڸ������ �ƴ��� ���� Ȯ��
			Locomotion* _nowLocomotion = dynamic_cast<Locomotion*>(m_AnimLayer_V[i]->GetStates().at(stateIndex).get());

			if (_nowLocomotion == nullptr)
			{
				State* _nowState = dynamic_cast<State*>(m_AnimLayer_V[i]->GetStates().at(stateIndex).get());
				_nowState->m_AnimationIndex = pReosureManager->GetAnimationClipDataIdnex(_nowState->m_AnimationName);
				_nowState->m_TotalKeyframe = pReosureManager->GetTotalKeyFrame(_nowState->m_AnimationIndex);

				// �� ���¿����� �̹� EventPositionRatio�� �������� ��
				_nowState->m_EventPosition = _nowState->m_TotalKeyframe * _nowState->m_EventPositionRatio;
			}
			else
			{	// ���ڸ���̶�� ����ִ� �ִϸ��̼� �̸��鿡 �´� index�� �Ҵ����ش�

				_nowLocomotion->m_LocomotionAnimIndex = pReosureManager->GetAnimationClipDataIdnex(_nowLocomotion->m_LocomotionAnimName);
				
				for (unsigned int animName = 0; animName < _nowLocomotion->m_LocomotionAnimName_V.size(); animName++)
				{
					unsigned int _nowAnimIndex = pReosureManager->GetAnimationClipDataIdnex(_nowLocomotion->m_LocomotionAnimName_V[i]);
					_nowLocomotion->m_LocomotionAnimIndex_V.push_back(_nowAnimIndex);
				}
			}
		}
	}
}

