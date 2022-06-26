#include "BinarySerializer.h"
#include "binaryFileLayout.h"
#include "JMFBXparser.h"
#include <string.h>

BinarySerializer::BinarySerializer()
	: m_DataFile(nullptr), m_AnimFile(nullptr)
{
}

BinarySerializer::~BinarySerializer()
{
}

void BinarySerializer::SaveBinaryFileFBX(
	FBXModel* originModel,
	std::vector<MaterialDesc>& materialDesc,
	const char* filePath)
{
	m_DataFile = new FBXModelBinaryFile();

	OrganizeFBXModelFile(originModel, materialDesc);
	WriteBinarayFile(filePath);
	//WriteTextFile(filePath);
}

void BinarySerializer::SaveAnimFile(FBXModel* originModel, const char* filePath)
{
	if (originModel->pAnimation_V.size() <= 0)
	{
		return;
	}
	
	m_AnimFile = new AnimFileForWrite;
	
	OrganizeAnim(originModel);
	WriteAnimFile(filePath, originModel);
}

void BinarySerializer::OrganizeFBXModelFile(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc)
{
	/// 시리얼라이즈 정보를 가지고 있는 헤더
	OrganizeFBXModelHeader(originModel, materialDesc);

	/// 시리얼라이즈할 데이터
	OrganizeFBXModelData(originModel, materialDesc);
}

void BinarySerializer::WriteBinarayFile(const char* filePath)
{
	std::string _fullName(filePath);
	_fullName.append(".bin");

	std::ofstream _binaryFile(_fullName.c_str(), std::ios::out | std::ios::binary);

	if (_binaryFile.is_open())
	{
		/// 헤더
		WriteBinarayHeaderFile(&_binaryFile);

		/// 데이터
		WriteBinarayDataFile(&_binaryFile);
	}

	_binaryFile.close();
}

void BinarySerializer::WriteTextFile(const char* filePath)
{
	std::string _fullName(filePath);
	_fullName.append(".txt");

	std::ofstream _binaryFile(_fullName.c_str(), std::ios::out);

	///헤더 정보
	_binaryFile << "=======================================================" << "\n";
	_binaryFile << "//Header" << "\n";
	_binaryFile << "=======================================================" << "\n";

	_binaryFile << "Signature : " << m_DataFile->binHeader.mSignature << "\n";

	//-------------------------------------
	// 매쉬
	//-------------------------------------
	_binaryFile << "//MeshInfo-----------------------------------------------" << "\n";
	_binaryFile << "meshCount : " << m_DataFile->binHeader.meshCount << "\n";

	_binaryFile << "vertexCount :" << "\n";
	for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
	{
		_binaryFile << "\t" << "vertexCount [" << i << "] : " << m_DataFile->binHeader.vertexCount_V[i] << "\n";
	}

	_binaryFile << "indexCount :" << "\n";
	for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
	{
		_binaryFile << "\t" << "indexCount [" << i << "] : " << m_DataFile->binHeader.indexCount_V[i] << "\n";
	}

	_binaryFile << "vertexType : " << ((int)m_DataFile->binHeader.vertexType) << "\n";
	_binaryFile << "-------------------------------------------------------" << "\n";
	_binaryFile << "\n";

	//-------------------------------------
	// 메테리얼
	//-------------------------------------
	_binaryFile << "//Materialnfo--------------------------------------------" << "\n";
	std::string _isMaterialString = m_DataFile->binHeader.isMaterial ? "True" : "False";
	_binaryFile << "isMaterial : " << _isMaterialString << "\n";

	if (m_DataFile->binHeader.isMaterial == true)
	{
		_binaryFile << "textureCount : " << m_DataFile->binHeader.materialCount << "\n";

		for (int i = 0; i < m_DataFile->binHeader.materialNameLength_V.size(); ++i)
		{
			_binaryFile << "materialNameLength_V[" << i << "] : " << m_DataFile->binHeader.materialNameLength_V[i] << "\n";
		}
	}
	_binaryFile << "-------------------------------------------------------" << "\n";
	_binaryFile << "\n";

	/// 데이터 정보 
	//-------------------------------------
	// 매쉬
	//-------------------------------------
	_binaryFile << "=======================================================" << "\n";
	_binaryFile << "//Data" << "\n";
	_binaryFile << "=======================================================" << "\n";
	_binaryFile << "//MeshInfo-----------------------------------------------" << "\n";
	for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
	{
		for (int j = 0; j < m_DataFile->binData.mesh_V[i].vertex_V.size(); ++j)
		{
			_binaryFile << "mesh[" << i << "] vertex [" << j << "] pos.x : " << m_DataFile->binData.mesh_V[i].vertex_V[j].pos.x << "\n";
			_binaryFile << "mesh[" << i << "] vertex [" << j << "] pos.y : " << m_DataFile->binData.mesh_V[i].vertex_V[j].pos.y << "\n";
			_binaryFile << "mesh[" << i << "] vertex [" << j << "] pos.z : " << m_DataFile->binData.mesh_V[i].vertex_V[j].pos.z << "\n";

			_binaryFile << "mesh[" << i << "] vertex [" << j << "] texCoord.x : " << m_DataFile->binData.mesh_V[i].vertex_V[j].texCoord.x << "\n";
			_binaryFile << "mesh[" << i << "] vertex [" << j << "] texCoord.y : " << m_DataFile->binData.mesh_V[i].vertex_V[j].texCoord.y << "\n\n";
		}
	}

	for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
	{
		for (int j = 0; j < m_DataFile->binData.mesh_V[i].index_V.size(); ++j)
		{
			if (j % 3 == 0)
			{
				_binaryFile << "\n";
			}
			_binaryFile << m_DataFile->binData.mesh_V[i].index_V[j] << " ";
		}
	}
	_binaryFile << "\n";
	_binaryFile << "-------------------------------------------------------" << "\n";
	_binaryFile << "\n";

	//-------------------------------------
	// 메테리얼
	//-------------------------------------
	if (m_DataFile->binHeader.isMaterial == true)
	{
		_binaryFile << "//Materialnfo--------------------------------------------" << "\n";

		for (int i = 0; i < m_DataFile->binHeader.materialNameLength_V.size(); ++i)
		{
			//_binaryFile << "materialName[" << i << "] : " << m_DataFile->binData.materialName_V[i] << "\n";
		}
		_binaryFile << "-------------------------------------------------------" << "\n";
	}

	_binaryFile.close();
}

