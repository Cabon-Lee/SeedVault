#include "NodeData.h"
#include "VertexHelper.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BoundingVolume.h"

#include "CATrace.h"

void NodeData::FindParentNode()
{

}

void NodeData::CalculateTransform()
{
	if (pParentNode != nullptr)
	{
		worldTM = nodeTM * pParentNode->worldTM;
	}
	else
	{
		worldTM = nodeTM;
	}
}

void NodeData::CalculateAnimTransform()
{
	if (pParentNode != nullptr)
	{
		worldTM = animationTM * pParentNode->worldTM;
	}
	else
	{
		worldTM = animationTM;
	}
}


void NodeData::CalculateTransform(const DirectX::XMMATRIX& worldTM)
{
	if (pParentNode != nullptr)
	{
		animationTM = this->meshLocalTM * pParentNode->animationTM;
	}
	else
	{
		animationTM = worldTM;
	}
}

void BoneNodeData::CalculateTransform()
{
	if (pParentBone != nullptr)
	{
		worldTM = nodeTM * pParentBone->worldTM;
	}
	else
	{
		worldTM = animationTM;
	}
}


void BoneNodeData::CalculateAnimTransform()
{
	if (pParentBone != nullptr)
	{
		worldTM = animationTM * pParentBone->worldTM;
	}
	else
	{
		// ��Ʈ ��忡 ������ ��
		// ��Ʈ ����� ����� ���� ������Ʈ�� World ���� �������� ������ ����
		worldTM = animationTM;
	}
}

void SkeletonData::SortingTorsoLeg()
{

	// �θ��� �ε����� 0�� ģ��


}


void SkeletonData::BindPose()
{
	CalculateTransform(pBones_V.front());

}

void SkeletonData::BindPose(const DirectX::SimpleMath::Matrix& worldTM)
{
	pBones_V.front()->animationTM = worldTM * pBones_V.front()->nodeTM;
	CalculateTransform(pBones_V.front());
}

void SkeletonData::CalculateTransform(BoneNodeData* pChild)
{
	// �����̶�� 
	if (pChild == nullptr)
	{
		return;
	}

	pChild->CalculateTransform();

	// �ڽ��� ���ٸ� ���Ѵ�
	for (int i = 0; i < pChild->pChildren_V.size(); i++)
	{
		CalculateTransform(pChild->pChildren_V[i]);
	}
}


void SkeletonData::CalculateAnimTransform(BoneNodeData* pChild)
{
	// �����̶�� 
	if (pChild == nullptr)
	{
		return;
	}

	pChild->CalculateAnimTransform();

	// �ڽ��� ���ٸ� ���Ѵ�
	for (int i = 0; i < pChild->pChildren_V.size(); i++)
	{
		CalculateAnimTransform(pChild->pChildren_V[i]);
	}
}


void SkeletonData::KeyFrameAnimate(
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int animIndex,
	unsigned int keyFrame)
{
	// ��Ʈ���� ����TM�� �����Ͽ� ��Ű�� �ִϸ��̼��� ��ġ�� �ٲ۴�
	pBones_V[0]->worldTM = worldTM;

	// testAnim���� ��� ��忡 Ű�����ӿ� �ش��ϴ� ����� �����ش�
	for (BoneNodeData* _nowBone : pBones_V)
	{

	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());
}

