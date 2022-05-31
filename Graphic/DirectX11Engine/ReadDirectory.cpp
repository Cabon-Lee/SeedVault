#include <windows.h>
#include "ReadDirectory.h"
#include "CATrace.h"

ReadDirectory* ReadDirectory::m_Instance = nullptr;

ReadDirectory::ReadDirectory()
{

}

ReadDirectory::~ReadDirectory()
{

}

void ReadDirectory::FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames)
{
	fileNames.clear();

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

			fileNames.push_back(_str);

		} while (FindNextFile(_hFind, &_data) != 0);

		FindClose(_hFind);
	}
}

void ReadDirectory::FindFileFromDirectory(std::string path, std::vector<std::string>& fileNames, eFileType fileType)
{
	fileNames.clear();

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

			fileNames.push_back(_str);

		} while (FindNextFile(_hFind, &_data) != 0);

		FindClose(_hFind);
	}

	ValidateFileType(fileNames, fileType);

	// ���� �̸��� ���� ��θ� �տ� �ٿ��ִ� �۾��� �߰��� �����Ѵ�.
	for (auto& _nowFileName : fileNames)
	{
		std::string _newStr(std::move(_nowFileName));
		_nowFileName.clear();
		_nowFileName.append(path);
		_nowFileName.append(_newStr);
	}
}

void ReadDirectory::ValidateFileType(std::vector<std::string>& fileNames, eFileType fileType)
{
	switch (fileType)
	{
	case eFileType::TEXTURE:
		IsTextureFileName(fileNames);
		return;
	case eFileType::ASE:
		IsASEFileName(fileNames);
		return;
	case eFileType::FX:
		IsFXFileName(fileNames);
		return;
	default:
		break;
	}

}

bool ReadDirectory::IsTextureDDS(std::string& fileName)
{
	switch (CheckExtension(fileName))
	{
	case eEXTENSION::MISCELLANEOUS:
	case eEXTENSION::ASE:
	case eEXTENSION::EXE:
		CA_TRACE("�ؽ��İ� �ƴ� ���� �̸�");
		break;
	case eEXTENSION::DDS:
		return true;
	default:
		return false;
	}

	return false;
}

void ReadDirectory::IsTextureFileName(std::vector<std::string>& fileName)
{
	std::vector<std::string> _temp;

	for (size_t i = 0; i < fileName.size(); i++)
	{
		// �ϴ� �����ϰ� ���̰� 4������ ��� �����ϵ��� �������.
		// Ȯ���ڸ� �����ؼ� 4�ڶ�� ����
		if (5 > fileName[i].size())
		{
			continue;
		}
		else
		{
			// �̻��� ���ڿ��� ��������� �ɷ�����.
			bool _isValid = true;
			for (size_t _char = 0; _char < fileName[i].size(); _char++)
			{
				if (0 > fileName[i].at(_char))
				{
					_isValid = false;
					break;
				}
			}
			if (_isValid == false) continue;

			// EXE ���� �� ��Ÿ �ٸ� ���ϵ��� �ɷ�����.
			eEXTENSION _extension = CheckExtension(fileName[i]);
			if (eEXTENSION::EXE == _extension ||
				eEXTENSION::MISCELLANEOUS == _extension)
			{
				continue;
				//exe �����̶�� ��, �׷� ���Ѵ�.
			}
			else
			{
				_temp.push_back(std::move(fileName[i]));
			}
		}
	}

	fileName.clear();
	fileName.reserve(_temp.size());

	for (size_t i = 0; i < _temp.size(); i++)
	{
		fileName.push_back(std::move(_temp[i]));
	}
}

void ReadDirectory::IsASEFileName(std::vector<std::string>& fileName)
{
	std::vector<std::string> _temp;

	for (size_t i = 0; i < fileName.size(); i++)
	{
		if (5 > fileName[i].size())
		{
			continue;
		}
		else
		{
			bool _isValid = true;
			for (size_t _char = 0; _char < fileName[i].size(); _char++)
			{
				if (0 > fileName[i].at(_char))
				{
					_isValid = false;
					break;
				}
			}
			if (_isValid == false) continue;

			// EXE ���� �� ��Ÿ �ٸ� ���ϵ��� �ɷ�����.
			eEXTENSION _extension = CheckExtension(fileName[i]);
			if (eEXTENSION::ASE == _extension)
			{
				// ASE ���ϸ� �����´�
				_temp.push_back(std::move(fileName[i]));
			}
			else
			{
				continue;
			}
		}
	}

	fileName.clear();
	fileName.reserve(_temp.size());

	for (size_t i = 0; i < _temp.size(); i++)
	{
		fileName.push_back(std::move(_temp[i]));
	}
}