void BinarySerializer::OrganizeFBXModelHeader(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc)
{
	/// 시리얼라이즈 정보를 가지고 있는 헤더
	m_DataFile->binHeader.mSignature[0] = 'F';
	m_DataFile->binHeader.mSignature[1] = 'M';
	m_DataFile->binHeader.mSignature[2] = 'B';
	m_DataFile->binHeader.mSignature[3] = 'D';

	//-------------------------------------
	// 매쉬
	//-------------------------------------
	m_DataFile->binHeader.meshCount = static_cast<int>(originModel->pMesh_V.size());
	m_DataFile->binHeader.vertexCount_V.resize(originModel->pMesh_V.size());
	m_DataFile->binHeader.indexCount_V.resize(originModel->pMesh_V.size());

	for (int i = 0; i < originModel->pMesh_V.size(); ++i)
	{
		m_DataFile->binHeader.vertexCount_V[i] = static_cast<int>(originModel->pMesh_V[i]->optVertex_V.size());
		m_DataFile->binHeader.indexCount_V[i] = static_cast<int>(originModel->pMesh_V[i]->optIndexNum * 3);
	}

	//-------------------------------------
	// 머테리얼
	//-------------------------------------
	// 머테리얼이 있는지
	if (materialDesc.size() > 0)
	{
		m_DataFile->binHeader.isMaterial = true;
	}

	if (m_DataFile->binHeader.isMaterial == true)
	{
		// 머테리얼의 개수	
		m_DataFile->binHeader.materialCount = materialDesc.size();
		// 머테리얼 이름의 길이
		m_DataFile->binHeader.materialNameLength_V.resize(m_DataFile->binHeader.materialCount);
		for (int i = 0; i < m_DataFile->binHeader.materialCount; ++i)
		{
			m_DataFile->binHeader.materialNameLength_V[i] = static_cast<int>(materialDesc[i].materialName.length()) + 1;
		}
		// 머테리얼에 종속되어있는 텍스쳐들의 갯수
		m_DataFile->binHeader.textureNameCount_V.resize(m_DataFile->binHeader.materialCount);
		for (int i = 0; i < m_DataFile->binHeader.materialCount; ++i)
		{
			m_DataFile->binHeader.textureNameCount_V[i] = static_cast<int>(materialDesc[i].textureName_V.size());
		}
		// 머테리얼에 종속되어있는 텍스쳐들의 이름의 길이
		m_DataFile->binHeader.textureNameLength_V.resize(m_DataFile->binHeader.materialCount);
		for (int i = 0; i < m_DataFile->binHeader.materialCount; ++i)
		{
			m_DataFile->binHeader.textureNameLength_V[i].resize(materialDesc[i].textureName_V.size());
			for (int j = 0; j < materialDesc[i].textureName_V.size(); ++j)
			{
				m_DataFile->binHeader.textureNameLength_V[i][j] = static_cast<int>(materialDesc[i].textureName_V[j].length()) + 1;
			}
		}
		// 머테리얼의 인덱스
		m_DataFile->binHeader.materialMapping_V.resize(m_DataFile->binHeader.meshCount);
		for (int i = 0; i < m_DataFile->binHeader.meshCount; i++)
		{
			for (int j = 0; j < materialDesc.size(); j++)
			{
				if (originModel->pMesh_V[i]->materialName.compare(materialDesc[j].materialName) == 0)
				{
					m_DataFile->binHeader.materialMapping_V[i] = j;
					break;
				}
			}
		}
	}

	//-------------------------------------
	// 스켈레톤
	//-------------------------------------
	// 본이 있는지
	if (originModel->animationType == eANIM_TYPE::SKINNED || originModel->pSkeleton != nullptr)
	{
		m_DataFile->binHeader.isSkeleton = true;
	}
	if (m_DataFile->binHeader.isSkeleton == true)
	{
		m_DataFile->binHeader.boneCount = originModel->pSkeleton->m_Bones.size();
		m_DataFile->binHeader.boneOffsetCount = originModel->pSkeleton->m_BoneOffsets.size();
		m_DataFile->binHeader.boneNameLength_V.resize(m_DataFile->binHeader.boneCount);
		for (int i = 0; i < m_DataFile->binHeader.boneCount; i++)
		{
			m_DataFile->binHeader.boneNameLength_V[i] = static_cast<int>(originModel->pSkeleton->m_Bones[i]->boneName.length()) + 1;
		}
	}

	//-------------------------------------
	// 애니메이션
	//-------------------------------------
	if (originModel->isSkinnedAnimation == true)
	{
		m_DataFile->binHeader.isSkinnedAnimation = true;
	}
	if (originModel->isMeshAnimation == true)
	{
		m_DataFile->binHeader.isMeshAnimation = true;
	}
	if (m_DataFile->binHeader.isSkinnedAnimation == true || m_DataFile->binHeader.isMeshAnimation == true)
	{
		m_DataFile->binHeader.aniCount = originModel->pAnimation_V.size();

		m_DataFile->binHeader.aniNameLength.resize(originModel->pAnimation_V.size());
		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			m_DataFile->binHeader.aniNameLength[i] = static_cast<int>(originModel->pAnimation_V[i]->m_AnimationName.length() + 1);
		}

		// 애니 키프레임 갯수
		m_DataFile->binHeader.animationKeyFrameCount_V.resize(originModel->pAnimation_V.size());

		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			int _nowAnimCount = static_cast<int>(originModel->pAnimation_V[i]->animationKeyFrame[0].size());
			m_DataFile->binHeader.animationKeyFrameCount_V[i] = _nowAnimCount;
		}
	}

	if (m_DataFile->binHeader.isSkeleton == true && m_DataFile->binHeader.isSkinnedAnimation == true)
	{
		m_DataFile->binHeader.vertexType = VertexType::Type_VertexSkinned;
	}
	else
	{
		m_DataFile->binHeader.vertexType = VertexType::Type_Vertex;
	}

}

