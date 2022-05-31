#include "BinaryDeserializer.h"
#include "binaryFileLayout.h"
#include <fstream>

BinaryDeserializer::BinaryDeserializer() :
	m_DataFile(nullptr), m_deserializeContainer(nullptr), m_Index(0), m_fileSize(0)
{
}

BinaryDeserializer::~BinaryDeserializer()
{
	delete(m_deserializeContainer);
	m_deserializeContainer = 0;
}

FBXModelBinaryFile* BinaryDeserializer::LoadBinaryFileFBX(const char* filePath)
{
	m_DataFile = new FBXModelBinaryFile();

	//std::string _fullName(filePath);
	//_fullName.append(".bin");

	std::ifstream _binaryFile(filePath, std::ios::in | std::ios::binary);
	if (_binaryFile.is_open() == false)
	{
		return nullptr;
	}

	//파일 전체의 크기
	_binaryFile.seekg(0, _binaryFile.end);
	m_fileSize = (int)_binaryFile.tellg();
	_binaryFile.seekg(0, _binaryFile.beg);

	if (_binaryFile.is_open())
	{
		/// 헤더
		{
			_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binHeader.mSignature), sizeof(m_DataFile->binHeader.mSignature));

			// 매쉬
			_binaryFile.read(reinterpret_cast<char*>(&(m_DataFile->binHeader.meshCount)), sizeof(int));
			m_DataFile->binHeader.vertexCount_V.resize(m_DataFile->binHeader.meshCount);
			m_DataFile->binHeader.indexCount_V.resize(m_DataFile->binHeader.meshCount);
			_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binHeader.vertexCount_V.data()), sizeof(int) * m_DataFile->binHeader.meshCount);
			_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binHeader.indexCount_V.data()), sizeof(int) * m_DataFile->binHeader.meshCount);
			_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binHeader.vertexType), sizeof(VertexType));

			// 머테리얼
			_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binHeader.isMaterial), sizeof(bool));
			if (m_DataFile->binHeader.isMaterial == true)
			{
				_binaryFile.read(reinterpret_cast<char*>(&(m_DataFile->binHeader.materialCount)), sizeof(size_t));
				
				m_DataFile->binHeader.materialNameLength_V.resize(m_DataFile->binHeader.materialCount);
				_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binHeader.materialNameLength_V.data()), sizeof(int) * m_DataFile->binHeader.materialCount);
				
				m_DataFile->binHeader.textureNameCount_V.resize(m_DataFile->binHeader.materialCount);
				_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameCount_V.data()), sizeof(int) * m_DataFile->binHeader.materialCount);
				
				m_DataFile->binHeader.textureNameLength_V.resize(m_DataFile->binHeader.materialCount);
				for (int i = 0; i < m_DataFile->binHeader.textureNameLength_V.size(); ++i)
				{
					m_DataFile->binHeader.textureNameLength_V[i].resize(m_DataFile->binHeader.textureNameCount_V[i]);
					_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameLength_V[i].data()), ((sizeof(int) * m_DataFile->binHeader.textureNameCount_V[i])));
				}
			}
		}

		/// 데이터
		{
			// 매쉬
			m_DataFile->binData.mesh_V.resize(m_DataFile->binHeader.meshCount);

			for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
			{
				m_DataFile->binData.mesh_V[i].vertex_V.resize(m_DataFile->binHeader.vertexCount_V[i]);
				m_DataFile->binData.mesh_V[i].index_V.resize(m_DataFile->binHeader.indexCount_V[i]);
			}

			for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
			{
				_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].vertex_V.data()), sizeof(Vertex::SerializerVertex) * m_DataFile->binData.mesh_V[i].vertex_V.size());
				_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].index_V.data()), sizeof(DWORD) * m_DataFile->binData.mesh_V[i].index_V.size());
			}

			// 머테리얼
			if (m_DataFile->binHeader.isMaterial == true)
			{
				m_DataFile->binData.material_V.resize(m_DataFile->binHeader.materialCount);
				for (int i = 0; i < m_DataFile->binHeader.materialCount; ++i)
				{
					m_DataFile->binData.material_V[i].materialName.resize(m_DataFile->binHeader.materialNameLength_V[i]);
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].materialName[0]), m_DataFile->binHeader.materialNameLength_V[i]);
					
					m_DataFile->binData.material_V[i].textureName_V.resize(m_DataFile->binHeader.textureNameCount_V[i]);
					for (int j = 0; j < m_DataFile->binHeader.textureNameCount_V[i]; ++j)
					{
						m_DataFile->binData.material_V[i].textureName_V[j].resize(m_DataFile->binHeader.textureNameLength_V[i][j]);
						_binaryFile.read(reinterpret_cast<char*>(m_DataFile->binData.material_V[i].textureName_V[j].data()), m_DataFile->binHeader.textureNameLength_V[i][j]);
					}
				
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.x), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.y), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.z), sizeof(float));
					
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.x), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.y), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.z), sizeof(float));
					
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.x), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.y), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.z), sizeof(float));
					
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.x), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.y), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.z), sizeof(float));
					
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.x), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.y), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.z), sizeof(float));
					
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].transparentFactor), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specularPower), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].shineness), sizeof(float));
					_binaryFile.read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].reflactionFactor), sizeof(float));
				
				}
			}
		}
	}
	_binaryFile.close();

	return m_DataFile;
}


bool BinaryDeserializer::ReadBinaryFile(const char* filePath, char*& _fileData, int& _fileSize)
{
	std::ifstream _binaryFile(filePath, std::ios::in | std::ios::binary);
	if (_binaryFile.is_open() == false)
	{
		return false;
	}

	_binaryFile.seekg(0, _binaryFile.end);
	_fileSize = (int)_binaryFile.tellg();
	_binaryFile.seekg(0, _binaryFile.beg);

	char* _buffer = new char[_fileSize];

	_binaryFile.read((char*)_buffer, _fileSize);
	_binaryFile.close();

	_fileData = _buffer;
	return true;
}
