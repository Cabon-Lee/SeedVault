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

	IDXGIAdapter* pAdapter = nullptr;	//��ǻ���� �ϵ���� �� ����Ʈ���� ����� �߻�ȭ�� ���Դϴ�. 
										//�ٸ� ����, �ϳ� �̻��� GPU, DAC, ���� �޸𸮸� �����ϴ� ���÷��� ����ý����� ǥ���մϴ�
	DXGI_ADAPTER_DESC description;		//�׷���ī�� ����

	/*
	typedef struct DXGI_ADAPTER_DESC {
		WCHAR  Description[128];        // ����� ����,
		UINT   VendorId;                //�ϵ���� ���޾�ü�� PCI ID
		UINT   DeviceId;                //�ϵ���� ��ġ�� PCI ID
		UINT   SubSysId;                //����ý����� PCI ID
		UINT   Revision;                //����� ����(revision) ��ȣ�� PCI ID
		SIZE_T DedicatedVideoMemory;    //CPU�� �������� ���� ���� ���� �޸��� ����Ʈ ��
		SIZE_T DedicatedSystemMemory;   //CPU�� �������� �ʴ� ���� �ý��� �޷θ� ����Ʈ ��
		SIZE_T SharedSystemMemory;      //���� �ý��� �޸��� ����Ʈ ��
		LUID   AdapterLuid;             //����͸� �ĺ��ϴ� ������(Locally Unique Identifier)
	} DXGI_ADAPTER_DESC;
	*/
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
private:
	//vector�� �޴� ���� : 2���� â�� ����� �ݺ��ϰ���� �ʾƼ�
	static std::vector<AdapterData> adapters;
};

