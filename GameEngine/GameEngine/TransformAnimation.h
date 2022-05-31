#pragma once
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "EffectUIEnum.h"

__interface IRenderer;
__interface IResourceManager;

class Transform;
struct Actor;

/// <summary>
/// Ʈ������ �ִϸ��̼� ������Ʈ
///	 : �Ž� �̵�, ȸ�� ��ų �� �ִ� �ִϸ��̼�
///	   	
/// 2022.02.12 B.Bunny
/// </summary>
class TransformAnimation : public ComponentBase
{
public:
	_DLL TransformAnimation();
	_DLL virtual ~TransformAnimation();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override {};	

	_DLL void SetPlayType(ePlayType playType);
	_DLL void SetFrameRate(eFrameRate frameRate);
	_DLL void AddActor(GameObject* gameObject);
	_DLL Actor* GetActor(std::string gameObjectName);
	
	_DLL void Reset();
	_DLL void Play();
	
private:
	void SetKeyFrameInfo();
	void SetPlayTime();

private:
	ePlayType m_playType;			// �ѹ��� �������, ��������
	eFrameRate m_FrameRate;			// fps

	float m_fps;					// fps�� ��(24, 30, 60...)
	float m_playTime;				// �ִϸ��̼��� ��� ����
	float m_timeStack;				// �帥 �ð�
	bool m_isPlay;					// �ִϸ��̼� ����, ��� ����
	
	std::vector<Actor*> m_actor_V;

};

struct KeyFrame
{
	KeyFrame() {};
	KeyFrame(float dur, 
		unsigned int frame, 
		const DirectX::SimpleMath::Matrix& tm)
		: duration(dur), frame(frame), keyMatrix(tm) {};

	KeyFrame(float dur,
		unsigned int frame)
		: duration(dur), frame(frame) {};

	void TransformUpdate(Transform* transform);
	void TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos);
	void TransformUpdate(Transform* transform, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot);

	float duration;							// �� Ű�������� ����ð�
	unsigned int frame;						// Ű�������� ������

	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion rotation;
	DirectX::SimpleMath::Vector3 position;

	DirectX::SimpleMath::Vector3 changeScale;
	DirectX::SimpleMath::Vector3 changeRotation;
	DirectX::SimpleMath::Vector3 changePosition;

	DirectX::SimpleMath::Matrix keyMatrix;
};

struct Actor
{
	Actor() {};
	Actor(GameObject* pGO);

	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos);
	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 rot);
	_DLL void AddKeyFrame(float dur, DirectX::SimpleMath::Matrix mat);

	void OriginTransformUpdata();

	unsigned int currKeyframe;	// ���� Ű������
	unsigned int endFrame;		// ���Ͱ� ������ ������
	GameObject* pGameObject;	// ������ ��� ������Ʈ
	//DirectX::SimpleMath::Quaternion currRot;
	DirectX::SimpleMath::Matrix originTM;

	std::vector<KeyFrame> keyFrame_V;
};