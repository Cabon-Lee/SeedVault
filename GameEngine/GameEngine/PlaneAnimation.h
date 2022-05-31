#pragma once
#include <memory>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "EffectUIEnum.h"

struct PlaneAnimation_Save;

/// <summary>
/// 스프라이트 애니메이션 컴포넌트
/// 
/// 2022.01.26 B.Bunny
/// </summary>
class PlaneAnimation : public ComponentBase
{
public:
	_DLL PlaneAnimation();
	_DLL virtual ~PlaneAnimation();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	_DLL void SetFPS(eFrameRate, int);

	void SetAniIdx(int wholeIdxCount);
	int GetCurAniIndx();

private:
	eFrameRate m_FrameRate;
	int m_frameCount;
	float m_fps;
	float m_timeCount;

	int m_wholeIdxCount;
	int m_curIdx;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	PlaneAnimation_Save* m_SaveData;
};

struct PlaneAnimation_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(PlaneAnimation_Save, (), (
	m_bEnable,
	m_ComponentId
	))



