#include "AdapterManager.h"

#include "StringHelper.h"
#include "COMException.h"
#include "ErrorLogger.h"

AdapterManager::AdapterManager() :
	m_hWnd(0),
	m_DXGIFactory(nullptr)
{

}

AdapterManager::~AdapterManager()
{

}

void AdapterManager::Initialize(HWND hwnd, ID3D11Device* pDevice)
{
	m_hWnd = hwnd;

	//---------------------------------------------------------------------------------------
	// 어뎁터 정보 가져오기 위한 작업
	//---------------------------------------------------------------------------------------
	try
	{
		HRESULT hr;
		CComPtr<IDXGIDevice> _dxgiDevice = nullptr;
		hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&_dxgiDevice);

		COM_ERROR_IF_FAILED(hr, "ID3D11Device QueryInterface 실패");

		CComPtr < IDXGIAdapter> _dxgiAdapter = nullptr;
		hr = _dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&_dxgiAdapter);

		COM_ERROR_IF_FAILED(hr, "ID3D11Device GetParent 실패");

		hr = _dxgiAdapter->GetParent(IID_PPV_ARGS(&m_DXGIFactory));

		COM_ERROR_IF_FAILED(hr, "DxgiAdpter GetParent 실패");

		UINT _OutputCnt = 0;
		UINT _AdapterCnt = 0;

		// 그래픽 카드/모니터 정보를 담을 CCOmPtr
		CComPtr < IDXGIAdapter> _pAdapter = nullptr;
		CComPtr < IDXGIOutput> _pOutput = nullptr;

		while (m_DXGIFactory->EnumAdapters(_AdapterCnt, &_pAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			while (_pAdapter->EnumOutputs(_OutputCnt, &_pOutput) != DXGI_ERROR_NOT_FOUND)
			{
				SetOutputInfo(_pOutput);
				m_Outputs_V.push_back(_pOutput);
				++_OutputCnt;
				_pOutput = nullptr;
			}

			SetAdapterInfo(_pAdapter);
			m_Adapters_V.push_back(_pAdapter);
			++_AdapterCnt;

			_pAdapter = nullptr;
		}


		for (CComPtr<IDXGIOutput>& _nowIDXGIOutput : m_Outputs_V)
		{
			// 최종적으로 디스플레이 Mode의 상세를 알기 위한 과정
			UINT _numModes = 0;
			DXGI_FORMAT _format = DXGI_FORMAT_R8G8B8A8_UNORM;
			UINT _flags = DXGI_ENUM_MODES_INTERLACED;

			// GetDisplayModeList의 마지막 인자에 NULL을 넘겨서 _numModes의 수를 파악한다.
			hr = _nowIDXGIOutput->GetDisplayModeList(_format, _flags, &_numModes, NULL);

			COM_ERROR_IF_FAILED(hr, "IDXGIOutput GetDisplayModeList 실패");

			// _numModes의 크기에 맞게 DXGI_MODE_DESC의 포인터(배열)를 생성하고, 이를 인자로 넘겨 채워온다.
			DXGI_MODE_DESC* _pDescs = new DXGI_MODE_DESC[_numModes];
			hr = _nowIDXGIOutput->GetDisplayModeList(_format, _flags, &_numModes, _pDescs);

			COM_ERROR_IF_FAILED(hr, "IDXGIOutput GetDisplayModeList 실패");

			m_DXGIMode_V.push_back(_pDescs);
		}
	}
	catch(COMException& e)
	{
		ErrorLogger::Log(e);
	}
}


DXGI_RATIONAL AdapterManager::GetDXGIRational(UINT outputIndex)
{
	return m_DXGIMode_V[outputIndex]->RefreshRate;
}

IDXGIFactory2* AdapterManager::GetIDXGIFactory()
{
	return m_DXGIFactory;
}

DXGI_MODE_DESC* AdapterManager::GetDXGIModeDesc(int idx)
{
	return m_DXGIMode_V[idx];
}

bool AdapterManager::GetMonitorDesc(size_t idx, DXGI_OUTPUT_DESC& output)
{
	// idx가 현재 사이즈와 같거나 크다면, 더 이상 줄 수 있는게 없으므로
	if (m_Outputs_V.size() <= idx)
	{
		return false;
	}
	else
	{
		m_Outputs_V[idx]->GetDesc(&output);
		return true;
	}
}

bool AdapterManager::GetVideoCardDesc(size_t idx, DXGI_ADAPTER_DESC& adapter)
{
	if (m_Adapters_V.size() <= idx)
	{
		return false;
	}
	else
	{
		m_Adapters_V[idx]->GetDesc(&adapter);
		return true;
	}
}

RECT AdapterManager::GetMonitorCoordinates(int idx)
{
	DXGI_OUTPUT_DESC _nowdesc;
	m_Outputs_V[idx]->GetDesc(&_nowdesc);
	
	return  _nowdesc.DesktopCoordinates;
}

std::string AdapterManager::GetMonitorName(int idx)
{
	DXGI_OUTPUT_DESC _nowdesc;
	m_Outputs_V[idx]->GetDesc(&_nowdesc);

	return  StringHelper::WideToString(_nowdesc.DeviceName);
}

std::string AdapterManager::GetVideoCardName(int idx)
{
	DXGI_ADAPTER_DESC _nowdesc;
	m_Adapters_V[idx]->GetDesc(&_nowdesc);

	return  StringHelper::WideToString(_nowdesc.Description);
}

int AdapterManager::GetVideoCardMemory(int idx)
{
	DXGI_ADAPTER_DESC _nowdesc;
	m_Adapters_V[idx]->GetDesc(&_nowdesc);

	return ((int)(_nowdesc.DedicatedVideoMemory / 1024));
}


void AdapterManager::WindowMessageMonitoring(UINT flag)
{
	m_DXGIFactory->MakeWindowAssociation(m_hWnd, flag);
}

void AdapterManager::SetOutputInfo(CComPtr<IDXGIOutput> output)
{
	DXGI_OUTPUT_DESC _dxgiAOutput;
	output->GetDesc(&_dxgiAOutput);

	m_DesktopCoordinates_V.push_back(_dxgiAOutput.DesktopCoordinates);
}

void AdapterManager::SetAdapterInfo(CComPtr<IDXGIAdapter> adapter)
{
	// 어뎁터의 정보를 가져와서 넣는다.
	DXGI_ADAPTER_DESC _dxgiAdapter;
	adapter->GetDesc(&_dxgiAdapter);

	m_VideoMemory_V.push_back((int)(_dxgiAdapter.DedicatedVideoMemory / 1024));
	m_VideoDesc_V.push_back(StringHelper::WideToString(_dxgiAdapter.Description));
}
