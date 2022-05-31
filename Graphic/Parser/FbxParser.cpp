#define SAFE_DELETE(x) { if(x != nullptr) {delete x; x = nullptr;} }

#define FBXSDK_SHARED
#include <fbxsdk.h>

using namespace fbxsdk;

#include <windows.h>
#include <cassert>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

//#include "DLLDefine.h"
//#include "KHMath.h"
#include "SimpleMath.h"
#include <DirectXMath.h>

//#include "AnimationData.h"
#include "ParsingDataClass.h"
using namespace ParserData;

#include "BoneWeights.h"
#include "FBXParser.h"

FBXParser::FBXParser()
{
	Initalize();
}

FBXParser::~FBXParser()
{
}

void FBXParser::Initalize()
{
	// FBX SDK Manager 생성..
	pManager = FbxManager::Create();

	if (!pManager)
		throw std::exception("error: unable to create FBX manager!\n");

	// Scene 생성..
	pScene = FbxScene::Create(pManager, "My Scene");

	// IOSettings 객체 생성 및 설정
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	FbxString lExtension = "dll";
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer(), lExtension.Buffer());

	// FbxImporter 객체 생성
	pImporter = FbxImporter::Create(pManager, "");

	// Convert 객체 생성
	pConverter = new FbxGeometryConverter(pManager);

	if (!pScene)
		throw std::exception("error: unable to create FBX scene\n");
}


void FBXParser::SceneSetting()
{
	if (!pImporter->Initialize(fbxFileName, -1, pManager->GetIOSettings()))
		throw std::exception("error: initialize importer\n");

	// fbx 파일 내용을 Scene으로 가져온다..
	pImporter->Import(pScene);

	// Scene 내에 있는 데이터들의 단위를 변경해준다..
	if (m_Scaling)
	{
		FbxSystemUnit lFbxFileSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
		FbxSystemUnit lFbxOriginSystemUnit = pScene->GetGlobalSettings().GetOriginalSystemUnit();
		double factor = lFbxOriginSystemUnit.GetScaleFactor();
		const FbxSystemUnit::ConversionOptions lConversionOptions =
		{
		  true,
		  true,
		  true,
		  true,
		  true,
		  true
		};
		lFbxFileSystemUnit.m.ConvertScene(pScene, lConversionOptions);
		lFbxOriginSystemUnit.m.ConvertScene(pScene, lConversionOptions);
	}

	// Scene 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다..
	// 3D Max 안에서 Editable poly 상태라면 이 작업을 안해야 한다..
	pConverter->Triangulate(pScene, true);
}


void FBXParser::Release()
{
	pImporter->Destroy();
	SAFE_DELETE(pConverter);
	pScene->Destroy();
	pManager->Destroy();
}

void FBXParser::LoadScene(fbxsdk::FbxString fileName, bool scaling, bool onlyAni)
{
	m_Model = new FBXModel();
	pMesh = nullptr;
	fbxMaterials.clear();
	fbxFileName = fileName;
	m_Scaling = scaling;
	m_OnlyAni = onlyAni;

	// Scene 설정..
	SceneSetting();

	// Scene에서 RootNode 가져오기..
	fbxsdk::FbxNode* pRootNode = pScene->GetRootNode();

	// 혹시라도 RootNode가 없을 경우를 대비..
	if (pRootNode == nullptr) return;

	/// Material Data Loading..
	LoadMaterial();

	/// Bone Data Loading..
	LoadNode(pRootNode, FbxNodeAttribute::eSkeleton);

	/// Mesh Data Loading..
	LoadNode(pRootNode, FbxNodeAttribute::eMesh);

	/// Mesh Data Optimizing..
	OptimizeData();
}

void FBXParser::LoadMaterial()
{
	// 애니메이션만 뽑을경우..
	if (m_OnlyAni) return;

	// Scene에 존재하는 Material 개수만큼 생성..
	int mcount = pScene->GetMaterialCount();
	for (int matIndex = 0; matIndex < pScene->GetMaterialCount(); matIndex++)
	{
		FbxSurfaceMaterial* material = pScene->GetMaterial(matIndex);
		m_materialdata = new CMaterial();
		m_materialdata->m_materialnumber = matIndex;

		// Material Data 삽입..
		SetMaterial(material);

		// Texture Data 삽입..
		SetTexture(material, FbxSurfaceMaterial::sDiffuse);
		SetTexture(material, FbxSurfaceMaterial::sNormalMap);
		//SetTexture(material, FbxSurfaceMaterial::sAmbient);
		//SetTexture(material, FbxSurfaceMaterial::sEmissive);
		//SetTexture(material, FbxSurfaceMaterial::sSpecular);

		m_Model->m_list_materialdata.push_back(m_materialdata);
		fbxMaterials.push_back(material);

		m_materialdata = nullptr;
		m_Model->m_materialcount++;
	}
}

void FBXParser::LoadNode(fbxsdk::FbxNode* node, fbxsdk::FbxNodeAttribute::EType attribute)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != nullptr)
	{
		FbxString nodename = node->GetName();
		FbxNodeAttribute::EType attributeType = nodeAttribute->GetAttributeType();
		if (attributeType == attribute)
		{
			switch (nodeAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::eSkeleton:
				//ProcessSkeleton(node);
				//LoadAnimation(node);
				break;
			case FbxNodeAttribute::eMesh:
				ProcessMesh(node);
				//LoadAnimation(node);
				break;
			case FbxNodeAttribute::eMarker:
			case FbxNodeAttribute::eNurbs:
			case FbxNodeAttribute::ePatch:
			case FbxNodeAttribute::eCamera:
			case FbxNodeAttribute::eLight:
			case FbxNodeAttribute::eNull:
				break;
			default:
				break;
			}
		}
	}

	// 재귀 구조..
	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		LoadNode(node->GetChild(i), attribute);
	}
}

