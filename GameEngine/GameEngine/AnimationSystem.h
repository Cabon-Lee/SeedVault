#pragma once

/// FBX�� �ִϸ��̼� ������ Clip���� ������.
/// Clip�� State�� ����, State�� ���� � Clip�� ������� �޶�����

#include "DLLDefine.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "Animation.h"
#include "ResourceObserver.h"

__interface IRenderer;
class MeshFilter;
class Transition;
class Animator;
class AnimLayer;
class State;

class Parameter
{
public:
	virtual bool ParameterCheck() abstract;
};

class BooleanType : public Parameter
{
public:
	// Modified by �� �� ȯ
	BooleanType(const bool* parameter, const bool standard = true) : m_Parameter(parameter), m_Standard(standard) {}
	const bool* m_Parameter;	// ���� ������ �� 
	const bool m_Standard;		// ��� �Ǵ� ����	 
	bool ParameterCheck() override
	{
		return (*m_Parameter == m_Standard) ? true : false;
	}
};

class FloatType : public Parameter
{
public:
	FloatType(const float standard, float* parameter, const bool greater) :
		m_Standard(standard), m_Parameter(parameter), m_IsGreater(greater) {}

	const float m_Standard;		// ��� �� �Ǵ� ����(ó���� �������� �ٲ��� ����)
	float* m_Parameter;	// Ŭ���̾�Ʈ�� �Ѱ��� ��(����ؼ� ��ȭ��)
	const bool m_IsGreater;

	bool ParameterCheck() override
	{
		// ū ������ �Ǵ�
		if (m_IsGreater == true)
		{
			return m_Standard < *m_Parameter ? true : false;
		}
		else
		{
			return m_Standard > *m_Parameter ? true : false;
		}
	}
};

class Transition
{
public:
	Transition(std::shared_ptr<State> target
		, State** pCurrState, State** pPrevState, Transition** pPrevTransition)
		: m_pCurrState(pCurrState), m_pPrevState(pPrevState), m_pPrevTransition(pPrevTransition)
		, m_TargetState(target)
		, m_FadingPeriod(0) {};
	_DLL void AddParameter(const float standard, float* parameter, const bool greater);
	_DLL void AddParameter(const bool* parameter, const bool standard = true);

	void TransitionUpdate();

	// ��ü �ִϸ��̼��� ���̿� �� �ۼ�Ʈ����
	unsigned int m_FadingPeriod;
	bool m_IsCrossFadingOver;

private:
	std::vector< std::shared_ptr<Parameter>> m_Parameter_V;
	const std::shared_ptr<State> m_TargetState;
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;
};

enum class eANIMATION_MODE
{
	LOOP,		// Ű ��ǲ or Ư�� ��Ȳ(�̹�Ʈ) �� �ִϸ��̼� ��� �ݺ�, ���� �⺻
	TRIGGER,	// Ű ��ǲ or Ư�� ��Ȳ(�̹�Ʈ) �� �ִϸ��̼� ��, ���� State�� ����
	LOCKED,		// TRIGGER�� ���� ����, �ش� �ִϸ��̼��� ����ñ��� State ���� �Ұ���
};

class State
{
public:
	State();
	State(State** pCurrState, State** pPrevState, Transition** pPrevTransition)
		: m_pCurrState(pCurrState), m_pPrevState(pPrevState), m_pPrevTransition(pPrevTransition)
		, m_AnimationIndex(0)
		, m_Speed(1.0f)
		, m_StateOffsetAngle(0.0f)
		, m_NowKeyFrame(0)
		, m_IsLocomotion(false)
		, m_TotalKeyframe(0)
		, m_EventPosition(0)
		, m_EventPositionRatio(0.0f)
		, m_Event_V()
		, m_AniamtionMode(eANIMATION_MODE::LOOP)
	{
		m_Transition_V = new std::vector<std::shared_ptr<Transition>>();
	}
	~State() {};

	_DLL void AddTrnasition(std::shared_ptr<State> state);
	_DLL void AddTrnasition(const std::string& stateName);
	_DLL void AddEvent(std::function<void()> pEvnet, float position);	// Locomotion�� �� �� ����

	void SetNoneAnimLayer(AnimLayer* pAnimLayer);

	virtual void Update();

	std::string m_StateName;
	unsigned int m_StateIndex;

	std::string m_AnimationName;
	unsigned int m_AnimationIndex;

