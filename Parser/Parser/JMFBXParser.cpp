#include "JMFBXParser.h"
#include "AnimationData.h"
#include "MaterialData.h"
#include "BoneWeights.h"

#include "StringHelper.h"

#include "StopWatch.h"
#include <DirectXMath.h>
#include "CATrace.h"

#include "BinarySerializer.h"

#define FBXSDK_SHARED

using namespace DirectX;
using namespace SimpleMath;
using namespace fbxsdk;

FBXModel::FBXModel()
	: isSkinnedAnimation(false)
	, isMeshAnimation(false)
	, pSkeleton(nullptr)
	, animationType(eANIM_TYPE::NONE)
{
	pMesh_V.clear();
}

FBXModel::~FBXModel()
{
	CA_TRACE("FBXModel 해제");
}

JMFBXParser::JMFBXParser() :
	m_pManager(nullptr),
	m_pImporter(nullptr),
	m_pScene(nullptr),
	m_pMesh(nullptr)
{
}

JMFBXParser::~JMFBXParser()
{
}


FbxAMatrix multT(FbxNode* pNode)
{
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	FbxNode* pParentNode = pNode->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}

	FbxAMatrix matrix = parentMatrix * localMatrix * matrixGeo;
	//FbxVector4 result = matrix.MultT(vector);
	return matrix;
}

FbxVector4 multT(FbxNode* pNode, FbxVector4 vector)
{
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	FbxNode* pParentNode = pNode->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}

	FbxAMatrix matrix = parentMatrix * localMatrix * matrixGeo;
	FbxVector4 result = matrix.MultT(vector);
	return result;
}

FbxAMatrix multTGeom(FbxNode* pNode)
{
	FbxAMatrix matrixGeo;
	matrixGeo.SetIdentity();
	if (pNode->GetNodeAttribute())
	{
		const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
		matrixGeo.SetT(lT);
		matrixGeo.SetR(lR);
		matrixGeo.SetS(lS);
	}
	FbxAMatrix localMatrix = pNode->EvaluateLocalTransform();

	FbxNode* pParentNode = pNode->GetParent();
	FbxAMatrix parentMatrix = pParentNode->EvaluateLocalTransform();
	while ((pParentNode = pParentNode->GetParent()) != NULL)
	{
		parentMatrix = pParentNode->EvaluateLocalTransform() * parentMatrix;
	}

	FbxAMatrix matrix = parentMatrix * localMatrix * matrixGeo;
	//FbxVector4 result = matrix.MultT(vector);
	return matrixGeo;
}

void JMFBXParser::Initalize()
{
	// FBX SDK Manager 생성..
	m_pManager = FbxManager::Create();

	// Scene 생성..
	m_pScene = FbxScene::Create(m_pManager, "My Scene");

	// IOSettings 객체 생성 및 설정
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	// FbxImporter 객체 생성
	m_pImporter = FbxImporter::Create(m_pManager, "");

	m_pGeometryConverter = new FbxGeometryConverter(m_pManager);
}

void JMFBXParser::SceneSetting()
{
	//씬 임포트
	// 여러개의 FBX를 한꺼번에 로드할때 문제가 발생한다.
	// m_pImporter를 리셋한다음에 다시 만들어야할까?
	bool status = m_pImporter->Initialize(m_fbxFileName, -1, m_pManager->GetIOSettings());
	m_pImporter->Import(m_pScene);

	if (!status)
	{
		CA_TRACE("Call to FbxImporter::Initialize() failed.\n");
		CA_TRACE("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());

		exit(-1);
	}

	FbxAxisSystem sceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem(); // 씬 내의 좌표축을 바꾼다. 
	FbxAxisSystem::MayaYUp.ConvertScene(m_pScene);

	// 씬 내에서 삼각형화 할 수 있는 모든 노드를 삼각형화 시킨다. 
	m_pGeometryConverter->Triangulate(m_pScene, true, true);
}

void JMFBXParser::Destroy()
{
}

void JMFBXParser::LoadScene(
	std::string fileName,
	std::shared_ptr<FBXModel> outfbxModel,
	std::unordered_map<std::string, MaterialDesc*>& outMaterialMap)
{
	const char* _fileName = fileName.c_str();

	m_fbxFileName = _fileName;
	m_HasSkeleton = false;

	// 로드가 불릴 때 마다 초기화 해준다
	outMaterialMap.clear();
	m_pFBXModel.reset();
	m_pFBXModel = outfbxModel;
	m_pFBXModel->fileName = fileName;

	SceneSetting();

	// Scene에서 RootNode 가져오기..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	// 머트리얼을 먼저 챙긴다
	LoadMaterial(outMaterialMap);

	// 머트리얼을 벡터로 옮겨 담는다
	std::unordered_map<std::string, MaterialDesc*> ::iterator iter;
	std::vector<MaterialDesc> material_V(outMaterialMap.size());
	int i = 0;
	for (iter = outMaterialMap.begin(); iter != outMaterialMap.end(); ++iter)
	{
		material_V[i] = *(iter->second);
		++i;
	}

	// 노드 로드
	LoadNode(_pRootNode, FbxNodeAttribute::eSkeleton);
	if (m_HasSkeleton == true)
	{

		m_BoneOffsets.resize(m_pSkeleton->m_Bones.size());
	}
	LoadNode(_pRootNode, FbxNodeAttribute::eMesh);
	LoadAnimations();

	// m_pTempSkeleton에 대한 접근이 모두 끝나면 넘겨주자
	if (m_pSkeleton != nullptr)
	{
		m_pFBXModel->pSkeleton.swap(m_pSkeleton);
	}

	///시리얼라이저 Test(임시)
	BinarySerializer* ser = new BinarySerializer();
	ser->SaveBinaryFileFBX(m_pFBXModel.get(), material_V, "../Data/bin/serialize");
}

void JMFBXParser::LoadScene(std::string& fileName, std::string& writePath)
{
	//StopWatch::StartMeasure();
	indexDebug = 0;
	const char* _fileName = fileName.c_str();

	std::string _nowWritePath(writePath);
	_nowWritePath += StringHelper::GetFileName(fileName, false);

	m_fbxFileName = _fileName;
	m_HasSkeleton = false;

	// 로드가 불릴 때 마다 초기화 해준다
	m_pFBXModel = std::make_shared<FBXModel>();
	m_pFBXModel->fileName = fileName;

	SceneSetting();

	// Scene에서 RootNode 가져오기..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	std::vector<MaterialDesc> _material_V;
	// 머트리얼을 먼저 챙긴다
	LoadMaterial(_material_V);

	// 노드 로드
	LoadNode(_pRootNode, FbxNodeAttribute::eSkeleton);
	if (m_HasSkeleton == true)
	{
		// 여기서 명확하게 판가름이 날것임
		m_BoneOffsets.resize(m_pSkeleton->m_Bones.size());
	}
	LoadNode(_pRootNode, FbxNodeAttribute::eMesh);
	LoadAnimations();

	// m_pTempSkeleton에 대한 접근이 모두 끝나면 넘겨주자
	if (m_pSkeleton != nullptr)
	{
		m_pFBXModel->pSkeleton.swap(m_pSkeleton);
	}

	///시리얼라이저 Test(임시)
	BinarySerializer* ser = new BinarySerializer();
	ser->SaveBinaryFileFBX(m_pFBXModel.get(), _material_V, _nowWritePath.c_str());
	ser->SaveAnimFile(m_pFBXModel.get(), _nowWritePath.c_str());
	//ser->SaveBinaryFileFBX(m_pFBXModel.get(), VertexType::Type_Vertex, _material_V, "../Data/bin/serialize");
	//ser->SaveBinaryFileFBX(m_pFBXModel.get(), VertexType::Type_Vertex, _material_V, "../test");
	//StopWatch::StopMeasure();

}


void JMFBXParser::LoadNode(fbxsdk::FbxNode* node, FbxNodeAttribute::EType attribute)
{
	FbxNodeAttribute* _pNodeAtrribute = node->GetNodeAttribute();

	if (_pNodeAtrribute != nullptr)
	{
		FbxString nodename = node->GetName();
		FbxNodeAttribute::EType attributeType = _pNodeAtrribute->GetAttributeType();
		if (attributeType == attribute)
		{
			switch (_pNodeAtrribute->GetAttributeType())
			{
			case FbxNodeAttribute::eSkeleton:
				ProcessSkeleton(node);
				break;
			case FbxNodeAttribute::eMesh:
				ProcessMesh(node);
				break;
			case FbxNodeAttribute::eLight:
				break;
			default:
				break;
			}
		}
	}

	const unsigned int _childCount = node->GetChildCount();
	for (unsigned int i = 0; i < _childCount; i++)
	{
		// 자식 노드가 인덱싱되어있다는 사실을 알 수 있다
		std::string _name = node->GetName();
		LoadNode(node->GetChild(i), attribute);
	}
}

void JMFBXParser::ProcessMesh(fbxsdk::FbxNode* node)
{
	// 새로운 매쉬(m_pMesh) 생성
	m_pMesh = std::make_shared<JMParserData::Mesh>();

	++indexDebug;

	m_pFBXModel->pMesh_V.push_back(m_pMesh);

	// 몇번째 인덱스인지 
	m_pMesh->meshIndex = m_pFBXModel->pMesh_V.size() - 1;

	m_pFBXmesh = node->GetMesh();

	// 머트리얼의 항상 첫번째 것을 가져온다 + 있으면 Mesh의 머트리얼 이름을 알려준다
	FbxSurfaceMaterial* _mat = node->GetMaterial(0);
	if (_mat != nullptr)
	{
		m_pFBXModel->materialName_V.push_back(_mat->GetName());
		m_pMesh->materialName = _mat->GetName();
	}

	m_pMesh->nodeName = node->GetName();

	//부모, 자식 노드 정보 저장
	if (node->GetParent() != nullptr)
	{
		m_pMesh->isParentExist = true;
		m_pMesh->nodeParentName = node->GetParent()->GetName();

		m_pMesh->pNodeParentMesh = m_pFBXModel->GetMesh(m_pMesh->nodeParentName);
		if (m_pMesh->pNodeParentMesh == nullptr)
		{
			m_pMesh->isParentExist = false;
		}
		if (m_pMesh->pNodeParentMesh != nullptr)
		{
			m_pMesh->pNodeParentMesh->childList_V.push_back(m_pMesh);
		}
	}
	else
	{
		m_pMesh->isParentExist = false;
	}

	// Node TRS 설정, Mesh만 있기 때문에 두번째 인자는 null만 넘겨준다
	SetTransform(node, nullptr);



	std::vector<BoneWeights> boneWeights;
	if (m_HasSkeleton == true)
	{
		// Vertex 개수만큼 BoneWeight 생성..
		// 해당 Vertex Index와 동일한 Index에 가중치 값, Bone Index Data 설정..
		boneWeights.resize(m_pFBXmesh->GetControlPointsCount());

		bool _isSkin = false;
		// Box처럼 Bone이 없는 구조도 생각하자
		_isSkin = ProcessBoneWeights(node, boneWeights);
		m_pMesh->isSkinningObject = m_HasSkeleton;

	}
	CreateVertex(m_pFBXmesh, boneWeights, m_pFBXmesh->GetControlPointsCount());

	// Bone Data 설정 결과에 따른 Skinning Object 판별..
	/// 인덱스 정보 저장
	m_pMesh->meshNumIndex = ProcessMeshVertexIndex();

	/// uv, 노말, 바이노말, 탄젠트값 추가
	ProcessMeshVertexInfo();

	if (m_pSkeleton != nullptr)
	{
		m_pSkeleton->m_BoneOffsets = m_BoneOffsets;
	}

	/// 버텍스 스플릿
	SplitMesh2();

	for (int i = 0; i < m_pMesh->meshFace_V.size(); i++)
	{
		MakeTangentSpace(m_pFBXmesh, i);
	}

	for (int i = 0; i < m_pMesh->meshFace_V.size(); i++)
	{
		MakeBinormal(i);
	}

	

	/// 버텍스 스플릿후 인덱스 정보 저장
	//?? 인덱스는 동일하게 주니까 안줘도 되나??
	unsigned int _optVertexCount = 0;

	// 버텍스 스플릿 후 인덱스 설정
	m_pMesh->pOptIndex = new JMParserData::IndexList[m_pMesh->meshFace_V.size() * 3];

	for (unsigned int i = 0; i < m_pMesh->meshFace_V.size(); i++)
	{
		JMParserData::Face* _nowFace = m_pMesh->meshFace_V[i];

		for (unsigned int j = 0; j < 3; j++)
		{
			switch (j)
			{
			case 0:
				m_pMesh->pOptIndex[i].index_A[j] = _nowFace->vertexIndex_A[0];
				break;
			case 1:
				m_pMesh->pOptIndex[i].index_A[j] = _nowFace->vertexIndex_A[2];
				break;
			case 2:
				m_pMesh->pOptIndex[i].index_A[j] = _nowFace->vertexIndex_A[1];
				break;
			default:
				break;
			}
		}
		//인덱스 개수를 센다
		_optVertexCount++;
	}

	m_pMesh->optIndexNum = _optVertexCount;
	//+ 각 버텍스마다 나눠줘야한다  
}

void JMFBXParser::ProcessSkeleton(fbxsdk::FbxNode* node)
{
	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == nullptr) return;

	// 새로운 메쉬인지를 먼저 확인
	if (m_pSkeleton == nullptr)
	{
		m_pSkeleton = std::make_unique<JMParserData::Skeleton>();

		// 스켈레톤이 있다면 스키닝 애니메이션
		m_pFBXModel->isSkinnedAnimation = true;
		m_HasSkeleton = true;
	}

	JMParserData::Bone* _nowBone = new JMParserData::Bone();

	// 부모 Bone Index가 -1 이면 최상위 Root Node..
	// 최상위 노드(부모노드)인지 확인..
	int parentBoneIndex = -1;
	FbxNode* parentNode = node->GetParent();


	if (parentNode != nullptr)
	{
		std::string nodeName = parentNode->GetName();
		parentBoneIndex = m_pSkeleton->FindBoneIndex(nodeName);

		_nowBone->parentBoneName = node->GetParent()->GetName();
		_nowBone->isParentExist = true;

		if (0 <= parentBoneIndex)
		{
			_nowBone->pParentBone = m_pSkeleton->m_Bones[parentBoneIndex];
		}

		if (_nowBone->pParentBone != nullptr)
		{
			_nowBone->pParentBone->childBoneList_V.push_back(_nowBone);
		}
	}
	else
	{
		_nowBone->isParentExist = false;
	}

	_nowBone->boneName = node->GetName();
	_nowBone->parentBoneIndex = parentBoneIndex;
	m_pSkeleton->AddBone(_nowBone);

	// Node TRS 설정
	SetTransform(node, _nowBone);

	// 뼈대 없으면 return
	if (node->GetSkeleton() == nullptr)
	{
		return;
	}

	_nowBone->boneIndex = static_cast<int>(m_pSkeleton->m_Bones.size() - 1);	// 0번부터 시작하게 하려면
	_nowBone->isBoneSet = true;
	_nowBone->minmum = Vector3(+FLT_MAX, +FLT_MAX, +FLT_MAX);
	_nowBone->maximum = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

}

