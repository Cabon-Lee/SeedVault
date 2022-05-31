#pragma once

#include "SceneBase.h"

class CLMaterialTest final : public SceneBase
{
public:
	CLMaterialTest();
	virtual ~CLMaterialTest();
	CLMaterialTest(const CLMaterialTest& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:

	std::vector<GameObject*> m_Objs;

	GameObject* m_pDirectionalLight;
	GameObject* m_pMainCamera;		// ī�޶�� SceneBase�� �־ ���� �ʳ�?
									// ������ �ʿ��ϴ�
	GameObject* m_TestObject;
	GameObject* m_TestObject2;
	GameObject* m_TestObject3;
	GameObject* m_TestObject4;
	GameObject* m_TestObject5;


	GameObject* m_ReflectionProbe;
	GameObject* m_ReflectionProbe2;
	GameObject* m_ReflectionProbe3;
	class MuzzleFlash* m_MuzzleFlash;
	class HitPoint* m_HitPoint;

};

