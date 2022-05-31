#include "pch.h"

#include <iterator>
#include "DX11MeshFactory.h"

#include "VertexHelper.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"

#include "NodeData.h"
#include "BinaryFileLayout.h"

#include "NaviMeshFace.h"
#include "CenterCreator.h"
#include "BoundingVolume.h"

DX11MeshFactory::DX11MeshFactory()
{
}

DX11MeshFactory::~DX11MeshFactory()
{
}


void StoreMatrix(DirectX::SimpleMath::Matrix& data, const Float4x4& origin)
{
	data._11 = origin.m00;	data._12 = origin.m01;	data._13 = origin.m02;	data._14 = origin.m03;
	data._21 = origin.m10; 	data._22 = origin.m11;	data._23 = origin.m12;	data._24 = origin.m13;
	data._31 = origin.m20; 	data._32 = origin.m21;	data._33 = origin.m22;	data._34 = origin.m23;
	data._41 = origin.m30; 	data._42 = origin.m31;	data._43 = origin.m32;	data._44 = origin.m33;
}

void DX11MeshFactory::GetModelMeshBinary(
	ID3D11Device* pDevice,
	std::shared_ptr<FBXModelBinaryFile> binData,
	std::shared_ptr<ModelMesh> outModel,
	std::vector<std::shared_ptr<MaterialDesc>>& outMaterial_V)
{
	HRESULT _hr;

	// 최종적으로 담겨져야할 클래스
	std::shared_ptr<ModelMesh> _nowModelMesh = outModel;

	// 매쉬의 중점을 만들어줄 센터 크리에이터
	m_CenterCreator = std::make_shared<CenterCreator>();

#pragma region MESH

	// 위 클래스에 담겨야할 내부 원소
	std::vector<std::shared_ptr<NodeData>>* _pNodeData_V = new std::vector<std::shared_ptr<NodeData>>;

	for (int i = 0; i < binData->binData.mesh_V.size(); ++i)
	{
		std::shared_ptr<NodeData> _newNodeData = std::make_shared<NodeData>();



		std::vector<Vertex::SerializerVertex> _vertices = binData->binData.mesh_V[i].vertex_V;

		IndexBuffer* _nowIndexBuffer = new IndexBuffer();

		try
		{
			/// 이니셜라이즈 하기전에 버텍스 종류별로 형변환 필요
			if (binData->binHeader.vertexType == VertexType::Type_Vertex)
			{
				// 바운딩박스
				_newNodeData->m_BoundingVolume = std::make_unique<BoundingVolume>();

				std::vector<Vertex::Vertex> _vertex(binData->binData.mesh_V[i].vertex_V.size());

				for (int j = 0; j < binData->binData.mesh_V[i].vertex_V.size(); ++j)
				{
					_vertex[j].pos = binData->binData.mesh_V[i].vertex_V[j].pos;
					_vertex[j].normal = binData->binData.mesh_V[i].vertex_V[j].normal;
					_vertex[j].texCoord = binData->binData.mesh_V[i].vertex_V[j].texCoord;
					_vertex[j].tangent = binData->binData.mesh_V[i].vertex_V[j].tangent;

					// 센터크리에이터가 매쉬의 모든 버텍스의 좌표를 읽어들여 minimum과 maximum을 찾아낸다(매쉬)
					m_CenterCreator->ApplyNowPos(binData->binData.mesh_V[i].vertex_V[j].pos);
				}

				// 바운딩 박스 셋팅
				m_CenterCreator->GetFinalCenter(_newNodeData->m_BoundingVolume->GetOriginCenter());
				m_CenterCreator->GetFinalExtents(_newNodeData->m_BoundingVolume->GetExtent());
				_newNodeData->m_BoundingVolume->SetRadius();

				VertexBuffer<Vertex::Vertex>* _nowVertexBuffer = new VertexBuffer<Vertex::Vertex>;
				_hr = _nowVertexBuffer->Initialize(pDevice, _vertex.data(), static_cast<UINT>(_vertices.size()));

				_newNodeData->m_VertexBuffer.Swap(_nowVertexBuffer->GetPtr());
				_newNodeData->m_Stride = _nowVertexBuffer->Stride();

				COM_ERROR_IF_FAILED(_hr, "VertexBuffer Initialize Fail");
			}
			if (binData->binHeader.vertexType == VertexType::Type_VertexSkinned)
			{
				_newNodeData->m_BoundingVolume = std::make_unique<BoundingVolume>(true);

				std::vector<Vertex::VertexNormalSkinned> _vertex(binData->binData.mesh_V[i].vertex_V.size());

				for (int j = 0; j < binData->binData.mesh_V[i].vertex_V.size(); ++j)
				{
					_vertex[j].pos = binData->binData.mesh_V[i].vertex_V[j].pos;
					_vertex[j].normal = binData->binData.mesh_V[i].vertex_V[j].normal;
					_vertex[j].texCoord = binData->binData.mesh_V[i].vertex_V[j].texCoord;
					_vertex[j].tangent = binData->binData.mesh_V[i].vertex_V[j].tangent;

					_vertex[j].weights[0] = binData->binData.mesh_V[i].vertex_V[j].weights.x;
					_vertex[j].weights[1] = binData->binData.mesh_V[i].vertex_V[j].weights.y;
					_vertex[j].weights[2] = binData->binData.mesh_V[i].vertex_V[j].weights.z;
					_vertex[j].weights[3] = 0.0f;

					_vertex[j].boneIndices[0] = binData->binData.mesh_V[i].vertex_V[j].boneIndices[0];
					_vertex[j].boneIndices[1] = binData->binData.mesh_V[i].vertex_V[j].boneIndices[1];
					_vertex[j].boneIndices[2] = binData->binData.mesh_V[i].vertex_V[j].boneIndices[2];
					_vertex[j].boneIndices[3] = binData->binData.mesh_V[i].vertex_V[j].boneIndices[3];

					// 센터크리에이터가 매쉬의 모든 버텍스의 좌표를 읽어들여 minimum과 maximum을 찾아낸다(본)
					m_CenterCreator->ApplyNowPos(binData->binData.mesh_V[i].vertex_V[j].pos);
				}

				// 바운딩 박스 셋팅
				m_CenterCreator->GetFinalCenter(_newNodeData->m_BoundingVolume->GetOriginCenter());
				m_CenterCreator->GetFinalExtents(_newNodeData->m_BoundingVolume->GetExtent());
				_newNodeData->m_BoundingVolume->SetRadius(2.0f);

				VertexBuffer<Vertex::VertexNormalSkinned>* _nowVertexBuffer = new VertexBuffer<Vertex::VertexNormalSkinned>;
				_hr = _nowVertexBuffer->Initialize(pDevice, _vertex.data(), static_cast<UINT>(_vertices.size()));

				_newNodeData->m_VertexBuffer.Swap(_nowVertexBuffer->GetPtr());
				_newNodeData->m_Stride = _nowVertexBuffer->Stride();


				COM_ERROR_IF_FAILED(_hr, "VertexBuffer Initialize Fail");

			}

			std::vector<DWORD> _indices = binData->binData.mesh_V[i].index_V;

			_hr = _nowIndexBuffer->Initialize(pDevice, _indices.data(), static_cast<UINT>(_indices.size()));

			// 본 없이 매쉬로만 이루어져 있을때만 저장
			if (binData->binHeader.vertexType == VertexType::Type_Vertex)
			{
				_newNodeData->isExistParent = binData->binData.mesh_V[i].isParent;

				StoreMatrix(_newNodeData->meshWorldTM, binData->binData.mesh_V[i].meshWorldTM);
				StoreMatrix(_newNodeData->meshLocalTM, binData->binData.mesh_V[i].meshLocalTM);

				if (_newNodeData->isExistParent == true)
				{
					_newNodeData->parentIndex = binData->binData.mesh_V[i].parentIndex;
					_newNodeData->pParentNode = _pNodeData_V->at(_newNodeData->parentIndex);

					DirectX::SimpleMath::Matrix _nowWorld = _newNodeData->pParentNode->meshWorldTM;

					//_newNodeData->meshLocalTM = _newNodeData->meshWorldTM * _nowWorld.Invert();
				}
			}

			COM_ERROR_IF_FAILED(_hr, "IndexBuffer Initialize Fail");

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}

		if (binData->binHeader.isMaterial == true)
		{
			for (int i = 0; i < binData->binData.material_V.size(); ++i)
			{
				outMaterial_V[i]->materialName = binData->binData.material_V[i].materialName.c_str();
				outMaterial_V[i]->textureName_V = binData->binData.material_V[i].textureName_V;

				outMaterial_V[i]->diffuse = binData->binData.material_V[i].diffuse;
				outMaterial_V[i]->emissive = binData->binData.material_V[i].emissive;
				outMaterial_V[i]->ambient = binData->binData.material_V[i].ambient;
				outMaterial_V[i]->fresnelR0 = binData->binData.material_V[i].fresnelR0;
				outMaterial_V[i]->specular = binData->binData.material_V[i].specular;

				outMaterial_V[i]->transparentFactor = binData->binData.material_V[i].transparentFactor;
				outMaterial_V[i]->specularPower = binData->binData.material_V[i].specularPower;
				outMaterial_V[i]->shineness = binData->binData.material_V[i].shineness;
				outMaterial_V[i]->reflactionFactor = binData->binData.material_V[i].reflactionFactor;
			}
		}

		_newNodeData->m_IndexBuffer.Swap(_nowIndexBuffer->GetPtr());
		_newNodeData->m_IndexCount = _nowIndexBuffer->IndexCount();

		if (binData->binHeader.isMaterial == true)
		{
			int _nowMatIndex = binData->binHeader.materialMapping_V[i];
			_newNodeData->materialName = binData->binData.material_V[_nowMatIndex].materialName.c_str();
		}
		_pNodeData_V->push_back(_newNodeData);
		_nowModelMesh->m_pNodeData_V = _pNodeData_V;
	}
#pragma endregion

#pragma region SKELETON
	if (binData->binHeader.isSkeleton == true)
	{
		outModel->m_pSkeleton = std::make_unique<SkeletonData>();

		size_t _boneCount = binData->binHeader.boneCount;
		std::vector<DirectX::SimpleMath::Matrix> _boneOffset(_boneCount);
		std::vector<BoneNodeData*> _bones(_boneCount);
		std::vector<int> _boneIndex(_boneCount, -1);

		for (int i = 0; i < _boneCount; i++)
		{
			_bones[i] = new BoneNodeData();

			_bones[i]->boneName = binData->binData.skeleton.bone_V[i].boneName;
			_bones[i]->boneIndex = binData->binData.skeleton.bone_V[i].boneIndex;
			_bones[i]->parentBoneIndex = binData->binData.skeleton.bone_V[i].parentBoneIndex;

			// 부모를 찾아서 넣어준다
			if (_bones.empty() == false)
			{
				if (_bones[i]->parentBoneIndex >= 0)
				{
					_bones[i]->pParentBone = _bones[_bones[i]->parentBoneIndex];
					_bones[_bones[i]->parentBoneIndex]->pChildren_V.push_back(_bones[i]);
				}
			}

			StoreMatrix(_bones[i]->bindPoseTM, binData->binData.skeleton.bone_V[i].bindPoseTransform);
			StoreMatrix(_bones[i]->worldTM, binData->binData.skeleton.bone_V[i].worldTM);
			StoreMatrix(_bones[i]->nodeTM, binData->binData.skeleton.bone_V[i].localTM);
			StoreMatrix(_boneOffset[i], binData->binData.skeleton.boneOffset_V[i]);

			_boneIndex[i] = _bones[i]->boneIndex;
		}

		outModel->m_pSkeleton->pBones_V.swap(_bones);
		outModel->m_pSkeleton->boneOffsetTM_V.swap(_boneOffset);

		// 상하체 분리를 위해 상체의 본만 모은다
		std::vector<BoneNodeData*> _torso;

		outModel->m_pSkeleton->SetTorsoSkeletalIndex(2, _torso);
		outModel->m_pSkeleton->pTorso_V.swap(_torso);

	}
#pragma endregion
}

