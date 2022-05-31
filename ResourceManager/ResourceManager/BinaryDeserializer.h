#pragma once
#include <fstream>
#include <memory>

struct FBXModelBinaryFile;
struct AnimFileForRead;

/// <summary>
/// binary������ �޾Ƽ� vertex�� ������ȭ �ϴ� ����
/// binary ���� �ϳ��� �ϳ��� �ν��Ͻ��� ����ؾ��Ѵ�.
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
	///��ü ��
	///---------------------------------
	void ReadBinarayHeaderFile(std::ifstream* out);
	void ReadBinarayDataFile(std::ifstream* out);

	///---------------------------------
	/// �ִϸ��̼� ����
	///---------------------------------

private:

	std::shared_ptr<FBXModelBinaryFile> m_DataFile;
	std::shared_ptr<AnimFileForRead> m_AnimFile;
	int				m_fileSize;

};
