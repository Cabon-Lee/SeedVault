#pragma once
#include <fstream>
#include <memory>

struct FBXModelBinaryFile;
struct AnimFileForRead;

/// <summary>
/// binary파일을 받아서 vertex를 역직렬화 하는 역할
/// binary 파일 하나당 하나의 인스턴스를 사용해야한다.
/// 
/// 2021. 10. 30  B.B 
/// </summary>
class BinaryDeserializer
{
public:
	BinaryDeserializer();
	~BinaryDeserializer();

	std::shared_ptr<FBXModelBinaryFile> LoadBinaryFileFBX(const char* filePath);
	std::shared_ptr<AnimFileForRead> LoadAnimFile(const char* filePath);

private:
	///---------------------------------
	///전체 모델
	///---------------------------------
	void ReadBinarayHeaderFile(std::ifstream* out);
	void ReadBinarayDataFile(std::ifstream* out);

	///---------------------------------
	/// 애니메이션 정보
	///---------------------------------

private:

	std::shared_ptr<FBXModelBinaryFile> m_DataFile;
	std::shared_ptr<AnimFileForRead> m_AnimFile;
	int				m_fileSize;

};