DirectX::SimpleMath::Matrix JMFBXParser::GetLocalTransformFromCurrentTake(FbxNode* node, FbxTime time)
{
	if (node == nullptr)
	{
		DirectX::SimpleMath::Matrix _mat;// = DirectX::SimpleMath::Matrix::Identity;
		return _mat;
	}

	FbxAMatrix _matrix = node->EvaluateLocalTransform(time);

	DirectX::SimpleMath::Matrix _result;
#ifdef LEGACY
	_result = ConvertMatrix(_matrix);

	if (m_pFBXModel->animationType == eANIM_TYPE::SKINNED)
	{
		std::string _boneName = node->GetName();
		JMParserData::Bone* _bone = m_pSkeleton->FindBone(_boneName);

		if (_bone->parentBoneIndex == -1)
		{
			const auto yaw = -90.0f * XM_PI / 180.0f;
			DirectX::SimpleMath::Quaternion q = XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

			_result *= XMMatrixRotationQuaternion(q);
		}
	}



#else
	_result = AniConvertMatrix(_matrix);
#endif


	return _result;
}

void JMFBXParser::LoadAnimations()
{
	FbxNode* _rootNode = m_pScene->GetRootNode();

	/// 원래는 fbx에서 지정된 FrameRate만큼 뽑을 수 있지만 그러면 키프레임의 해상도가 너무 낮다
	/// 이에 따라 일괄적으로 60으로 수정
	//초당 프레임(24frame, 30frame, 60frame 등..)
	//float _frameRate = (float)FbxTime::GetFrameRate(m_pScene->GetGlobalSettings().GetTimeMode());
	float _frameRate = (float)60.0f;

	// 애니메이션 이름을 가져올 자료구조를 만들기 위한 사전 작업
	FbxArray<FbxString*> _animationArray;
	FbxDocument* _document = dynamic_cast<FbxDocument*>(m_pScene);
	if (_document != nullptr)
	{
		_document->FillAnimStackNameArray(_animationArray);
	}

	// 애님스택에 있는 애니메이션 수를 가져와서 그 만큼 AnimationData를 동적할당
	size_t _nowAnimationCount = m_pImporter->GetAnimStackCount();
	m_pFBXModel->animationCount = static_cast<int>(_nowAnimationCount);

	// 애니메이션이 있는지 없는지 검사한다
	if (_nowAnimationCount <= 0) return;


	m_pFBXModel->pAnimation_V.resize(_nowAnimationCount);
	m_pFBXModel->animationName_V.resize(_nowAnimationCount);

	for (size_t _animIndex = 0; _animIndex < _nowAnimationCount; _animIndex++)
	{
		std::shared_ptr<AnimationData> _newAnimData = std::make_shared<AnimationData>();
		m_pFBXModel->pAnimation_V[_animIndex] = _newAnimData;

		if (m_HasSkeleton == true)
		{
			m_pFBXModel->pAnimation_V[_animIndex]->animationKeyFrame.resize(m_pSkeleton->m_Bones.size());
		}

		FbxTakeInfo* _animationInfo = m_pImporter->GetTakeInfo(static_cast<int>(_animIndex));
		std::string _animationName = _animationInfo->mName.Buffer();
		m_pFBXModel->animationName_V[_animIndex] = _animationName;

		FbxTimeSpan _span = _animationInfo->mLocalTimeSpan;

		// 애니메이션의 시작시간과 종료시간에대한 정보를 가지고있음
		double _tempStart = _span.GetStart().GetSecondDouble();
		double _tempStop = _span.GetStop().GetSecondDouble();

		// 시작시간이 종료시간보다 클때
		if (_tempStart < _tempStop)
		{
			// 구동시간 동안 총 몇 프레임이 수행될지를 keyFrames에 담아줌
			int _keyFrames = (int)((_tempStop - _tempStart) * (double)_frameRate);
			double _ticksperFrame = (_tempStop - _tempStart) / _keyFrames;

			m_pFBXModel->pAnimation_V[_animIndex]->m_AnimationName = _animationName;
			m_pFBXModel->pAnimation_V[_animIndex]->m_TicksPerFrame = (float)_ticksperFrame; // 한 프레임 재생 시간

			// 애니메이션 총 프레임
			m_pFBXModel->pAnimation_V[_animIndex]->m_TotalKeyFrames = _keyFrames;
			m_pFBXModel->pAnimation_V[_animIndex]->m_LastKeyFrame = _keyFrames;


			ProcessAnimation(_rootNode, _animationName, _animIndex, _frameRate, static_cast<float>(_tempStart), static_cast<float>(_tempStop));
		}
	}
}

