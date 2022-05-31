#include "StringHelper.h"
#include <algorithm>

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
	return filepath.substr(0, std::max(off1, off2));
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
