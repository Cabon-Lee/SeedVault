//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#define D3DUTIL_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

///----------------------------------------
#define _XM_NO_INTRINSICS_	// LEHIDE : �÷��� �� ���� �Լ��� ������� �ʰ� float�����. ���� ���带 ���ؼ� �߰���.

// LEHIDE : DirectX11�� Windows 8.0 SDK���� �ٲ�鼭 ���� ���̺귯���� �����ؾ� �� �ʿ伺�� �����.
/// Windows 8.0 �̻��� �Ǹ鼭, Windows SDK�� ����� ���⿡ ���Ե� ���̴�.
// �� ó���� �� ���� ������ DX11��å�� �������� ���� �� �� ����.

/// �̰��� �����
//#include <d3dx11.h>			<= 'Deprecated' �� �̻� ������ �ʴ´�.
//#include "d3dx11Effect.h"		
//#include <xnamath.h>			<= XNA ���� �Լ���, 2013�� ���ķ� �� �̻� ���ߵ��� �ʴ´�.
//#include <dxerr.h>
/// d3d11�� d3dx11 �� �ٸ���. d3d11�� �ʼ�, d3dx11�� Extension���� �����̴�. �׸��� ������ ����.
/// https://en.wikipedia.org/wiki/D3DX <- d3dx11�� ���� �����̴�.
/// ������ڸ�, DX7�������� DX9���� ������ High Level API library��.
/// ����, ��� ������ ����� 3D ���� ���� �������� �Լ����� �����Ѵ�.
/// 2012�� ���Ŀ��� ���� ���״�� ���̺귯���� �� �̻� �������� ������, ���Ϳ� ����� DirectXMath ������ ���յƴ�.

/// �̰͵��� �߰��Ѵ�.
#include "d3dx11Effect.h"
#include "d3dcompiler.h"		// Inputlayout Reflaction�� ���
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

// DirectX �ڿ� ���� �� ����ó���� ���� ������� ���⿡ �ְڴ�
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
#define SAFE_RELEASE(x) { if(x){ x->Release(); x = 0; } }	// ���� ��Ÿ��


//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }