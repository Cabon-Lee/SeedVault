#pragma once
#ifdef BINARYFILEMANAGER_EXPORTS
#define BINARYFILEMANAGER_DLL __declspec(dllexport)
#else
#define BINARYFILEMANAGER_DLL __declspec(dllimport)
#endif // !BINARYFILEMANAGER_EXPORT

#include <memory.h>

/// <summary>
/// [2021-10-22][최민경]
/// 바이너리 파일을 생성하거나 읽어오는 매니저 클래스
/// </summary>
class BINARYFILEMANAGER_DLL BinaryFileManager
{
public:
	BinaryFileManager();
	~BinaryFileManager();

public:
	// 바이너리 파일을 읽어오는 함수
	static bool ReadBinaryFile(const char* filePath, char*& _fileData, int& _fileSize);
	// 바이너리 파일을 작성하는 함수
	static bool WriteBinarayFile(const char* filePath, char*& _fileData, const int& _fileSize);
	// Serialize 하는 함수
	template <typename T>
	static void Serialization(char*& _fileBuffer, int _bufferSize, int& _bufferIndex, T* _sourcePtr, rsize_t _sourceSize);
	// Deserialize 하는 함수
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
		_targetBuffer,							// 목적하는 복사 버퍼
		_targetBufferSize,						// 복사대상 버퍼 크기
		_fileBuffer + _fileBufferIndex,			
		_targetBufferSize);

	_fileBufferIndex += _targetBufferSize;
}