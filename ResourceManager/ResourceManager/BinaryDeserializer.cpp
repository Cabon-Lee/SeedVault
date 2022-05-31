#include "pch.h"

#include "BinaryDeserializer.h"
#include "binaryFileLayout.h"
#include <fstream>


BinaryDeserializer::BinaryDeserializer() 
	: m_DataFile(nullptr)
	, m_AnimFile(nullptr)

{
}

BinaryDeserializer::~BinaryDeserializer()
{

}

std::shared_ptr<FBXModelBinaryFile> BinaryDeserializer::LoadBinaryFileFBX(const char* filePath)
{
	// .bin 파일이 맞는지 체크
	//std::string _fullName(filePath);
	//int isBinFile = _fullName.find(".bin");
	//if (isBinFile == -1)
	//{
	//	return nullptr;
	//}

	m_DataFile = std::make_shared<FBXModelBinaryFile>();
	m_fileSize = 0;

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
		ReadBinarayHeaderFile(&_binaryFile);

		/// 데이터
		ReadBinarayDataFile(&_binaryFile);
	}
	_binaryFile.close();

	return m_DataFile;
}

std::shared_ptr<AnimFileForRead> BinaryDeserializer::LoadAnimFile(const char* filePath)
{
	// .anim 파일이 맞는지 체크
	//std::string _fullName(filePath);
	//int isAnimFile = _fullName.find(".anim");
	//if (isAnimFile == -1)
	//{
	//	return nullptr;
	//}

	m_AnimFile = std::make_shared<AnimFileForRead>();
	m_fileSize = 0;

	std::ifstream _animFile(filePath, std::ios::in | std::ios::binary);
	if (_animFile.is_open() == false)
	{
		return nullptr;
	}

	//파일 전체의 크기
	_animFile.seekg(0, _animFile.end);
	m_fileSize = (int)_animFile.tellg();
	_animFile.seekg(0, _animFile.beg);

	if (_animFile.is_open())
	{
		/// 헤더
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.isSkinnedAnimation), sizeof(bool));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.isMeshAnimation), sizeof(bool));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.isSkeleton), sizeof(bool));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.boneCount), sizeof(size_t));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.meshCount), sizeof(size_t));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.aniNameLength), sizeof(int));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animHeader.animationKeyFrameCount), sizeof(int));

		/// 데이터
		//m_AnimFile->animData.animationName.resize(m_AnimFile->animHeader.aniNameLength);
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animData.animationName[0]), m_AnimFile->animHeader.aniNameLength);

		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animData.ticksPerFrame), sizeof(float));
		_animFile.read(reinterpret_cast<char*>(&m_AnimFile->animData.totalKeyFrames), sizeof(unsigned int));

		// 스키닝 애니메이션
		if (m_AnimFile->animHeader.isSkinnedAnimation == true)
		{
			// 애니메이션의 키 프레임은 본의 크기만큼 리사이즈해준다
			m_AnimFile->animData.animationKeyFrame.resize(m_AnimFile->animHeader.boneCount);

			// 애니메이션의 본 내부를 채우기 위해 본의 크기만큼 포문을 돈다
			for (int j = 0; j < m_AnimFile->animHeader.boneCount; ++j)
			{
				m_AnimFile->animData.animationKeyFrame[j].resize(m_AnimFile->animHeader.animationKeyFrameCount);
				_animFile.read(reinterpret_cast<char*>(m_AnimFile->animData.animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_AnimFile->animHeader.animationKeyFrameCount)));
			}
		}
		// 매쉬 애니메이션
		else if (m_AnimFile->animHeader.isMeshAnimation == true)
		{
			// 애니메이션의 키 프레임은 매쉬의 크기만큼 리사이즈해준다
			m_AnimFile->animData.animationKeyFrame.resize(m_AnimFile->animHeader.meshCount);

			// 애니메이션의 본 내부를 채우기 위해 매쉬의 크기만큼 포문을 돈다
			for (int j = 0; j < m_AnimFile->animHeader.meshCount; ++j)
			{
				m_AnimFile->animData.animationKeyFrame[j].resize(m_AnimFile->animHeader.animationKeyFrameCount);
				_animFile.read(reinterpret_cast<char*>(m_AnimFile->animData.animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_AnimFile->animHeader.animationKeyFrameCount)));
			}
		}
	}
	_animFile.close();

	return m_AnimFile;
}

