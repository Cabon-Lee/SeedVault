#pragma once
#include "DLLDefine.h"
#include "ComponentBase.h"

struct Audio_Save;
class SoundEvent;

class Audio : public ComponentBase
{
public:
	_DLL Audio();
	_DLL ~Audio();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;

	virtual void OnPreRender() override;
	virtual void OnRender() override;

	_DLL SoundEvent* PlayEvent(const char* path);
	_DLL SoundEvent* PlayEvent(const char* path, const char* parameter, float parameterValue);
	_DLL float GetEventDistance(const char* path);
	_DLL void AllEventStop();


private:
	std::vector<SoundEvent*> m_3DEvent_V;
	std::vector<SoundEvent*> m_2DEvent_V;

public:
	virtual void SaveData();
	virtual void LoadData();

protected:
	Audio_Save* m_SaveData;

};

struct Audio_Save
{
	bool				m_bEnable;
	unsigned int		m_ComponentId;
};

BOOST_DESCRIBE_STRUCT(Audio_Save, (), (
	m_bEnable,
	m_ComponentId
	))