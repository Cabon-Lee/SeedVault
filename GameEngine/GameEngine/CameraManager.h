#pragma once

class Camera;

/// ī�޶� �������� ��찡 ��������
/// �׷��� �Ŵ����� ����� �ξ���

class CameraManager
{
public:
   CameraManager();
   ~CameraManager();

   void SetHWND(HWND hWnd) ;
   void SetNowCamera(std::string name);
   void SetNowCamera(unsigned int index);
   Camera* GetNowCamera();
   Camera* GetCamera(unsigned int index);
   void AddCamera(Camera* pCamera, std::string name);
   void RemoveAllCamera();

   DirectX::SimpleMath::Matrix& GetViewTM();
   DirectX::SimpleMath::Matrix& GetProjTM();


   void SetLensOnResize(int width, int height);
  
   unsigned int GetCameraCount();

private:
   std::unordered_map<std::string, Camera*> m_Camera_UM;
   std::unordered_map<std::string, int> m_test_UM;

   //std::vector<Camera*> m_Camera_V; // ���� ��� �������, 
                                    // �׼�ī�޶� �̸������� ���õ� ī�޶� �����ϰ� �ҷ��ð� ������ map���� ����?
   Camera* m_pNowCamera;

   // ���÷��� ���κ긦 ���ؼ� �߰��� �������
   DirectX::SimpleMath::Matrix m_ViewTM;
   DirectX::SimpleMath::Matrix m_ProjTM;

   POINT m_PrevMousePos;
   POINT m_CurrMousePos;

   HWND m_hWnd;
};

