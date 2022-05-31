#pragma once

/// <summary>
/// ������ ���³� �������� �͵��� �����ϴ� �Ŵ��� Ŭ����
/// 
/// ���� ���ӸŴ����� ������Ʈ�� �ϳ� ����� �ΰ� �ٿ�����.
/// 
/// �ۼ��� : YoKing
/// </summary>

/*
	save/load ���� �̱���
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
	void LockingMouseCursor();		// TPS ī�޶� ����� �� ���콺 Ŀ�� ȭ�� �ȿ� ���α�

private:
	static bool s_bDebugMode;		// ������� 
	static bool s_bTPSMode;			// TPS ī�޶� ���� ���(true�� ���콺�� ���ΰ� Ŀ�� �̵��� ���� ī�޶�ȸ��)

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

	bool s_bDebugMode;		// ������� 
	bool s_bTPSMode;		// TPS ī�޶� ���� ���(true�� ���콺�� ���ΰ� Ŀ�� �̵��� ���� ī�޶�ȸ��)

};

BOOST_DESCRIBE_STRUCT(GameManager_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_GameState,

	s_bIsUIVisible,

	s_bDebugMode,
	s_bTPSMode
	))