void SkeletonData::AnimateByAnimationClip(
	std::shared_ptr<AnimationClipData> animationClip,
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int animIndex,
	bool isCopy,
	float offsetAngle)
{
	// �ִϸ��̼ǿ� ���� �����ִ°� �־ ������ ������ Root�� ���� ������ �ش�
	if (offsetAngle != 0.0f)
	{
		const auto yaw = -offsetAngle * DirectX::XM_PI / 180.0f;
		DirectX::SimpleMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

		DirectX::SimpleMath::Matrix _rootLocal = animationClip->keyFrames_V[0][animIndex];

		_rootLocal *= worldTM;
		pBones_V.front()->animationTM = _rootLocal *= XMMatrixRotationQuaternion(q);
	}
	else
	{
		const auto yaw = 90 * DirectX::XM_PI / 180.0f;
		DirectX::SimpleMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);


		DirectX::SimpleMath::Matrix _yUp = worldTM;
		DirectX::SimpleMath::Matrix _nowAnimTM = animationClip->keyFrames_V[0][animIndex];
		DirectX::SimpleMath::Matrix _finTM;

		_finTM = _nowAnimTM * _yUp;

		pBones_V.front()->animationTM = animationClip->keyFrames_V[0][animIndex] * worldTM;
	}



	for (int i = 1; i < pBones_V.size(); i++)
	{
		// i�� ���� �ε���
		DirectX::SimpleMath::Matrix _nowLocal = animationClip->keyFrames_V[i][animIndex];
		pBones_V[i]->animationTM = _nowLocal;

	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());

	for (int i = 0; i < pBones_V.size(); i++)
	{
		if (isCopy == true)
			pBones_V[i]->prevAnimationTM = pBones_V[i]->worldTM;
	}
}

DirectX::SimpleMath::Matrix MatrixLerp(
	DirectX::SimpleMath::Matrix& easeInTM,
	DirectX::SimpleMath::Matrix& easeOutTM,
	float blend)
{
	DirectX::SimpleMath::Vector3 _size;
	DirectX::SimpleMath::Vector3 _easeInPos;
	DirectX::SimpleMath::Quaternion _easeInQuaternion;
	DirectX::SimpleMath::Vector3 _easeOutPos;
	DirectX::SimpleMath::Quaternion _easeOutQuaternion;

	easeInTM.Decompose(_size, _easeInQuaternion, _easeInPos);
	easeOutTM.Decompose(_size, _easeOutQuaternion, _easeOutPos);

	DirectX::SimpleMath::Vector3 _nowPos;
	DirectX::SimpleMath::Quaternion _nowQuaternion;

	_nowPos = DirectX::SimpleMath::Vector3::Lerp(_easeOutPos, _easeInPos, blend);
	_nowQuaternion = DirectX::SimpleMath::Quaternion::Slerp(_easeOutQuaternion, _easeInQuaternion, blend);

	return
		DirectX::SimpleMath::Matrix::CreateScale(1.0f) *
		DirectX::SimpleMath::Matrix::CreateFromQuaternion(_nowQuaternion) *
		DirectX::SimpleMath::Matrix::CreateTranslation(_nowPos);
}

void SkeletonData::AnimationCrossFading(
	std::shared_ptr<AnimationClipData> easeInAnimClip,
	std::shared_ptr<AnimationClipData> easeOutAnimClip,
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int easeInAnimIndex,
	unsigned int easeOutAnimIdex,
	float blending,
	float offsetAngle /*= 0.0f*/)
{
	// �ִϸ��̼ǿ� ���� �����ִ°� �־ ������ ������ Root�� ���� ������ �ش�
	if (offsetAngle != 0.0f)
	{
		const auto yaw = -offsetAngle * DirectX::XM_PI / 180.0f;
		DirectX::SimpleMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

		DirectX::SimpleMath::Matrix _easeInLocalTM = easeInAnimClip->keyFrames_V[0][easeInAnimIndex];
		DirectX::SimpleMath::Matrix _easeOutLocalTM = easeOutAnimClip->keyFrames_V[0][easeOutAnimIdex];
		DirectX::SimpleMath::Matrix _rootLocal;
		_rootLocal = MatrixLerp(_easeInLocalTM, _easeOutLocalTM, blending);

		_rootLocal *= worldTM;

		pBones_V.front()->animationTM = _rootLocal *= XMMatrixRotationQuaternion(q);
	}
	else
	{
		pBones_V.front()->animationTM = easeInAnimClip->keyFrames_V[0][easeInAnimIndex] * worldTM;
	}

	for (int i = 1; i < pBones_V.size(); i++)
	{
		// ������ ���̵��̱� ������ easeOutLocalTM�� �׻� ������ ����
		DirectX::SimpleMath::Matrix _easeInLocalTM = easeInAnimClip->keyFrames_V[i][easeInAnimIndex];
		DirectX::SimpleMath::Matrix _easeOutLocalTM = easeOutAnimClip->keyFrames_V[i][easeOutAnimIdex];

		pBones_V[i]->animationTM = MatrixLerp(_easeInLocalTM, _easeOutLocalTM, blending);
	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());

}

