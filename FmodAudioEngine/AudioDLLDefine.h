#pragma once

#ifdef _FMODAUDIO_DLL
// DLL 제작
// DLL 을 만들기 위해  함수/클래스등을 "export" 로 선언.
#define _FADLL  __declspec(dllexport)
//#define _TEMPLATE

#else
// DLL 사용
// DLL 을 사용하기 위한 헤더파일. "import" 로 선언.
#define _FADLL  __declspec(dllimport)
//#define _TEMPLATE extern

#endif 