void JMFBXParser::ProcessAnimation(
	fbxsdk::FbxNode* node,
	std::string animationName,
	unsigned int animationIndex,
	float frameRate,
	float start,
	float stop)
{
	FbxNodeAttribute* _nodeAttribute = node->GetNodeAttribute();

	if (_nodeAttribute != nullptr)
	{
		if (_nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			m_pFBXModel->animationType = eANIM_TYPE::SKINNED;

			if (m_pSkeleton != nullptr)
			{
				std::string _boneName = node->GetName();
				JMParserData::Bone* _bone = m_pSkeleton->FindBone(_boneName);

				if (_bone != nullptr)
				{

					double _time = 0;
					double _fps = 1.0f / frameRate;
					unsigned int _keyFrameCount = static_cast<unsigned int>((stop / _fps) + 1);

					std::vector<DirectX::SimpleMath::Matrix> _thisBoneKeyFrame;
					_thisBoneKeyFrame.resize(_keyFrameCount);

					for (unsigned int i = 0; i < _keyFrameCount; i++)
					{
						_time += _fps;

						FbxTime _animationTime;
						_animationTime.SetSecondDouble(_time);
						_thisBoneKeyFrame[i] = GetLocalTransformFromCurrentTake(node, _animationTime);
					}

					// 애니메이션 [0][0] bone - matrix 순서이다
					// 원래는 PushBack으로 넣었는데, BoneIndex 보장이 되지 않으므로, resize 후 boneIndex 접근으로 수정했다.
					m_pFBXModel->pAnimation_V[animationIndex]->animationKeyFrame[_bone->boneIndex] = _thisBoneKeyFrame;
				}
			}
		}
		else if (_nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{

			if (m_pFBXModel->animationType == eANIM_TYPE::SKINNED) return;

			m_pFBXModel->animationType = eANIM_TYPE::MESH;

			std::vector<DirectX::SimpleMath::Matrix> _thisBoneKeyFrame;

			double _time = 0;
			double _fps = 1.0f / frameRate;
			unsigned int _keyFrameCount = static_cast<unsigned int>((stop / _fps) + 1);

			_thisBoneKeyFrame.resize(_keyFrameCount);

			for (unsigned int i = 0; i < _keyFrameCount; i++)
			{
				_time += _fps;

				FbxTime _animationTime;
				_animationTime.SetSecondDouble(_time);
				_thisBoneKeyFrame[i] = GetLocalTransformFromCurrentTake(node, _animationTime);
			}

			// 메쉬라면 본이 없으니, 메쉬 본체 하나에만 애니메이션 정보가 찍힐 것
			m_pFBXModel->pAnimation_V[animationIndex]->animationKeyFrame.push_back(_thisBoneKeyFrame);
		}
	}

	for (int i = 0; i < node->GetChildCount(); i++)
	{
		ProcessAnimation(node->GetChild(i), animationName, animationIndex, frameRate, start, stop);
	}

}

bool JMFBXParser::ProcessBoneWeights(fbxsdk::FbxNode* node, std::vector<BoneWeights>& meshBoneWeights)
{
	int deformerCount = m_pFBXmesh->GetDeformerCount();

	// DeformerCount가 1보다 작으면 Bone Data가 없다고 가정..
	if (deformerCount < 1) return false;

	// 스켈레톤-본의 size만큼 BoneAnimation을 Resize한다.


	for (int i = 0; i < deformerCount; ++i)
	{
		// Deform : 변형되다
		FbxDeformer* deformer = m_pFBXmesh->GetDeformer(i);

		if (deformer == nullptr) continue;

		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
		{
			FbxSkin* skin = reinterpret_cast<FbxSkin*>(m_pFBXmesh->GetDeformer(i, FbxDeformer::eSkin));

			if (skin == nullptr) continue;

			// Cluster는 가중치를 가진 메쉬들의 부분집합이다
			// 스킨 안에 있는 모든 클러스트들의 개수
			int clusterCount = skin->GetClusterCount();

			FbxCluster::ELinkMode linkMode;

			// Skin Mesh 체크..
			//JMParserData::Mesh* skinMesh = FindMesh(node->GetName());
			std::vector<BoneWeights> skinBoneWeights(meshBoneWeights.size());
			for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				if (cluster == nullptr) continue;
				linkMode = cluster->GetLinkMode();

				FbxNode* pLinkNode = cluster->GetLink();
				if (pLinkNode == nullptr) continue;
				std::string _currJointName = pLinkNode->GetName();

				// 현재 joint(본)의 이름을 통해 index를 구해온다
				int _boneIndex = m_pSkeleton->FindBoneIndex(_currJointName);
				if (_boneIndex < 0)
				{
					continue;
				}

				// Bone Index에 해당하는 Bone 추출..
				JMParserData::Bone* _nowBone = m_pSkeleton->m_Bones[_boneIndex];

				FbxAMatrix matClusterTransformMatrix;
				FbxAMatrix matClusterLinkTransformMatrix;

				cluster->GetTransformMatrix(matClusterTransformMatrix);
				cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

				// Bone Matrix 설정..
				Matrix clusterMatrix = ConvertMatrix(matClusterTransformMatrix);
				Matrix clusterlinkMatrix = ConvertMatrix(matClusterLinkTransformMatrix);

				_nowBone->bindPoseTransform = clusterlinkMatrix;
				_nowBone->boneReferenceTransform = clusterMatrix;

				// BindPose 행렬을 구하는 공식
				const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eDestinationPivot);
				const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eDestinationPivot);
				const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eDestinationPivot);

				FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);
				Matrix geometryMatrix = ConvertMatrix(geometryTransform);

				// OffsetMatrix는 WorldBindPose의 역행렬
				Matrix offsetMatrix = clusterMatrix * clusterlinkMatrix.Invert() * geometryMatrix;

				m_BoneOffsets[_boneIndex] = offsetMatrix;


				// Bone Mesh 체크..
				// 매트릭스랑 본의 index를 맞춰주기 위한 작업인듯

				for (int j = 0; j < cluster->GetControlPointIndicesCount(); ++j)
				{
					int index = cluster->GetControlPointIndices()[j];
					double weight = cluster->GetControlPointWeights()[j];

					skinBoneWeights[index].AddBoneWeight(_boneIndex, (float)weight);
				}
			}

			switch (linkMode)
			{
			case FbxCluster::eNormalize:
			{
				// 가중치 합을 1.0으로 만드는 작업..
				for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
					skinBoneWeights[i].Normalize();
			}
			break;

			case FbxCluster::eAdditive:
				// 가중치가 몇이든 그냥 합산
				break;

			case FbxCluster::eTotalOne:
				// 이미 정규화가 되어있는 모드
				break;
			}

			for (size_t i = 0; i < meshBoneWeights.size(); i++)
				meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
		}
	}

	return true;
}

#pragma region Material

Vector3 Double3ToVector3(const FbxDouble3& double3)
{
	Vector3 _vector3;

	_vector3.x = static_cast<float>(double3.mData[0]);
	_vector3.y = static_cast<float>(double3.mData[1]);
	_vector3.z = static_cast<float>(double3.mData[2]);

	return _vector3;
}

float DoubleToFloat(const FbxDouble& double1)
{
	float val = static_cast<float>(double1);
	return val;
}

void JMFBXParser::LoadMaterial(std::unordered_map<std::string, MaterialDesc*>& outMaterialMap)
{
	unsigned int _nowMatCount = m_pScene->GetMaterialCount();

	/*
	for (unsigned int i = 0; i < _nowMatCount; i++)
	{
		MaterialDesc* _materialDesc = new MaterialDesc;

		fbxsdk::FbxSurfaceMaterial* _nowMat = m_pScene->GetMaterial(i);

		// 텍스쳐 이름을 가져올때 뒤에 확장자가 안붙는다.
		GetMaterialTexture(_nowMat, _materialDesc);

		if (_nowMat->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			_materialDesc->diffuse = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Diffuse);
			_materialDesc->ambient = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Ambient);
			_materialDesc->emissive = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Emissive);
			_materialDesc->fresnelR0 = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Reflection);
			_materialDesc->specular = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Specular);
			_materialDesc->transparentFactor = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->TransparencyFactor);
			_materialDesc->specularPower = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->SpecularFactor);
			_materialDesc->shineness = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Shininess);
			_materialDesc->reflactionFactor = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->ReflectionFactor);
		}
		else if (_nowMat->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			_materialDesc->diffuse = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Diffuse);
			_materialDesc->ambient = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Ambient);
			_materialDesc->emissive = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Emissive);
		}

		outMaterialMap.insert({ _materialDesc->materialName, _materialDesc });
	}
	*/
}

void JMFBXParser::LoadMaterial(std::vector<MaterialDesc>& outMaterials)
{
	unsigned int _nowMatCount = m_pScene->GetMaterialCount();


	for (unsigned int i = 0; i < _nowMatCount; i++)
	{
		MaterialDesc _materialDesc;

		fbxsdk::FbxSurfaceMaterial* _nowMat = m_pScene->GetMaterial(i);

		/*
		unsigned int textureIndex = 0;
		FbxProperty property;

		FBXSDK_FOR_EACH_TEXTURE(textureIndex)
		{
			property = _nowMat->FindProperty(FbxLayerElement::sTextureChannelNames[textureIndex]);
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
								if (textureType == "DiffuseColor")
								{

								}
								else if (textureType == "SpecularColor")
								{
								}
								else if (textureType == "Bump")
								{
									int a = 0;
								}
								else if (textureType == "NormalMap")
								{
									int a = 0;
								}

							}
						}
					}
				}
			}
		}

		*/













		// 텍스쳐 이름을 가져올때 뒤에 확장자가 안붙는다.
		GetMaterialTexture(_nowMat, _materialDesc);

		if (_nowMat->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			_materialDesc.diffuse = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Diffuse);
			_materialDesc.ambient = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Ambient);
			_materialDesc.emissive = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Emissive);
			_materialDesc.fresnelR0 = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Reflection);
			_materialDesc.specular = Double3ToVector3(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Specular);
			_materialDesc.transparentFactor = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->TransparencyFactor);
			_materialDesc.specularPower = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->SpecularFactor);
			_materialDesc.shineness = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->Shininess);
			_materialDesc.reflactionFactor = DoubleToFloat(reinterpret_cast<FbxSurfacePhong*>(_nowMat)->ReflectionFactor);
		}
		else if (_nowMat->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			_materialDesc.diffuse = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Diffuse);
			_materialDesc.ambient = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Ambient);
			_materialDesc.emissive = Double3ToVector3(reinterpret_cast<FbxSurfaceLambert*>(_nowMat)->Emissive);
		}

		outMaterials.push_back(_materialDesc);
	}
}

