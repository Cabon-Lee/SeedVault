//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#define D3DUTIL_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

///----------------------------------------
#define _XM_NO_INTRINSICS_	// LEHIDE : 플랫폼 별 내장 함수를 사용하지 않고 float사용함. 예제 빌드를 위해서 추가함.

// LEHIDE : DirectX11이 Windows 8.0 SDK으로 바뀌면서 이전 라이브러리를 제거해야 할 필요성이 생겼다.
/// Windows 8.0 이상이 되면서, Windows SDK가 생기며 여기에 포함된 것이다.
// 이 처리를 해 주지 않으면 DX11용책의 예제들을 빌드 할 수 없다.

/// 이것을 지우고
//#include <d3dx11.h>			<= 'Deprecated' 더 이상 사용되지 않는다.
//#include "d3dx11Effect.h"		
//#include <xnamath.h>			<= XNA 관련 함수로, 2013년 이후로 더 이상 개발되지 않는다.
//#include <dxerr.h>
/// d3d11과 d3dx11 은 다르다. d3d11은 필수, d3dx11은 Extension으로 선택이다. 그리고 지금은 없다.
/// https://en.wikipedia.org/wiki/D3DX <- d3dx11에 대한 설명이다.
/// 요약하자면, DX7에서부터 DX9까지 발전된 High Level API library다.
/// 벡터, 행렬 연산을 비롯한 3D 관련 여러 보조적인 함수들을 제공한다.
/// 2012년 이후에는 상기된 사항대로 라이브러리가 더 이상 지원되지 않으며, 벡터와 행렬은 DirectXMath 등으로 통합됐다.

/// 이것들을 추가한다.
#include "d3dx11Effect.h"
#include "d3dcompiler.h"		// Inputlayout Reflaction에 사용
#include <wrl/client.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <DirectXPackedVector.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )	// adapter info

// DirectX 자원 생성 시 예외처리를 위한 헤더들을 여기에 넣겠다
#include "COMException.h"
#include "ErrorLogger.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

///----------------------------------------

#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <atlbase.h>		// CComPtr
#include "LightHelper.h"

#include <winerror.h> //

#ifndef D3DX11INLINE
#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#define D3DX11INLINE __forceinline
#else
#define D3DX11INLINE __inline
#endif
#else
#ifdef __cplusplus
#define D3DX11INLINE inline
#else
#define D3DX11INLINE
#endif
#endif
#endif

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release();} }
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = 0; } }	// 예전 스타일


//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }