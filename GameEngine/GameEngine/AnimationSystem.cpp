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

	// 여기까지 왔다면 모두 true
	// 이중 포인터에 현재 자신의 타겟 포인터를 넘겨준다
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

		// 전이가 되었다는 것을 알려준다
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

	// 스테이트의 수 만큼 크기를 늘리고 접근한다.
	// 비어있는 벡터가 생기는 점은 아쉽다
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
	// Modified by 최 요 환
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
	// Modified by 최 요 환
	// 비활성화 레이어면 업데이트 하지 않는다.
	if (m_bEnabled == false)
	{
		return;
	}

	State* _pCurrState = nullptr;
	State* _pPrevState = nullptr;
	Transition* _pNowTransition = nullptr;

	if (m_States_V.size() == 1)
	{
		// 스테이트가 단순할 떄
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

	// 로코모션을 제외한 모든 애니메이션에 해당 애니메이션이 끝났는지 묻는다
	bool _isAnimationOver = false;
	switch (m_MaskingType)
	{
	case eMASKING_TYPE::NONE:
	{

		// 이전 State가 없는 애니메이션은 아래에서 먼저 처리한다.
		// 크로스 페이딩 때문인데
		// 크로스 페이딩은 이전 스테이트의 애니메이션 프레임 중 하나를 보간하기 때문에 필요한 것
		// 근데 이전 State가 없는 경우가 있어(Entry State), 이때 크로스 페이딩을 시도하면 오류로 이어진다
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
					// PrevState의 키프레임은 변치 않음
					_pNowTransition->m_FadingPeriod,
					_pNowTransition->m_IsCrossFadingOver,	
					// 크로스 페이딩이 끝났는지 bool&로 가져온다
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
				*_nowLoco->m_pLocomotionParameter,		// 가중치, 포인터이므로 넘길 때 애스터리스크
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
			// 애니메이션을 이전 State로 돌려준다
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

// Modified by 최 요 환
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

	// 모든 레이어들은 미리 세팅이 되어있어야함
	for (auto& _nowAnimLayer : m_AnimLayer_V)
	{
		_nowAnimLayer->LayerStart();
	}
}

void Animator::Update(float dTime)
{
	if (TimeCheck(dTime))
	{
		// 타임 체크를 통과해야만 레이어 전체가 업데이트된다
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
	// Modified by 최 요 환
	// 바로 UM 에서 값을 리턴하다가는 잘못된 string 이 들어왔을 때 프로그램이 바로 터져버려서 어찌할 수가 없다..(out of range)
	// 먼저 find로 찾아서 없으면 nullptr 리턴하고 caller 쪽에서 예외처리를 하자.
	if (m_AnimLayer_UM.find(name) == m_AnimLayer_UM.end())
	{
		return nullptr;
	}

	// 정상적으로 찾으면 해당 value 리턴
	return m_AnimLayer_UM.at(name);
}

_DLL std::vector<std::shared_ptr<AnimLayer>>& Animator::GetAnimLayer()
{
	return m_AnimLayer_V;
}

/// <summary>
/// now None Layer를 선택
/// overrideLayer 는 영향 X
/// </summary>
/// <param name="layer">선택할 none layer</param>
/// <returns>성공 여부</returns>
_DLL bool Animator::SetNoneAnimLayer(std::string noneLayer)
{
	bool _ret = false;

	// 레이어 벡터를 순회한다
	for (auto& layer : m_AnimLayer_V)
	{
		// 레이어의 마스킹타입이 NONE 일 경우에
		if (layer->m_MaskingType == eMASKING_TYPE::NONE)
		{
			// 설정할 Layer 라면 활성화
			if (layer->m_LayerName == noneLayer)
			{
				layer->m_bEnabled = true;

				_ret = true;
			}

			// 이외의 레이어는 비활성화
			else
			{
				layer->m_bEnabled = false;
			}

		}
	}

	return _ret;
}

/// <summary>
/// now Override Layer 를 선택
/// noneLayer 은 영향 X
/// </summary>
/// <param name="overrideLayer">선택할 override layer</param>
/// /// <param name="reset">애니메이션 프레임 리셋 여부</param>
/// <returns>성공 여부</returns>
_DLL bool Animator::SetOverrideAnimLayer(std::string overrideLayer, bool reset)
{
	bool _ret = false;

	// 레이어 벡터를 순회한다
	for (auto& layer : m_AnimLayer_V)
	{
		// 레이어의 마스킹타입이 OVERRINDING 일 경우에
		if (layer->m_MaskingType == eMASKING_TYPE::OVERRINDING)
		{
			// 설정할 Layer 라면 활성화
			if (layer->m_LayerName == overrideLayer)
			{
				layer->m_bEnabled = true;

				_ret = true;

				// 리셋 요청이 들어오면
				if (reset == true)
				{
					// 키프레임을 0으로 초기화
					layer->GetNowState()->m_NowKeyFrame = 0;
				}
			}

			// 이외의 레이어는 비활성화
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
			// 로코모션인지 아닌지 먼저 확인
			Locomotion* _nowLocomotion = dynamic_cast<Locomotion*>(m_AnimLayer_V[i]->GetStates().at(stateIndex).get());

			if (_nowLocomotion == nullptr)
			{
				State* _nowState = dynamic_cast<State*>(m_AnimLayer_V[i]->GetStates().at(stateIndex).get());
				_nowState->m_AnimationIndex = pReosureManager->GetAnimationClipDataIdnex(_nowState->m_AnimationName);
				_nowState->m_TotalKeyframe = pReosureManager->GetTotalKeyFrame(_nowState->m_AnimationIndex);

				// 이 상태에서는 이미 EventPositionRatio가 정해졌을 떄
				_nowState->m_EventPosition = _nowState->m_TotalKeyframe * _nowState->m_EventPositionRatio;
			}
			else
			{	// 로코모션이라면 들어있는 애니메이션 이름들에 맞는 index를 할당해준다

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