void JMFBXParser::GetMaterialTexture(FbxSurfaceMaterial* mat, MaterialDesc& outDesc)
{

	unsigned int _textureIndex = 0;
	FbxProperty _prop;

	FBXSDK_FOR_EACH_TEXTURE(_textureIndex)
	{
		_prop = mat->FindProperty(FbxLayerElement::sTextureChannelNames[_textureIndex]);

		outDesc.materialName = mat->GetName();

		if (_prop.IsValid())
		{
			_prop = mat->FindProperty(FbxSurfaceMaterial::sEmissive);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sDiffuse);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sSpecular);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sShininess);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sBump);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sNormalMap);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sTransparentColor);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sReflection);
			LoadTextureProperty(_prop, outDesc);

			_prop = mat->FindProperty(FbxSurfaceMaterial::sDisplacementColor);
			LoadTextureProperty(_prop, outDesc);
		}
	}
}

void JMFBXParser::LoadTextureProperty(FbxProperty& prop, MaterialDesc& outDesc)
{
	std::string _propertyName = prop.GetName().Buffer();

	int numberOfLayeredTextures = prop.GetSrcObjectCount<FbxLayeredTexture>();

	if (0 != numberOfLayeredTextures)
	{
		return;
	}
	else
	{
		int numberOfTextures = prop.GetSrcObjectCount<FbxTexture>();

		if (1 < numberOfTextures)
		{
			for (int i = 0; i < numberOfTextures; i++)
			{
				FbxTexture* _texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(i));

				FbxFileTexture* _fileTexture = FbxCast<FbxFileTexture>(_texture);

				LoadTextureToMaterial(_propertyName, _fileTexture, outDesc);
			}
		}

		if (1 == numberOfTextures)
		{
			FbxTexture* _texture = FbxCast<FbxTexture>(prop.GetSrcObject<FbxTexture>(0));

			FbxFileTexture* _fileTexture = FbxCast<FbxFileTexture>(_texture);

			LoadTextureToMaterial(_propertyName, _fileTexture, outDesc);
		}
	}

}