//void FBXParser::LoadAnimation(fbxsdk::FbxNode* node)
//{
//	float frameRate = (float)FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode());
//
//	FbxTakeInfo* takeInfo = pImporter->GetTakeInfo(0);
//
//	if (takeInfo == nullptr) return;
//
//	FbxTime start = FbxTime(FbxLongLong(0x7fffffffffffffff));
//	FbxTime stop = FbxTime(FbxLongLong(-0x7fffffffffffffff));
//
//	FbxTimeSpan span = takeInfo->mLocalTimeSpan;
//
//	double tempStart = span.GetStart().GetSecondDouble();
//	double tempStop = span.GetStop().GetSecondDouble();
//
//	if (tempStart < tempStop)
//	{
//		// 구동시간 동안 총 몇 프레임이 수행될지를 keyFrames에 담아줌
//		int keyFrames = (int)((tempStop - tempStart) * (double)frameRate);
//		double ticksperFrame = (tempStop - tempStart) / keyFrames;
//
//		m_Model->m_OneAnimation = new AnimationData();
//		m_OneAnimation = m_Model->m_OneAnimation;
//
//		// 한 프레임 재생 시간..
//		m_OneAnimation->m_ticksperFrame = (float)ticksperFrame;
//
//		// 애니메이션 시작 프레임..
//		m_OneAnimation->m_startFrame = (int)(tempStart)*keyFrames;
//		m_OneAnimation->m_endFrame = (int)(tempStop)*keyFrames;
//
//		// 애니메이션 총 프레임..
//		m_OneAnimation->m_totalFrame = keyFrames;
//
//		ProcessAnimation(node);
//	}
//}

//void FBXParser::ProcessSkeleton(fbxsdk::FbxNode* node)
//{
//	pMesh = node->GetMesh();
//
//	// 새로운 Mesh 생성..
//	CreateMesh();
//
//	m_OneMesh->m_nodename = node->GetName();
//	m_OneMesh->m_IsBone = true;
//
//	// 애니메이션만 뽑을 경우..
//	if (m_OnlyAni) return;
//
//	// 현 Node Parent 찾기..
//	std::string parentName = node->GetParent()->GetName();
//	Mesh* parentMesh = FindMesh(parentName);
//	m_OneMesh->m_nodeparent = parentName;
//	m_OneMesh->m_parent = parentMesh;
//
//	if (parentMesh != nullptr)
//		parentMesh->m_childlist.push_back(m_OneMesh);
//
//	// Node TRS 설정..
//	SetTransform(node);
//
//	// 새로운 Bone 생성..
//	CreateBone();
//
//	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
//
//	if (fbxSkeleton == nullptr) return;
//
//	// 부모 Bone Index 초기값..
//	// 부모 Bone Index가 -1 이면 최상위 Root Node..
//	int parentBoneIndex = -1;
//
//	// 최상위 노드(부모노드)인지 확인..
//	FbxNode* parentNode = node->GetParent();
//
//	if (m_OneMesh->m_parent != nullptr)
//	{
//		string nodeName = parentNode->GetName();
//		parentBoneIndex = FindBoneIndex(nodeName);
//	}
//
//	string boneName = node->GetName();
//	m_OneBone->m_bone_name = boneName;
//	m_OneBone->m_parent_bone_number = parentBoneIndex;
//	m_OneBone->m_bone_number = m_Model->m_AllBoneList.size();
//	m_OneBone->m_isCol = true;
//	m_OneBone->m_Min = DXVector3(+KH_MATH::Infinity, +KH_MATH::Infinity, +KH_MATH::Infinity);
//	m_OneBone->m_Max = DXVector3(-KH_MATH::Infinity, -KH_MATH::Infinity, -KH_MATH::Infinity);
//	m_OneMesh->m_bone = m_OneBone;
//	m_Model->m_AllBoneList.push_back(BonePair(boneName, m_OneBone));
//}

void FBXParser::ProcessMesh(fbxsdk::FbxNode* node)
{
	pMesh = node->GetMesh();

	// 새로운 Mesh 생성..
	CreateMesh();

	m_OneMesh->m_NodeName = node->GetName();

	// 애니메이션만 뽑을 경우..
	if (m_OnlyAni) return;

	// 현 Node Parent 찾기..
	string parentName = node->GetParent()->GetName();
	Mesh* parentMesh = FindMesh(parentName);
	m_OneMesh->m_ParentNodeName = parentName;
	m_OneMesh->m_pParentMesh = parentMesh;

	// 부모의 Mesh가 존재한다면 ChildList에 추가..
	if (parentMesh != nullptr)
		parentMesh->m_childlist.push_back(m_OneMesh);

	// Node TRS 설정..
	SetTransform(node);

	int vertexTotalCount = pMesh->GetControlPointsCount();
	int polygonTotalCount = pMesh->GetPolygonCount();

	// 만약 Vertex 정보가 없을경우 처리하지 않기 위함..
	if (vertexTotalCount < 1) return;

	// Vertex 개수만큼 BoneWeight 생성..
	// 해당 Vertex Index와 동일한 Index에 가중치 값, Bone Index Data 설정..
	vector<BoneWeights> boneWeights(vertexTotalCount);
	//bool isSkin = ProcessBoneWeights(node, boneWeights);

	// Bone Data 설정 결과에 따른 Skinning Object 판별..
	//m_OneMesh->m_IsSkinningObject = isSkin;

	// 새로운 버텍스 생성..
	CreateVertex(pMesh, boneWeights, vertexTotalCount);

	int vertexCount = 0; // 정점의 개수
	for (int pi = 0; pi < polygonTotalCount; pi++)
	{
		// Polygon 개수만큼 Face 생성..
		m_OneMesh->m_MeshFaces.push_back(new Face);

		for (int vi = 0; vi < 3; vi++)
		{
			int vertexIndex = pMesh->GetPolygonVertex(pi, vi);
			int uvIndex = pMesh->GetTextureUVIndex(pi, vi);

			if (vertexIndex < 0 || vertexIndex >= vertexTotalCount)	continue;

			Vector3 fbxNormal;
			Vector2 fbxUV;

			fbxNormal = GetNormal(pMesh, vertexIndex, vertexCount);
			fbxUV = GetUV(pMesh, vertexIndex, uvIndex);

			// 일단 하나의 Mesh 당 하나의 Material로 생각하고 가자..
			if (m_OneMesh->m_materialdata == nullptr)
				LinkMaterialByPolygon(pMesh, pi, vertexIndex);

			// Face Vertex Index Data
			m_OneMesh->m_MeshFaces[pi]->m_VertexIndex[vi] = vertexIndex;
			// Face Vertex Normal Data
			m_OneMesh->m_MeshFaces[pi]->m_VertexNormal[vi] = fbxNormal;
			// Face Vertex UV Data
			m_OneMesh->m_MeshFaces[pi]->m_uvvertex[vi] = fbxUV;

			vertexCount++;
		}
	}
}


