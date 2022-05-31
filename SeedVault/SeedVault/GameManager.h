#pragma once

/// <summary>
/// 게임의 상태나 전반적인 것들을 관리하는 매니져 클래스
/// 
/// 씬에 게임매니져용 오브젝트를 하나 만들어 두고 붙여주자.
/// 
/// 작성자 : YoKing
/// </summary>

/*
	save/load 아직 미구현
*/

class GameManager : public ComponentBase
{
public:
	enum class GameState
	{
		Play,
		Pause,
		Max,
	};

	static bool IsDebugMode();
	static bool IsTPSMode();

	GameState m_GameState;

	static bool s_bIsUIVisible;

public:
	GameManager();
	~GameManager();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void SetDebugMode(const bool& val);
	bool TogleDebugMode();
	void UIRender();

public:
	virtual void SaveData() override;
	virtual void LoadData() override;
	
	struct GameManager_Save* m_SaveData;

protected:

private:
	void ConnectUI();
	void LockingMouseCursor();		// TPS 카메라 모드일 떄 마우스 커서 화면 안에 가두기

private:
	static bool s_bDebugMode;		// 디버깅모드 
	static bool s_bTPSMode;			// TPS 카메라 시점 모드(true면 마우스를 가두고 커서 이동에 따라 카메라회전)

	Text* m_Monster_1_Name;
	Text* m_Monster_1_State;

	Sprite2D* m_CrossHead;
};

struct GameManager_Save
{
	bool m_bEnable;
	uint m_ComponentId;

	int m_GameState;

	bool s_bIsUIVisible;

	bool s_bDebugMode;		// 디버깅모드 
	bool s_bTPSMode;		// TPS 카메라 시점 모드(true면 마우스를 가두고 커서 이동에 따라 카메라회전)

};

BOOST_DESCRIBE_STRUCT(GameManager_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_GameState,

	s_bIsUIVisible,

	s_bDebugMode,
	s_bTPSMode
	))

