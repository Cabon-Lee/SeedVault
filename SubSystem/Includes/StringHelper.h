#pragma once
#include <string>

class StringHelper
{
public:
	static wchar_t* StringToWchar(std::string str);		// Added by YoKing

	static std::wstring StringToWide(std::string str);
	static std::string WideToString(std::wstring str);
	static std::string GetDirectoryFromPath(const std::string& filepath);
	static std::string GetFileExtension(const std::string& filename);

	// ex�� true�� ��� Ȯ���ڸ� �ٿ��ش�
	static void ReverseString(std::string& str);
	static std::string GetFileName(std::string& name, bool ex);
};