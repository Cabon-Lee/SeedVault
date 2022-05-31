#pragma once

#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"

__interface IRenderer;
class MeshFilter;

const float FRAME_60 = 1 / 60.0f;
const float FRAME_30 = 1 / 30.0f;
const float FRAME_24 = 1 / 24.0f;

enum class eANIM_FRAMERATE
{
	FPS60 = 0,
	FPS30,
	FPS24,
};

class Animation : public ComponentBase, public ResourceObserver
{
public:
	_DLL Animation();
	_DLL virtual ~Animation();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void SetTagetBone(unsigned int boneIndex);

	_DLL DirectX::XMMATRIX* GetTargetBoneTM();
	_DLL DirectX::XMMATRIX* GetBoneOffSetTM();

protected:
	// �ð��� tickPerTime�� �������� üũ
	bool TimeCheck(float dTime);
	float m_TimeStack;

public:
	// �ִϸ��̼� Ŭ������ �ܼ��� �ڽ��� ������ �ִ� �ִϸ��̼� Ŭ���� �����Ű�� ���ҷ�
	// �ִϸ��̼� Ŭ���� �����ϴ� ���� �ƴϱ� ������ ���� ������ �����ϴ�
	std::string m_AnimationClipName;
	unsigned int m_AnimationClipIndex;

	eANIM_FRAMERATE m_AnimationFrameRate;

protected:
	std::shared_ptr<IRenderer> m_pIRenderer;
	std::shared_ptr<IResourceManager> m_pIResourceManager;
	MeshFilter* m_MyMeshFilter;

	// ��ӹ��� �ִϸ����Ͱ� ����� �� ������
	unsigned int m_NowKeyFrame;
	unsigned int m_TargetBoneIndex;

	DirectX::XMMATRIX* m_BoneOffsetTM;
	DirectX::XMMATRIX* m_TargetBoneTM;	// ���� ������ �����ϱ� ���ؼ�

private:
	// �ִϸ����Ϳ� �ִ� ��� ������Ʈ���� �������� ���� �ȵȴ�
	float m_OffsetAnlge;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

};

