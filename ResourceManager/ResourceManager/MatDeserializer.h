#pragma once
#include <memory>
#include <fstream>

struct MatFile;

/// <summary>
/// 메테리얼 정보를 담은 .mat 파일을 읽어들이는 Deserializer
/// 
/// 2022.01.05 B.Bunny
/// </summary>
class MatDeserializer
{
public:
	MatDeserializer();
	~MatDeserializer();

	std::shared_ptr<MatFile> LoadMatFile(const char* filePath);
	
private:
	void ReadMatHeaderFile(std::ifstream* in);
	void ReadMatDataFile(std::ifstream* in);

private:
	std::shared_ptr<MatFile> m_MatFile;

};