void BinarySerializer::WriteBinarayHeaderFile(std::ofstream* out)
{
	/// 헤더
	out->write(reinterpret_cast<char*>(&(m_DataFile->binHeader.mSignature)), sizeof(m_DataFile->binHeader.mSignature));

	//-------------------------------------
	// 매쉬
	//-------------------------------------
	out->write(reinterpret_cast<char*>(&(m_DataFile->binHeader.meshCount)), sizeof(m_DataFile->binHeader.meshCount));
	out->write(reinterpret_cast<char*>(m_DataFile->binHeader.vertexCount_V.data()), sizeof(int) * m_DataFile->binHeader.vertexCount_V.size());
	out->write(reinterpret_cast<char*>(m_DataFile->binHeader.indexCount_V.data()), sizeof(int) * m_DataFile->binHeader.indexCount_V.size());
	out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.vertexType), sizeof(m_DataFile->binHeader.vertexType));

	//-------------------------------------
	// 머테리얼
	//-------------------------------------
	out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.isMaterial), sizeof(m_DataFile->binHeader.isMaterial));
	if (m_DataFile->binHeader.isMaterial == true)
	{
		out->write(reinterpret_cast<char*>(&(m_DataFile->binHeader.materialCount)), sizeof(m_DataFile->binHeader.materialCount));
		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.materialNameLength_V.data()), sizeof(int) * m_DataFile->binHeader.materialNameLength_V.size());
		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameCount_V.data()), sizeof(int) * m_DataFile->binHeader.textureNameCount_V.size());
		for (int i = 0; i < m_DataFile->binHeader.textureNameLength_V.size(); ++i)
		{
			out->write(reinterpret_cast<char*>(m_DataFile->binHeader.textureNameLength_V[i].data()), ((sizeof(int) * m_DataFile->binHeader.textureNameLength_V[i].size())));
		}
		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.materialMapping_V.data()), sizeof(int) * m_DataFile->binHeader.materialMapping_V.size());
	}

	//-------------------------------------
	// 스켈레톤
	//-------------------------------------
	out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.isSkeleton), sizeof(m_DataFile->binHeader.isSkeleton));
	if (m_DataFile->binHeader.isSkeleton == true)
	{
		out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.boneCount), sizeof(m_DataFile->binHeader.boneCount));
		out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.boneOffsetCount), sizeof(m_DataFile->binHeader.boneOffsetCount));
		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.boneNameLength_V.data()), sizeof(int) * m_DataFile->binHeader.boneNameLength_V.size());
	}

	//-------------------------------------
	// 애니메이션
	//-------------------------------------
	out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.isSkinnedAnimation), sizeof(m_DataFile->binHeader.isSkinnedAnimation));
	out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.isMeshAnimation), sizeof(m_DataFile->binHeader.isMeshAnimation));
	if (m_DataFile->binHeader.isSkinnedAnimation == true || m_DataFile->binHeader.isMeshAnimation == true)
	{
		out->write(reinterpret_cast<char*>(&m_DataFile->binHeader.aniCount), sizeof(m_DataFile->binHeader.aniCount));

		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.aniNameLength.data()), sizeof(int) * m_DataFile->binHeader.aniNameLength.size());

		out->write(reinterpret_cast<char*>(m_DataFile->binHeader.animationKeyFrameCount_V.data()), sizeof(int) * m_DataFile->binHeader.animationKeyFrameCount_V.size());

	}
}

