#pragma once
#include "AlignedAllocationPolicy.h"
#include "d3dx11Effect.h"

class Camera;

class DebugCircle : public AlignedAllocation<16>
{
public:
   DebugCircle(Matrix world, float radius);
   ~DebugCircle();

   void Initialize();
   void Update(Camera* pCamera, float angle, Vector3 pos);
   void Render();
   bool CollisionCheck(DebugCircle* pTarget);

   void BuildGeometryBuffer();

   bool GetIsCollided() const { return m_IsCollided; }
   void SetIsCollided(bool val) { m_IsCollided = val; }


   int GetDebugIndex() const { return m_DebugIndex; }
   void SetDebugIndex(int val) { m_DebugIndex = val; }

   float GetRadius() const { return m_Radius; }
   void SetRadius(float val) { m_Radius = val; }

   std::vector<int> GetCollidedCircles() const { return m_CollidedCircles_v; }
   void SetCollidedCircles(std::vector<int> val) { m_CollidedCircles_v = val; }

   void SetIsRendering(bool val) { m_IsRendering = val; }

private:

   ID3DX11Effect* m_pFX;
   ID3DX11EffectTechnique* m_pTech;
   ID3DX11EffectMatrixVariable* m_pFXWorldViewProj;

   UINT m_RenderingIndex;

   bool m_IsRendering;

   // 정점 관련
   ID3D11Buffer* m_pVBGreen;
   ID3D11Buffer* m_pVBRed;
   ID3D11Buffer* m_pIB;

   UINT m_IndexCount;

   Matrix m_World;	// 월드 변환 행렬 (로컬->월드)
   Matrix m_View; // 시야 변환 행렬 (카메라 뷰)
   Matrix m_Proj; // 투영 변환 행렬 (원근/직교)
   
   float m_Radius;
   Matrix m_ScaleTM;
   Matrix m_RotationTM;
   Matrix m_TransTM;
   float m_Angle;
   Vector3 m_ForwardVecOffset;

   int m_DebugIndex;

   bool m_IsCollided;

   vector<int> m_CollidedCircles_v;
};

