#pragma once

/// <summary>
/// �׽�Ʈ ��2
/// 
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>


#include "SceneBase.h"

class CLViewerScene final : public SceneBase
{
public:
	CLViewerScene();
	virtual ~CLViewerScene();
	CLViewerScene(const CLViewerScene& other) = default;


	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

	//virtual void Render() override;



private:
	GameObject* m_pDirectionalLight;
	GameObject* m_PointLight;
	GameObject* m_SpotLight;

	GameObject* m_TestObject;
	GameObject* m_pTestObject2;

	GameObject* m_pMainCamera;

	bool _bParent;

	float* m_TestSpeed;
	bool* m_TestGround;

};

