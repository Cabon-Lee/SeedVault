#pragma once

#include "D3D11Define.h"
#include <vector> 

/// ����͸� �����ϴ� Ŭ����
/// DXGI�� ���� �����, ����ī�� ������ �����Ѵ�.
/// �����ϸ� �����, ���� ī�� ��ü, V-Sync On/Off�� �����ϸ� ��

class AdapterManager
{
public:
	AdapterManager();
	~AdapterManager();

	void Initialize(HWND hwnd, ID3D11Device* pDevice);

	// ����󵵸� ��Ÿ��
	DXGI_RATIONAL GetDXGIRational(UINT outputIndex);
	IDXGIFactory2* GetIDXGIFactory();
	DXGI_MODE_DESC* GetDXGIModeDesc(int idx);

	// �׷��� ī��/����Ϳ� ���� ������ �������� �Լ�
	bool GetMonitorDesc(size_t idx, DXGI_OUTPUT_DESC& output);
	bool GetVideoCardDesc(size_t idx, DXGI_ADAPTER_DESC& adapter);

	// �� �Լ��� ���� �� �߰� �ɰ���, ���� ���ϰ� DXGI_OUTPUT_DESC��� ����ü�� �� �ʿ䰡 ����
	RECT GetMonitorCoordinates(int idx);
	std::string GetMonitorName(int idx);
	std::string GetVideoCardName(int idx);
	int GetVideoCardMemory(int idx);


	// https://docs.microsoft.com/ko-kr/windows/win32/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
	// �ش� ���ȿ��� ���� ü�� �� ���� ResizeBuffers�� ���õ� ������ Ȯ���� �� �ִ�.
	// DXGI_MWA_NO_WINDOW_CHANGES	DXGI�� ���ø����̼� �޽��� ��⿭�� ����͸����� ���ϵ��� �մϴ�. 
	//								�̷� ���� DXGI�� ��� ���濡 ������ �� �����ϴ�.
	// DXGI_MWA_NO_ALT_ENTER		DXGI�� Alt-Enter �������� �������� �ʵ��� �մϴ�.
	// DXGI_MWA_NO_PRINT_SCREEN		DXGI�� �μ� ȭ�� Ű�� �������� �ʵ��� �մϴ�.
	void WindowMessageMonitoring(UINT flag);

private:
	void SetOutputInfo(CComPtr<IDXGIOutput> output);	// ����� ����
	void SetAdapterInfo(CComPtr<IDXGIAdapter> adapter);	// ����ī�� ����

private:
	// ������ ���� ����
	HWND m_hWnd;

	// ����ü���� ���鶧�� ���Ǵ� ���丮
	CComPtr<IDXGIFactory2> m_DXGIFactory;


private:
	std::vector<CComPtr<IDXGIOutput>> m_Outputs_V;
	std::vector<DXGI_MODE_DESC*> m_DXGIMode_V;
	std::vector<CComPtr<IDXGIAdapter>> m_Adapters_V;
	/*
	typedef struct DXGI_MODE_DESC {
	  UINT                     Width;				// �ػ� ����
	  UINT                     Height;				// �ػ� ����
	  DXGI_RATIONAL            RefreshRate;			// �츣���� ��Ÿ����
	  DXGI_FORMAT              Format;				// ���÷����� ����(DXGI_FORMAT_R32_UNIT ��)
	  DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;	//
	  DXGI_MODE_SCALING        Scaling;				// �ػ󵵿� �°� �̹����� �����ϸ� ó���� ��� ����
	} DXGI_MODE_DESC;
	*/

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

	std::vector<int> m_VideoMemory_V;
	std::vector<std::string> m_VideoDesc_V;
	std::vector<RECT> m_DesktopCoordinates_V;
};