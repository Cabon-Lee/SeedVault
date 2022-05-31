#pragma once

#ifdef _ID_EXPORT
#define  _ID_DLL __declspec(dllexport)


#else
#define  _ID_DLL __declspec(dllimport)


#endif // 

//#pragma data_seg(".SHARE")
//#pragma data_seg()