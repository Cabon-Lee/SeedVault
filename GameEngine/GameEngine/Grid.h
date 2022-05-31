#pragma once

/// 모델 뷰어, 오브젝트 배치 시 필요한 그리드 클래스
/// 별도의 조작이나 움직임이 필요없으므로 게임오브젝트나 트랜스폼 클래스가 필요없다

#include "SimpleMath.h"

class Grid
{
public:
	Grid();
	~Grid();

private:
	DirectX::SimpleMath::Vector3 m_Pos[2];

};