void DX11MeshFactory::GetAnimationData(
	std::shared_ptr<AnimFileForRead> binData,
	std::shared_ptr<AnimationClipData> outAnimationData)
{

	if (binData->animHeader.isSkinnedAnimation == true)
	{
		std::shared_ptr< AnimationClipData> _nowAnimData = outAnimationData;
		_nowAnimData->animType = eANIM_TYPE::SKINNED;

		_nowAnimData->m_AnimationName = binData->animData.animationName.c_str();
		_nowAnimData->m_TicksPerFrame = binData->animData.ticksPerFrame;
		_nowAnimData->m_TotalKeyFrames = binData->animData.totalKeyFrames;
		_nowAnimData->m_LastKeyFrame = _nowAnimData->m_TotalKeyFrames;

		size_t _boneCount = binData->animHeader.boneCount;
		size_t _nowAnimKeyFrameCount = binData->animHeader.animationKeyFrameCount;

		_nowAnimData->keyFrames_V.resize(_boneCount);

		for (size_t _boneIndex = 0; _boneIndex < _boneCount; _boneIndex++)
		{
			_nowAnimData->keyFrames_V[_boneIndex].resize(_nowAnimKeyFrameCount);
			for (size_t _nowKeyframe = 0; _nowKeyframe < _nowAnimKeyFrameCount; _nowKeyframe++)
			{
				// 타겟은 심플매스 매트릭스, 원본은 float4x4

				StoreMatrix(
					_nowAnimData->keyFrames_V[_boneIndex][_nowKeyframe],
					binData->animData.animationKeyFrame[_boneIndex][_nowKeyframe]);
			}
		}
	}
	else if (binData->animHeader.isSkinnedAnimation == false)
	{

		std::shared_ptr< AnimationClipData> _nowAnimData = outAnimationData;
		_nowAnimData->animType = eANIM_TYPE::MESH;

		_nowAnimData->m_AnimationName = binData->animData.animationName.c_str();
		_nowAnimData->m_TicksPerFrame = binData->animData.ticksPerFrame;
		_nowAnimData->m_TotalKeyFrames = binData->animData.totalKeyFrames;
		_nowAnimData->m_LastKeyFrame = _nowAnimData->m_TotalKeyFrames;

		size_t _meshCount = binData->animHeader.meshCount;
		size_t _nowAnimKeyFrameCount = binData->animHeader.animationKeyFrameCount;

		_nowAnimData->keyFrames_V.resize(_meshCount);

		for (size_t _meshIndex = 0; _meshIndex < _meshCount; _meshIndex++)
		{
			_nowAnimData->keyFrames_V[_meshIndex].resize(_nowAnimKeyFrameCount);
			for (int _nowKeyframe = 0; _nowKeyframe < _nowAnimKeyFrameCount; _nowKeyframe++)
			{
				StoreMatrix(
					_nowAnimData->keyFrames_V[_meshIndex][_nowKeyframe],
					binData->animData.animationKeyFrame[_meshIndex][_nowKeyframe]);
			}
		}
	}
}


