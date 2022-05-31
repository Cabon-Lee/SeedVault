#pragma once
#include "ErrorLogger.h"
//#pragma comment(lib,"d3d11.lib")
//#pragma comment(lib,"DirectXTK.lib")
//#pragma comment(lib,"DXGI.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <vector> 

class AdapterData
{
public:
	AdapterData(IDXGIAdapter* pAdapter);

	IDXGIAdapter* pAdapter = nullptr;	//컴퓨터의 하드웨어 및 소프트웨어 기능을 추상화한 것입니다. 
										//다른 말로, 하나 이상의 GPU, DAC, 비디오 메모리를 포함하는 디스플레이 서브시스템을 표현합니다
	DXGI_ADAPTER_DESC description;		//그래픽카드 정보

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
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	//vector로 받는 이유 : 2개의 창을 띄울경우 반복하고싶지 않아서
	static std::vector<AdapterData> adapters;
};