void BinarySerializer::OrganizeFBXModelData(FBXModel* originModel, std::vector<MaterialDesc>& materialDesc)
{
	//-------------------------------------
	// 매쉬
	//-------------------------------------
	m_DataFile->binData.mesh_V.resize(originModel->pMesh_V.size());

	for (int i = 0; i < originModel->pMesh_V.size(); ++i)
	{
		// 매쉬가 가지고있는 버텍스수만큼 크기를 정해준다.
		m_DataFile->binData.mesh_V[i].vertex_V.resize(originModel->pMesh_V[i]->optVertex_V.size());

		for (int j = 0; j < originModel->pMesh_V[i]->optVertex_V.size(); j++)
		{
			// 어떤 버텍스에도 대응할수있는 모든데이터 정보를 가지고있는 버텍스에 정보를 담는다
			m_DataFile->binData.mesh_V[i].vertex_V[j].pos.x = originModel->pMesh_V[i]->optVertex_V[j]->pos.x;
			m_DataFile->binData.mesh_V[i].vertex_V[j].pos.y = originModel->pMesh_V[i]->optVertex_V[j]->pos.y;
			m_DataFile->binData.mesh_V[i].vertex_V[j].pos.z = originModel->pMesh_V[i]->optVertex_V[j]->pos.z;

			m_DataFile->binData.mesh_V[i].vertex_V[j].texCoord.x = originModel->pMesh_V[i]->optVertex_V[j]->u;
			m_DataFile->binData.mesh_V[i].vertex_V[j].texCoord.y = originModel->pMesh_V[i]->optVertex_V[j]->v;

			m_DataFile->binData.mesh_V[i].vertex_V[j].normal.x = originModel->pMesh_V[i]->optVertex_V[j]->normal.x;
			m_DataFile->binData.mesh_V[i].vertex_V[j].normal.y = originModel->pMesh_V[i]->optVertex_V[j]->normal.y;
			m_DataFile->binData.mesh_V[i].vertex_V[j].normal.z = originModel->pMesh_V[i]->optVertex_V[j]->normal.z;

			m_DataFile->binData.mesh_V[i].vertex_V[j].tangent.x = originModel->pMesh_V[i]->optVertex_V[j]->tangent.x;
			m_DataFile->binData.mesh_V[i].vertex_V[j].tangent.y = originModel->pMesh_V[i]->optVertex_V[j]->tangent.y;
			m_DataFile->binData.mesh_V[i].vertex_V[j].tangent.z = originModel->pMesh_V[i]->optVertex_V[j]->tangent.z;
			m_DataFile->binData.mesh_V[i].vertex_V[j].tangent.w = originModel->pMesh_V[i]->optVertex_V[j]->tangent.w;

			m_DataFile->binData.mesh_V[i].vertex_V[j].weights.x = originModel->pMesh_V[i]->optVertex_V[j]->boneWeight_Arr[0];
			m_DataFile->binData.mesh_V[i].vertex_V[j].weights.y = originModel->pMesh_V[i]->optVertex_V[j]->boneWeight_Arr[1];
			m_DataFile->binData.mesh_V[i].vertex_V[j].weights.z = originModel->pMesh_V[i]->optVertex_V[j]->boneWeight_Arr[2];

			m_DataFile->binData.mesh_V[i].vertex_V[j].boneIndices[0] = originModel->pMesh_V[i]->optVertex_V[j]->boneIndex_Arr[0];
			m_DataFile->binData.mesh_V[i].vertex_V[j].boneIndices[1] = originModel->pMesh_V[i]->optVertex_V[j]->boneIndex_Arr[1];
			m_DataFile->binData.mesh_V[i].vertex_V[j].boneIndices[2] = originModel->pMesh_V[i]->optVertex_V[j]->boneIndex_Arr[2];
			m_DataFile->binData.mesh_V[i].vertex_V[j].boneIndices[3] = originModel->pMesh_V[i]->optVertex_V[j]->boneIndex_Arr[3];

			m_DataFile->binData.mesh_V[i].vertex_V[j].rgba.x = 0.f;
			m_DataFile->binData.mesh_V[i].vertex_V[j].rgba.y = 0.f;
			m_DataFile->binData.mesh_V[i].vertex_V[j].rgba.z = 0.f;
			m_DataFile->binData.mesh_V[i].vertex_V[j].rgba.w = 0.f;
		}

		m_DataFile->binData.mesh_V[i].index_V.resize(static_cast<size_t>(originModel->pMesh_V[i]->optIndexNum * 3));

		// ParsingData에서 인덱스 값을 가져온다
		for (int j = 0; j < originModel->pMesh_V[i]->optIndexNum; j++)
		{
			m_DataFile->binData.mesh_V[i].index_V[j * 3 + 0] = static_cast<DWORD>(originModel->pMesh_V[i]->pOptIndex[j].index_A[0]);
			m_DataFile->binData.mesh_V[i].index_V[j * 3 + 1] = static_cast<DWORD>(originModel->pMesh_V[i]->pOptIndex[j].index_A[1]);
			m_DataFile->binData.mesh_V[i].index_V[j * 3 + 2] = static_cast<DWORD>(originModel->pMesh_V[i]->pOptIndex[j].index_A[2]);
		}
		
		m_DataFile->binData.mesh_V[i].meshIndex = originModel->pMesh_V[i]->meshIndex;
		m_DataFile->binData.mesh_V[i].isParent = originModel->pMesh_V[i]->isParentExist;
		if (m_DataFile->binData.mesh_V[i].isParent == true)
		{
			m_DataFile->binData.mesh_V[i].parentIndex = originModel->pMesh_V[i]->pNodeParentMesh->meshIndex;
		}

		Float4x4Func _temp;

		_temp = originModel->pMesh_V[i]->worldTM;
		m_DataFile->binData.mesh_V[i].meshWorldTM = _temp;

		_temp = originModel->pMesh_V[i]->localTM;
		m_DataFile->binData.mesh_V[i].meshLocalTM = _temp;
	}

	//-------------------------------------
	// 머테리얼
	//-------------------------------------
	if (m_DataFile->binHeader.isMaterial == true)
	{
		m_DataFile->binData.material_V = materialDesc;
	}

	//-------------------------------------
	// 스켈레톤
	//-------------------------------------
	if (m_DataFile->binHeader.isSkeleton == true)
	{
		// bone
		m_DataFile->binData.skeleton.bone_V.resize(originModel->pSkeleton->m_Bones.size());
		for (int i = 0; i < originModel->pSkeleton->m_Bones.size(); ++i)
		{
			m_DataFile->binData.skeleton.bone_V[i].boneName = originModel->pSkeleton->m_Bones[i]->boneName;
			m_DataFile->binData.skeleton.bone_V[i].boneIndex = originModel->pSkeleton->m_Bones[i]->boneIndex;
			m_DataFile->binData.skeleton.bone_V[i].parentBoneIndex = originModel->pSkeleton->m_Bones[i]->parentBoneIndex;

			Float4x4Func _temp;
			_temp = originModel->pSkeleton->m_Bones[i]->bindPoseTransform;
			m_DataFile->binData.skeleton.bone_V[i].bindPoseTransform = _temp;

			_temp = originModel->pSkeleton->m_Bones[i]->worldTM;
			m_DataFile->binData.skeleton.bone_V[i].worldTM = _temp;

			_temp = originModel->pSkeleton->m_Bones[i]->localTM;
			m_DataFile->binData.skeleton.bone_V[i].localTM = _temp;
		}

		// boneOffset
		std::vector<Float4x4Func> boneOffsetTemp_V(originModel->pSkeleton->m_BoneOffsets.size());
		m_DataFile->binData.skeleton.boneOffset_V.resize(originModel->pSkeleton->m_BoneOffsets.size());
		for (int i = 0; i < originModel->pSkeleton->m_BoneOffsets.size(); ++i)
		{
			boneOffsetTemp_V[i] = originModel->pSkeleton->m_BoneOffsets[i];
			m_DataFile->binData.skeleton.boneOffset_V[i] = boneOffsetTemp_V[i];
		}
	}

	//-------------------------------------
	// 애니메이션
	//-------------------------------------
	if (m_DataFile->binHeader.isSkinnedAnimation == true || m_DataFile->binHeader.isMeshAnimation == true)
	{
		m_DataFile->binData.animation_V.resize(originModel->pAnimation_V.size());
		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			m_DataFile->binData.animation_V[i].animationName = originModel->pAnimation_V[i]->m_AnimationName;
			m_DataFile->binData.animation_V[i].ticksPerFrame = originModel->pAnimation_V[i]->m_TicksPerFrame;
			m_DataFile->binData.animation_V[i].totalKeyFrames = originModel->pAnimation_V[i]->m_TotalKeyFrames;
		}

		// 애니메이션 키프레임

		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			m_DataFile->binData.animation_V[i].animationKeyFrame.resize(originModel->pAnimation_V[i]->animationKeyFrame.size());
		}

		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			for (int j = 0; j < originModel->pAnimation_V[i]->animationKeyFrame.size(); ++j)
			{
				m_DataFile->binData.animation_V[i].animationKeyFrame[j].resize(originModel->pAnimation_V[i]->animationKeyFrame[j].size());
			}
		}

		for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
		{
			for (int j = 0; j < originModel->pAnimation_V[i]->animationKeyFrame.size(); ++j)
			{
				for (int k = 0; k < originModel->pAnimation_V[i]->animationKeyFrame[j].size(); ++k)
				{
					Float4x4Func _temp;
					_temp = originModel->pAnimation_V[i]->animationKeyFrame[j][k];
					m_DataFile->binData.animation_V[i].animationKeyFrame[j][k] = _temp;
				}
			}
		}
	}
}

