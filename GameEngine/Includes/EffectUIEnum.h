#pragma once

/// �̹��� ���ҽ��� Ÿ��
enum class eResourceType
{
	eSingleImage = 0,	// ����
	eSingleAni,			// ���� �ִϸ��̼�
	eSeveralAni			// ������ �ִϸ��̼�
};

/// �������� ����
enum class eBillboardType
{
	eBillboard,		// ������
	eBillboardText	// ������ �ؽ�Ʈ
};

/// ��ư�� ����
enum class eButtonState
{
	OFF = 1,		// ��
	PUT,			// ����
	OVER,
};

/// ������ ȸ��Ÿ��
enum class eRotationType
{
	YAxis,			// Y�� ȸ��
	LookAt,			// ī�޶� �������� ȸ��
};

/// �ִϸ��̼� ������
enum class eFrameRate
{
	FPS24 = 0,
	FPS30,
	FPS60,
};

namespace frameRate
{
	const float FRAME_60 = 60.0f;
	const float FRAME_30 = 30.0f;
	const float FRAME_24 = 24.0f;
}

/// �ִϸ��̼� �÷���Ÿ��
enum class ePlayType
{
	Loop = 0,
	Once,
};

///UI �׸��� �߽���
enum class UIAxis
{
	None,
	RightUp,
	RightDown,
	LeftUp,
	LeftDown,
	Center,
};
