#pragma once
#include <string>

class StringHelper
{
public:
	static wchar_t* StringToWchar(std::string str);		// Added by 최 요 환

	static std::wstring StringToWide(std::string str);
	static std::string WideToString(std::wstring str);
	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);

	// ex가 true일 경우 확장자를 붙여준다
	static void ReverseString(std::string& str);
	static std::string GetFileName(std::string& name, bool ex);
};