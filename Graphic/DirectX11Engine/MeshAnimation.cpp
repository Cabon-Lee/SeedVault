#include "MeshAnimation.h"

#include "NodeData.h"

#include "SimpleMath.h"

void MeshAnimation::DoAniamtion(
	ModelMesh* model,
	std::shared_ptr<AnimationClipData> anim,
	const DirectX::SimpleMath::Matrix& worldTM,
	unsigned int keyFrame)
{
	for (int i = 0; i < model->m_pNodeData_V->size(); i++)
	{
		// i = 본의 역할
		model->m_pNodeData_V->at(i)->animationTM = anim->keyFrames_V[i][keyFrame];
	}

	model->CalculateAnimTransform(model->m_pNodeData_V->at(0).get());
}
