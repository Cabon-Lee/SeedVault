#include "pch.h"
#include "MatSerializer.h"
#include "binaryFileLayout.h"
#include "NodeData.h"

MatSerializer::MatSerializer()
	: m_MatFile(nullptr)
{
}

MatSerializer::~MatSerializer()
{
}

void MatSerializer::SaveMatFile(
	std::shared_ptr<GraphicMaterial> graphicMaterial, const char* filePath)
{
	m_MatFile = new MatFile();

	OrganizeMatFile(graphicMaterial);
	WriteMatFile(filePath);
}


void MatSerializer::OrganizeMatFile(std::shared_ptr<GraphicMaterial> graphicMaterial)
{
	OrganizeMatData(graphicMaterial);
	OrganizeMatHeader();
}

void MatSerializer::WriteMatFile(const char* filePath)
{
	std::string _fullName(filePath);
	_fullName.append("/");
	_fullName.append(m_MatFile->matData.materialName.c_str());
	_fullName.append(".mat");

	std::ofstream _binaryFile(_fullName.c_str(), std::ios::out | std::ios::binary);

	if (_binaryFile.is_open())
	{
		/// 헤더
		WriteMatHeaderFile(&_binaryFile);

		/// 데이터
		WriteMatDataFile(&_binaryFile);
	}

	_binaryFile.close();
}

void MatSerializer::OrganizeMatHeader()
{
	m_MatFile->matHeader.materialNameLength = static_cast<int>(m_MatFile->matData.materialName.length()) + 1;

	m_MatFile->matHeader.albedoMapNameLength = static_cast<int>(m_MatFile->matData.albedoMap.length()) + 1;
	m_MatFile->matHeader.normalMapNameLength = static_cast<int>(m_MatFile->matData.normalMap.length()) + 1;
	m_MatFile->matHeader.metalRoughMapNameLength = static_cast<int>(m_MatFile->matData.metalRoughMap.length()) + 1;
	m_MatFile->matHeader.emissiveMapNameLength = static_cast<int>(m_MatFile->matData.emissiveMap.length()) + 1;
}

void MatSerializer::WriteMatHeaderFile(std::ofstream* out)
{
	out->write(reinterpret_cast<char*>(&(m_MatFile->matHeader.materialNameLength)), sizeof(m_MatFile->matHeader.materialNameLength));

	out->write(reinterpret_cast<char*>(&(m_MatFile->matHeader.albedoMapNameLength)), sizeof(m_MatFile->matHeader.albedoMapNameLength));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matHeader.normalMapNameLength)), sizeof(m_MatFile->matHeader.normalMapNameLength));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matHeader.metalRoughMapNameLength)), sizeof(m_MatFile->matHeader.metalRoughMapNameLength));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matHeader.emissiveMapNameLength)), sizeof(m_MatFile->matHeader.emissiveMapNameLength));
}

void MatSerializer::OrganizeMatData(std::shared_ptr<GraphicMaterial> graphicMaterial)
{
	m_MatFile->matData.materialName = graphicMaterial->materialName.c_str();

	if (graphicMaterial->albedoMap.size() == 0)
	{
		m_MatFile->matData.albedoMap = "None";
	}
	else
	{
		m_MatFile->matData.albedoMap = graphicMaterial->albedoMap.c_str();
	}
	
	if (graphicMaterial->normalMap.size() == 0)
	{
		m_MatFile->matData.normalMap = "None";
	}
	else
	{
		m_MatFile->matData.normalMap = graphicMaterial->normalMap.c_str();
	}
	
	if (graphicMaterial->metalRoughMap.size() == 0)
	{
		m_MatFile->matData.metalRoughMap = "None";
	}
	else
	{
		m_MatFile->matData.metalRoughMap = graphicMaterial->metalRoughMap.c_str();
	}
	
	if (graphicMaterial->emissiveMap.size() == 0)
	{
		m_MatFile->matData.emissiveMap = "None";
	}
	else
	{
		m_MatFile->matData.emissiveMap = graphicMaterial->emissiveMap.c_str();
	}

	m_MatFile->matData.metallic = graphicMaterial->metallic;
	m_MatFile->matData.shineness = graphicMaterial->smoothness;
	m_MatFile->matData.normalFactor = graphicMaterial->normalFactor;
	m_MatFile->matData.emissiveFactor = graphicMaterial->emissiveFactor;

	m_MatFile->matData.isTransparency = graphicMaterial->isTransparency;
}

void MatSerializer::WriteMatDataFile(std::ofstream* out)
{
	out->write(m_MatFile->matData.materialName.c_str(), static_cast<size_t>(m_MatFile->matHeader.materialNameLength) - 1);
	out->write("\0", 1);

	out->write(m_MatFile->matData.albedoMap.c_str(), static_cast<size_t>(m_MatFile->matHeader.albedoMapNameLength) - 1);
	out->write("\0", 1);

	out->write(m_MatFile->matData.normalMap.c_str(), static_cast<size_t>(m_MatFile->matHeader.normalMapNameLength) - 1);
	out->write("\0", 1);

	out->write(m_MatFile->matData.metalRoughMap.c_str(), static_cast<size_t>(m_MatFile->matHeader.metalRoughMapNameLength) - 1);
	out->write("\0", 1);

	out->write(m_MatFile->matData.emissiveMap.c_str(), static_cast<size_t>(m_MatFile->matHeader.emissiveMapNameLength) - 1);
	out->write("\0", 1);

	out->write(reinterpret_cast<char*>(&(m_MatFile->matData.metallic)), sizeof(m_MatFile->matData.metallic));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matData.shineness)), sizeof(m_MatFile->matData.shineness));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matData.normalFactor)), sizeof(m_MatFile->matData.normalFactor));
	out->write(reinterpret_cast<char*>(&(m_MatFile->matData.emissiveFactor)), sizeof(m_MatFile->matData.emissiveFactor));

	out->write(reinterpret_cast<char*>(&(m_MatFile->matData.isTransparency)), sizeof(m_MatFile->matData.isTransparency));

}