void SkeletonData::CrossFadingByPrevAnimatoinTM(
	std::shared_ptr<AnimationClipData> easeInAnimClip, 
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int easeInAnimIndex, float blending)
{
	const auto yaw = 90 * DirectX::XM_PI / 180.0f;
	DirectX::SimpleMath::Quaternion q = DirectX::XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);


	DirectX::SimpleMath::Matrix _yUp = worldTM;
	DirectX::SimpleMath::Matrix _nowAnimTM = easeInAnimClip->keyFrames_V[0][easeInAnimIndex];
	DirectX::SimpleMath::Matrix _finTM;

	_finTM = _nowAnimTM * _yUp;

	pBones_V.front()->animationTM = easeInAnimClip->keyFrames_V[0][easeInAnimIndex] * worldTM;

	for (int i = 1; i < pBones_V.size(); i++)
	{
		// i�� ���� �ε���
		DirectX::SimpleMath::Matrix _nowLocal = easeInAnimClip->keyFrames_V[i][easeInAnimIndex];
		pBones_V[i]->animationTM = _nowLocal;
	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());

	for (int i = 0; i < pBones_V.size(); i++)
	{
		pBones_V[i]->worldTM = 
			MatrixLerp(pBones_V[i]->worldTM, pBones_V[i]->prevAnimationTM, blending);
	}
}

void SkeletonData::MaskingAnimation(
	std::shared_ptr<AnimationClipData> targetAnimClip,
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int targetAnimIndex)
{
	for (int i = 0; i < pTorso_V.size(); i++)
	{
		int _nowBoneIndex = pTorso_V[i]->boneIndex;
		DirectX::SimpleMath::Matrix _targetTM = targetAnimClip->keyFrames_V[_nowBoneIndex][targetAnimIndex];
		pTorso_V[i]->animationTM = _targetTM;
	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());
}

