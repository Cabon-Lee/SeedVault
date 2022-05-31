#pragma once
#include <memory.h>
#include <fstream>
#include <map>
#include "VertexHelper.h"
#include "MaterialData.h"

struct FBXModel;
struct FBXModelBinaryFile;
struct AnimFileForWrite;

/// <summary>
/// vertex를 직렬화하고 binary로 저장하는 역할
/// binary 파일 하나당 하나의 인스턴스를 사용해야한다.
/// 
/// 2021. 10. 30  B.B
/// </summary>
class BinarySerializer
{
public:
	BinarySerializer();
	~BinarySerializer();

	void SaveBinaryFileFBX(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc, const char* filePath);
	void SaveAnimFile(FBXModel* originModel, const char* filePath);

private:
	///---------------------------------
	///전체 모델
	///---------------------------------
	// 1. binary로 만들 모델정보를 정리한다.
	void OrganizeFBXModelFile(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	// 2. m_DataFile를 직렬화하고 바이너리파일로 쓴다
	void WriteBinarayFile(const char* filePath);
	// 3. 텍스트파일을 쓴다
	void WriteTextFile(const char* filePath);
	
	/// 헤더
	void OrganizeFBXModelHeader(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	void WriteBinarayHeaderFile(std::ofstream* out);

	/// 데이터
	void OrganizeFBXModelData(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	void WriteBinarayDataFile(std::ofstream* out);

private:
	///---------------------------------
	/// 애니메이션 정보
	///---------------------------------
	void OrganizeAnim(FBXModel* originModel);
	void WriteAnimFile(const char* filePath, FBXModel* originModel);
	
private:
	FBXModelBinaryFile* m_DataFile;
	AnimFileForWrite* m_AnimFile;
};
