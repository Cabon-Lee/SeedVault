#pragma once

/// fbx �޽��� �ִϸ��̼� ������ ������ ���� Ŭ����
/// �ִϸ��̼��� ������ ��� �ִ´�
/// ���� �ִϸ��̼ǿ� ���� ������ ���Ⱑ �ƴ϶� �������� ���� �� ����
/// �̸��� ���� Skinning�� ���� ������ Mesh���� �������� ���� ��
/// ��������Ʈ �ִϸ��̼ǵ� ���� ���̶�� ���� ����

/// �ñ������� �ִϸ��̼ǿ� ���� ������ �ǳ��ִ� ���� Animator���� �� ���̱� ������
/// �� Ŭ������ �� �������

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

	virtual void Start() override;					//Start�� Update �޼��尡 ó�� ȣ��Ǳ� ����, ��ũ��Ʈ�� Ȱ��ȭ �Ǹ� ȣ��Ǿ�� �Ѵ�. 
	virtual void Update(float dTime) override;		//fixedUpdate(����������)�� ��������� ���� ������?
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