//bool FBXParser::ProcessBoneWeights(fbxsdk::FbxNode* node, vector<BoneWeights>& meshBoneWeights)
//{
//	FbxMesh* mesh = node->GetMesh();
//
//	int deformerCount = mesh->GetDeformerCount();
//
//	// DeformerCount가 1보다 작으면 Bone Data가 없다고 가정..
//	if (deformerCount < 1) return false;
//
//	for (int i = 0; i < deformerCount; ++i)
//	{
//		FbxDeformer* deformer = mesh->GetDeformer(i);
//
//		if (deformer == nullptr) continue;
//
//		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
//		{
//			FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));
//
//			if (skin == nullptr) continue;
//
//			FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;
//
//			int clusterCount = skin->GetClusterCount();
//
//			// Skin Mesh 체크..
//			Mesh* skinMesh = FindMesh(node->GetName());
//			vector<BoneWeights> skinBoneWeights(meshBoneWeights.size());
//			for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
//			{
//				FbxCluster* cluster = skin->GetCluster(clusterIndex);
//				if (cluster == nullptr) continue;
//
//				linkMode = cluster->GetLinkMode();
//				FbxNode* pLinkNode = cluster->GetLink();
//
//				if (pLinkNode == nullptr) continue;
//
//				string lineNodeName = pLinkNode->GetName();
//
//				int BoneIndex = FindBoneIndex(lineNodeName);
//
//				if (BoneIndex < 0)
//				{
//					continue;
//				}
//
//				// Bone Index에 해당하는 Bone 추출..
//				Bone* nowBone = m_Model->m_AllBoneList[BoneIndex].second;
//
//				// Bone Mesh 체크..
//				Mesh* boneMesh = FindMesh(lineNodeName);
//
//				FbxAMatrix matClusterTransformMatrix;
//				FbxAMatrix matClusterLinkTransformMatrix;
//
//				const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eDestinationPivot);
//				const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eDestinationPivot);
//				const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eDestinationPivot);
//
//				FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);
//
//				cluster->GetTransformMatrix(matClusterTransformMatrix);
//				cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);
//
//				// Bone Matrix 설정..
//				DXMatrix4X4 clusterMatrix = ConvertMatrix(matClusterTransformMatrix);
//				DXMatrix4X4 clusterlinkMatrix = ConvertMatrix(matClusterLinkTransformMatrix);
//				DXMatrix4X4 geometryMatrix = ConvertMatrix(geometryTransform);
//
//				DXMatrix4X4 offsetMatrix = clusterMatrix * clusterlinkMatrix.Inverse() * geometryMatrix;
//
//				skinMesh->m_boneTMList.emplace_back(offsetMatrix);
//				skinMesh->m_boneList.emplace_back(boneMesh);
//
//				int c = cluster->GetControlPointIndicesCount();
//				for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
//				{
//					int index = cluster->GetControlPointIndices()[j];
//					double weight = cluster->GetControlPointWeights()[j];
//
//					if (weight == 0) continue;
//
//					skinBoneWeights[index].AddBoneWeight(clusterIndex, (float)weight);
//				}
//			}
//
//			switch (linkMode)
//			{
//			case FbxCluster::eNormalize:
//			{
//				// 가중치 합을 1.0으로 만드는 작업..
//				for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
//					skinBoneWeights[i].Normalize();
//			}
//			break;
//
//			case FbxCluster::eAdditive:
//				break;
//
//			case FbxCluster::eTotalOne:
//				break;
//			}
//
//			for (size_t i = 0; i < meshBoneWeights.size(); i++)
//				meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
//		}
//	}
//
//	return true;
//}