void JMFBXParser::LoadTextureToMaterial(std::string propertyName, FbxFileTexture* fileTexture, MaterialDesc& outDesc)
{
	if (fileTexture != nullptr)
	{
		std::string _nowTextureName = fileTexture->GetFileName();

		if (propertyName == "EmissiveColor")
		{
			outDesc.textureName_V[EMISSIVE_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "DiffuseColor")
		{
			outDesc.textureName_V[DIFFUSE_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "SpecularColor")
		{
			outDesc.textureName_V[SPECULAR_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "ShininessExponent")
		{
			outDesc.textureName_V[SHININESS_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "Bump" || propertyName == "NormalMap")
		{
			outDesc.textureName_V[BUMP_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "TransparentColor")
		{
			outDesc.textureName_V[TRANSPARENT_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "ReflectionColor")
		{
			outDesc.textureName_V[REFLECTION_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
		else if (propertyName == "DisplacementColor")
		{
			outDesc.textureName_V[DISPLACEMENT_TEXUTRE] = StringHelper::GetFileName(_nowTextureName, true);
		}
	}
}

#pragma endregion


// 매쉬의 3개의 정점마다 인덱스값을 넣어준다
unsigned int JMFBXParser::ProcessMeshVertexIndex()
{
	unsigned int _triCount = m_pFBXmesh->GetPolygonCount(); // 메쉬의 삼각형 개수를 가져온다. 
	unsigned int _vertexCount = 0; // 정점의 개수 

	std::vector<JMParserData::Face*> _tempFaces;
	_tempFaces.resize(_triCount);

	JMParserData::Face* nowFaces = (JMParserData::Face*)malloc(sizeof(JMParserData::Face) * _triCount);
	memset(nowFaces, 0, sizeof(JMParserData::Face) * _triCount);


	for (unsigned int i = 0; i < _triCount; ++i) // 삼각형의 개수 
	{
		_tempFaces[i] = &nowFaces[i];

		for (unsigned int j = 0; j < 3; ++j) // 삼각형은 세 개의 정점으로 구성 
		{
			int _controlPointIndex = m_pFBXmesh->GetPolygonVertex(i, j); // 제어점 인덱스를 가져온다.

			// 면을 이루고있는 인덱스들
			_tempFaces[i]->vertexIndex_A[j] = _controlPointIndex;

			_vertexCount++; // 인덱스의 개수++ 
		}
	}

	m_pMesh->meshFace_V.swap(_tempFaces);

	return _vertexCount;
}

#pragma region MeshVertexInfo

// 매쉬의 3개의 정점마다  + 노멀, 바이노멀, 탄젠트, UV
// ProcessMeshVertexIndex 와 같이 돌려야할까 따로빼야할까
void JMFBXParser::ProcessMeshVertexInfo()
{
	// 폴리곤 카운트를 가져온다
	unsigned int _triCount = m_pFBXmesh->GetPolygonCount(); // 메쉬의 삼각형 개수를 가져온다. 
	unsigned int _vertexCount = 0; // 정점의 개수 

	for (unsigned int i = 0; i < _triCount; ++i) // 삼각형의 개수 
	{
		for (unsigned int j = 0; j < 3; ++j) // 삼각형은 세 개의 정점으로 구성 
		{
			int _controlPointIndex = m_pFBXmesh->GetPolygonVertex(i, j); // 제어점 인덱스를 가져온다.

			// 면을 이루고있는 버텍스들의 노말
			if (m_pFBXmesh->GetElementNormalCount() > 0) // 노말의 개수를 센다. 
			{
				m_pMesh->meshFace_V[i]->vertexNormal_A[j] = ReadNormal(m_pFBXmesh, _controlPointIndex, _vertexCount);
			}

			// 면을 이루고있는 버텍스들의 UV
			if (m_pFBXmesh->GetElementUVCount() > 0)
			{
				m_pMesh->meshFace_V[i]->vertexUV_A[j] = ReadUV(m_pFBXmesh, _controlPointIndex, m_pFBXmesh->GetTextureUVIndex(i, j));
			}

			// 면을 이루고있는 버텍스들의 바이노말
			if (m_pFBXmesh->GetElementBinormalCount() > 0)
			{
				m_pMesh->meshFace_V[i]->vertexBinormal_A[j] = RadeBinormal(m_pFBXmesh, _controlPointIndex, _vertexCount);
			}

			// 면을 이루고있는 버텍스들의 탄젠트
			if (m_pFBXmesh->GetElementTangentCount() > 0)
			{
				m_pMesh->meshFace_V[i]->vertexTangent_A[j] = ReadTangent(m_pFBXmesh, _controlPointIndex, _vertexCount);
			}

			_vertexCount++;
		}
	}


}

JMParserData::Float3 JMFBXParser::ReadNormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	const FbxGeometryElementNormal* _pVertexNormal = mesh->GetElementNormal(0); // 노말 획득 
	JMParserData::Float3 _result; // 노말 벡터를 저장할 벡터 

	// 매핑 모드 
	// 3ds MAX나 블렌더 등에서 
	// 버텍스 스플릿을 하여 같은 좌표점에 면에따라 다른노말을 넣을것이냐
	// 버텍스 스플릿을 하지않고 좌표점 하나에 면에따른 노말의 평균을 낸 값을 노말으로 넣을것이냐를 나누는 설정
	switch (_pVertexNormal->GetMappingMode())
	{
		// control point mapping 
		// 정점을 기준으로 노멀값이 들어간다
	case FbxGeometryElement::eByControlPoint:
	{
		switch (_pVertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			_result.x = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			_result.z = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			_result.y = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexNormal->GetIndexArray().GetAt(controlPointIndex); // 인덱스를 얻어온다. 
			_result.x = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[0]);
			_result.z = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[1]);
			_result.y = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// 인덱스를 기준으로 노멀값이 들어간다
	// 버텍스 스플릿이 필요하다
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (_pVertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			_result.x = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			_result.z = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			_result.y = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexNormal->GetIndexArray().GetAt(vertexCounter); // 인덱스를 얻어온다. 
			_result.x = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[0]);
			_result.z = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[1]);
			_result.y = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	}
	return _result;
}

JMParserData::Float2 JMFBXParser::ReadUV(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	const FbxGeometryElementUV* _pVertexUV = mesh->GetElementUV(0);
	//JMParserData::Float2 result;
	FbxVector2 fbxUV;

	// 매핑 모드 
	// 3ds MAX나 블렌더 등에서 
	// 버텍스 스플릿을 하여 같은 좌표점에 면에따라 다른노말을 넣을것이냐
	// 버텍스 스플릿을 하지않고 좌표점 하나에 면에따른 노말의 평균을 낸 값을 노말으로 넣을것이냐를 나누는 설정

	switch (_pVertexUV->GetMappingMode()) // 매핑 모드 
	{
	case FbxGeometryElement::eByControlPoint: // control point mapping 
	{
		switch (_pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			fbxUV = _pVertexUV->GetDirectArray().GetAt(controlPointIndex);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexUV->GetIndexArray().GetAt(controlPointIndex); // 인덱스를 얻어온다. 
			fbxUV = _pVertexUV->GetDirectArray().GetAt(index);
		}
		break;
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (_pVertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			fbxUV = _pVertexUV->GetDirectArray().GetAt(vertexCounter);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			fbxUV = _pVertexUV->GetDirectArray().GetAt(vertexCounter);
		}
		break;
		}
	}
	break;
	}

	return JMParserData::Float2Func(static_cast<float>(fbxUV.mData[0]), 1.0f - static_cast<float>(fbxUV.mData[1]));
}

JMParserData::Float3 JMFBXParser::ReadTangent(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	const FbxGeometryElementTangent* _pVertexTanget = mesh->GetElementTangent(0);
	JMParserData::Float3 result;

	// 매핑 모드 
	// 3ds MAX나 블렌더 등에서 
	// 버텍스 스플릿을 하여 같은 좌표점에 면에따라 다른노말을 넣을것이냐
	// 버텍스 스플릿을 하지않고 좌표점 하나에 면에따른 노말의 평균을 낸 값을 노말으로 넣을것이냐를 나누는 설정
	switch (_pVertexTanget->GetMappingMode())
	{
		// control point mapping 
		// 정점을 기준으로 노멀값이 들어간다
	case FbxGeometryElement::eByControlPoint:
	{
		switch (_pVertexTanget->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result.y = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			result.z = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexTanget->GetIndexArray().GetAt(controlPointIndex); // 인덱스를 얻어온다. 
			result.x = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// 인덱스를 기준으로 노멀값이 들어간다
	// 버텍스 스플릿이 필요하다
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (_pVertexTanget->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(vertexCounter).mData[0]);
			result.y = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(vertexCounter).mData[1]);
			result.z = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexTanget->GetIndexArray().GetAt(vertexCounter); // 인덱스를 얻어온다. 
			result.x = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	}
	return result;
}

JMParserData::Float3 JMFBXParser::RadeBinormal(const FbxMesh* mesh, int controlPointIndex, int vertexCounter)
{
	const FbxGeometryElementBinormal* _pVertexBinormal = mesh->GetElementBinormal(0);
	JMParserData::Float3 result;

	// 매핑 모드 
	// 3ds MAX나 블렌더 등에서 
	// 버텍스 스플릿을 하여 같은 좌표점에 면에따라 다른노말을 넣을것이냐
	// 버텍스 스플릿을 하지않고 좌표점 하나에 면에따른 노말의 평균을 낸 값을 노말으로 넣을것이냐를 나누는 설정
	switch (_pVertexBinormal->GetMappingMode())
	{
		// control point mapping 
		// 정점을 기준으로 노멀값이 들어간다
	case FbxGeometryElement::eByControlPoint:
	{
		switch (_pVertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result.y = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			result.z = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexBinormal->GetIndexArray().GetAt(controlPointIndex); // 인덱스를 얻어온다. 
			result.x = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// 인덱스를 기준으로 노멀값이 들어간다
	// 버텍스 스플릿이 필요하다
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (_pVertexBinormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result.x = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[0]);
			result.y = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[1]);
			result.z = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(vertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = _pVertexBinormal->GetIndexArray().GetAt(vertexCounter); // 인덱스를 얻어온다. 
			result.x = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	}

	return result;
}



DirectX::SimpleMath::Vector3 Float3ToVector3(JMParserData::Float3& val)
{
	DirectX::SimpleMath::Vector3 _result;
	_result.x = val.x;
	_result.y = val.y;
	_result.z = val.z;
	return _result;
}

DirectX::SimpleMath::Vector2 Float2ToVector2(JMParserData::Float2& val)
{
	DirectX::SimpleMath::Vector2 _result;
	_result.x = val.x;
	_result.y = val.y;
	return _result;
}

JMParserData::Float3 Vector3ToFloat3(DirectX::SimpleMath::Vector3& val)
{
	JMParserData::Float3 _result;
	_result.x = val.x;
	_result.y = val.y;
	_result.z = val.z;
	return _result;
}

void JMFBXParser::MakeTangentSpace(const FbxMesh* mesh, int polygonIndex)
{
	// 폴리곤 자체를 받을 수단이 보이지 않아서 원하는 폴리곤의 버텍스에 접근하는 방법을 취함
	unsigned int _nowVertexIndex[3];
	for (unsigned int i = 0; i < 3; i++)
	{
		_nowVertexIndex[i] = m_pMesh->meshFace_V[polygonIndex]->vertexIndex_A[i];
	}

	/*
	// split하기 전 모든 메쉬들의 버텍스가 채워져있다는 전제 하에
	auto _nowVertexPos0 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[0]]->pos);
	auto _nowVertexPos1 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[1]]->pos);
	auto _nowVertexPos2 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[2]]->pos);

	// Vertex와 메쉬 데이터가 분리되어있고, meshFace에 대한 정보가 모두 차있다는 전제 하에
	auto _nowTexPos0 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[0]]->u, m_pMesh->optVertex_V[_nowVertexIndex[0]]->v);
	auto _nowTexPos1 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[1]]->u, m_pMesh->optVertex_V[_nowVertexIndex[1]]->v);
	auto _nowTexPos2 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[2]]->u, m_pMesh->optVertex_V[_nowVertexIndex[2]]->v);

	// 메쉬의 정보를 통해 메쉬 삼각형 변 벡터를 구한다
	DirectX::SimpleMath::Vector3  _edge1 = _nowVertexPos1 - _nowVertexPos0;
	DirectX::SimpleMath::Vector3  _edge2 = _nowVertexPos2 - _nowVertexPos0;

	float _X1 = _nowTexPos1.x - _nowTexPos0.x;
	float _X2 = _nowTexPos2.x - _nowTexPos0.x;
	float _Y1 = _nowTexPos1.y - _nowTexPos0.y;
	float _Y2 = _nowTexPos2.y - _nowTexPos0.y;

	// 역함수를 구해서 TBN을 구한다. 정확히는 T만 있으면 됨
	float _inverseScalar = 1.0f / (_X1 * _Y2 - _X2 * _Y1);
	DirectX::SimpleMath::Vector3 _tangent = (_edge1 * _Y2 - _edge2 * _Y1) * _inverseScalar;
	DirectX::SimpleMath::Vector3 _bitangent = (_edge1 * _X2 - _edge2 * _X1) * _inverseScalar;
	DirectX::SimpleMath::Vector3 _tagnetResult;
	DirectX::SimpleMath::Vector3 _bitagentResult;
	_tangent.Normalize(_tagnetResult);
	_bitangent.Normalize(_bitagentResult);

		auto _normalCrossTargent = [this, _tagnetResult, _bitagentResult, _nowVertexIndex](unsigned int idx)
	{
		DirectX::SimpleMath::Vector3 _normal = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[idx]]->normal);
		DirectX::SimpleMath::Vector3 _biTan;
		_biTan = _normal.Cross(_tagnetResult);
		return (_biTan.Dot(_bitagentResult) < 0.0f ? -1.0f : 1.0f);
	};

	float _wFactor[3];
	auto _combineTangetData = [this, _tagnetResult, _nowVertexIndex, &_wFactor](unsigned int idx)
	{
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.x = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.x + _tagnetResult.x;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.y = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.y + _tagnetResult.y;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.z = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.z + _tagnetResult.z;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.w = _wFactor[idx];
	};

	for (unsigned int i = 0; i < 3; i++)
	{
		_wFactor[i] = _normalCrossTargent(i);
		_combineTangetData(i);
	}

	*/

	// split하기 전 모든 메쉬들의 버텍스가 채워져있다는 전제 하에
	auto v1 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[0]]->pos);
	auto v2 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[1]]->pos);
	auto v3 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[2]]->pos);

	// Vertex와 메쉬 데이터가 분리되어있고, meshFace에 대한 정보가 모두 차있다는 전제 하에
	auto w1 = Vector2(m_pMesh->optVertex_V[_nowVertexIndex[0]]->u, m_pMesh->optVertex_V[_nowVertexIndex[0]]->v);
	auto w2 = Vector2(m_pMesh->optVertex_V[_nowVertexIndex[1]]->u, m_pMesh->optVertex_V[_nowVertexIndex[1]]->v);
	auto w3 = Vector2(m_pMesh->optVertex_V[_nowVertexIndex[2]]->u, m_pMesh->optVertex_V[_nowVertexIndex[2]]->v);


	float x1 = v2.x - v1.x;
	float x2 = v3.x - v1.x;
	float y1 = v2.y - v1.y;
	float y2 = v3.y - v1.y;
	float z1 = v2.z - v1.z;
	float z2 = v3.z - v1.z;

	float s1 = w2.x - w1.x;
	float s2 = w3.x - w1.x;
	float t1 = w2.y - w1.y;
	float t2 = w3.y - w1.y;

	float r = 1.0F / (s1 * t2 - s2 * t1);
	Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
	Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);


	auto _tangentBitangent = [this, sdir, tdir, _nowVertexIndex](unsigned int idx)
	{
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.x += sdir.x;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.y += sdir.y;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.z += sdir.z;

		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->binormal.x += tdir.x;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->binormal.y += tdir.y;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->binormal.z += tdir.z;
	};

	for (unsigned int i = 0; i < 3; i++)
	{
		_tangentBitangent(i);
	}

}

void JMFBXParser::MakeBinormal(int polygonIndex)
{

	unsigned int _nowVertexIndex[3];
	for (unsigned int i = 0; i < 3; i++)
	{
		_nowVertexIndex[i] = m_pMesh->meshFace_V[polygonIndex]->vertexIndex_A[i];
	}

	auto _normalCrossTargent = [this, _nowVertexIndex](unsigned int idx)
	{
		DirectX::SimpleMath::Vector3 _tangent;
		_tangent.x = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.x;
		_tangent.y = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.y;
		_tangent.z = m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.z;

		DirectX::SimpleMath::Vector3 _normal = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[idx]]->normal);
		_tangent = _tangent - _normal * _normal.Dot(_tangent);
		_tangent.Normalize();

		DirectX::SimpleMath::Vector3 _bitangent = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[idx]]->binormal);
		DirectX::SimpleMath::Vector3 _tempBi;
		_tempBi = _normal.Cross(_tangent);

		float _wFactor = 0.0f;
		if (_tempBi.Dot(_bitangent) < 0.0f)
			_wFactor = -1.0f;
		else
			_wFactor = 1.0f;

		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.x = _tangent.x;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.y = _tangent.y;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.z = _tangent.z;
		m_pMesh->optVertex_V[_nowVertexIndex[idx]]->tangent.w = _wFactor;
	};

	for (unsigned int i = 0; i < 3; i++)
	{
		_normalCrossTargent(i);
	}


}

