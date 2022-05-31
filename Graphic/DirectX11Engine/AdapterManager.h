#pragma once

#include "D3D11Define.h"
#include <vector> 

/// 어댑터를 관리하는 클래스
/// DXGI를 통해 모니터, 비디오카드 정보를 관리한다.
/// 가능하면 모니터, 비디오 카드 교체, V-Sync On/Off도 가능하면 굳

class AdapterManager
{
public:
	AdapterManager();
	~AdapterManager();

	void Initialize(HWND hwnd, ID3D11Device* pDevice);

	// 재생빈도를 나타냄
	DXGI_RATIONAL GetDXGIRational(UINT outputIndex);
	IDXGIFactory2* GetIDXGIFactory();
	DXGI_MODE_DESC* GetDXGIModeDesc(int idx);

	// 그래픽 카드/모니터에 대한 정보를 가져오는 함수
	bool GetMonitorDesc(size_t idx, DXGI_OUTPUT_DESC& output);
	bool GetVideoCardDesc(size_t idx, DXGI_ADAPTER_DESC& adapter);

	// 위 함수를 조금 더 잘게 쪼갠것, 쓰기 편하고 DXGI_OUTPUT_DESC라는 구조체를 알 필요가 없다
	RECT GetMonitorCoordinates(int idx);
	std::string GetMonitorName(int idx);
	std::string GetVideoCardName(int idx);
	int GetVideoCardMemory(int idx);


	// https://docs.microsoft.com/ko-kr/windows/win32/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
	// 해당 토픽에서 스왑 체인 백 버퍼 ResizeBuffers와 관련된 내용을 확인할 수 있다.
	// DXGI_MWA_NO_WINDOW_CHANGES	DXGI가 애플리케이션 메시지 대기열을 모니터링하지 못하도록 합니다. 
	//								이로 인해 DXGI는 모드 변경에 응답할 수 없습니다.
	// DXGI_MWA_NO_ALT_ENTER		DXGI가 Alt-Enter 시퀀스에 응답하지 않도록 합니다.
	// DXGI_MWA_NO_PRINT_SCREEN		DXGI가 인쇄 화면 키에 응답하지 않도록 합니다.
	void WindowMessageMonitoring(UINT flag);

private:
	void SetOutputInfo(CComPtr<IDXGIOutput> output);	// 모니터 정보
	void SetAdapterInfo(CComPtr<IDXGIAdapter> adapter);	// 비디오카드 정보

private:
	// 윈도우 정보 관련
	HWND m_hWnd;

	// 스왑체인을 만들때도 사용되는 팩토리
	CComPtr<IDXGIFactory2> m_DXGIFactory;


private:
	std::vector<CComPtr<IDXGIOutput>> m_Outputs_V;
	std::vector<DXGI_MODE_DESC*> m_DXGIMode_V;
	std::vector<CComPtr<IDXGIAdapter>> m_Adapters_V;
	/*
	typedef struct DXGI_MODE_DESC {
	  UINT                     Width;				// 해상도 가로
	  UINT                     Height;				// 해상도 세로
	  DXGI_RATIONAL            RefreshRate;			// 헤르츠를 나타낸다
	  DXGI_FORMAT              Format;				// 디스플레이의 포맷(DXGI_FORMAT_R32_UNIT 등)
	  DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;	//
	  DXGI_MODE_SCALING        Scaling;				// 해상도에 맞게 이미지의 스케일링 처리를 어떻게 할지
	} DXGI_MODE_DESC;
	*/

	/*
	typedef struct DXGI_ADAPTER_DESC {
		WCHAR  Description[128];        // 어댑터 설명,
		UINT   VendorId;                //하드웨어 공급업체의 PCI ID
		UINT   DeviceId;                //하드웨어 장치의 PCI ID
		UINT   SubSysId;                //서브시스템의 PCI ID
		UINT   Revision;                //어댑터 개정(revision) 번호의 PCI ID
		SIZE_T DedicatedVideoMemory;    //CPU와 공유되지 않은 전용 비디오 메모리의 바이트 수
		SIZE_T DedicatedSystemMemory;   //CPU와 공유되지 않는 전용 시스템 메로리 바이트 수
		SIZE_T SharedSystemMemory;      //공유 시스템 메모리의 바이트 수
		LUID   AdapterLuid;             //어댑터를 식별하는 고유값(Locally Unique Identifier)
	} DXGI_ADAPTER_DESC;
	*/

	std::vector<int> m_VideoMemory_V;
	std::vector<std::string> m_VideoDesc_V;
	std::vector<RECT> m_DesktopCoordinates_V;
};