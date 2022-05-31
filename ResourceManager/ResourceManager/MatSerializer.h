#pragma once
#include <memory>
#include <fstream>

struct MatFile;
struct GraphicMaterial;

/// <summary>
/// 메테리얼 정보를 담는 .mat 파일을 만드는 Serializer
/// 
/// 2022.01.05 B.Bunny
/// </summary>
class MatSerializer
{
public:
	MatSerializer();
	~MatSerializer();

	void SaveMatFile(
		std::shared_ptr<GraphicMaterial> graphicMaterial,
		const char* filePath);

private:
	///---------------------------------
	///전체 모델
	///---------------------------------
	// 1. .mat으로 만들 모델정보를 정리한다.
	void OrganizeMatFile(std::shared_ptr<GraphicMaterial> graphicMaterial);
	// 2. m_DataFile를 직렬화하고 바이너리파일로 쓴다																									 
	void WriteMatFile(const char* filePath);

	/// 헤더
	void OrganizeMatHeader();
	void WriteMatHeaderFile(std::ofstream* out);

	/// 데이터
	void OrganizeMatData(std::shared_ptr<GraphicMaterial> graphicMaterial);
	void WriteMatDataFile(std::ofstream* out);

private:
	MatFile* m_MatFile;

};

