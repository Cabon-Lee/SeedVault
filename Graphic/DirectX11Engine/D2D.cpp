#include "D2D.h"
#include "UIData.h"
#include "FontInfo.h"
#include "EffectUIEnum.h"

#include "ComException.h"
#include "ErrorLogger.h"

D2D::D2D()
{
}

D2D::~D2D()
{
	//m_TextLayOut->Release();
	//m_TextColor->Release();
}

HRESULT D2D::Initialize(HWND _hWnd, IDXGISwapChain1* _3D_SwapChain, int width, int height, float _Font_Size)
{
	HRESULT _hr;
	try
	{
		g_hWnd = _hWnd;
		// 백버퍼 가져오기

		_hr = _3D_SwapChain->GetBuffer(0, IID_PPV_ARGS(m_3D_BackBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(_hr, "_3D_SwapChain GetBuffer Fail");

		DXGI_SURFACE_DESC* _nowDesc = new DXGI_SURFACE_DESC;
		m_3D_BackBuffer->GetDesc(_nowDesc);

		_hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_2D_Factory);
		COM_ERROR_IF_FAILED(_hr, "D2D1CreateFactory Fail");

		// 화면의 dpi를 가져옴.
		float dpi;
		dpi = GetDpiForWindow(_hWnd);

		// 생성할 RenderTarget 에 대한 정보 설정.
		m_Property =
			D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
				dpi, dpi);

		// DXGI 를 활용해 3D 백버퍼에서 2D 랜더타겟을 생성.
		_hr = m_2D_Factory->CreateDxgiSurfaceRenderTarget(m_3D_BackBuffer.Get(), &m_Property, m_2D_RenderTarget.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateDxgiSurfaceRenderTarget Fail");

		// pWriteFactory5 생성
		_hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_WriteFactory),
			reinterpret_cast<IUnknown**>(&m_WriteFactory));
		COM_ERROR_IF_FAILED(_hr, "DWriteCreateFactory Fail");

		m_ClientWidth = width;
		m_ClientHeight = height;
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	return _hr;
}

