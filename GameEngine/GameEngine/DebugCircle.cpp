#include "pch.h"

//#include "Effect.h"
#include "Camera.h"
//#include "DXRenderState.h"
//#include "StructAndConst.h"
#include "MathHelper.h"
#include "DXDevice.h"
#include "DebugCircle.h"



DebugCircle::DebugCircle(Matrix world, float radius)
   : m_World(world)
   , m_Radius(radius)
   , m_IsRendering(true)
{
   m_IsCollided = false;
}

DebugCircle::~DebugCircle()
{
}

void DebugCircle::Initialize()
{

}

void DebugCircle::Update(Camera* pCamera, float angle, Vector3 pos)
{
   m_View = pCamera->View();
   m_Proj = pCamera->Proj();

   m_Angle = angle;

   m_TransTM._41 = pos.x;
   m_TransTM._42 = pos.y + 0.3f;
   m_TransTM._43 = pos.z;

   m_RotationTM = XMMatrixRotationY(m_Angle);
   Matrix _forwardRotMat = XMMatrixRotationY(m_Angle);
   Vector3 _tempVec = { 0.0f, 0.0f, -1.0f };
   m_ForwardVecOffset = XMVector3TransformNormal(_tempVec, _forwardRotMat);

   m_World = m_ScaleTM * m_RotationTM * m_TransTM;
}

void DebugCircle::Render()
{
   if (m_IsRendering == true)
   {
      // 입력 배치 객체 셋팅
      DXDevice::g_DeviceContext->IASetInputLayout(InputLayout::PosColor);
      DXDevice::g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

      // 인덱스버퍼와 버텍스버퍼 셋팅
      UINT stride = sizeof(PosColor);
      UINT offset = 0;

      ID3D11Buffer* VB;
      ID3D11Buffer* IB = m_pIB;

      if (m_CollidedCircles_v.size() > 0)
      {
         VB = m_pVBRed;
      }
      else
      {
         VB = m_pVBGreen;
      }

      DXDevice::g_DeviceContext->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
      DXDevice::g_DeviceContext->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);

      /// WVP TM등을 셋팅
      // Set constants
      Matrix worldViewProj = m_World * m_View * m_Proj;
      StaticEffect::ColorFX->SetWorldViewProj(worldViewProj);

      // 렌더스테이트
      DXDevice::g_DeviceContext->RSSetState(DXRenderState::g_WireFrame);

      m_pTech = StaticEffect::ColorFX->LightTech;

      D3DX11_TECHNIQUE_DESC techDesc;
      m_pTech->GetDesc(&techDesc);

      // 랜더패스는...
      for (UINT p = 0; p < techDesc.Passes; ++p)
      {
         m_pTech->GetPassByIndex(p)->Apply(0, DXDevice::g_DeviceContext);

         // 20개의 인덱스로 그리드를 그린다.
         DXDevice::g_DeviceContext->DrawIndexed(m_IndexCount, 0, 0);
      }
   }

   m_CollidedCircles_v.clear();
}

bool DebugCircle::CollisionCheck(DebugCircle* pTarget)
{
   float _myX = m_TransTM._41;
   float _myY = m_TransTM._43;
   float _TargetX = pTarget->m_TransTM._41;
   float _TargetY = pTarget->m_TransTM._43;

   float _distance = sqrtf(powf((_TargetX - _myX), 2) + powf((_TargetY - _myY), 2));

   if (_distance < this->m_Radius + pTarget->m_Radius && pTarget != nullptr)
   {
      this->m_IsCollided = true;
      pTarget->m_IsCollided = true;

      //m_CollidedCircles_v.push_back(true);
      //pTarget->m_CollidedCircles_v.push_back(true);
      m_CollidedCircles_v.push_back(pTarget->m_DebugIndex);
      pTarget->m_CollidedCircles_v.push_back(this->m_DebugIndex);

      return true;
   }
   else
   {
      this->m_IsCollided = false;
      pTarget->m_IsCollided = false;
      return false;
   }
}

void DebugCircle::BuildGeometryBuffer()
{
   float pi = MathHelper::Pi;

   ///////////////////////// Red

   PosColor verticesRed[32];
   for (int i = 0; i < 31; i++)
   {
      verticesRed[i].Pos = Vector3(m_Radius * sinf((2 * pi / 30) * i), 0.0f, m_Radius * -cosf((2 * pi / 30) * i));
      verticesRed[i].Color = Vector4((const float*)&Colors::Red);
   }
   verticesRed[31].Pos = { 0.0f, 0.0f, 0.0f };
   verticesRed[31].Color = Vector4((const float*)&Colors::Red);

   D3D11_BUFFER_DESC vbdRed;
   vbdRed.Usage = D3D11_USAGE_IMMUTABLE;
   vbdRed.ByteWidth = sizeof(PosColor) * 32;
   vbdRed.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   vbdRed.CPUAccessFlags = 0;
   vbdRed.MiscFlags = 0;
   vbdRed.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA vinitDataRed;
   vinitDataRed.pSysMem = verticesRed;
   HR(DXDevice::g_Device->CreateBuffer(&vbdRed, &vinitDataRed, &m_pVBRed));

   ////////////////// Green

   PosColor verticesGreen[32];
   for (int i = 0; i < 31; i++)
   {
      verticesGreen[i].Pos = Vector3(m_Radius * sinf((2 * pi / 30) * i), 0.0f, m_Radius * -cosf((2 * pi / 30) * i));
      verticesGreen[i].Color = Vector4((const float*)&Colors::Green);
   }
   verticesGreen[31].Pos = { 0.0f, 0.0f, 0.0f };
   verticesGreen[31].Color = Vector4((const float*)&Colors::Green);


   D3D11_BUFFER_DESC vbdGreen;
   vbdGreen.Usage = D3D11_USAGE_IMMUTABLE;
   vbdGreen.ByteWidth = sizeof(PosColor) * 32;
   vbdGreen.BindFlags = D3D11_BIND_VERTEX_BUFFER;
   vbdGreen.CPUAccessFlags = 0;
   vbdGreen.MiscFlags = 0;
   vbdGreen.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA vinitDataGreen;
   vinitDataGreen.pSysMem = verticesGreen;
   HR(DXDevice::g_Device->CreateBuffer(&vbdGreen, &vinitDataGreen, &m_pVBGreen));

   // 인덱스 버퍼를 생성한다.
   m_IndexCount = 32;
   UINT indicesGreen[32];
   for (int i = 0; i < m_IndexCount - 2; i++)
   {
      indicesGreen[i] = i;
   }
   indicesGreen[30] = 0;
   indicesGreen[31] = 31;


   D3D11_BUFFER_DESC ibdGreen;
   ibdGreen.Usage = D3D11_USAGE_IMMUTABLE;
   ibdGreen.ByteWidth = sizeof(UINT) * m_IndexCount;
   ibdGreen.BindFlags = D3D11_BIND_INDEX_BUFFER;
   ibdGreen.CPUAccessFlags = 0;
   ibdGreen.MiscFlags = 0;
   ibdGreen.StructureByteStride = 0;
   D3D11_SUBRESOURCE_DATA iinitDataGreen;
   iinitDataGreen.pSysMem = indicesGreen;
   HR(DXDevice::g_Device->CreateBuffer(&ibdGreen, &iinitDataGreen, &m_pIB));
}