void BinarySerializer::WriteBinarayDataFile(std::ofstream* out)
{
	//-------------------------------------
	// 매쉬
	//-------------------------------------
	for (int i = 0; i < m_DataFile->binData.mesh_V.size(); ++i)
	{
		out->write(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].vertex_V.data()), sizeof(Vertex::SerializerVertex) * m_DataFile->binData.mesh_V[i].vertex_V.size());
		out->write(reinterpret_cast<char*>(m_DataFile->binData.mesh_V[i].index_V.data()), sizeof(DWORD) * m_DataFile->binData.mesh_V[i].index_V.size());
	 
		out->write(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshIndex), sizeof(int));
		out->write(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].isParent), sizeof(bool));
		out->write(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].parentIndex), sizeof(int));

		out->write(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshWorldTM), sizeof(Float4x4));
		out->write(reinterpret_cast<char*>(&m_DataFile->binData.mesh_V[i].meshLocalTM), sizeof(Float4x4));
	}

	//-------------------------------------
	// 머테리얼
	//-------------------------------------
	if (m_DataFile->binHeader.isMaterial == true)
	{
		for (int i = 0; i < m_DataFile->binData.material_V.size(); ++i)
		{
			out->write(m_DataFile->binData.material_V[i].materialName.c_str(), static_cast<size_t>(m_DataFile->binHeader.materialNameLength_V[i]) - 1);
			out->write("\0", 1);

			for (int j = 0; j < m_DataFile->binData.material_V[i].textureName_V.size(); ++j)
				//for (int j = 0; j < m_DataFile->binData.material_V.size(); ++j)
			{
				out->write(m_DataFile->binData.material_V[i].textureName_V[j].c_str(), static_cast<size_t>(m_DataFile->binHeader.textureNameLength_V[i][j]) - 1);
				out->write("\0", 1);
			}

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.x), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.y), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].diffuse.z), sizeof(float));

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.x), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.y), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].emissive.z), sizeof(float));

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.x), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.y), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].ambient.z), sizeof(float));

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.x), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.y), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].fresnelR0.z), sizeof(float));

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.x), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.y), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specular.z), sizeof(float));

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].transparentFactor), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].specularPower), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].shineness), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.material_V[i].reflactionFactor), sizeof(float));
		}
	}

	//-------------------------------------
	// 스켈레톤
	//-------------------------------------
	if (m_DataFile->binHeader.isSkeleton == true)
	{
		// 본
		for (int i = 0; i < m_DataFile->binData.skeleton.bone_V.size(); ++i)
		{
			out->write(m_DataFile->binData.skeleton.bone_V[i].boneName.c_str(), static_cast<size_t>(m_DataFile->binHeader.boneNameLength_V[i]) - 1);
			out->write("\0", 1);

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].boneIndex), sizeof(int));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].parentBoneIndex), sizeof(int));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].bindPoseTransform), sizeof(Float4x4));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].worldTM), sizeof(Float4x4));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.skeleton.bone_V[i].localTM), sizeof(Float4x4));
		}

		// 본 오프셋
		out->write(reinterpret_cast<char*>(m_DataFile->binData.skeleton.boneOffset_V.data()), sizeof(Float4x4) * m_DataFile->binData.skeleton.boneOffset_V.size());
	}

	//-------------------------------------
	// 애니메이션
	//-------------------------------------

	if (m_DataFile->binHeader.isSkinnedAnimation == true || m_DataFile->binHeader.isMeshAnimation == true)
	{
		for (int i = 0; i < m_DataFile->binData.animation_V.size(); ++i)
		{
			out->write(m_DataFile->binData.animation_V[i].animationName.c_str(), static_cast<size_t>(m_DataFile->binHeader.aniNameLength[i]) - 1);
			out->write("\0", 1);

			out->write(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].ticksPerFrame), sizeof(float));
			out->write(reinterpret_cast<char*>(&m_DataFile->binData.animation_V[i].totalKeyFrames), sizeof(unsigned int));

			for (int j = 0; j < m_DataFile->binData.animation_V[i].animationKeyFrame.size(); ++j)
			{
				out->write(reinterpret_cast<char*>(m_DataFile->binData.animation_V[i].animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_DataFile->binData.animation_V[i].animationKeyFrame[j].size())));
			}
		}
	}
}

