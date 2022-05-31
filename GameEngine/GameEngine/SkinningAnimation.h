#pragma once

/// fbx 메쉬의 애니메이션 정보를 연산을 돕는 클래스
/// 애니메이션의 정보를 담고 있는다
/// 실제 애니메이션에 대한 연산은 여기가 아니라 렌더러가 해줄 것 같다
/// 이름에 굳이 Skinning을 넣은 이유는 Mesh와의 연관성을 위한 것
/// 스프라이트 애니메이션도 있을 것이라고 보기 떄문

/// 궁극적으로 애니메이션에 대한 정보를 건네주는 것은 Animator에서 할 것이기 때문에
/// 이 클래스는 곧 사라진다

#include "DLLDefine.h"

#include "ComponentBase.h"

__interface IRenderer;
struct MeshMetadata;
class MeshFilter;
class Animator;

struct SkinningAnimation_SaveData;

class SkinningAnimation : public ComponentBase
{
public:
	_DLL SkinningAnimation();
	_DLL virtual ~SkinningAnimation();

	virtual void Start() override;					//Start는 Update 메서드가 처음 호출되기 직전, 스크립트가 활성화 되면 호출되어야 한다. 
	virtual void Update(float dTime) override;		//fixedUpdate(고정프레임)과 구별지어야 하지 않을까?
	virtual void OnRender() override;

	void SetMeshdata(std::vector<MeshMetadata>* data);

private:
	std::string m_NowMeshName;
	std::string m_AnimationName;

public:
	const std::string& GetNowMeshName();
	const std::string& GetNowAnimationName();


	int* m_Test;


private:
	std::shared_ptr<IRenderer> m_pRenderer;

	MeshFilter* m_MyMeshFilter;
	MeshMetadata* m_MyMeshData;
	Animator* m_MyAnimator;

private:
	void SetMyMeshData(MeshMetadata* data);

private:
	std::vector<MeshMetadata>* m_MeshMetadata;

public:
	const bool GetIsAniPossibleMesh();
	const size_t GetMeshAnimationCnt();
	const std::string GetMeshAnimationName(size_t index);
	void SetMeshAnimationIndex(size_t index);

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	SkinningAnimation_SaveData* m_SaveData;
};

struct SkinningAnimation_SaveData
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	std::string m_NowMeshName;
	std::string m_AnimationName;

};

BOOST_DESCRIBE_STRUCT(SkinningAnimation_SaveData, (), (
	m_bEnable,
	m_ComponentId,
	m_NowMeshName,
	m_AnimationName
	))