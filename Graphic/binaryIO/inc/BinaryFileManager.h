#pragma once
#ifdef BINARYFILEMANAGER_EXPORTS
#define BINARYFILEMANAGER_DLL __declspec(dllexport)
#else
#define BINARYFILEMANAGER_DLL __declspec(dllimport)
#endif // !BINARYFILEMANAGER_EXPORT

#include <memory.h>

/// <summary>
/// [2021-10-22][�ֹΰ�]
/// ���̳ʸ� ������ �����ϰų� �о���� �Ŵ��� Ŭ����
/// </summary>
class BINARYFILEMANAGER_DLL BinaryFileManager
{
public:
	BinaryFileManager();
	~BinaryFileManager();

public:
	// ���̳ʸ� ������ �о���� �Լ�
	static bool ReadBinaryFile(const char* filePath, char*& _fileData, int& _fileSize);
	// ���̳ʸ� ������ �ۼ��ϴ� �Լ�
	static bool WriteBinarayFile(const char* filePath, char*& _fileData, const int& _fileSize);
	// Serialize �ϴ� �Լ�
	template <typename T>
	static void Serialization(char*& _fileBuffer, int _bufferSize, int& _bufferIndex, T* _sourcePtr, rsize_t _sourceSize);
	// Deserialize �ϴ� �Լ�
	template <typename T>
	static void Deserializtion(T* _targetBuffer, int _targetBufferSize, char*& _fileBuffer, int& _fileBufferIndex);
};

template<typename T>
inline void BinaryFileManager::Serialization(char*& _fileBuffer, int _bufferSize, int& _bufferIndex, T* _sourcePtr, rsize_t _sourceSize)
{
	memcpy_s(_fileBuffer + _bufferIndex, _bufferSize - _bufferIndex, _sourcePtr, _sourceSize);
	_bufferIndex += _sourceSize;
}


template<typename T>
inline void BinaryFileManager::Deserializtion(T* _targetBuffer, int _targetBufferSize, char*& _fileBuffer, int& _fileBufferIndex)
{
	memcpy_s(
		_targetBuffer,							// �����ϴ� ���� ����
		_targetBufferSize,						// ������ ���� ũ��
		_fileBuffer + _fileBufferIndex,			
		_targetBufferSize);

	_fileBufferIndex += _targetBufferSize;
}