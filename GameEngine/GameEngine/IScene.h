#pragma once
/// Scene의 최상위 인터페이스
/// 어떻게 쓰는지 잘 모르니
/// 일단 두자 2개월 뒤에 보자

__interface IScene
{
public:
   virtual void Initialize() abstract;
   virtual void Update(float dTime) abstract;
   virtual void Render(Matrix view, Matrix proj) abstract;

   virtual void Reset() abstract;
   virtual void Remove() abstract;

   // 기타 기능들 추가
};