#pragma endregion

// 버텍스 스플릿
void JMFBXParser::SplitMesh()
{
	size_t _nowVertexCount = m_pMesh->meshVertex_V.size();

	std::vector<JMParserData::Vertex*> _tempVertices;
	_tempVertices.resize(_nowVertexCount);

	JMParserData::Vertex* _nowVertex = (JMParserData::Vertex*)malloc(sizeof(JMParserData::Vertex) * _nowVertexCount);
	memset(_nowVertex, 0, sizeof(JMParserData::Vertex) * _nowVertexCount);

	for (unsigned int i = 0; i < _nowVertexCount; i++)
	{
		_tempVertices[i] = &_nowVertex[i];

		// 위치값과 인덱스를 넣어준다.
		_tempVertices[i]->pos = m_pMesh->meshVertex_V[i]->pos;
		_tempVertices[i]->index = m_pMesh->meshVertex_V[i]->index;
		_tempVertices[i]->u = m_pMesh->meshVertex_V[i]->u;
		_tempVertices[i]->v = m_pMesh->meshVertex_V[i]->v;

		_tempVertices[i]->tangent = m_pMesh->meshVertex_V[i]->tangent;

		if (m_HasSkeleton == true)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				_tempVertices[i]->boneIndex_Arr[j] = m_pMesh->meshVertex_V[i]->boneIndex_Arr[j];
				_tempVertices[i]->boneWeight_Arr[j] = m_pMesh->meshVertex_V[i]->boneWeight_Arr[j];
			}
		}
	}

	m_pMesh->optVertex_V.swap(_tempVertices);

	/// 현재 : m_meshVertex_V(그냥 버텍스의 개수) == m_optVertex_V(스플릿한 버텍스의 개수)

	// 스플릿될 버텍스들은 Face의 갯수 * 3만큼 존재해야 한다.
	// 버텍스가 하나라도 페이스마다 각각의 버텍스의 노말이 존재하기 때문이다.
	// SplitVertex의 첫 시작 지점은 MeshVertex의 끝에서 직하기 때문에 Size로 지정해준다.

	size_t _faceCount = m_pMesh->meshFace_V.size();			// 면의 갯수
	size_t _vertexCount = m_pMesh->meshVertex_V.size();		// 매쉬를 이루는 버텍스의 개수(원본 버텍스의 개수)
	size_t _splitVertexCount = 0;							// 나눠진 버텍스의 개수

	size_t _resize_VertexIndex = m_pMesh->meshVertex_V.size();

	/// 버텍스를 만들지 말지 결정하자
	bool _createNew = false;

	/// 메쉬의 페이스리스트를 기준으로 * 3만큼 돈다.
	// 면이 존재하는만큼 
	for (unsigned int i = 0; i < _faceCount; i++)
	{
		// 면에는 버텍스 3개 존재 
		for (int j = 0; j < 3; j++)
		{
			// 페이스의 갯수 * 인덱스[0~2] 값을 통해 optVertex에 접근 
			int _vertexIndexMakeUpFace = m_pMesh->meshFace_V[i]->vertexIndex_A[j];
			/// _pNowVertex는 앞으로 최적화 시켜야할 버텍스
			JMParserData::Vertex* _pNowVertex = m_pMesh->optVertex_V[_vertexIndexMakeUpFace];

			/// //////////////////////////////////////
			/// 이번에 접근할 버텍스에 노말값이 없다면(= 맨 위에서 처음 복사한 optVertex들. 원본값과 똑같다.)
			/// //////////////////////////////////////
			if (_pNowVertex->isNormalSet == false)
			{
				_pNowVertex->normal = m_pMesh->meshFace_V[i]->vertexNormal_A[j];
				_pNowVertex->isNormalSet = true;
			}
			if (m_pMesh->meshFace_V.size() && _pNowVertex->isUVSet == false)
			{
				_pNowVertex->u = m_pMesh->meshFace_V[i]->vertexUV_A[j].x;
				_pNowVertex->v = m_pMesh->meshFace_V[i]->vertexUV_A[j].y;
				_pNowVertex->isUVSet = true;
			}

			/// //////////////////////////////////////
			/// 버텍스를 스플릿할지 말지 결정하는 부분(노말과 UV하나라도 다르면 false)
			/// //////////////////////////////////////

			/// 1. 이번에 접근할 버텍스에 노말값이 있고
			//if (_pNowVertex->isNormalSet == true)
			{
				// 현재 접근한 면의 normal[0~2]과 (Face의 Normal[0~2])
				// 현재 버텍스의 값이 (Vertex의 Normal)
				// 같은지 판단
				/// 그런데 노말이 일치하지 않는다면(원래있던 노말값과는 다른값이다! 버텍스를 늘려야지)
				if (JMParserData::Float3Func(m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->normal)
					!= m_pMesh->meshFace_V[i]->vertexNormal_A[j])
				{
					_createNew = true;
				}
			}

			/// 2. 이번에 접근할 버텍스에 UV가 있고
			//if (_pNowVertex->isUVSet == true)
			{
				/// 그런데 UV가 일치하지 않는다면
				if ((m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->u != m_pMesh->meshFace_V[i]->vertexUV_A[j].x) ||
					(m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->v != m_pMesh->meshFace_V[i]->vertexUV_A[j].y))
				{
					_createNew = true;
				}
			}

			if (_createNew)
			{
				/// 3. 새로만든 버텍스가 생겼을때(원본말고)
				//if (_splitVertexCount > 0)
				if (_resize_VertexIndex > m_pMesh->meshVertex_V.size())
					//if (_vertexCount > m_pMesh->meshVertex_V.size())
				{
					/// 4. 스플릿된 버텍스의 개수만큼 돌면서 검사를 해준다.
					//for (size_t k = 0; k < _splitVertexCount; k++)
					for (unsigned int k = m_pMesh->meshVertex_V.size(); k < _resize_VertexIndex; k++)
						//for (size_t k = m_pMesh->meshVertex_V.size(); k < _vertexCount; k++)
					{
						/// 5. 스플릿된 버텍스의 인덱스와 노말이, 현재 접근 중인 face의 vertex 인덱스와 노말이 같고
						//if (m_pMesh->optVertex_V[_vertexCount + k]->index == m_pMesh->meshFace_V[i]->vertexIndex_A[j] &&
						//	JMParserData::Float3Func(m_pMesh->optVertex_V[_vertexCount + k]->normal) == m_pMesh->meshFace_V[i]->vertexNormal_A[j])
						if (m_pMesh->optVertex_V[k]->index == m_pMesh->meshFace_V[i]->vertexIndex_A[j] &&
							JMParserData::Float3Func(m_pMesh->optVertex_V[k]->normal) == m_pMesh->meshFace_V[i]->vertexNormal_A[j])
						{
							/// 6. 스플릿된 버텍스의 UV와, 현재 접근 중인 face의 vertex의 UV가 같다면
							//if (m_pMesh->optVertex_V[_vertexCount + k]->u == m_pMesh->meshFace_V[i]->vertexUV_A[j].x &&
							//	m_pMesh->optVertex_V[_vertexCount + k]->v == m_pMesh->meshFace_V[i]->vertexUV_A[j].y)
							if (m_pMesh->optVertex_V[k]->u == m_pMesh->meshFace_V[i]->vertexUV_A[j].x &&
								m_pMesh->optVertex_V[k]->v == m_pMesh->meshFace_V[i]->vertexUV_A[j].y)
							{
								/// 해당 Face 정보에 존재하는 인덱스 버퍼값을 넣어준다.
								//m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(_vertexCount + k);
								m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(k);
								_createNew = false;	// 더 만들지 않는다.
								break;
							}
						}
					}
				}
			}

			/// //////////////////////////////////////
			/// 버텍스를 스플릿하는 부분
			/// //////////////////////////////////////
			if (_createNew)
			{
				// 같은 위치값과 같은 인덱스 버퍼를 사용하는 버퍼를 새로 만든다.
				// 그리고 버텍스의 Normal값이 face의 vertexNormal(버텍스의 노말)와 일치하도록 해준다.
				JMParserData::Vertex* _pVertex = new JMParserData::Vertex;
				_pVertex->pos = _pNowVertex->pos;
				_pVertex->index = _pNowVertex->index;

				_pVertex->normal = m_pMesh->meshFace_V[i]->vertexNormal_A[j];
				_pVertex->isNormalSet = true;

				_pVertex->tangent = _pNowVertex->tangent;

				_pVertex->u = m_pMesh->meshFace_V[i]->vertexUV_A[j].x;
				_pVertex->v = m_pMesh->meshFace_V[i]->vertexUV_A[j].y;
				_pVertex->isUVSet = true;


				//스키닝
				if (m_pMesh->isSkinningObject == true)
				{
					for (unsigned int i = 0; i < 4; i++)
					{
						_pVertex->boneIndex_Arr[i] = m_pMesh->meshVertex_V[_vertexIndexMakeUpFace]->boneIndex_Arr[i];
						_pVertex->boneWeight_Arr[i] = m_pMesh->meshVertex_V[_vertexIndexMakeUpFace]->boneWeight_Arr[i];
					}
				}

				m_pMesh->optVertex_V.push_back(_pVertex);
				//m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(_vertexCount + _splitVertexCount);
				//_splitVertexCount++;

				m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(_resize_VertexIndex);
				_resize_VertexIndex++;
				_createNew = false;
			}
		}
	}
}