	// State�� ������Ʈ�� ������ ���ͼ� üũ��
	std::vector<std::shared_ptr<Transition>>* m_Transition_V;
	AnimLayer* m_pMyAnimLayer;	// ���� �ִϸ������� ����

	unsigned int m_TotalKeyframe;
	unsigned int m_EventPosition;
	float m_EventPositionRatio;

public:
	unsigned int m_NowKeyFrame;		// �߰��� Ű�������� �ٲ�� ������Ʈ�� ��� �ִ´�
	float m_Speed;
	float m_StateOffsetAngle;
	bool m_Mirror;					// �¿�����ؼ� �ִϸ��̼��� ������ ���ΰ�? ���� ������� ��
	eANIMATION_MODE m_AniamtionMode;

	bool GetIsLocomotion() { return m_IsLocomotion; };

private:
	std::vector<std::pair<std::function<void()>, float>> m_Event_V;
	

protected:
	bool m_IsLocomotion;

	// ��ǥ ������Ʈ, �ִϸ����Ϳ��� ���� ������Ʈ�� �˷��� �뵵
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;
};

// ���ڸ���� ��ġ ������Ʈó�� �۵��ϹǷ�
class Locomotion : public State
{
public:
	Locomotion(State** pCurrState, State** pPrevState, Transition** pPrevTransition)
		: State(pCurrState, pPrevState, pPrevTransition)
	{ 
		m_IsLocomotion = true; 
	};
	~Locomotion() {};

	_DLL void AddLocomotionAnimName(const std::string& name);
	_DLL void SetLocomotionParameter(float* pLocoPara);

	virtual void Update() override;

	std::string m_LocomotionAnimName;
	unsigned int m_LocomotionAnimIndex;

	std::vector<std::string> m_LocomotionAnimName_V;
	std::vector<unsigned int> m_LocomotionAnimIndex_V;

	float* m_pLocomotionParameter;
};


enum class eMASKING_TYPE
{
	NONE = 0,
	OVERRINDING,
};

class AnimLayer
{
public:
	_DLL AnimLayer();
	_DLL AnimLayer(Animation* pAnimation);
	_DLL virtual ~AnimLayer();

	_DLL void AddStateMap(std::string name, const std::string& animationClipName);
	_DLL void AddLocomotion(std::string name, const std::string& animationClipName, float* pPara);
	_DLL State* GetNowState();

	// ������Ʈ �ý����� ������Ʈ�� �����ؼ� �ִϸ��̼� �ε����� �����ϱ� ����
	std::vector<std::shared_ptr<State>>& GetStates();
	_DLL std::shared_ptr<State> GetState(const std::string& name);

	std::string m_LayerName;
	void LayerStart();
	void LayerUpdate(GameObject* pGameObject, MeshFilter* pMeshFilter, std::shared_ptr<IRenderer> pRenderer);

	// �ִϸ����ʹ� �ټ��� State�� ������ �ִ�
	std::vector<std::shared_ptr<State>> m_States_V;
	// ������Ʈ�� �̸�(Idle, Run ��)
	std::unordered_map<std::string, std::shared_ptr<State>> m_State_UM;

	eMASKING_TYPE m_MaskingType;

	// Modified by �� �� ȯ
	_DLL void SetEnabled(const bool val);

	bool m_bEnabled;	// ���̾� On/Off ���� True�� ����(������Ʈ)
	bool m_IsStateChanged;

private:
	// ���� �������ִ� state�� ���������� �˷��� ����� �ʿ�
	State** m_pCurrState;
	State** m_pPrevState;
	Transition** m_pPrevTransition;

	Animation* m_pMyAnimation;
};

class Animator : public Animation
{
public:
	_DLL Animator();
	_DLL virtual ~Animator();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void AddAnimLayer(std::string name);
	_DLL std::shared_ptr<AnimLayer> GetAnimLayer(const std::string& name);
	_DLL std::vector<std::shared_ptr<AnimLayer>>& GetAnimLayer();

	// Added by �� �� ȯ
	_DLL bool SetNoneAnimLayer(std::string noneLayer);
	_DLL bool SetOverrideAnimLayer(std::string overrideLayer, bool reset = false);

	std::vector<std::shared_ptr<AnimLayer>> m_AnimLayer_V;
	std::unordered_map<std::string, std::shared_ptr<AnimLayer>> m_AnimLayer_UM;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

};