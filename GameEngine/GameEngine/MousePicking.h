#pragma once

class Scene;
class PhysicsActor;

enum Command 
{
   PickStart,
   PickStop,
   ScreenMotionCursor,
   ScreenMotionCursorDepth
};

/// 마우스로 피킹하기위한 클래스

class MousePicking
{
public:
   MousePicking(Scene* pScene);
   ~MousePicking();

   bool IsPicked() const;
   bool Pick();


private:
   void GrabActor(DirectX::SimpleMath::Vector3& worldImpact, DirectX::SimpleMath::Vector3& rayOrigin);
   void MoveActor(int x, int y);

   PhysicsActor* m_pSelectedActor;
   PhysicsActor* m_pMouseActor;
   PhysicsActor* m_pMouseActorToDelete;
   float m_DistanceToPicked;
   int m_MouseScreenX, m_MouseScreenY;
   Scene* m_pNowScene;
};