void D2D::OnResize(IDXGISwapChain1* _3D_SwapChain, int width, int height)
{
	HRESULT _hr;
	try
	{
		m_2D_RenderTarget.Reset();
		m_3D_BackBuffer.Reset();

		_hr = _3D_SwapChain->GetBuffer(0, IID_PPV_ARGS(m_3D_BackBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(_hr, "_3D_SwapChain GetBuffer Fail");

		DXGI_SURFACE_DESC* _nowDesc = new DXGI_SURFACE_DESC;
		m_3D_BackBuffer->GetDesc(_nowDesc);

		// DXGI 를 활용해 3D 백버퍼에서 2D 랜더타겟을 생성.
		_hr = m_2D_Factory->CreateDxgiSurfaceRenderTarget(m_3D_BackBuffer.Get(), &m_Property, m_2D_RenderTarget.GetAddressOf());
		COM_ERROR_IF_FAILED(_hr, "CreateDxgiSurfaceRenderTarget Fail");

		D2D1_SIZE_F targetSize = m_2D_RenderTarget->GetSize();

		m_ClientWidth = width;
		m_ClientHeight = height;

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void D2D::Push_DrawText(std::shared_ptr<TextBlock> pTextBlock)
{
	//m_textInfo_V = textInfo_V;
	//m_textBuffer_V = textBuffer_V;
	//m_setupConverPoint_V = setupConverPoint_V;	

	m_TextBlock_Q.push(pTextBlock); 

	//m_textInfo_V.insert(m_textInfo_V.end(), textInfo_V.begin(), textInfo_V.end());
	//m_textBuffer_V.insert(m_textBuffer_V.end(), textBuffer_V.begin(), textBuffer_V.end());
	//m_setupConverPoint_V.insert(m_setupConverPoint_V.end(), setupConverPoint_V.begin(), setupConverPoint_V.end());
}

//void D2D::Push_DrawText(POINT _Pos, float _Width, float r, float g, float b, float a, float _Size, const wchar_t* _Input_String, ...)
//{
//	// 버퍼 준비 최대 글자수 128
//	wchar_t* pStr = new wchar_t[128];
//
//	va_list args;
//	// 가변인자의 위치를 처음으로 셋팅하기 위함.
//	va_start(args, _Input_String);
//
//	// 우리가 준비한 버퍼에 완성된 결과를 저장.
//	vswprintf(pStr, 128, _Input_String, args);
//	va_end(args);
//
//	Text_Queue_Data* Queue_Data = new Text_Queue_Data;
//
//	Queue_Data->Position_X = _Pos.x;
//	Queue_Data->Position_Y = _Pos.y;
//	Queue_Data->Text_Width = _Width;
//
//	Queue_Data->Color_R = r;
//	Queue_Data->Color_G = g;
//	Queue_Data->Color_B = b;
//	Queue_Data->Color_Alpha = a;
//
//	Queue_Data->Text_Size = _Size;
//	Queue_Data->Text_String = pStr;
//
//	Msg_Queue.push(Queue_Data);
//}

unsigned int D2D::FontInitialize(std::shared_ptr<Text_Queue_Info> pFontDesc)
{
	std::shared_ptr<FontInfo> _nowFontInfo = std::make_shared<FontInfo>();
	_nowFontInfo->Initialize(m_WriteFactory, pFontDesc);

	m_FontInfo_V.push_back(_nowFontInfo);

	return m_FontInfo_V.size() - 1;
}


void D2D::Draw_AllText()
{


	D2D1_SIZE_F targetSize = m_2D_RenderTarget->GetSize();
	m_2D_RenderTarget->BeginDraw();

	while (!m_TextBlock_Q.empty())
	{

		//행간을 설정값에따라 다르게 적용하기 위한 값
		std::shared_ptr<FontInfo> _nowFontInfo = m_FontInfo_V[m_TextBlock_Q.front()->fontInfoIndex];


		int _lineSpace = _nowFontInfo->m_FontSize + _nowFontInfo->m_LineSpace;

		m_2D_RenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(_nowFontInfo->m_R,
				_nowFontInfo->m_G,
				_nowFontInfo->m_B,
				_nowFontInfo->m_A), &m_TextColor);

		std::vector<std::shared_ptr<TextString>> _nowTextStrings;
		_nowTextStrings.swap(m_TextBlock_Q.front()->textString_V);

		int _nowLineSpace = 0;
		for (unsigned int i = 0; i < _nowTextStrings.size(); i++)
		{
			_nowLineSpace += _lineSpace;

			m_WriteFactory->CreateTextLayout(
				_nowTextStrings[i]->text,
				wcslen(_nowTextStrings[i]->text),
				_nowFontInfo->m_TextFormat,
				_nowFontInfo->m_LineWidth,
				std::numeric_limits<float>::infinity(),
				&_nowFontInfo->m_TextLayOut);

			DWRITE_TEXT_RANGE textRange;
			textRange.startPosition = 0;
			textRange.length = wcslen(_nowTextStrings[i]->text);

			//_nowFontInfo->m_TextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			_nowFontInfo->m_TextLayOut->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			_nowFontInfo->m_TextLayOut->SetFontSize(_nowFontInfo->m_FontSize, textRange);

			DWRITE_TEXT_METRICS dtm;
			_nowFontInfo->m_TextLayOut->GetMetrics(&dtm);
			float minHeight = dtm.height;
			float minWidth = dtm.widthIncludingTrailingWhitespace;


			/// 중심이되는 모서리에따라 포지션 변경됨
			float _Xpos = 0;
			float _Ypos = 0;

			switch (_nowFontInfo->m_textPivotType)
			{
			case eUIAxis::None:
			case eUIAxis::LeftUp:
				_Xpos = _nowFontInfo->m_PosX;
				_Ypos = _nowFontInfo->m_PosY;
				break;
			case eUIAxis::RightUp:
				_Xpos = m_ClientWidth - _nowFontInfo->m_PosX;
				_Ypos = _nowFontInfo->m_PosY;
				break;
			case eUIAxis::RightDown:
				_Xpos = m_ClientWidth - _nowFontInfo->m_PosX;
				_Ypos = m_ClientHeight - _nowFontInfo->m_PosY;
				break;
			case eUIAxis::LeftDown:
				_Xpos = _nowFontInfo->m_PosX;
				_Ypos = m_ClientHeight - _nowFontInfo->m_PosY;
				break;
			case eUIAxis::Center:
				_Xpos = m_ClientWidth / 2 + _nowFontInfo->m_PosX;
				_Ypos = m_ClientHeight / 2 + _nowFontInfo->m_PosY;
				break;
			}

			switch (_nowFontInfo->m_textPoint)
			{
			case eTextPoint::LeftUP :
				m_2D_RenderTarget->DrawTextLayout(
					D2D1::Point2F(_Xpos , _Ypos),
					_nowFontInfo->m_TextLayOut, m_TextColor);				
				break;

			case eTextPoint::Center:
				m_2D_RenderTarget->DrawTextLayout(
					D2D1::Point2F(_Xpos - minWidth / 2, _Ypos - minHeight / 2),
					_nowFontInfo->m_TextLayOut, m_TextColor);
				break;
			}

		}

		m_TextBlock_Q.pop();
	}

	m_2D_RenderTarget->EndDraw();
}