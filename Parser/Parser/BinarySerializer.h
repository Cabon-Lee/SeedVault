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
/// vertex�� ����ȭ�ϰ� binary�� �����ϴ� ����
/// binary ���� �ϳ��� �ϳ��� �ν��Ͻ��� ����ؾ��Ѵ�.
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
	///��ü ��
	///---------------------------------
	// 1. binary�� ���� �������� �����Ѵ�.
	void OrganizeFBXModelFile(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	// 2. m_DataFile�� ����ȭ�ϰ� ���̳ʸ����Ϸ� ����
	void WriteBinarayFile(const char* filePath);
	// 3. �ؽ�Ʈ������ ����
	void WriteTextFile(const char* filePath);
	
	/// ���
	void OrganizeFBXModelHeader(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	void WriteBinarayHeaderFile(std::ofstream* out);

	/// ������
	void OrganizeFBXModelData(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc);
	void WriteBinarayDataFile(std::ofstream* out);

private:
	///---------------------------------
	/// �ִϸ��̼� ����
	///---------------------------------
	void OrganizeAnim(FBXModel* originModel);
	void WriteAnimFile(const char* filePath, FBXModel* originModel);
	
private:
	FBXModelBinaryFile* m_DataFile;
	AnimFileForWrite* m_AnimFile;
};