void BinarySerializer::OrganizeAnim(FBXModel* originModel)
{
	m_AnimFile->animHeader.resize(originModel->pAnimation_V.size());
	m_AnimFile->animData.resize(originModel->pAnimation_V.size());

	for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
	{
		/// 헤더
		if (originModel->animationType == eANIM_TYPE::MESH)
		{
			m_AnimFile->animHeader[i].isMeshAnimation = true;
		}
		if (originModel->animationType == eANIM_TYPE::SKINNED)
		{
			m_AnimFile->animHeader[i].isSkinnedAnimation = true;
			m_AnimFile->animHeader[i].isSkeleton = true;
		};
		if (m_AnimFile->animHeader[i].isSkeleton == true)
		{
			m_AnimFile->animHeader[i].boneCount = originModel->pSkeleton->m_Bones.size();
		}
		m_AnimFile->animHeader[i].meshCount = originModel->pMesh_V.size();
		m_AnimFile->animHeader[i].aniNameLength = static_cast<int>(originModel->pAnimation_V[i]->m_AnimationName.length() + 1);

		int _nowAnimCount = static_cast<int>(originModel->pAnimation_V[i]->animationKeyFrame[0].size());
		m_AnimFile->animHeader[i].animationKeyFrameCount = _nowAnimCount;

		/// 데이터
		m_AnimFile->animData[i].animationName = originModel->pAnimation_V[i]->m_AnimationName;
		m_AnimFile->animData[i].ticksPerFrame = originModel->pAnimation_V[i]->m_TicksPerFrame;
		m_AnimFile->animData[i].totalKeyFrames = originModel->pAnimation_V[i]->m_TotalKeyFrames;


		m_AnimFile->animData[i].animationKeyFrame.resize(originModel->pAnimation_V[i]->animationKeyFrame.size());
		for (int j = 0; j < originModel->pAnimation_V[i]->animationKeyFrame.size(); ++j)
		{
			m_AnimFile->animData[i].animationKeyFrame[j].resize(originModel->pAnimation_V[i]->animationKeyFrame[j].size());
		}

		for (int j = 0; j < originModel->pAnimation_V[i]->animationKeyFrame.size(); ++j)
		{
			for (int k = 0; k < originModel->pAnimation_V[i]->animationKeyFrame[j].size(); ++k)
			{
				Float4x4Func _temp;
				_temp = originModel->pAnimation_V[i]->animationKeyFrame[j][k];
				m_AnimFile->animData[i].animationKeyFrame[j][k] = _temp;
			}
		}
	}
}