//void FBXParser::ProcessAnimation(fbxsdk::FbxNode* node)
//{
//	// 애니메이션만 뽑을경우..
//	if (m_OnlyAni)
//	{
//		FbxMesh* mesh = node->GetMesh();
//
//		if (mesh != nullptr)
//		{
//			int deformerCount = mesh->GetDeformerCount();
//
//			// DeformerCount가 0보다 크면 Skinning Mesh..
//			if (deformerCount > 0)
//				return;
//		}
//	}
//
//	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
//
//	// 만약 스키닝 오브젝트라면 애니메이션 데이터는 Bone에 저장되어 있으므로..
//	if (m_OneMesh->m_IsSkinningObject) return;
//
//	if (nodeAttribute != nullptr)
//	{
//		string nodeName = node->GetName();
//		Mesh* mesh = FindMesh(nodeName);
//
//		if (mesh != nullptr)
//		{
//			FbxTime::EMode timeMode = pScene->GetGlobalSettings().GetTimeMode();
//			for (FbxLongLong index = 0; index < m_OneAnimation->m_totalFrame; index++)
//			{
//				FbxTime takeTime;
//				takeTime.SetFrame(m_OneAnimation->m_startFrame + index, timeMode);
//
//				// Local Transform = 부모 Bone의 Global Transform의 Inverse Transform * 자신 Bone의 Global Transform
//				FbxAMatrix nodeTransform = node->EvaluateLocalTransform(takeTime);
//
//				DXMatrix4X4 nodeTRS = ConvertMatrix(nodeTransform);
//
//				XMVECTOR scale;
//				XMVECTOR rot;
//				XMVECTOR pos;
//
//				XMMatrixDecompose(&scale, &rot, &pos, nodeTRS);
//
//				CAnimation_pos* newPos = new CAnimation_pos;
//				CAnimation_rot* newRot = new CAnimation_rot;
//				CAnimation_scl* newScale = new CAnimation_scl;
//
//				newPos->m_time = (float)index;
//				newPos->m_pos = DXVector3(pos);
//
//				newRot->m_time = (float)index;
//				newRot->m_rotQT_accumulation = Quaternion(rot);
//
//				newScale->m_time = (float)index;
//				newScale->m_scale = DXVector3(scale);
//
//				m_OneAnimation->m_position.push_back(newPos);
//				m_OneAnimation->m_rotation.push_back(newRot);
//				m_OneAnimation->m_scale.push_back(newScale);
//			}
//
//			m_OneAnimation->m_lastPosFrame = m_OneAnimation->m_totalFrame - 1;
//			m_OneAnimation->m_lastRotFrame = m_OneAnimation->m_totalFrame - 1;
//			m_OneAnimation->m_lastScaleFrame = m_OneAnimation->m_totalFrame - 1;
//			m_OneAnimation->m_isPosAnimation = true;
//			m_OneAnimation->m_isRotAnimation = true;
//			m_OneAnimation->m_isScaleAnimation = true;
//			m_Model->m_isAnimation = true;
//
//			// 해당 Mesh에 애니메이션 삽입..
//			mesh->m_animation = m_OneAnimation;
//			mesh->m_isAnimation = true;
//		}
//	}
//}

void FBXParser::OptimizeData()
{
	// 애니메이션만 뽑을경우..
	if (m_OnlyAni) return;

	// Optimize Data
	for (unsigned int i = 0; i < m_Model->m_MeshList.size(); i++)
	{
		OptimizeVertex(m_Model->m_MeshList[i]);
	}
}

