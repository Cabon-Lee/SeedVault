#pragma once


#include "IObject.h"
#include "IComponent.h"
#include "ComponentBase.h"
#include "ObjectBase.h"
#include "ObjectManager.h"
#include "ComponentSystem.h"


#ifdef _COMP_DLL
// DLL 제작
// DLL 을 만들기 위해  함수/클래스등을 "export" 로 선언.
#define _API  __declspec(dllexport)					//C++
#define _TEMPLATE

#else
// DLL 사용
// DLL 을 사용하기 위한 헤더파일. "import" 로 선언.
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

