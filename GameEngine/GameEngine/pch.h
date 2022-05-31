#pragma once
//#include <crtdbg.h>

#include <typeinfo>
#include <cassert>
#include <ctime>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <winerror.h>
#include <tchar.h>
#include <stdarg.h>
#include <stdio.h>
#include <thread>
#include "Singleton.h"

// 디버깅 관련 
#include "CATrace.h"
 
// 수학 관련
#include <DirectXMath.h>
#include "MathHelper.h"
#include "SimpleMath.h"

// 물리 관련
#include "PxPhysics.h" // PhysX DLL
#include "PxPhysicsAPI.h"
using namespace physx;
#include "PhysicsEngine.h"
#include "Collision.h"
//#include "PhysicsGeometry.h"
//#include "PhysicsUtility.h"

//제이슨 저장 관련
#include <fstream>
#include "BoostDescribeAssist.h"
#include "JsonReflectionHelper.h"

using namespace DirectX::SimpleMath;
using namespace std;

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)																		\
	{																					\
		HRESULT hr = (x);															\
		if(FAILED(hr))																\
		{																				\
			LPWSTR output;                                 				\
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |					\
							  FORMAT_MESSAGE_IGNORE_INSERTS |				\
							  FORMAT_MESSAGE_ALLOCATE_BUFFER,				\
							  NULL,													\
							  hr,														\
							  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
							  (LPTSTR) &output,									\
							  0,														\
							  NULL);													\
			MessageBox(NULL, output, L"Error", MB_OK);					\
		}																				\
	}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

// COM 객체를 편하게 릴리즈하기 위해
#define Safe_Release(x) { if(x) { x->release(); x = nullptr; } } // 예전 스타일
#define Safe_Delete(x) if(x) { delete x; x = nullptr; }