void FBXParser::OptimizeVertex(ParserData::Mesh* pMesh)
{
	if (pMesh->m_MeshVertices.empty()) return;

	bool new_VertexSet = false;
	unsigned int resize_VertexIndex = pMesh->m_MeshVertices.size();

	// 기본 Vertex 넣어두고 시작..
	for (unsigned int i = 0; i < pMesh->m_MeshVertices.size(); i++)
	{
		Vertex* newVertex = new Vertex;
		newVertex->m_Pos = pMesh->m_MeshVertices[i]->m_Pos;
		newVertex->m_Indices = pMesh->m_MeshVertices[i]->m_Indices;
		newVertex->m_boneIndices = pMesh->m_MeshVertices[i]->m_boneIndices;
		newVertex->m_boneWeights = pMesh->m_MeshVertices[i]->m_boneWeights;

		pMesh->m_opt_vertex.push_back(newVertex);
	}

	// 각각 Face마다 존재하는 3개의 Vertex 비교..
	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			unsigned int vertexIndex = pMesh->m_MeshFaces[i]->m_VertexIndex[j];

			Vertex* nowVertex = pMesh->m_opt_vertex[vertexIndex];

			// 텍스처가 있고, 설정하지 않았으면 텍스처 u,v 설정..
			if (nowVertex->m_istextureset == false)
			{
				nowVertex->u = pMesh->m_MeshFaces[i]->m_uvvertex[j].x;
				nowVertex->v = pMesh->m_MeshFaces[i]->m_uvvertex[j].y;
				nowVertex->m_istextureset = true;
			}

			// 최초 인덱스 노말값 검사시엔 넣어주고 시작..
			if (nowVertex->m_isnormalset == false)
			{
				nowVertex->m_Normal = pMesh->m_MeshFaces[i]->m_VertexNormal[j];
				nowVertex->m_isnormalset = true;
			}

			// Normal, U, V 값중 한개라도 다르면 Vertex 새로 생성..
			if ((pMesh->m_opt_vertex[vertexIndex]->m_Normal != pMesh->m_MeshFaces[i]->m_VertexNormal[j]))
			{
				new_VertexSet = true;
			}

			if ((pMesh->m_opt_vertex[vertexIndex]->u != pMesh->m_MeshFaces[i]->m_uvvertex[j].x) ||
				(pMesh->m_opt_vertex[vertexIndex]->v != pMesh->m_MeshFaces[i]->m_uvvertex[j].y))
			{
				new_VertexSet = true;
			}

			if (new_VertexSet)
			{
				// 추가된 Vertex가 있다면 체크..
				if (resize_VertexIndex > (int)pMesh->m_MeshVertices.size())
				{
					for (unsigned int k = pMesh->m_MeshVertices.size(); k < resize_VertexIndex; k++)
					{
						// 새로 추가한 Vertex와 동일한 데이터를 갖고있는 Face 내의 Vertex Index 수정..
						if ((pMesh->m_opt_vertex[k]->m_Indices == pMesh->m_MeshFaces[i]->m_VertexIndex[j]) &&
							(pMesh->m_opt_vertex[k]->m_Normal == pMesh->m_MeshFaces[i]->m_VertexNormal[j]))
						{
							if ((pMesh->m_opt_vertex[k]->u == pMesh->m_MeshFaces[i]->m_uvvertex[j].x) &&
								(pMesh->m_opt_vertex[k]->v == pMesh->m_MeshFaces[i]->m_uvvertex[j].y))
							{
								pMesh->m_MeshFaces[i]->m_VertexIndex[j] = (int)k;
								new_VertexSet = false;
								break;
							}
						}
					}
				}
			}

			// 새로 추가해야할 Vertex..
			if (new_VertexSet == true)
			{
				Vertex* newVertex = new Vertex;
				newVertex->m_Pos = nowVertex->m_Pos;
				newVertex->m_Indices = nowVertex->m_Indices;
				newVertex->m_Normal = pMesh->m_MeshFaces[i]->m_VertexNormal[j];
				newVertex->m_boneIndices = nowVertex->m_boneIndices;
				newVertex->m_boneWeights = nowVertex->m_boneWeights;
				newVertex->m_isnormalset = true;

				newVertex->u = pMesh->m_MeshFaces[i]->m_uvvertex[j].x;
				newVertex->v = pMesh->m_MeshFaces[i]->m_uvvertex[j].y;
				newVertex->m_istextureset = true;

				pMesh->m_opt_vertex.push_back(newVertex);
				pMesh->m_MeshFaces[i]->m_VertexIndex[j] = resize_VertexIndex;
				resize_VertexIndex++;
				new_VertexSet = false;
			}
		}
	}

	// Tanget 값 설정..
	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		int index0 = pMesh->m_MeshFaces[i]->m_VertexIndex[0];
		int index1 = pMesh->m_MeshFaces[i]->m_VertexIndex[1];
		int index2 = pMesh->m_MeshFaces[i]->m_VertexIndex[2];

		Vector3 ep1 = pMesh->m_opt_vertex[index1]->m_Pos - pMesh->m_opt_vertex[index0]->m_Pos;
		Vector3 ep2 = pMesh->m_opt_vertex[index2]->m_Pos - pMesh->m_opt_vertex[index0]->m_Pos;

		Vector3 uv1 = { pMesh->m_opt_vertex[index1]->u - pMesh->m_opt_vertex[index0]->u,
						pMesh->m_opt_vertex[index1]->v - pMesh->m_opt_vertex[index0]->v, 1 };
		Vector3 uv2 = { pMesh->m_opt_vertex[index2]->u - pMesh->m_opt_vertex[index0]->u,
						pMesh->m_opt_vertex[index2]->v - pMesh->m_opt_vertex[index0]->v, 1 };

		float den = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);

		// 현재 픽셀 쉐이더에서 연산을 통해 T, B, N을 얻는데
		// 픽셀 쉐이더 내의 연산은 버텍스 쉐이더의 연산에 비해 호출 횟수가 차원이 다르게 크므로 부하가 올 수 있다..
		// 법선맵의 픽셀의 색은 픽셀 쉐이더 안이 아니면 얻을수 없기 때문에 픽셀 쉐이더에서 연산을 한다고 한다..
		/// 현재 연산과정을 버텍스 쉐이더로 옮겨둠
		Vector3 tangent = (ep1 * uv2.y - ep2 * uv1.y) * den;
		tangent.Normalize();

		// 유사 정점은 값을 누적하여 쉐이더에서 평균값을 사용하도록 하자..
		pMesh->m_opt_vertex[index0]->m_Tanget += tangent;
		pMesh->m_opt_vertex[index1]->m_Tanget += tangent;
		pMesh->m_opt_vertex[index2]->m_Tanget += tangent;
	}

	// 인덱스는 그냥 복사
	pMesh->m_opt_index = new IndexList[pMesh->m_MeshFaces.size()];

	for (unsigned int i = 0; i < pMesh->m_MeshFaces.size(); i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			switch (j)
			{
			case 0:
				pMesh->m_opt_index[i].Index[j] = pMesh->m_MeshFaces[i]->m_VertexIndex[0];
				break;
			case 1:
				pMesh->m_opt_index[i].Index[j] = pMesh->m_MeshFaces[i]->m_VertexIndex[2];
				break;
			case 2:
				pMesh->m_opt_index[i].Index[j] = pMesh->m_MeshFaces[i]->m_VertexIndex[1];
				break;
			default:
				break;
			}
		}
	}
}

