#pragma once
#include <memory>
#include <fstream>

struct MatFile;
struct GraphicMaterial;

/// <summary>
/// ���׸��� ������ ��� .mat ������ ����� Serializer
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
	///��ü ��
	///---------------------------------
	// 1. .mat���� ���� �������� �����Ѵ�.
	void OrganizeMatFile(std::shared_ptr<GraphicMaterial> graphicMaterial);
	// 2. m_DataFile�� ����ȭ�ϰ� ���̳ʸ����Ϸ� ����																									 
	void WriteMatFile(const char* filePath);

	/// ���
	void OrganizeMatHeader();
	void WriteMatHeaderFile(std::ofstream* out);

	/// ������
	void OrganizeMatData(std::shared_ptr<GraphicMaterial> graphicMaterial);
	void WriteMatDataFile(std::ofstream* out);

private:
	MatFile* m_MatFile;

};

