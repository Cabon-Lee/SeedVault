#include "pch.h"
#include "FmodHelper.h"

const FMOD_VECTOR VectorToFMODVec(const DirectX::SimpleMath::Vector3& vector)
{
	FMOD_VECTOR _fv = { vector.x, vector.y, vector.z };
	return _fv;
}