void FBXParser::RecombinationTM(ParserData::Mesh* pMesh)
{
	// WorldTM -> LocalTM 변환..
	Vector3 row0 = pMesh->m_TMRow0;
	Vector3 row1 = pMesh->m_TMRow1;
	Vector3 row2 = pMesh->m_TMRow2;
	Vector3 row3 = pMesh->m_TMRow3;
	//row3.w = 1;

	//
	/// Negative Scale Check
	// 3D Max의 작업상 Mirroring으로 인해 Scale값이 음수가 나올 경우를 Negative Scale 이라고 한다
	// 처리를 안해줄 경우 Normal 처리나 Animation 처리시에 문제가 된다
	// Rotaion을 나타내는 축 Row1 ~ Row3 중 두축을 외적 한 값과 다른 한 축을 내적하여 음수이면 축이 반대이고 양수이면 축이 일치한다
	//
	XMVECTOR crossVec = XMVector3Cross(pMesh->m_TMRow0, pMesh->m_TMRow1);
	XMVECTOR dotVec = XMVector3Dot(crossVec, pMesh->m_TMRow2);

	XMFLOAT3 resultVec;

	XMStoreFloat3(&resultVec, dotVec);

	// 축 방향이 다른 경우만 처리해주자..
	if (resultVec.x < 0 || resultVec.y < 0 || resultVec.z < 0)
	{
		XMVECTOR nodeScale;
		XMVECTOR nodetRot;
		XMVECTOR nodePos;

		// 기존 매트릭스에서 분리..
		XMMatrixDecompose(&nodeScale, &nodetRot, &nodePos, pMesh->m_worldTM);

		// 스케일 재조정..
		nodeScale *= -1;

		// 노말값 재조정..
		for (auto& k : pMesh->m_opt_vertex)
		{
			k->m_Normal *= -1;
		}

		/// 텍스쳐나 맵핑 데이터도 변환을 해주어야 할까..?

		// 매트릭스 재조합..
		pMesh->m_worldTM = XMMatrixScalingFromVector(nodeScale) * XMMatrixRotationQuaternion(nodetRot) * XMMatrixTranslationFromVector(nodePos);
	}
}

Vector3 FBXParser::GetPos(fbxsdk::FbxMesh* mesh, int vertexIndex)
{
	FbxVector4 fbxPos;

	fbxPos = mesh->GetControlPointAt(vertexIndex);

	return ConvertVector3(fbxPos);
}

Vector3 FBXParser::GetNormal(fbxsdk::FbxMesh* mesh, int vertexIndex, int vertexCount)
{
	if (mesh->GetElementNormalCount() < 1) return Vector3();

	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal();

	FbxVector4 fbxNormal;

	switch (vertexNormal->GetMappingMode()) // 매핑 모드 
	{
	case FbxGeometryElement::eByControlPoint: // control point mapping 
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			fbxNormal = vertexNormal->GetDirectArray().GetAt(vertexIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexIndex); // 인덱스를 얻어온다. 
			fbxNormal = vertexNormal->GetDirectArray().GetAt(index);
		}
		break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			fbxNormal = vertexNormal->GetDirectArray().GetAt(vertexCount);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(vertexCount); // 인덱스를 얻어온다. 
			fbxNormal = vertexNormal->GetDirectArray().GetAt(index);
		}
		break;
		}
	}
	break;
	}

	return ConvertVector3(fbxNormal);
}


Vector2 FBXParser::GetUV(fbxsdk::FbxMesh* mesh, int vertexIndex, int uvIndex)
{
	if (mesh->GetElementUVCount() < 1) return Vector2();

	FbxGeometryElementUV* vertexUV = mesh->GetLayer(0)->GetUVs();

	FbxVector4 fbxUV;

	switch (vertexUV->GetMappingMode()) // 매핑 모드 
	{
	case FbxGeometryElement::eByControlPoint: // control point mapping 
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			fbxUV = vertexUV->GetDirectArray().GetAt(vertexIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(vertexIndex); // 인덱스를 얻어온다. 
			fbxUV = vertexUV->GetDirectArray().GetAt(index);
		}
		break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			fbxUV = vertexUV->GetDirectArray().GetAt(uvIndex);
		}
		break;
		}
	}
	break;
	}

	// uv Vertex Count 증가..
	m_OneMesh->m_Mesh_NumTVertex++;

	return ConvertVector2(fbxUV);
}

void FBXParser::LinkMaterialByPolygon(fbxsdk::FbxMesh* mesh, int polygonIndex, int vertexIndex)
{
	FbxNode* node = mesh->GetNode();
	if (node == nullptr) return;

	FbxLayerElementMaterial* fbxMaterial = mesh->GetLayer(0)->GetMaterials();
	if (fbxMaterial == nullptr) return;

	int mappingIndex = 0;
	switch (fbxMaterial->GetMappingMode())
	{
	case FbxLayerElement::eAllSame:
		mappingIndex = 0;
		break;
	case FbxLayerElement::eByControlPoint:
		mappingIndex = vertexIndex;
		break;
	case FbxLayerElement::eByPolygonVertex:
		mappingIndex = polygonIndex * 3;
		break;
	case FbxLayerElement::eByPolygon:
		mappingIndex = polygonIndex;
		break;
	}

	if (mappingIndex < 0) return;

	FbxLayerElement::EReferenceMode refMode = fbxMaterial->GetReferenceMode();
	if (refMode == FbxLayerElement::eDirect)
	{
		if (mappingIndex < node->GetMaterialCount())
		{
			// materialIndex : 우리가 부여한 인덱스
			int materialIndex = GetMaterialIndex(node->GetMaterial(mappingIndex));
			m_OneMesh->m_materialdata = m_Model->m_list_materialdata[materialIndex];
		}
	}
	else if (refMode == FbxLayerElement::eIndexToDirect)
	{
		FbxLayerElementArrayTemplate<int>& indexArr = fbxMaterial->GetIndexArray();

		if (mappingIndex < indexArr.GetCount())
		{
			int tempIndex = indexArr.GetAt(mappingIndex);

			if (tempIndex < node->GetMaterialCount())
			{
				// materialIndex : 우리가 부여한 인덱스
				int materialIndex = GetMaterialIndex(node->GetMaterial(tempIndex));
				m_OneMesh->m_materialdata = m_Model->m_list_materialdata[materialIndex];
			}
		}
	}
}