// 버텍스 스플릿
void JMFBXParser::SplitMesh2()
{
	if (m_pMesh->meshVertex_V.empty())
	{
		return;
	}

	size_t _vCount = m_pMesh->meshVertex_V.size();
	size_t _fCount = m_pMesh->meshFace_V.size();

	for (size_t i = 0; i < _vCount; i++)
	{
		m_pMesh->m_OriginVertexList.push_back(m_pMesh->meshVertex_V[i]->pos);
	}

	for (size_t i = 0; i < _fCount; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			switch (j)
			{
			case 0:
				m_pMesh->m_OriginIndexList.push_back(m_pMesh->meshFace_V[i]->vertexIndex_A[0]);
				break;
			case 1:
				m_pMesh->m_OriginIndexList.push_back(m_pMesh->meshFace_V[i]->vertexIndex_A[2]);
				break;
			case 2:
				m_pMesh->m_OriginIndexList.push_back(m_pMesh->meshFace_V[i]->vertexIndex_A[1]);
				break;
			default:
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////

	size_t _nowVertexCount = m_pMesh->meshVertex_V.size();

	std::vector<JMParserData::Vertex*> _tempVertices;
	_tempVertices.resize(_nowVertexCount);

	JMParserData::Vertex* _nowVertex = (JMParserData::Vertex*)malloc(sizeof(JMParserData::Vertex) * _nowVertexCount);
	memset(_nowVertex, 0, sizeof(JMParserData::Vertex) * _nowVertexCount);

	for (unsigned int i = 0; i < _nowVertexCount; i++)
	{
		_tempVertices[i] = &_nowVertex[i];

		// 위치값과 인덱스를 넣어준다.
		_tempVertices[i]->pos = m_pMesh->meshVertex_V[i]->pos;
		_tempVertices[i]->index = m_pMesh->meshVertex_V[i]->index;
		_tempVertices[i]->u = m_pMesh->meshVertex_V[i]->u;
		_tempVertices[i]->v = m_pMesh->meshVertex_V[i]->v;

		_tempVertices[i]->tangent = m_pMesh->meshVertex_V[i]->tangent;

		_tempVertices[i]->normal = m_pMesh->meshVertex_V[i]->normal;

		if (m_HasSkeleton == true)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				_tempVertices[i]->boneIndex_Arr[j] = m_pMesh->meshVertex_V[i]->boneIndex_Arr[j];
				_tempVertices[i]->boneWeight_Arr[j] = m_pMesh->meshVertex_V[i]->boneWeight_Arr[j];
			}
		}
	}

	m_pMesh->optVertex_V.swap(_tempVertices);

	//////////////////////////////////////////////////////////////////////////////////////////////


	bool _createNew = false;
	size_t _vSplitCount = 0;							// 나눠진 버텍스의 개수
	//size_t _nowVCount = (int)m_pMesh->meshVertex_V.size();	// <- //size_t _nowVertexCount = m_pMesh->meshVertex_V.size();

	// 각각 Face마다 존재하는 3개의 Vertex 비교..
	for (size_t i = 0; i < _fCount; i++)
	{
		JMParserData::Face* _nowFace = m_pMesh->meshFace_V[i];

		for (int j = 0; j < 3; j++)
		{
			unsigned int _vertexIndex = _nowFace->vertexIndex_A[j];

			JMParserData::Vertex* _nowVertex = m_pMesh->optVertex_V[_vertexIndex];
			JMParserData::Float3 _nowNormal = _nowFace->vertexNormal_A[j];
			JMParserData::Float2 _nowUV = JMParserData::Float2Func(_nowFace->vertexUV_A[j].x, _nowFace->vertexUV_A[j].y);
			int _nowIndex = _nowFace->vertexIndex_A[j];

			// 텍스처가 있고, 설정하지 않았으면 텍스처 u,v 설정..
			if (_nowVertex->isUVSet == false)
			{
				_nowVertex->u = _nowUV.x;
				_nowVertex->v = _nowUV.y;
				_nowVertex->isUVSet = true;
			}

			// 최초 인덱스 노말값 검사시엔 넣어주고 시작..
			if (_nowVertex->isNormalSet == false)
			{
				_nowVertex->normal = _nowNormal;
				_nowVertex->isNormalSet = true;
			}

			// Normal, U, V 값중 한개라도 다르면 Vertex 새로 생성..
			if (JMParserData::Float3Func(_nowVertex->normal) != _nowNormal)
			{
				_createNew = true;
			}

			//if (_nowVertex->u != _nowUV.x || _nowVertex->v != _nowUV.y)
			if (JMParserData::Float2Func(_nowVertex->u, _nowVertex->v) != _nowUV)
			{
				_createNew = true;
			}

			if (_createNew)
			{
				// 추가된 Vertex가 있다면 체크..
				if (_vCount + _vSplitCount > _vCount)
				{
					for (size_t k = 0; k < _vSplitCount; k++)
					{
						JMParserData::Vertex* _checkVertex = m_pMesh->optVertex_V[k];

						// 새로 추가한 Vertex와 동일한 데이터를 갖고있는 Face 내의 Vertex Index 수정..
						if (_checkVertex->index != _nowIndex)
						{
							continue;
						}

						if (JMParserData::Float3Func(_checkVertex->normal) != _nowNormal)
						{
							continue;
						}

						if (JMParserData::Float2Func(_checkVertex->u, _checkVertex->v) == _nowUV)
						{
							_nowFace->vertexIndex_A[j] = (int)k;
							_createNew = false;
							break;
						}
					}
				}
			}

			// 새로 추가해야할 Vertex..
			if (_createNew == true)
			{
				JMParserData::Vertex* _newVertex = new JMParserData::Vertex();
				_newVertex->pos = _nowVertex->pos;
				_newVertex->index = _nowVertex->index;
				_newVertex->normal = _nowNormal;				// 수정된 코드

				_newVertex->boneIndex_Arr[0] = _nowVertex->boneIndex_Arr[0];
				_newVertex->boneIndex_Arr[1] = _nowVertex->boneIndex_Arr[1];
				_newVertex->boneIndex_Arr[2] = _nowVertex->boneIndex_Arr[2];
				_newVertex->boneIndex_Arr[3] = _nowVertex->boneIndex_Arr[3];

				_newVertex->boneWeight_Arr[0] = _nowVertex->boneWeight_Arr[0];
				_newVertex->boneWeight_Arr[1] = _nowVertex->boneWeight_Arr[1];
				_newVertex->boneWeight_Arr[2] = _nowVertex->boneWeight_Arr[2];
				_newVertex->boneWeight_Arr[3] = _nowVertex->boneWeight_Arr[3];

				_newVertex->isNormalSet = true;

				_newVertex->u = _nowUV.x;
				_newVertex->v = _nowUV.y;
				_newVertex->isUVSet = true;


				m_pMesh->optVertex_V.push_back(_newVertex);
				_nowFace->vertexIndex_A[j] = (int)_vCount + _vSplitCount;
				_vSplitCount++;
				_createNew = false;
			}
		}
	}

	// Tanget 값 설정..
	//for (unsigned int i = 0; i < _fCount; i++)
	//{
	//	int _index0 = m_pMesh->meshFace_V[i]->vertexIndex_A[0];
	//	int _index1 = m_pMesh->meshFace_V[i]->vertexIndex_A[1];
	//	int _index2 = m_pMesh->meshFace_V[i]->vertexIndex_A[2];
	//
	//	JMParserData::Vertex* _vertex0 = m_pMesh->meshVertex_V[_index0];
	//	JMParserData::Vertex* _vertex1 = m_pMesh->meshVertex_V[_index1];
	//	JMParserData::Vertex* _vertex2 = m_pMesh->meshVertex_V[_index2];
	//
	//	JMParserData::Float3 _ep1 = JMParserData::Float3Func(_vertex1->pos) - _vertex0->pos;
	//	JMParserData::Float3 _ep2 = JMParserData::Float3Func(_vertex2->pos) - _vertex0->pos;
	//
	//	JMParserData::Float2 _uv1 = { _vertex1->u - _vertex0->u,
	//								  _vertex1->v - _vertex0->v };
	//	JMParserData::Float2 _uv2 = { _vertex2->u - _vertex0->u,
	//								  _vertex2->v - _vertex0->v };
	//
	//	float den = 1.0f / (_uv1.x * _uv2.y - _uv2.x * _uv1.y);
	//
	//	// 현재 픽셀 쉐이더에서 연산을 통해 T, B, N을 얻는데
	//	// 픽셀 쉐이더 내의 연산은 버텍스 쉐이더의 연산에 비해 호출 횟수가 차원이 다르게 크므로 부하가 올 수 있다..
	//	// 법선맵의 픽셀의 색은 픽셀 쉐이더 안이 아니면 얻을수 없기 때문에 픽셀 쉐이더에서 연산을 한다고 한다..
	//	/// 현재 연산과정을 버텍스 쉐이더로 옮겨둠
	//	DirectX::SimpleMath::Vector3 _tangent = (_ep1 * _uv2.y - _ep2 * _uv1.y) * den;
	//	_tangent.Normalize();
	//
	//	// 유사 정점은 값을 누적하여 쉐이더에서 평균값을 사용하도록 하자..
	//	_vertex0->tangent = _vertex0->tangent + _tangent;
	//	_vertex1->tangent = _vertex1->tangent + _tangent;
	//	_vertex2->tangent = _vertex2->tangent + _tangent;
	//}

	// 인덱스는 그냥 복사
	//for (unsigned int i = 0; i < _fCount; i++)
	//{
	//	m_pMesh->pOptIndex_V.push_back(new JMParserData::IndexList);
	//
	//	JMParserData::IndexList* _nowIndexList = m_pMesh->pOptIndex_V[i];
	//	JMParserData::Face* _nowFace = m_pMesh->meshFace_V[i];
	//
	//	for (unsigned int j = 0; j < 3; j++)
	//	{
	//		switch (j)
	//		{
	//		case 0:
	//			_nowIndexList->index_A[j] = _nowFace->vertexIndex_A[0];
	//			break;
	//		case 1:
	//			_nowIndexList->index_A[j] = _nowFace->vertexIndex_A[2];
	//			break;
	//		case 2:
	//			_nowIndexList->index_A[j] = _nowFace->vertexIndex_A[1];
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}
	//
	//m_pMesh->meshFace_V.clear();
}


