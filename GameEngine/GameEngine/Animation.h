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
	// 시간이 tickPerTime을 지났는지 체크
	bool TimeCheck(float dTime);
	float m_TimeStack;

public:
	// 애니메이션 클래스는 단순히 자신이 가지고 있는 애니메이션 클립을 실행시키는 역할로
	// 애니메이션 클립을 관리하는 것은 아니기 때문에 쉬운 접근이 가능하다
	std::string m_AnimationClipName;
	unsigned int m_AnimationClipIndex;

	eANIM_FRAMERATE m_AnimationFrameRate;

	void CopyPrevAnimation();

	DirectX::SimpleMath::Matrix* m_pPrevAnimatonTM;
	DirectX::SimpleMath::Matrix* m_pInterpolateAnimationTM;

protected:
	std::shared_ptr<IRenderer> m_pIRenderer;
	std::shared_ptr<IResourceManager> m_pIResourceManager;
	MeshFilter* m_MyMeshFilter;

	// 상속받은 애니메이터가 사용할 수 없도록
	unsigned int m_NowKeyFrame;
	unsigned int m_TargetBoneIndex;

	DirectX::XMMATRIX* m_BoneOffsetTM;
	DirectX::XMMATRIX* m_TargetBoneTM;	// 값을 복사해 보관하기 위해서

private:
	// 애니메이터에 있는 모든 스테이트에서 공통으로 쓰면 안된다
	float m_OffsetAnlge;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;

};