void FBXParser::SetTransform(fbxsdk::FbxNode* node)
{
	if (node == nullptr) return;

	FbxMatrix worldpos = pScene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);
	FbxMatrix localpos = pScene->GetAnimationEvaluator()->GetNodeLocalTransform(node);

	XMMATRIX world = ConvertMatrix(worldpos);
	XMMATRIX local = ConvertMatrix(localpos);

	if (m_OneMesh->m_pParentMesh == nullptr)
	{
		const auto yaw = -90.0f * (DirectX::XM_2PI/2) / 180.0f;

		Quaternion q = XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

		world *= XMMatrixRotationQuaternion(q);
		local *= XMMatrixRotationQuaternion(q);
	}


	XMFLOAT4X4 world_F = XMFLOAT4X4();
	XMStoreFloat4x4(&world_F,world);

	Vector4 r1 = { world_F._11, world_F._12, world_F._13, world_F._14 };
	Vector4 r2 = { world_F._21, world_F._22, world_F._23, world_F._24 };
	Vector4 r3 = { world_F._31, world_F._32, world_F._33, world_F._34 };
	Vector4 r4 = { world_F._41, world_F._42, world_F._43, world_F._44 };

	m_OneMesh->m_TMRow0 = Vector3(r1);
	m_OneMesh->m_TMRow1 = Vector3(r2);
	m_OneMesh->m_TMRow2 = Vector3(r3);
	m_OneMesh->m_TMRow3 = Vector3(r4);

	m_OneMesh->m_worldTM = world;
	m_OneMesh->m_LocalTM = local;
}

XMMATRIX FBXParser::GetGlobalAnimationTransform(fbxsdk::FbxNode* node, fbxsdk::FbxTime time)
{
	if (node == nullptr) return XMMATRIX();

	FbxAMatrix matrix = node->EvaluateGlobalTransform(time);
	return ConvertMatrix(matrix);
}

int FBXParser::GetMaterialIndex(fbxsdk::FbxSurfaceMaterial* material)
{
	for (unsigned int index = 0; index < fbxMaterials.size(); index++)
	{
		if (fbxMaterials[index] == material)
			return index;
	}

	return -1;
}

void FBXParser::SetMaterial(fbxsdk::FbxSurfaceMaterial* material)
{
	m_materialdata->m_material_name = material->GetName();

	if (material->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		// Ambient Data
		m_materialdata->m_material_ambient.x = static_cast<float>(((FbxSurfacePhong*)material)->Ambient.Get()[0]) * 10.0f;
		m_materialdata->m_material_ambient.y = static_cast<float>(((FbxSurfacePhong*)material)->Ambient.Get()[1]) * 10.0f;
		m_materialdata->m_material_ambient.z = static_cast<float>(((FbxSurfacePhong*)material)->Ambient.Get()[2]) * 10.0f;

		// Diffuse Data
		m_materialdata->m_material_diffuse.x = static_cast<float>(((FbxSurfacePhong*)material)->Diffuse.Get()[0]);
		m_materialdata->m_material_diffuse.y = static_cast<float>(((FbxSurfacePhong*)material)->Diffuse.Get()[1]);
		m_materialdata->m_material_diffuse.z = static_cast<float>(((FbxSurfacePhong*)material)->Diffuse.Get()[2]);

		// Specular Data
		m_materialdata->m_material_specular.x = static_cast<float>(((FbxSurfacePhong*)material)->Specular.Get()[0]);
		m_materialdata->m_material_specular.y = static_cast<float>(((FbxSurfacePhong*)material)->Specular.Get()[1]);
		m_materialdata->m_material_specular.z = static_cast<float>(((FbxSurfacePhong*)material)->Specular.Get()[2]);

		// Emissive Data
		m_materialdata->m_material_emissive.x = static_cast<float>(((FbxSurfacePhong*)material)->Emissive.Get()[0]);
		m_materialdata->m_material_emissive.y = static_cast<float>(((FbxSurfacePhong*)material)->Emissive.Get()[1]);
		m_materialdata->m_material_emissive.z = static_cast<float>(((FbxSurfacePhong*)material)->Emissive.Get()[2]);

		// Transparecy Data
		m_materialdata->m_material_transparency = static_cast<float>(((FbxSurfacePhong*)material)->TransparencyFactor.Get());

		// Shininess Data
		m_materialdata->m_material_shininess = static_cast<float>(((FbxSurfacePhong*)material)->Shininess.Get());

		// Reflectivity Data
		m_materialdata->m_material_reflectivity = static_cast<float>(((FbxSurfacePhong*)material)->ReflectionFactor.Get());
	}
	else if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		// Ambient Data
		m_materialdata->m_material_ambient.x = static_cast<float>(((FbxSurfaceLambert*)material)->Ambient.Get()[0]);
		m_materialdata->m_material_ambient.y = static_cast<float>(((FbxSurfaceLambert*)material)->Ambient.Get()[1]);
		m_materialdata->m_material_ambient.z = static_cast<float>(((FbxSurfaceLambert*)material)->Ambient.Get()[2]);

		// Diffuse Data
		m_materialdata->m_material_diffuse.x = static_cast<float>(((FbxSurfaceLambert*)material)->Diffuse.Get()[0]);
		m_materialdata->m_material_diffuse.y = static_cast<float>(((FbxSurfaceLambert*)material)->Diffuse.Get()[1]);
		m_materialdata->m_material_diffuse.z = static_cast<float>(((FbxSurfaceLambert*)material)->Diffuse.Get()[2]);

		// Emissive Data
		m_materialdata->m_material_emissive.x = static_cast<float>(((FbxSurfaceLambert*)material)->Emissive.Get()[0]);
		m_materialdata->m_material_emissive.y = static_cast<float>(((FbxSurfaceLambert*)material)->Emissive.Get()[1]);
		m_materialdata->m_material_emissive.z = static_cast<float>(((FbxSurfaceLambert*)material)->Emissive.Get()[2]);

		// Transparecy Data
		m_materialdata->m_material_transparency = static_cast<float>(((FbxSurfaceLambert*)material)->TransparencyFactor.Get());
	}
}