void ReadDirectory::IsFXFileName(std::vector<std::string>& fileName)
{
	std::vector<std::string> _temp;

	for (size_t i = 0; i < fileName.size(); i++)
	{
		if (5 > fileName[i].size())
		{
			continue;
		}
		else
		{
			bool _isValid = true;
			for (size_t _char = 0; _char < fileName[i].size(); _char++)
			{
				if (0 > fileName[i].at(_char))
				{
					_isValid = false;
					break;
				}
			}
			if (_isValid == false) continue;

			// EXE ���� �� ��Ÿ �ٸ� ���ϵ��� �ɷ�����.
			eFX _extension = CheckExtensionFX(fileName[i]);
			if (eFX::CSO == _extension)
			{
				// ASE ���ϸ� �����´�
				_temp.push_back(std::move(fileName[i]));
			}
			else
			{
				continue;
			}
		}
	}

	fileName.clear();
	fileName.reserve(_temp.size());

	for (size_t i = 0; i < _temp.size(); i++)
	{
		fileName.push_back(std::move(_temp[i]));
	}
}

void ReadDirectory::ReverseString(std::string& str)
{
	char _temp;
	for (size_t i = 0; i < str.size() / 2; i++)
	{
		_temp = str[i];
		str[i] = str[(str.size() - 1) - i];
		str[(str.size() - 1) - i] = _temp;
	}
}

void ReadDirectory::GetFileName(std::string& name, bool remainEx)
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
			// �� �Լ��� ��θ� �ɷ����� ���̱� ������ 
			// �Ʒ� ���ڰ� �����Ҷ����� ����ؼ� �����Ѵ�.
			// i�� 0�̰ų� �� ������ ��� ������ ������ �ʿ�
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

	if (remainEx == true)
	{
		_result.append(_extension);
		name = _result;
		return;
	}
	else
	{
		name = _result;
		return;
	}
}

eEXTENSION ReadDirectory::CheckExtension(std::string& name)
{
	// �������� �о�鿩�� �´��� �ƴ��� �Ǵ��Ѵ�.
	std::string _sample;

	for (size_t i = name.size() - 1; i > 0; i--)
	{
		_sample += name.at(i);

		// Ȯ���� ���� ��� ���� �ڿ� �������� �˻��Ѵ�.
		if (name.at(i) == '.')
		{
			ReverseString(_sample);

			if (_sample == ".dds")
			{
				return eEXTENSION::DDS;
			}
			else if (_sample == ".png")
			{
				return eEXTENSION::PNG;
			}
			else if (_sample == ".bmp")
			{
				return eEXTENSION::BMP;
			}
			else if (_sample == ".tif")
			{
				return eEXTENSION::TIF;
			}
			else if (_sample == ".jpg")
			{
				return eEXTENSION::JPG;
			}
			else if (_sample == ".exe")
			{
				return eEXTENSION::EXE;
			}
			else if (_sample == ".ASE")
			{
				return eEXTENSION::ASE;
			}
			else
			{
				return eEXTENSION::MISCELLANEOUS;
			}

		}
		else
		{

		}

	}

}

eFX ReadDirectory::CheckExtensionFX(std::string& name)
{
	// �������� �о�鿩�� �´��� �ƴ��� �Ǵ��Ѵ�.
	std::string _sample;

	for (size_t i = name.size() - 1; i > 0; i--)
	{
		_sample += name.at(i);

		// Ȯ���� ���� ��� ���� �ڿ� �������� �˻��Ѵ�.
		if (name.at(i) == '.')
		{
			ReverseString(_sample);

			if (_sample == ".cso")
			{
				return eFX::CSO;
			}
			else if (_sample == ".fx")
			{
				return eFX::FX;
			}
			else if (_sample == ".fxo")
			{
				return eFX::FXO;
			}

		}
		else
		{

		}

	}
}
