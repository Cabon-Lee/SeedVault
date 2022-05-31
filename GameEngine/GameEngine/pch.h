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

// ����� ���� 
#include "CATrace.h"
 
// ���� ����
#include <DirectXMath.h>
#include "MathHelper.h"
#include "SimpleMath.h"

// ���� ����
#include "PxPhysics.h" // PhysX DLL
#include "PxPhysicsAPI.h"
using namespace physx;
#include "PhysicsEngine.h"
#include "Collision.h"
//#include "PhysicsGeometry.h"
//#include "PhysicsUtility.h"

//���̽� ���� ����
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

// COM ��ü�� ���ϰ� �������ϱ� ����
#define Safe_Release(x) { if(x) { x->release(); x = nullptr; } } // ���� ��Ÿ��
#define Safe_Delete(x) if(x) { delete x; x = nullptr; }


