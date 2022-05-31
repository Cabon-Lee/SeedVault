#pragma once
#include <vector>
#include "SimpleMath.h"


struct KeyFrame
{
	~KeyFrame() { animationKeyFrame.clear(); };

	std::string animationName;
	std::vector<DirectX::SimpleMath::Matrix> animationKeyFrame;
};

///--------------------------------------------------
/// 위 클래스를 조합한 Animation 클래스
///--------------------------------------------------
struct AnimationData
{
	// 애니메이션 데이터의 이름. 유일한 참조수단이다
	std::string						m_nodeName;					
	std::string						m_AnimationName;			/// 0

	// 한 프레임의 틱 크기. 
	// AnimationTable을 가지는 각각의 오브젝트마다 다를 수 있다.
	float	m_TicksPerFrame = 0.0f;								/// 0
	// 한 애니메이션이 가지고 있는 keyFrame의 양
	unsigned int m_TotalKeyFrames = 0;							/// 0

	// 수종이 정의한 멤버 변수
	unsigned int m_NowKeyFrame = 0;
	unsigned int m_LastKeyFrame = 0;
	unsigned int m_FirstKeyFrame = 0;

	std::vector<std::vector<DirectX::SimpleMath::Matrix>> animationKeyFrame;		/// 0
};
