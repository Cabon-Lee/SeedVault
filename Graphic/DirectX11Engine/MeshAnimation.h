#pragma once

/// �޽����� ������ �ϴ� �ִϸ��̼�

#include <memory>

class ModelMesh;
struct AnimationClipData;

namespace DirectX 
{
	namespace SimpleMath
	{
		struct Matrix;
	}
}

class MeshAnimation
{
public:
	static void DoAniamtion(
		ModelMesh* model, 
		std::shared_ptr<AnimationClipData> anim,
		const DirectX::SimpleMath::Matrix& worldTM,
		unsigned int keyFrame);

};

