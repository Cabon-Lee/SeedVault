#include "DemoProcess.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					  _In_opt_ HINSTANCE hPrevInstance,
					  _In_ LPWSTR    lpCmdLine,
					  _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DemoProcess* _pGameDemo = new DemoProcess;
	_pGameDemo->Initialize(hInstance);
	_pGameDemo->Loop();
	_pGameDemo->Finalize();

	delete _pGameDemo;
	_pGameDemo = nullptr;



	return 0;
}
