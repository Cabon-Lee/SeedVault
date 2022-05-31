#include <fstream>
#include "BinaryFileManager.h"

BinaryFileManager::BinaryFileManager()
{
}

BinaryFileManager::~BinaryFileManager()
{
}

bool BinaryFileManager::ReadBinaryFile(const char* filePath, char*& _fileData, int& _fileSize)
{
	std::ifstream _binaryFile(filePath, std::ios::in | std::ios::binary);
	if (_binaryFile.is_open() == false)
	{
		return false;
	}

	_binaryFile.seekg(0, _binaryFile.end);
	_fileSize = (int)_binaryFile.tellg();
	_binaryFile.seekg(0, _binaryFile.beg);

	char* _buffer = new char[_fileSize];

	_binaryFile.read((char*)_buffer, _fileSize);
	_binaryFile.close();

	_fileData = _buffer;
	return true;
}

bool BinaryFileManager::WriteBinarayFile(const char* filePath, char*& _fileData, const int& _fileSize)
{
	std::ofstream _binaryFile;
	_binaryFile.open(filePath, std::ios::out | std::ios::binary);

	if (_binaryFile.is_open() == false)
	{
		return false;
	}

	_binaryFile.write(_fileData, _fileSize);
	_binaryFile.close();

	return true;
}