void SkeletonData::MaskingAnimation(
	std::shared_ptr<AnimationClipData> targetBaseAnimClip,
	std::shared_ptr<AnimationClipData> targetSubAnimClip,
	float alpha, unsigned int targetAnimIndex,
	const DirectX::SimpleMath::Matrix& worldTM)
{
	for (int i = 1; i < pTorso_V.size(); i++)
	{
		// ������ ���̵��̱� ������ easeOutLocalTM�� �׻� ������ ����
		DirectX::SimpleMath::Matrix _easeInLocalTM = targetBaseAnimClip->keyFrames_V[pTorso_V[i]->boneIndex][targetAnimIndex];
		DirectX::SimpleMath::Matrix _easeOutLocalTM = targetSubAnimClip->keyFrames_V[pTorso_V[i]->boneIndex][targetAnimIndex];

		pBones_V[pTorso_V[i]->boneIndex]->animationTM = MatrixLerp(_easeInLocalTM, _easeOutLocalTM, alpha);
	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(pBones_V.front());
}

void SkeletonData::MaskingAnimation(std::shared_ptr<AnimationClipData> targetAnimClip, float alpha)
{
	for (int i = 0; i < pTorso_V.size(); i++)
	{
		unsigned int _nowBoneIndex = pTorso_V[i]->boneIndex;

		// ù��° Ű �����Ӱ� ������ Ű ������ ��� ������ �����´�
		DirectX::SimpleMath::Matrix _easeInLocalTM = targetAnimClip->keyFrames_V[_nowBoneIndex].front();
		DirectX::SimpleMath::Matrix _easeOutLocalTM = targetAnimClip->keyFrames_V[_nowBoneIndex].back();

		pBones_V[_nowBoneIndex]->animationTM = MatrixLerp(_easeInLocalTM, _easeOutLocalTM, alpha);
	}

	CalculateAnimTransform(pBones_V.front());
}

void ModelMesh::MeshAnimation(
	std::shared_ptr<AnimationClipData> animationClip,
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int animIndex)
{

	for (int i = 0; i < m_pNodeData_V->size(); i++)
	{
		// i�� ���� �ε���
		DirectX::SimpleMath::Matrix _nowLocal = animationClip->keyFrames_V[i][animIndex];
		m_pNodeData_V->at(i)->animationTM = _nowLocal * worldTM;
	}

	// �� ������ ������ ��͸� ���� ��� �ִϸ��̼��� ��������� ���� �����Ѵ�
	CalculateAnimTransform(m_pNodeData_V->front());

}

void ModelMesh::CalculateAnimTransform(std::shared_ptr<NodeData> pChild)
{
	// �����̶�� 
	if (pChild == nullptr)
	{
		return;
	}

	pChild->CalculateAnimTransform();

	// �ڽ��� ���ٸ� ���Ѵ�
	for (int i = 0; i < pChild->pChildren_V.size(); i++)
	{
		CalculateAnimTransform(pChild->pChildren_V[i]);
	}
}

void ModelMesh::CalculateBoneOffsetTM(std::vector<DirectX::SimpleMath::Matrix>& boneTM)
{
	// �̱��� ����
}

void ModelMesh::CalculateBoneOffsetTM(int boneIndex, DirectX::XMMATRIX& boneTM)
{
	BoneNodeData* _nowBone = m_pSkeleton->pBones_V[boneIndex];

	m_BindPose = m_pSkeleton->pBones_V[boneIndex]->bindPoseTM;
	m_BindPoseInv = m_BindPose.Invert();

	m_NowBoneWorldTM = m_pSkeleton->pBones_V[boneIndex]->worldTM;

	m_NowMat = m_pSkeleton->boneOffsetTM_V[boneIndex] * m_NowBoneWorldTM;

	// VS Shader�� �������ֱ� ���� �����Ѵ�.
	_nowBone->nowBoneOffsetTM = m_NowMat;

	// �ٱ����� �������ش�.
	boneTM = m_NowMat;
}

void ModelMesh::CalculateBoneOffsetTM(DirectX::XMMATRIX* offsetArray)
{
	for (int i = 0; i < m_pSkeleton->pBones_V.size(); i++)
	{
		BoneNodeData* _nowBone = m_pSkeleton->pBones_V[i];

		m_BindPose = m_pSkeleton->pBones_V[i]->bindPoseTM;
		m_BindPoseInv = m_BindPose.Invert();

		m_NowBoneWorldTM = m_pSkeleton->pBones_V[i]->worldTM;

		m_NowMat = m_pSkeleton->boneOffsetTM_V[i] * m_NowBoneWorldTM;

		_nowBone->nowBoneOffsetTM = m_NowMat;

		offsetArray[i] = m_NowMat;
	}
}

void ModelMesh::CalculateBoneOffsetTM()
{
	for (int i = 0; i < m_pSkeleton->pBones_V.size(); i++)
	{
		BoneNodeData* _nowBone = m_pSkeleton->pBones_V[i];

		m_BindPose = m_pSkeleton->pBones_V[i]->bindPoseTM;
		m_BindPoseInv = m_BindPose.Invert();

		m_NowBoneWorldTM = m_pSkeleton->pBones_V[i]->worldTM;

		m_NowMat = m_pSkeleton->boneOffsetTM_V[i] * m_NowBoneWorldTM;

		_nowBone->nowBoneOffsetTM = m_NowMat;
	}
}

void ModelMesh::SetBoneOffsetToBuffer(DirectX::XMMATRIX* offsetArray)
{
	for (int i = 0; i < m_pSkeleton->pBones_V.size(); i++)
	{
		offsetArray[i] = m_pSkeleton->pBones_V[i]->nowBoneOffsetTM;
	}
}