void BinaryDeserializer::ReadBinarayHeaderFile(std::ifstream* in)
{
	/// 헤더
	{
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.mSignature), sizeof(m_DataFile->binHeader.mSignature));

		// 매쉬
		in->read(reinterpret_cast<char*>(&(m_DataFile->binHeader.meshCount)), sizeof(int));
		m_DataFile->binHeader.vertexCount_V.resize(m_DataFile->binHeader.meshCount);
		m_DataFile->binHeader.indexCount_V.resize(m_DataFile->binHeader.meshCount);
		in->read(reinterpret_cast<char*>(m_DataFile->binHeader.vertexCount_V.data()), sizeof(int) * m_DataFile->binHeader.meshCount);
		in->read(reinterpret_cast<char*>(m_DataFile->binHeader.indexCount_V.data()), sizeof(int) * m_DataFile->binHeader.meshCount);
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.vertexType), sizeof(VertexType));

		// 머테리얼
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.isMaterial), sizeof(bool));
		if (m_DataFile->binHeader.isMaterial == true)
		{
			in->read(reinterpret_cast<char*>(&(m_DataFile->binHeader.materialCount)), sizeof(size_t));

			m_DataFile->binHeader.materialNameLength_V.resize(m_DataFile->binHeader.materialCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.materialNameLength_V.data()), sizeof(int) * m_DataFile->binHeader.materialCount);

			m_DataFile->binHeader.textureNameCount_V.resize(m_DataFile->binHeader.materialCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameCount_V.data()), sizeof(int) * m_DataFile->binHeader.materialCount);

			m_DataFile->binHeader.textureNameLength_V.resize(m_DataFile->binHeader.materialCount);
			for (int i = 0; i < m_DataFile->binHeader.textureNameLength_V.size(); ++i)
			{
				m_DataFile->binHeader.textureNameLength_V[i].resize(m_DataFile->binHeader.textureNameCount_V[i]);
				in->read(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameLength_V[i].data()), ((sizeof(int) * m_DataFile->binHeader.textureNameCount_V[i])));
			}

			m_DataFile->binHeader.materialMapping_V.resize(m_DataFile->binHeader.meshCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.materialMapping_V.data()), sizeof(int) * m_DataFile->binHeader.meshCount);
		}

		// 스켈레톤
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.isSkeleton), sizeof(bool));
		if (m_DataFile->binHeader.isSkeleton == true)
		{
			in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.boneCount), sizeof(size_t));
			in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.boneOffsetCount), sizeof(size_t));
			m_DataFile->binHeader.boneNameLength_V.resize(m_DataFile->binHeader.boneCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.boneNameLength_V.data()), sizeof(int) * m_DataFile->binHeader.boneCount);
		}

		// 애니메이션
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.isSkinnedAnimation), sizeof(bool));
		in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.isMeshAnimation), sizeof(bool));
		if (m_DataFile->binHeader.isSkinnedAnimation == true || m_DataFile->binHeader.isMeshAnimation == true)
		{
			in->read(reinterpret_cast<char*>(&m_DataFile->binHeader.aniCount), sizeof(size_t));
			m_DataFile->binHeader.aniNameLength.resize(m_DataFile->binHeader.aniCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.aniNameLength.data()), sizeof(int) * m_DataFile->binHeader.aniCount);

			m_DataFile->binHeader.animationKeyFrameCount_V.resize(m_DataFile->binHeader.aniCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binHeader.animationKeyFrameCount_V.data()), ((sizeof(int) * m_DataFile->binHeader.aniCount)));
		}
	}
}

