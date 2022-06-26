#pragma once
#include <dxgi1_2.h>
#pragma comment(lib, "WindowsCodecs.lib")
#include <wincodec.h>
#pragma comment(lib, "d2d1")
#include <d2d1.h>
#pragma comment(lib, "Dwrite")
#include <dwrite.h>
#include <dwrite_3.h>

#include <wrl.h>
#include <queue>
#include <memory>

struct FontInfo;

struct Text_Queue_Info;
struct TextString;
struct TextBlock;



class D2D
{
private:
	// 메인 핸들
	HWND g_hWnd;
	// DX11에서 생성된 스왑체인.
	IDXGISwapChain1* m_3D_SwapChain = nullptr;
	// 백버퍼
	Microsoft::WRL::ComPtr<IDXGISurface> m_3D_BackBuffer;
	// Direct2D Factory
	ID2D1Factory* m_2D_Factory = nullptr;
	// 3D로 부터 생성된 2D RenderTarget
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_2D_RenderTarget;
	// 생성할 RenderTarget 에 대한 정보값.
	D2D1_RENDER_TARGET_PROPERTIES m_Property;

	// 커스텀 폰드 로드를 위한 팩토리
	IDWriteFactory5* m_WriteFactory = nullptr;
	// 텍스트 칼러
	ID2D1SolidColorBrush* m_TextColor = nullptr;

	std::queue<std::shared_ptr<TextBlock>> m_TextBlock_Q;

	std::vector<std::shared_ptr<FontInfo>> m_FontInfo_V;

	unsigned int m_ClientWidth = 0;
	unsigned int m_ClientHeight = 0;

	// 폰트 정보들
	//std::vector<FontInfo*> m_fontInfo_V;
	//std::vector<Text_Queue_Info*> m_textInfo_V;
	//std::vector<TextString*> m_textBuffer_V;
	//std::vector<int> m_setupConverPoint_V;

public:
	D2D();
	~D2D();

	HRESULT Initialize(HWND _hWnd, IDXGISwapChain1* _3D_SwapChain, int width, int height, float _Font_Size = 24.f);

	void OnResize(IDXGISwapChain1* _3D_SwapChain, int width, int height);

	unsigned int FontInitialize(std::shared_ptr<Text_Queue_Info> pFontDesc);
	void Push_DrawText(std::shared_ptr<TextBlock> pTextBlock);
	//void Push_DrawText(POINT _Pos, float _Width,
	//	float r, float g, float b, float a,
	//	float _Size, const wchar_t* _Input_String, ...);

	void Draw_AllText();
};