void FBXParser::SetTexture(fbxsdk::FbxSurfaceMaterial* material, const char* materialName)
{
	unsigned int textureIndex = 0;

	FbxProperty property;

	property = material->FindProperty(materialName);

	if (property.IsValid())
	{
		unsigned int textureCount = property.GetSrcObjectCount<FbxTexture>();
		for (unsigned int i = 0; i < textureCount; ++i)
		{
			FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(i);
			if (layeredTexture)
			{
				throw std::exception("Layered Texture is currently unsupported\n");
			}
			else
			{
				FbxTexture* texture = property.GetSrcObject<FbxTexture>(i);
				if (texture)
				{
					std::string textureType = property.GetNameAsCStr();
					FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(texture);

					if (fileTexture)
					{
						// 변환된 파일 경로..
						std::string fileRoute = ConvertFileRoute(fileTexture->GetFileName());
						std::string count = to_string(m_Model->m_materialcount);
						if (textureType == "DiffuseColor")
						{
							m_materialdata->m_DiffuseMap = new MaterialMap();
							m_materialdata->m_isDiffuseMap = true;
							m_materialdata->m_DiffuseMap->m_map_name = "Diffuse Map" + count;
							m_materialdata->m_DiffuseMap->m_bitmap = fileRoute;
							m_materialdata->m_MapList.push_back(m_materialdata->m_DiffuseMap);
						}
						else if (textureType == "SpecularColor")
						{
							m_materialdata->m_SpecularMap = new MaterialMap();
							m_materialdata->m_isSpecularMap = true;
							m_materialdata->m_SpecularMap->m_map_name = "Specular Map" + count;
							m_materialdata->m_SpecularMap->m_bitmap = fileRoute;
							m_materialdata->m_MapList.push_back(m_materialdata->m_SpecularMap);
						}
						else if (textureType == "NormalMap")
						{
							m_materialdata->m_BumpMap = new MaterialMap();
							m_materialdata->m_isBumpMap = true;
							m_materialdata->m_BumpMap->m_map_name = "Normal Map" + count;
							m_materialdata->m_BumpMap->m_bitmap = fileRoute;
							m_materialdata->m_MapList.push_back(m_materialdata->m_BumpMap);
						}
						else if (textureType == "TransparentColor")	// MaskMap
						{
						}
					}
				}
			}
		}
	}
}

void FBXParser::CreateVertex(fbxsdk::FbxMesh* mesh, std::vector<BoneWeights>& boneWeights, int vertexCount)
{
	FbxVector4 fbxPos;

	// Vertex 개수만큼 Vertex 생성..
	for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
	{
		m_OneMesh->m_MeshVertices.push_back(new Vertex);

		fbxPos = mesh->GetControlPointAt(vertexIndex);

		Vector3 pos = ConvertVector3(fbxPos);

		// Bounding Box 체크용 Position
		Vector3 nowPos = NoConvertVector3(fbxPos);

		m_OneMesh->m_MeshVertices[vertexIndex]->m_Pos = pos;
		m_OneMesh->m_MeshVertices[vertexIndex]->m_Indices = vertexIndex;

		// Bone Weight Data
		for (unsigned int boneIndex = 0; boneIndex < boneWeights[vertexIndex].m_BoneWeights.size(); boneIndex++)
		{
			int iBone = boneWeights[vertexIndex].m_BoneWeights[boneIndex].first;

			// FBX는 Bone Mesh Data 가 없으므로 연결된 Mesh Vertex 비교하여 BoundingBox 설정..
			m_Model->m_AllBoneList[iBone].second->m_Min = XMVectorMin(m_Model->m_AllBoneList[iBone].second->m_Min, nowPos);
			m_Model->m_AllBoneList[iBone].second->m_Max = XMVectorMax(m_Model->m_AllBoneList[iBone].second->m_Max, nowPos);
			m_Model->m_AllBoneList[iBone].second->m_isCol = true;

			m_OneMesh->m_MeshVertices[vertexIndex]->m_boneIndices.push_back(boneWeights[vertexIndex].m_BoneWeights[boneIndex].first);
			m_OneMesh->m_MeshVertices[vertexIndex]->m_boneWeights.push_back(boneWeights[vertexIndex].m_BoneWeights[boneIndex].second);
		}
	}
}

void FBXParser::CreateMesh()
{
	m_OneMesh = nullptr;
	m_OneMesh = new ParserData::Mesh();
	m_Model->m_MeshList.push_back(m_OneMesh);
}

void FBXParser::CreateBone()
{
	m_OneBone = nullptr;
	m_OneBone = new Bone();
}

int FBXParser::FindBoneIndex(std::string boneName)
{
	for (BonePair bone : m_Model->m_AllBoneList)
	{
		if (bone.first == boneName)
			return bone.second->m_bone_number;
	}

	return -1;
}

ParserData::Bone* FBXParser::FindBone(std::string boneName)
{
	for (BonePair bone : m_Model->m_AllBoneList)
	{
		if (bone.first == boneName)
			return bone.second;
	}

	return nullptr;
}

ParserData::Mesh* FBXParser::FindMesh(std::string meshName)
{
	for (Mesh* mesh : m_Model->m_MeshList)
	{
		if (mesh->m_NodeName == meshName)
			return mesh;
	}

	return nullptr;
}