void BinarySerializer::WriteAnimFile(const char* filePath, FBXModel* originModel)
{
	for (int i = 0; i < originModel->pAnimation_V.size(); ++i)
	{	
		std::string _fullName(filePath);
		_fullName.append("_" + originModel->pAnimation_V[i]->m_AnimationName + ".anim");

		std::ofstream _binaryFile(_fullName.c_str(), std::ios::out | std::ios::binary);

		if (_binaryFile.is_open())
		{
			/// 헤더
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].isSkinnedAnimation), sizeof(bool));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].isMeshAnimation), sizeof(bool));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].isSkeleton), sizeof(bool));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].boneCount), sizeof(size_t));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].meshCount), sizeof(size_t));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].aniNameLength), sizeof(int));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animHeader[i].animationKeyFrameCount), sizeof(int));

			/// 데이터
			_binaryFile.write(m_AnimFile->animData[i].animationName.c_str(), static_cast<size_t>(m_AnimFile->animHeader[i].aniNameLength) - 1);
			_binaryFile.write("\0", 1);

			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animData[i].ticksPerFrame), sizeof(float));
			_binaryFile.write(reinterpret_cast<char*>(&m_AnimFile->animData[i].totalKeyFrames), sizeof(unsigned int));

			for (int j = 0; j < m_AnimFile->animData[i].animationKeyFrame.size(); ++j)
			{
				_binaryFile.write(reinterpret_cast<char*>(m_AnimFile->animData[i].animationKeyFrame[j].data()), ((sizeof(Float4x4) * m_AnimFile->animData[i].animationKeyFrame[j].size())));
			}
		}

		_binaryFile.close();
	}
}
