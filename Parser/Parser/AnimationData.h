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
/// �� Ŭ������ ������ Animation Ŭ����
///--------------------------------------------------
struct AnimationData
{
	// �ִϸ��̼� �������� �̸�. ������ ���������̴�
	std::string						m_nodeName;					
	std::string						m_AnimationName;			/// 0

	// �� �������� ƽ ũ��. 
	// AnimationTable�� ������ ������ ������Ʈ���� �ٸ� �� �ִ�.
	float	m_TicksPerFrame = 0.0f;								/// 0
	// �� �ִϸ��̼��� ������ �ִ� keyFrame�� ��
	unsigned int m_TotalKeyFrames = 0;							/// 0

	// ������ ������ ��� ����
	unsigned int m_NowKeyFrame = 0;
	unsigned int m_LastKeyFrame = 0;
	unsigned int m_FirstKeyFrame = 0;

	std::vector<std::vector<DirectX::SimpleMath::Matrix>> animationKeyFrame;		/// 0
};
