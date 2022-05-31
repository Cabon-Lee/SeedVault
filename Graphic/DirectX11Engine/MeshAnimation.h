#pragma once

/// 메쉬만을 가지고 하는 애니메이션

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