void JMFBXParser::CreateVertex(fbxsdk::FbxMesh* mesh, std::vector<BoneWeights>& boneWeights, int vertexCount)
{
	FbxVector4 fbxPos;

	std::vector<JMParserData::Vertex*> _tempVertices;
	_tempVertices.resize(vertexCount);

	JMParserData::Vertex* _nowVertex = (JMParserData::Vertex*)malloc(sizeof(JMParserData::Vertex) * vertexCount);
	memset(_nowVertex, 0, sizeof(JMParserData::Vertex) * vertexCount);

	// Vertex 개수만큼 Vertex 생성..
	for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
	{
		_tempVertices[vertexIndex] = &_nowVertex[vertexIndex];

		fbxPos = mesh->GetControlPointAt(vertexIndex);

		JMParserData::Float3Func pos = JMParserData::Float3Func
		(
			static_cast<float>(fbxPos.mData[0]),
			static_cast<float>(fbxPos.mData[2]),
			static_cast<float>(fbxPos.mData[1])
		);

		// Bounding Box 체크용 Position
		Vector3 nowPos = Vector3
		(
			static_cast<float>(fbxPos.mData[0]),
			static_cast<float>(fbxPos.mData[1]),
			static_cast<float>(fbxPos.mData[2])
		);

		_tempVertices[vertexIndex]->pos = pos;
		_tempVertices[vertexIndex]->index = vertexIndex;

		// Bone Weight Data
		if (m_HasSkeleton == true)
		{
			size_t _weightSize = boneWeights[vertexIndex].m_BoneWeight_V.size();

			if (_weightSize > 4)
			{
				_weightSize = 4;
			}

			for (unsigned int boneIndex = 0; boneIndex < _weightSize; boneIndex++)
			{
				int _BoneIndex = boneWeights[vertexIndex].m_BoneWeight_V[boneIndex].first;

				// FBX는 Bone Mesh Data 가 없으므로 연결된 Mesh Vertex 비교하여 BoundingBox 설정..
				m_pSkeleton->m_Bones[_BoneIndex]->minmum = XMVectorMin(m_pSkeleton->m_Bones[_BoneIndex]->minmum, nowPos);
				m_pSkeleton->m_Bones[_BoneIndex]->maximum = XMVectorMax(m_pSkeleton->m_Bones[_BoneIndex]->maximum, nowPos);

				_tempVertices[vertexIndex]->boneIndex_Arr[boneIndex] = boneWeights[vertexIndex].m_BoneWeight_V[boneIndex].first;
				_tempVertices[vertexIndex]->boneWeight_Arr[boneIndex] = boneWeights[vertexIndex].m_BoneWeight_V[boneIndex].second;

			}
		}
	}

	m_pMesh->meshVertex_V.swap(_tempVertices);
}

void JMFBXParser::SetTransform(fbxsdk::FbxNode* node, JMParserData::Bone* bone)
{
	if (node == nullptr) return;

	FbxMatrix _worldpos = m_pScene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);
	FbxMatrix _localpos = m_pScene->GetAnimationEvaluator()->GetNodeLocalTransform(node);
	FbxMatrix _newMat = multT(node);
	FbxMatrix _newMatGeom = multTGeom(node);

#ifdef LEGACY
	// 받아온 월드와 로컬TM을 뒤집어준다(yz축 방향 때문이라고 추측)
	DirectX::SimpleMath::Matrix _world = ConvertMatrix(_worldpos);
	DirectX::SimpleMath::Matrix _local = ConvertMatrix(_localpos);
	DirectX::SimpleMath::Matrix _newMat2 = ConvertMatrix(_newMat);
	DirectX::SimpleMath::Matrix _newMatGeom2 = ConvertMatrix(_newMatGeom);

	switch (node->GetNodeAttribute()->GetAttributeType())
	{
	case FbxNodeAttribute::eSkeleton:
	{
		// 루트 본만 처리해준다?
		if (bone->pParentBone == nullptr)
		{
			const auto yaw = -90.0f * XM_PI / 180.0f;

			DirectX::SimpleMath::Quaternion q = XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

			_world *= XMMatrixRotationQuaternion(q);
			_local *= XMMatrixRotationQuaternion(q);
		}
		break;
	}
#else
	// 받아온 월드와 로컬TM을 뒤집어준다(yz축 방향 때문이라고 추측)
	DirectX::SimpleMath::Matrix _world = AniConvertMatrix(_worldpos);
	DirectX::SimpleMath::Matrix _local = AniConvertMatrix(_localpos);

	switch (node->GetNodeAttribute()->GetAttributeType())
	{
	case FbxNodeAttribute::eSkeleton:
	{
		if (bone->pParentBone == nullptr)
		{

		}
		break;
	}

#endif 


	case FbxNodeAttribute::eMesh:
	{
		if (m_pMesh->pNodeParentMesh == nullptr)
		{
			const auto yaw = -90.0f * XM_PI / 180.0f;

			DirectX::SimpleMath::Quaternion q = XMQuaternionRotationRollPitchYaw(yaw, 0.0f, 0.0f);

			_world *= XMMatrixRotationQuaternion(q);
			_local *= XMMatrixRotationQuaternion(q);
			_newMat2 *= XMMatrixRotationQuaternion(q);
			_newMatGeom2 *= XMMatrixRotationQuaternion(q);
		}
		break;
	}
	}


	DirectX::SimpleMath::Vector4 r1 = DirectX::SimpleMath::Vector4(_world._11, _world._12, _world._13, _world._14);
	DirectX::SimpleMath::Vector4 r2 = DirectX::SimpleMath::Vector4(_world._21, _world._22, _world._23, _world._24);
	DirectX::SimpleMath::Vector4 r3 = DirectX::SimpleMath::Vector4(_world._31, _world._32, _world._33, _world._34);
	DirectX::SimpleMath::Vector4 r4 = DirectX::SimpleMath::Vector4(_world._41, _world._42, _world._43, _world._44);

	//노드가 뼈대냐 매쉬냐에 따라 넣어주는곳 다름
	switch (node->GetNodeAttribute()->GetAttributeType())
	{
	case FbxNodeAttribute::eSkeleton:
	{
		bone->tmRow0 = DirectX::SimpleMath::Vector3(r1);
		bone->tmRow1 = DirectX::SimpleMath::Vector3(r2);
		bone->tmRow2 = DirectX::SimpleMath::Vector3(r3);
		bone->tmRow3 = DirectX::SimpleMath::Vector3(r4);

		bone->worldTM = _world;
		bone->localTM = _local;
		break;
	}
	case FbxNodeAttribute::eMesh:
	{
		m_pMesh->tmRow0 = DirectX::SimpleMath::Vector3(r1);
		m_pMesh->tmRow1 = DirectX::SimpleMath::Vector3(r2);
		m_pMesh->tmRow2 = DirectX::SimpleMath::Vector3(r3);
		m_pMesh->tmRow3 = DirectX::SimpleMath::Vector3(r4);

		if (m_pMesh->isParentExist == true)
		{
			m_pMesh->worldTM = DirectX::SimpleMath::Matrix();
			m_pMesh->localTM = _local;
		}
		else
		{
			m_pMesh->worldTM = _world;
			m_pMesh->localTM = DirectX::SimpleMath::Matrix();
		}
		break;
	}
	}
}

Matrix JMFBXParser::ConvertMatrix(FbxMatrix fbxMatrix)
{
	FbxVector4 r1 = fbxMatrix.GetRow(0);
	FbxVector4 r2 = fbxMatrix.GetRow(1);
	FbxVector4 r3 = fbxMatrix.GetRow(2);
	FbxVector4 r4 = fbxMatrix.GetRow(3);

	// 2행, 3행 변경..
	return Matrix
	(
		ConvertVector4(r1),
		ConvertVector4(r3),
		ConvertVector4(r2),
		ConvertVector4(r4)
	);
}

Vector4 JMFBXParser::ConvertVector4(FbxVector4 v4)
{
	// xyzw -> xzyw
	return Vector4
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[2]),
		static_cast<float>(v4.mData[1]),
		static_cast<float>(v4.mData[3])
	);
}

DirectX::SimpleMath::Matrix JMFBXParser::AniConvertMatrix(FbxMatrix fbxMatrix)
{
	FbxVector4 r1 = fbxMatrix.GetRow(0);
	FbxVector4 r2 = fbxMatrix.GetRow(1);
	FbxVector4 r3 = fbxMatrix.GetRow(2);
	FbxVector4 r4 = fbxMatrix.GetRow(3);

	// 2행, 3행 변경..
	return Matrix
	(
		AniConvertVector4(r1),
		AniConvertVector4(r2),
		AniConvertVector4(r3),
		AniConvertVector4(r4)
	);
}

DirectX::SimpleMath::Vector4 JMFBXParser::AniConvertVector4(FbxVector4 v4)
{
	return Vector4
	(
		static_cast<float>(v4.mData[0]),
		static_cast<float>(v4.mData[1]),
		static_cast<float>(v4.mData[2]),
		static_cast<float>(v4.mData[3])
	);
}

