#pragma once
#include <string>
#include <vector>

/// 이미지 리소스의 타입
enum class eResourceType
{
	eSingleImage = 0,	// 한장
	eSingleAni,			// 한장 애니메이션
	eSeveralAni			// 여러장 애니메이션
};

/// 빌보드의 종류
enum class eBillboardType
{
	eBillboard,		// 빌보드
	eBillboardText	// 빌보드 텍스트
};

/// 버튼의 상태
enum class eButtonState
{
	OFF = 1,		// 뗌
	PUT,			// 누름
	OVER,
};

/// 빌보드 회전타입
enum class eRotationType
{
	YAxis,			// Y축 회전
	LookAt,			// 카메라 방향으로 회전
};

/// 애니메이션 프레임
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

/// 애니메이션 플레이타입
enum class ePlayType
{
	Loop = 0,
	Once,
};

///UI 그리는 중심축
enum class eUIAxis
{
	None,
	RightUp,
	RightDown,
	LeftUp,
	LeftDown,
	Center,
};

/// 텍스트 렌더링 포지션(중앙을 기준으로 그릴지, 왼쪽위를 기준으로 그릴지)
enum class eTextPoint
{
	LeftUP,
	Center
};

enum class eSpeaker
{
	M,
	A, 
	None
};

struct QuestString
{
	unsigned int triggerIdx;
	std::wstring SubQuest;
};

struct QuestBlock
{
	unsigned int triggerIdx;
	std::wstring MainQuest;
	std::vector<QuestString> SubQuest_V;
};

struct DialogueString
{
	unsigned int triggerIdx;
	eSpeaker speaker;
	float time;
	std::wstring dialogue;
};

struct DialogueBlock
{
	unsigned int TriggerIdx;
	std::vector<DialogueString> dialogueString_V;
};