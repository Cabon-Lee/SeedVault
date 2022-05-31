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
	// ���� �ڵ�
	HWND g_hWnd;
	// DX11���� ������ ����ü��.
	IDXGISwapChain1* m_3D_SwapChain = nullptr;
	// �����
	Microsoft::WRL::ComPtr<IDXGISurface> m_3D_BackBuffer;
	// Direct2D Factory
	ID2D1Factory* m_2D_Factory = nullptr;
	// 3D�� ���� ������ 2D RenderTarget
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> m_2D_RenderTarget;
	// ������ RenderTarget �� ���� ������.
	D2D1_RENDER_TARGET_PROPERTIES m_Property;

	// Ŀ���� ���� �ε带 ���� ���丮
	IDWriteFactory5* m_WriteFactory = nullptr;
	// �ؽ�Ʈ Į��
	ID2D1SolidColorBrush* m_TextColor = nullptr;

	std::queue<std::shared_ptr<TextBlock>> m_TextBlock_Q;

	std::vector<std::shared_ptr<FontInfo>> m_FontInfo_V;

	unsigned int m_ClientWidth = 0;
	unsigned int m_ClientHeight = 0;

	// ��Ʈ ������
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