void BinaryDeserializer::ReadBinarayDataFile(std::ifstream* in)
{
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
			in->read(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].vertex_V.data()), sizeof(Vertex::SerializerVertex) * m_DataFile->binData.mesh_V[i].vertex_V.size());
			in->read(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].index_V.data()), sizeof(DWORD) * m_DataFile->binData.mesh_V[i].index_V.size());
			
			in->read(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshIndex), sizeof(int));
			in->read(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].isParent), sizeof(bool));
			in->read(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].parentIndex), sizeof(int));
			
			in->read(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshWorldTM), sizeof(Float4x4));
			in->read(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshLocalTM), sizeof(Float4x4));
		}

		// 머테리얼
		if (m_DataFile->binHeader.isMaterial == true)
		{
			m_DataFile->binData.material_V.resize(m_DataFile->binHeader.materialCount);
			for (int i = 0; i < m_DataFile->binHeader.materialCount; ++i)
			{
				m_DataFile->binData.material_V[i].materialName.resize(m_DataFile->binHeader.materialNameLength_V[i]);
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].materialName[0]), m_DataFile->binHeader.materialNameLength_V[i]);

				m_DataFile->binData.material_V[i].textureName_V.resize(m_DataFile->binHeader.textureNameCount_V[i]);
				for (int j = 0; j < m_DataFile->binHeader.textureNameCount_V[i]; ++j)
				{
					m_DataFile->binData.material_V[i].textureName_V[j].resize(m_DataFile->binHeader.textureNameLength_V[i][j]);
					in->read(reinterpret_cast<char*>(m_DataFile->binData.material_V[i].textureName_V[j].data()), m_DataFile->binHeader.textureNameLength_V[i][j]);
				}

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.x), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.y), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.z), sizeof(float));

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.x), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.y), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.z), sizeof(float));

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.x), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.y), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.z), sizeof(float));

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.x), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.y), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.z), sizeof(float));

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.x), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.y), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.z), sizeof(float));

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].transparentFactor), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specularPower), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].shineness), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].reflactionFactor), sizeof(float));

			}
		}

		// 스켈레톤
		if (m_DataFile->binHeader.isSkeleton == true)
		{
			// bone
			m_DataFile->binData.skeleton.bone_V.resize(m_DataFile->binHeader.boneCount);
			for (int i = 0; i < m_DataFile->binHeader.boneCount; ++i)
			{
				m_DataFile->binData.skeleton.bone_V[i].boneName.resize(m_DataFile->binHeader.boneNameLength_V[i]);
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].boneName[0]), m_DataFile->binHeader.boneNameLength_V[i]);

				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].boneIndex), sizeof(int));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].parentBoneIndex), sizeof(int));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].bindPoseTransform), sizeof(Float4x4));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].worldTM), sizeof(Float4x4));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].localTM), sizeof(Float4x4));
			}

			// boneOffset
			m_DataFile->binData.skeleton.boneOffset_V.resize(m_DataFile->binHeader.boneOffsetCount);
			in->read(reinterpret_cast<char*>(m_DataFile->binData.skeleton.boneOffset_V.data()), sizeof(Float4x4) * m_DataFile->binHeader.boneOffsetCount);
		}

		// 애니메이션
		if (m_DataFile->binHeader.isSkinnedAnimation == true)
		{
			// 실제 들어있는 애니메이션 클립의 수 만큼 리사이즈
			m_DataFile->binData.animation_V.resize(m_DataFile->binHeader.aniCount);

			// 애니메이션의 수 만큼 포문을 돈다
			for (int i = 0; i < m_DataFile->binHeader.aniCount; ++i)
			{
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].animationName[0]), m_DataFile->binHeader.aniNameLength[i]);
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].ticksPerFrame), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].totalKeyFrames), sizeof(unsigned int));

				// 애니메이션의 키 프레임은 본의 크기만큼 리사이즈해준다
				m_DataFile->binData.animation_V[i].animationKeyFrame.resize(m_DataFile->binHeader.boneCount);

				// 애니메이션의 본 내부를 채우기 위해 본의 크기만큼 포문을 돈다
				for (int j = 0; j < m_DataFile->binHeader.boneCount; ++j)
				{
					m_DataFile->binData.animation_V[i].animationKeyFrame[j].resize(m_DataFile->binHeader.animationKeyFrameCount_V[i]);
					in->read(reinterpret_cast<char*>(m_DataFile->binData.animation_V[i].animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_DataFile->binHeader.animationKeyFrameCount_V[i])));
				}
			}
		}
		else if (m_DataFile->binHeader.isMeshAnimation == true)
		{

			size_t _aniCount = m_DataFile->binHeader.aniCount;
			size_t _nowMeshCount = m_DataFile->binHeader.meshCount;

			m_DataFile->binData.animation_V.resize(_aniCount);

			for (int i = 0; i < _aniCount; ++i)
			{
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].animationName[0]), m_DataFile->binHeader.aniNameLength[i]);
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].ticksPerFrame), sizeof(float));
				in->read(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].totalKeyFrames), sizeof(unsigned int));

				// 애니메이션의 키 프레임은 본의 크기만큼 리사이즈해준다

				m_DataFile->binData.animation_V[i].animationKeyFrame.resize(_nowMeshCount);

				// 애니메이션의 본 내부를 채우기 위해 본의 크기만큼 포문을 돈다
				for (int j = 0; j < _nowMeshCount; ++j)
				{
					m_DataFile->binData.animation_V[i].animationKeyFrame[j].resize(m_DataFile->binHeader.animationKeyFrameCount_V[i]);
					in->read(reinterpret_cast<char*>(m_DataFile->binData.animation_V[i].animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_DataFile->binHeader.animationKeyFrameCount_V[i])));
				}
			}
		}
	}
}

