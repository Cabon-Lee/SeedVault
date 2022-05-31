#pragma once


#include "IObject.h"
#include "IComponent.h"
#include "ComponentBase.h"
#include "ObjectBase.h"
#include "ObjectManager.h"
#include "ComponentSystem.h"


#ifdef _COMP_DLL
// DLL ����
// DLL �� ����� ����  �Լ�/Ŭ�������� "export" �� ����.
#define _API  __declspec(dllexport)					//C++
#define _TEMPLATE

#else
// DLL ���
// DLL �� ����ϱ� ���� �������. "import" �� ����.
#define _API  __declspec(dllimport)					//C++
#define _TEMPLATE extern

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "ComponentEngine_Debug.lib")		//64bit + debug
#else
#pragma comment(lib, "ComponentEngine_Release.lib")		//64bit.
#endif 
#endif   
#endif

