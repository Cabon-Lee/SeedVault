#include "pch.h"
#include "MatDeserializer.h"
#include "binaryFileLayout.h"

MatDeserializer::MatDeserializer()
	: m_MatFile(nullptr)
{
}

MatDeserializer::~MatDeserializer()
{
}

std::shared_ptr<MatFile> MatDeserializer::LoadMatFile(const char* filePath)
{
	m_MatFile = std::make_shared<MatFile>();

	std::ifstream _matFile(filePath, std::ios::in | std::ios::binary);
	if (_matFile.is_open() == false)
	{
		return nullptr;
	}

	//파일 전체의 크기
	_matFile.seekg(0, _matFile.end);
	//m_fileSize = (int)_binaryFile.tellg();
	_matFile.seekg(0, _matFile.beg);

	if (_matFile.is_open())
	{
		/// 헤더
		ReadMatHeaderFile(&_matFile);

		/// 데이터
		ReadMatDataFile(&_matFile);
	}
	_matFile.close();

	return m_MatFile;
}

void MatDeserializer::ReadMatHeaderFile(std::ifstream* in)
{
	in->read(reinterpret_cast<char*>(&m_MatFile->matHeader.materialNameLength), sizeof(m_MatFile->matHeader.materialNameLength));

	in->read(reinterpret_cast<char*>(&m_MatFile->matHeader.albedoMapNameLength), sizeof(m_MatFile->matHeader.albedoMapNameLength));
	in->read(reinterpret_cast<char*>(&m_MatFile->matHeader.normalMapNameLength), sizeof(m_MatFile->matHeader.normalMapNameLength));
	in->read(reinterpret_cast<char*>(&m_MatFile->matHeader.metalRoughMapNameLength), sizeof(m_MatFile->matHeader.metalRoughMapNameLength));
	in->read(reinterpret_cast<char*>(&m_MatFile->matHeader.emissiveMapNameLength), sizeof(m_MatFile->matHeader.emissiveMapNameLength));
}

void MatDeserializer::ReadMatDataFile(std::ifstream* in)
{
	m_MatFile->matData.materialName.resize(m_MatFile->matHeader.materialNameLength);
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.materialName[0]), m_MatFile->matHeader.materialNameLength);


	m_MatFile->matData.albedoMap.resize(m_MatFile->matHeader.albedoMapNameLength);
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.albedoMap[0]), m_MatFile->matHeader.albedoMapNameLength);

	m_MatFile->matData.normalMap.resize(m_MatFile->matHeader.normalMapNameLength);
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.normalMap[0]), m_MatFile->matHeader.normalMapNameLength);

	m_MatFile->matData.metalRoughMap.resize(m_MatFile->matHeader.metalRoughMapNameLength);
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.metalRoughMap[0]), m_MatFile->matHeader.metalRoughMapNameLength);

	m_MatFile->matData.emissiveMap.resize(m_MatFile->matHeader.emissiveMapNameLength);
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.emissiveMap[0]), m_MatFile->matHeader.emissiveMapNameLength);


	in->read(reinterpret_cast<char*>(&m_MatFile->matData.metallic), sizeof(m_MatFile->matData.metallic));
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.shineness), sizeof(m_MatFile->matData.shineness));
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.normalFactor), sizeof(m_MatFile->matData.normalFactor));
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.emissiveFactor), sizeof(m_MatFile->matData.emissiveFactor));
	
	in->read(reinterpret_cast<char*>(&m_MatFile->matData.isTransparency), sizeof(m_MatFile->matData.isTransparency));



}
