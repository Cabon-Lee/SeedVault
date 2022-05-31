#pragma once
/// Scene�� �ֻ��� �������̽�
/// ��� ������ �� �𸣴�
/// �ϴ� ���� 2���� �ڿ� ����

__interface IScene
{
public:
   virtual void Initialize() abstract;
   virtual void Update(float dTime) abstract;
   virtual void Render(Matrix view, Matrix proj) abstract;

   virtual void Reset() abstract;
   virtual void Remove() abstract;

   // ��Ÿ ��ɵ� �߰�
};