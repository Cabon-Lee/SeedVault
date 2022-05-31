#include "pch.h"
#include <locale>
#include "StringHelper.h"
#include <algorithm>
#include <memory>


/// <summary>
/// std::string 을 wchar_t* 로 바꿔 리턴
/// 단 만들어서 리턴은 해주지만 해제는 여기서 할 수 없으므로
/// 호출한 쪽에서 잘 사용하고 반드시 해제해줘야 할 것이다..
/// (매우 위험해 보이는데?)
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
wchar_t* StringHelper::StringToWchar(std::string str)
{
	// 지역 설정, 안하면 한글이 깨지게 된다..
	setlocale(LC_ALL, "Korean");

	// 사이즈 구하고
	const size_t cSize = strlen(str.c_str()) + 1;

	// 사이즈만큼 동적할당
	wchar_t* text = new wchar_t[cSize];

	// std::string 을 wchar_t로 변환해서 text에 넣는다
	mbstowcs_s(nullptr, text, cSize, str.c_str(), cSize);

	// 호출한쪽에서 사용하고 반드시 delete 해줘야 메모리릭 안날것이다....
	return text;
}

std::wstring StringHelper::StringToWide(std::string str)
{
	std::wstring wide_string(str.begin(), str.end());
	return wide_string;
}

std::string StringHelper::WideToString(std::wstring str)
{
	std::string _nowString(str.begin(), str.end());
	return _nowString;
}

std::string StringHelper::GetDirectoryFromPath(const std::string& filepath)
{
	size_t off1 = filepath.find_last_of('\\');
	size_t off2 = filepath.find_last_of('/');
	if (off1 == std::string::npos && off2 == std::string::npos) //If no slash or backslash in path?
	{
		return "";
	}
	if (off1 == std::string::npos)
	{
		return filepath.substr(0, off2);
	}
	if (off2 == std::string::npos)
	{
		return filepath.substr(0, off1);
	}
	//If both exists, need to use the greater offset
	return filepath.substr(0, (off1 > off2) ? off1 : off2);
}

std::string StringHelper::GetFileExtension(const std::string& filename)
{
	size_t off = filename.find_last_of('.');
	if (off == std::string::npos)
	{
		return {};
	}
	return std::string(filename.substr(off + 1));
}


void StringHelper::ReverseString(std::string& str)
{
	char _temp;
	for (size_t i = 0; i < str.size() / 2; i++)
	{
		_temp = str[i];
		str[i] = str[(str.size() - 1) - i];
		str[(str.size() - 1) - i] = _temp;
	}
}

std::string StringHelper::GetFileName(std::string& name, bool ex)
{
	std::string _result;
	std::string _extension;
	size_t _sizeOfPath = name.size();

	for (size_t i = _sizeOfPath - 1; i > 0; i--)
	{
		_extension += name.at(i);

		if (name.at(i) == '.')
		{
			i--;
			// 이 함수는 경로를 걸러내는 것이기 때문에
			// 아래 문자가 등장할때까지 계속해서 진행한다.
			// i가 0이거나 더 적어질 경우 끝내는 로직도 필요
			while (!(name.at(i) == '/' || name.at(i) == '\\'))
			{
				_result += name.at(i);
				if (0 >= --i) break;

			}
			break;
		}
	}

	ReverseString(_result);
	ReverseString(_extension);

	if (ex == true)
	{
		_result.append(_extension);
		return _result;
	}
	else
	{
		return _result;
	}
}