void GetNormalVector3(DirectX::XMFLOAT3& outVector, DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b, DirectX::XMFLOAT3& c)
{
	float _x = a.x + b.x + c.x;
	float _y = a.y + b.y + c.y;
	float _z = a.z + b.z + c.z;

	float _distance = sqrt((_x * _x) + (_y * _y) + (_z * _z));

	outVector.x = _x / _distance;
	outVector.y = _y / _distance;
	outVector.z = _z / _distance;
}


void DX11MeshFactory::GetModelMeshBinaryAsNeviMesh(
	ID3D11Device* pDevice,
	std::shared_ptr<FBXModelBinaryFile> inputFile,
	std::shared_ptr<NaviMeshSet>& outNeviMesh)
{

	HRESULT _hr;

	// 메쉬가 여러 서브 메쉬로 분리되어 있을 가능성을 상정해야한다
	// 즉 모든 서브 메쉬들이 하나로 통일되어야하며, 인덱스도 하나로 통일되어야 한다
	std::vector<std::shared_ptr<NaviMeshFace>> _neviMeshFaces;

	DirectX::XMFLOAT3 _tempVector3;

	for (int i = 0; i < inputFile->binData.mesh_V.size(); ++i)
	{

		try
		{
			/// 이니셜라이즈 하기전에 버텍스 종류별로 형변환 필요
			if (inputFile->binHeader.vertexType == VertexType::Type_Vertex)
			{
				// 이 벡터도 비워야하나?
				std::vector<Vertex::VertexPosNormal> _tempVertex;
				_tempVertex.resize(inputFile->binData.mesh_V[i].vertex_V.size());

				for (int j = 0; j < inputFile->binData.mesh_V[i].vertex_V.size(); ++j)
				{
					_tempVertex[j].pos = inputFile->binData.mesh_V[i].vertex_V[j].pos;
					_tempVertex[j].normal = inputFile->binData.mesh_V[i].vertex_V[j].normal;
				}

				std::vector<DWORD> _indices = inputFile->binData.mesh_V[i].index_V;

				// 서브 메쉬들을 모두 합치기 위한 작업
				unsigned int _index;
				if (_neviMeshFaces.empty() == true)
				{
					_index = 0;
				}
				else
				{
					std::shared_ptr<NaviMeshFace> _nowFace = _neviMeshFaces.at(_neviMeshFaces.size() - 1);
					_index = _nowFace->m_Index;
					++_index;
				}

				for (int j = 0; j < inputFile->binData.mesh_V[i].index_V.size() - 1; j += 3)
				{

					unsigned int _v1 = inputFile->binData.mesh_V[i].index_V[j];
					unsigned int _v2 = inputFile->binData.mesh_V[i].index_V[j + 1];
					unsigned int _v3 = inputFile->binData.mesh_V[i].index_V[j + 2];


					GetNormalVector3(_tempVector3, _tempVertex[_v1].normal, _tempVertex[_v2].normal, _tempVertex[_v3].normal);
					//if (_tempVector3.y >= 0.8f)
					if (_tempVector3.y >= 0.5f)
					{
						std::shared_ptr<NaviMeshFace> _newFace = std::make_shared<NaviMeshFace>();

						_newFace->m_Index = _index;


						_newFace->m_Vertex_Arr[0] = _tempVertex[_v1].pos;
						_newFace->m_Vertex_Arr[1] = _tempVertex[_v2].pos;
						_newFace->m_Vertex_Arr[2] = _tempVertex[_v3].pos;

						_newFace->m_FaceNormal.x = _tempVector3.x;
						_newFace->m_FaceNormal.y = _tempVector3.y;
						_newFace->m_FaceNormal.z = _tempVector3.z;

						// 이후 Added by YoKing
						// 페이스의 포지션(중점)
						_newFace->m_CenterPosition.x = (_newFace->m_Vertex_Arr[0].x + _newFace->m_Vertex_Arr[1].x + _newFace->m_Vertex_Arr[2].x) / 3.0f;
						_newFace->m_CenterPosition.y = (_newFace->m_Vertex_Arr[0].y + _newFace->m_Vertex_Arr[1].y + _newFace->m_Vertex_Arr[2].y) / 3.0f;
						_newFace->m_CenterPosition.z = (_newFace->m_Vertex_Arr[0].z + _newFace->m_Vertex_Arr[1].z + _newFace->m_Vertex_Arr[2].z) / 3.0f;

						// Edge Vertex (변의 중점)
						_newFace->m_EdgeMid_Arr[0] =
						{
							(_newFace->m_Vertex_Arr[0].x + _newFace->m_Vertex_Arr[1].x) / 2.0f,
							(_newFace->m_Vertex_Arr[0].y + _newFace->m_Vertex_Arr[1].y) / 2.0f,
							(_newFace->m_Vertex_Arr[0].z + _newFace->m_Vertex_Arr[1].z) / 2.0f
						};

						// Edge의 중점(변의 중심 점)
						_newFace->m_EdgeMid_Arr[1] =
						{
							(_newFace->m_Vertex_Arr[1].x + _newFace->m_Vertex_Arr[2].x) / 2.0f,
							(_newFace->m_Vertex_Arr[1].y + _newFace->m_Vertex_Arr[2].y) / 2.0f,
							(_newFace->m_Vertex_Arr[1].z + _newFace->m_Vertex_Arr[2].z) / 2.0f
						};

						_newFace->m_EdgeMid_Arr[2] =
						{
							(_newFace->m_Vertex_Arr[2].x + _newFace->m_Vertex_Arr[0].x) / 2.0f,
							(_newFace->m_Vertex_Arr[2].y + _newFace->m_Vertex_Arr[0].y) / 2.0f,
							(_newFace->m_Vertex_Arr[2].z + _newFace->m_Vertex_Arr[0].z) / 2.0f
						};

						// 삼각형 Face의 Edge의 유향벡터
						_newFace->m_CycleEdgeVector_Arr[0] = { _newFace->m_Vertex_Arr[1] - _newFace->m_Vertex_Arr[0] };
						_newFace->m_CycleEdgeVector_Arr[1] = { _newFace->m_Vertex_Arr[2] - _newFace->m_Vertex_Arr[1] };
						_newFace->m_CycleEdgeVector_Arr[2] = { _newFace->m_Vertex_Arr[0] - _newFace->m_Vertex_Arr[2] };

						_neviMeshFaces.push_back(_newFace);
						++_index;
					}
					else
					{
						continue;
					}
				}

				/// <summary>
				/// Add by Yoking
				/// Face의 인접 Face 목록 리스트 추가
				/// </summary>
				for (auto& _criteriaFace : _neviMeshFaces)
				{
					for (auto& _compareFace : _neviMeshFaces)
					{
						if (_criteriaFace == _compareFace)
						{
							// 같은 Face를 비교하려 할 때 건너 띔.
							continue;
						}

						// Edge 의 중점 3개 만큼 반복하면서 검사
						// 같은 Edge 중점을 가진 Face가 있으면 인접 Face 목록에 추가
						for (size_t i = 0; i < 3; i++)
						{
							// 기준 Face의 EdgeMid 정점중 하나라도 비교대상 Face의 EdgeMid 정점과 같으면
							// 인접해 있는 것이다!!
							if (_criteriaFace->m_EdgeMid_Arr[i] == _compareFace->m_EdgeMid_Arr[0]
								|| _criteriaFace->m_EdgeMid_Arr[i] == _compareFace->m_EdgeMid_Arr[1]
								|| _criteriaFace->m_EdgeMid_Arr[i] == _compareFace->m_EdgeMid_Arr[2])
							{
								// 기준 Face에 비교 Face를 인접 Face로 추가
								_criteriaFace->m_AdjoinFace_V.push_back(_compareFace);

								// 추가되면 바로 다음 Face 비교하기 위해 탈출
								break;
							}
						}
					}
				}

			}
			if (inputFile->binHeader.vertexType == VertexType::Type_VertexSkinned)
			{
				_hr = S_FALSE;
				COM_ERROR_IF_FAILED(_hr, "NeviMesh, Only Can be made by static Object");
				return;

			}

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}
	}

	outNeviMesh->SetNeviMeshFaces(_neviMeshFaces);
}
