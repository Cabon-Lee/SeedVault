#include "pch.h"
#include "PlaneAnimation.h"

PlaneAnimation::PlaneAnimation()
	: ComponentBase(ComponentType::Animation)
	, m_SaveData(new PlaneAnimation_Save())
	, m_FrameRate(eFrameRate::FPS30)
	, m_frameCount(1)
	, m_fps(frameRate::FRAME_30)
	, m_timeCount(0)
	, m_wholeIdxCount(0)
	, m_curIdx(0)
{
}

PlaneAnimation::~PlaneAnimation()
{
}

void PlaneAnimation::Start()
{
	switch (m_FrameRate)
	{
	case eFrameRate::FPS24:
		m_fps = frameRate::FRAME_24;
		break;
	case eFrameRate::FPS30:
		m_fps = frameRate::FRAME_30;
		break;
	case eFrameRate::FPS60:
		m_fps = frameRate::FRAME_60;
		break;
	default:
		m_fps = frameRate::FRAME_30;
		break;
	}
}

void PlaneAnimation::Update(float dTime)
{
	if (m_wholeIdxCount == 0)
	{
		return;
	}

	m_timeCount += dTime;

	if (m_timeCount > m_frameCount	/ m_fps)
	{
		++m_curIdx;
		m_timeCount = 0.f;

		if (m_curIdx > m_wholeIdxCount - 1)
		{
			m_curIdx = 0;
		}
	}
}

void PlaneAnimation::OnRender()
{
}

void PlaneAnimation::SetFPS(eFrameRate frameRate, int frameCount)
{
	m_FrameRate = frameRate;
	m_frameCount = frameCount;
}

void PlaneAnimation::SetAniIdx(int wholeIdxCount)
{
	m_wholeIdxCount = wholeIdxCount;
}

int PlaneAnimation::GetCurAniIndx()
{
	return m_curIdx;
}

void PlaneAnimation::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void PlaneAnimation::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}
