
#include "pch.h"	// DLL일때만 등장하도록
#include <windows.h>
#include "DirectoryReader.h"
#include "StringHelper.h"

std::vector<std::string> DirectoryReader::m_TextureNames;
std::vector<std::string> DirectoryReader::m_MeshNames;
std::vector<std::string> DirectoryReader::m_BinaryNames;
std::vector<std::string> DirectoryReader::m_ShaderNames;
std::vector<std::string> DirectoryReader::m_AnimNames;
std::vector<std::string> DirectoryReader::m_MatNames;
std::vector<std::string> DirectoryReader::m_IBLNames;
std::vector<std::string> DirectoryReader::m_BankNames;
std::vector<std::string> DirectoryReader::m_QuestNames;
std::vector<std::string> DirectoryReader::m_DialogueNames;

void DirectoryReader::ReverseString(std::string& str)
{
	char _temp;
	for (size_t i = 0; i < str.size() / 2; i++)
	{
		_temp = str[i];
		str[i] = str[(str.size() - 1) - i];
		str[(str.size() - 1) - i] = _temp;
	}
}

void DirectoryReader::InputEntryDir(std::string& path)
{
	std::vector<std::string> _dirs;

	GetAllDir(path, _dirs);
}

std::vector<std::string>& DirectoryReader::GetTextureNames()
{
	return m_TextureNames;
}

std::vector<std::string>& DirectoryReader::GetMeshNames()
{
	return m_MeshNames;
}

std::vector<std::string>& DirectoryReader::GetShaderNames()
{
	return m_ShaderNames;
}

std::vector<std::string>& DirectoryReader::GetBinaryNames()
{
	return m_BinaryNames;
}

std::vector<std::string>& DirectoryReader::GetAnimNames()
{
	return m_AnimNames;
}

std::vector<std::string>& DirectoryReader::GetMatNames()
{
	return m_MatNames;
}


std::vector<std::string>& DirectoryReader::GetIBLNames()
{
	return m_IBLNames;
}

std::vector<std::string>& DirectoryReader::GetBankNames()
{
	return m_BankNames;
}

std::vector<std::string>& DirectoryReader::GetQuestNames()
{
	return m_QuestNames;
}

std::vector<std::string>& DirectoryReader::GetDialogueNames()
{
	return m_DialogueNames;
}

std::string DirectoryReader::GetFileName(std::string& name, bool ex)
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

void DirectoryReader::GetAllDir(std::string& path, std::vector<std::string>& dirs)
{
	// 시작 경로를 통해서 진입점을 잡고, 해당 경로의 모든 것들을 담는다.
	// 이후 폴더만을 추린다.

	// 모든 것들이 담길 컨테이너
	std::vector<std::string> _tempNames;

	GetContents(path, _tempNames);

	unsigned int _currentFolderCount = 0;
	bool _dirName;

	std::queue<std::string> _tempFolder;

	for (std::string _nowName : _tempNames)
	{
		_dirName = true;

		// 상하위 폴더 예약어는 넘어간다
		if (_nowName == "." || _nowName == "..")
		{
			continue;
		}

		for (size_t i = 0; i < _nowName.size(); i++)
		{
			if (_nowName.at(i) == '.')
			{
				_dirName = false;
				break;
			}
		}

		if (_dirName == true)
		{
			_tempFolder.push(_nowName);
			_currentFolderCount++;
		}
		else
		{
			std::string _newFileName = path + _nowName;
			StorePath(_newFileName);
		}
	}

	// 만약 현재 폴더 수가 1개 이상이면
	if (_currentFolderCount >= 1)
	{

		for (unsigned int i = 0; i < _currentFolderCount; i++)
		{

			std::string _newPath = path;
			_newPath.append(_tempFolder.front() + '/');
			_tempFolder.pop();

			dirs.push_back(_newPath);

			GetAllDir(_newPath, dirs);
		}
	}
	else	
	{
		return;
	}
}



void DirectoryReader::GetContents(std::string& path, std::vector<std::string>& dirs)
{
	std::wstring _pattern;
	_pattern.assign(path.begin(), path.end());
	_pattern.append(L"\\*");
	WIN32_FIND_DATA _data;
	HANDLE _hFind;

	if ((_hFind = FindFirstFile(_pattern.c_str(), &_data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::wstring _wstr(_data.cFileName);
			std::string _str(_wstr.begin(), _wstr.end());

			dirs.push_back(_str);

		} while (FindNextFile(_hFind, &_data) != 0);

		FindClose(_hFind);
	}
}

void DirectoryReader::StorePath(std::string& path)
{
	std::string _sample;

	for (size_t i = path.size() - 1; i > 0; i--)
	{
		_sample += path.at(i);

		// 확장자 명을 모두 읽은 뒤에 대조군을 검사한다.
		if (path.at(i) == '.')
		{
			ReverseString(_sample);

			if (_sample == ".dds" || _sample == ".png" || 	_sample == ".hdr" ||
				_sample == ".bmp" || _sample == ".tif" || 	_sample == ".jpg" || 
				_sample == ".tga")
			{
				m_TextureNames.push_back(path);
				return;
			}
			else if (_sample == ".fbx" ||  _sample == ".ase" ||	 _sample == ".ASE")
			{
				m_MeshNames.push_back(path);
				return;
			}
			else if (_sample == ".cso" ||  _sample == ".fx" || 	 _sample == ".fxo")
			{
				m_ShaderNames.push_back(path);
				return;
			}
			else if (_sample == ".bin")
			{
				m_BinaryNames.push_back(path);
				return;
			}
			else if (_sample == ".anim")
			{
				m_AnimNames.push_back(path);
				return;
			}
			else if (_sample == ".mat")
			{
				m_MatNames.push_back(path);
				return;
			}
			else if (_sample == ".ibl")
			{
				m_IBLNames.push_back(path);
				return;
			}
			else if (_sample == ".bank" || _sample == ".strings.bank")
			{
				m_BankNames.push_back(path);
				return;
			}
			else if (_sample == ".qst")
			{
				m_QuestNames.push_back(path);
				return;
			}
			else if (_sample == ".csv")
			{
				if (StringHelper::GetFileName(path, false).front() == 'Q')
				{
					m_QuestNames.push_back(path);
				}
				else if (StringHelper::GetFileName(path, false).front() == 'D')
				{
					m_DialogueNames.push_back(path);
				}
				return;
			}
		}
	}
}
