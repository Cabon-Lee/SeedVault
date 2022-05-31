#pragma once

class Camera;

/// 카메라가 여러개일 경우가 있을것임
/// 그래서 매니저를 만들어 두었음

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

   //std::vector<Camera*> m_Camera_V; // 아직 어떻게 써야할지, 
                                    // 액션카메라나 미리동선이 세팅된 카메라를 저장하고 불러올것 같은데 map으로 저장?
   Camera* m_pNowCamera;

   // 리플렉션 프로브를 위해서 추가된 멤버변수
   DirectX::SimpleMath::Matrix m_ViewTM;
   DirectX::SimpleMath::Matrix m_ProjTM;

   POINT m_PrevMousePos;
   POINT m_CurrMousePos;

   HWND m_hWnd;
};

