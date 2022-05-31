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
	CA_TRACE("FBXModel ����");
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
	// FBX SDK Manager ����..
	m_pManager = FbxManager::Create();

	// Scene ����..
	m_pScene = FbxScene::Create(m_pManager, "My Scene");

	// IOSettings ��ü ���� �� ����
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	// FbxImporter ��ü ����
	m_pImporter = FbxImporter::Create(m_pManager, "");

	m_pGeometryConverter = new FbxGeometryConverter(m_pManager);
}

void JMFBXParser::SceneSetting()
{
	//�� ����Ʈ
	// �������� FBX�� �Ѳ����� �ε��Ҷ� ������ �߻��Ѵ�.
	// m_pImporter�� �����Ѵ����� �ٽ� �������ұ�?
	bool status = m_pImporter->Initialize(m_fbxFileName, -1, m_pManager->GetIOSettings());
	m_pImporter->Import(m_pScene);

	if (!status)
	{
		CA_TRACE("Call to FbxImporter::Initialize() failed.\n");
		CA_TRACE("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());

		exit(-1);
	}

	FbxAxisSystem sceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem(); // �� ���� ��ǥ���� �ٲ۴�. 
	FbxAxisSystem::MayaYUp.ConvertScene(m_pScene);

	// �� ������ �ﰢ��ȭ �� �� �ִ� ��� ��带 �ﰢ��ȭ ��Ų��. 
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

	// �ε尡 �Ҹ� �� ���� �ʱ�ȭ ���ش�
	outMaterialMap.clear();
	m_pFBXModel.reset();
	m_pFBXModel = outfbxModel;
	m_pFBXModel->fileName = fileName;

	SceneSetting();

	// Scene���� RootNode ��������..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	// ��Ʈ������ ���� ì���
	LoadMaterial(outMaterialMap);

	// ��Ʈ������ ���ͷ� �Ű� ��´�
	std::unordered_map<std::string, MaterialDesc*> ::iterator iter;
	std::vector<MaterialDesc> material_V(outMaterialMap.size());
	int i = 0;
	for (iter = outMaterialMap.begin(); iter != outMaterialMap.end(); ++iter)
	{
		material_V[i] = *(iter->second);
		++i;
	}

	// ��� �ε�
	LoadNode(_pRootNode, FbxNodeAttribute::eSkeleton);
	if (m_HasSkeleton == true)
	{

		m_BoneOffsets.resize(m_pSkeleton->m_Bones.size());
	}
	LoadNode(_pRootNode, FbxNodeAttribute::eMesh);
	LoadAnimations();

	// m_pTempSkeleton�� ���� ������ ��� ������ �Ѱ�����
	if (m_pSkeleton != nullptr)
	{
		m_pFBXModel->pSkeleton.swap(m_pSkeleton);
	}

	///�ø�������� Test(�ӽ�)
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

	// �ε尡 �Ҹ� �� ���� �ʱ�ȭ ���ش�
	m_pFBXModel = std::make_shared<FBXModel>();
	m_pFBXModel->fileName = fileName;

	SceneSetting();

	// Scene���� RootNode ��������..
	fbxsdk::FbxNode* _pRootNode = m_pScene->GetRootNode();

	std::vector<MaterialDesc> _material_V;
	// ��Ʈ������ ���� ì���
	LoadMaterial(_material_V);

	// ��� �ε�
	LoadNode(_pRootNode, FbxNodeAttribute::eSkeleton);
	if (m_HasSkeleton == true)
	{
		// ���⼭ ��Ȯ�ϰ� �ǰ����� ������
		m_BoneOffsets.resize(m_pSkeleton->m_Bones.size());
	}
	LoadNode(_pRootNode, FbxNodeAttribute::eMesh);
	LoadAnimations();

	// m_pTempSkeleton�� ���� ������ ��� ������ �Ѱ�����
	if (m_pSkeleton != nullptr)
	{
		m_pFBXModel->pSkeleton.swap(m_pSkeleton);
	}

	///�ø�������� Test(�ӽ�)
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
		// �ڽ� ��尡 �ε��̵Ǿ��ִٴ� ����� �� �� �ִ�
		std::string _name = node->GetName();
		LoadNode(node->GetChild(i), attribute);
	}
}

void JMFBXParser::ProcessMesh(fbxsdk::FbxNode* node)
{
	// ���ο� �Ž�(m_pMesh) ����
	m_pMesh = std::make_shared<JMParserData::Mesh>();
	
	++indexDebug;

	m_pFBXModel->pMesh_V.push_back(m_pMesh);
	
	// ���° �ε������� 
	m_pMesh->meshIndex = m_pFBXModel->pMesh_V.size() - 1;

	m_pFBXmesh = node->GetMesh();

	// ��Ʈ������ �׻� ù��° ���� �����´� + ������ Mesh�� ��Ʈ���� �̸��� �˷��ش�
	FbxSurfaceMaterial* _mat = node->GetMaterial(0);
	if (_mat != nullptr)
	{
		m_pFBXModel->materialName_V.push_back(_mat->GetName());
		m_pMesh->materialName = _mat->GetName();
	}

	m_pMesh->nodeName = node->GetName();

	//�θ�, �ڽ� ��� ���� ����
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

	// Node TRS ����, Mesh�� �ֱ� ������ �ι�° ���ڴ� null�� �Ѱ��ش�
	SetTransform(node, nullptr);

	

	std::vector<BoneWeights> boneWeights;
	if (m_HasSkeleton == true)
	{
		// Vertex ������ŭ BoneWeight ����..
		// �ش� Vertex Index�� ������ Index�� ����ġ ��, Bone Index Data ����..
		boneWeights.resize(m_pFBXmesh->GetControlPointsCount());

		bool _isSkin = false;
		// Boxó�� Bone�� ���� ������ ��������
		_isSkin = ProcessBoneWeights(node, boneWeights);
		m_pMesh->isSkinningObject = m_HasSkeleton;

	}
	CreateVertex(m_pFBXmesh, boneWeights, m_pFBXmesh->GetControlPointsCount());

	// Bone Data ���� ����� ���� Skinning Object �Ǻ�..
	/// �ε��� ���� ����
	m_pMesh->meshNumIndex = ProcessMeshVertexIndex();

	/// uv, �븻, ���̳븻, ź��Ʈ�� �߰�
	ProcessMeshVertexInfo();

	if (m_pSkeleton != nullptr)
	{
		m_pSkeleton->m_BoneOffsets = m_BoneOffsets;
	}

	/// ���ؽ� ���ø�
	SplitMesh2();

	for (int i = 0; i < m_pMesh->meshFace_V.size(); i++)
	{
		MakeTangentSpace(m_pFBXmesh, i);
	}

	/// ���ؽ� ���ø��� �ε��� ���� ����
	//?? �ε����� �����ϰ� �ִϱ� ���൵ �ǳ�??
	unsigned int _optVertexCount = 0;

	// ���ؽ� ���ø� �� �ε��� ����
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
		//�ε��� ������ ����
		_optVertexCount++;
	}

	m_pMesh->optIndexNum = _optVertexCount;
	//+ �� ���ؽ����� ��������Ѵ�  
}

void JMFBXParser::ProcessSkeleton(fbxsdk::FbxNode* node)
{
	FbxSkeleton* fbxSkeleton = node->GetSkeleton();
	if (fbxSkeleton == nullptr) return;

	// ���ο� �޽������� ���� Ȯ��
	if (m_pSkeleton == nullptr)
	{
		m_pSkeleton = std::make_unique<JMParserData::Skeleton>();

		// ���̷����� �ִٸ� ��Ű�� �ִϸ��̼�
		m_pFBXModel->isSkinnedAnimation = true;
		m_HasSkeleton = true;
	}

	JMParserData::Bone* _nowBone = new JMParserData::Bone();

	// �θ� Bone Index�� -1 �̸� �ֻ��� Root Node..
	// �ֻ��� ���(�θ���)���� Ȯ��..
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

	// Node TRS ����
	SetTransform(node, _nowBone);

	// ���� ������ return
	if (node->GetSkeleton() == nullptr)
	{
		return;
	}

	_nowBone->boneIndex = static_cast<int>(m_pSkeleton->m_Bones.size() - 1);	// 0������ �����ϰ� �Ϸ���
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

	/// ������ fbx���� ������ FrameRate��ŭ ���� �� ������ �׷��� Ű�������� �ػ󵵰� �ʹ� ����
	/// �̿� ���� �ϰ������� 60���� ����
	//�ʴ� ������(24frame, 30frame, 60frame ��..)
	//float _frameRate = (float)FbxTime::GetFrameRate(m_pScene->GetGlobalSettings().GetTimeMode());
	float _frameRate = (float)60.0f;

	// �ִϸ��̼� �̸��� ������ �ڷᱸ���� ����� ���� ���� �۾�
	FbxArray<FbxString*> _animationArray;
	FbxDocument* _document = dynamic_cast<FbxDocument*>(m_pScene);
	if (_document != nullptr)
	{
		_document->FillAnimStackNameArray(_animationArray);
	}

	// �ִԽ��ÿ� �ִ� �ִϸ��̼� ���� �����ͼ� �� ��ŭ AnimationData�� �����Ҵ�
	size_t _nowAnimationCount = m_pImporter->GetAnimStackCount();
	m_pFBXModel->animationCount = static_cast<int>(_nowAnimationCount);

	// �ִϸ��̼��� �ִ��� ������ �˻��Ѵ�
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

		// �ִϸ��̼��� ���۽ð��� ����ð������� ������ ����������
		double _tempStart = _span.GetStart().GetSecondDouble();
		double _tempStop = _span.GetStop().GetSecondDouble();

		// ���۽ð��� ����ð����� Ŭ��
		if (_tempStart < _tempStop)
		{
			// �����ð� ���� �� �� �������� ��������� keyFrames�� �����
			int _keyFrames = (int)((_tempStop - _tempStart) * (double)_frameRate);
			double _ticksperFrame = (_tempStop - _tempStart) / _keyFrames;

			m_pFBXModel->pAnimation_V[_animIndex]->m_AnimationName = _animationName;
			m_pFBXModel->pAnimation_V[_animIndex]->m_TicksPerFrame = (float)_ticksperFrame; // �� ������ ��� �ð�

			// �ִϸ��̼� �� ������
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

					// �ִϸ��̼� [0][0] bone - matrix �����̴�
					// ������ PushBack���� �־��µ�, BoneIndex ������ ���� �����Ƿ�, resize �� boneIndex �������� �����ߴ�.
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

			// �޽���� ���� ������, �޽� ��ü �ϳ����� �ִϸ��̼� ������ ���� ��
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

	// DeformerCount�� 1���� ������ Bone Data�� ���ٰ� ����..
	if (deformerCount < 1) return false;

	// ���̷���-���� size��ŭ BoneAnimation�� Resize�Ѵ�.


	for (int i = 0; i < deformerCount; ++i)
	{
		// Deform : �����Ǵ�
		FbxDeformer* deformer = m_pFBXmesh->GetDeformer(i);

		if (deformer == nullptr) continue;

		if (deformer->GetDeformerType() == FbxDeformer::eSkin)
		{
			FbxSkin* skin = reinterpret_cast<FbxSkin*>(m_pFBXmesh->GetDeformer(i, FbxDeformer::eSkin));

			if (skin == nullptr) continue;

			// Cluster�� ����ġ�� ���� �޽����� �κ������̴�
			// ��Ų �ȿ� �ִ� ��� Ŭ����Ʈ���� ����
			int clusterCount = skin->GetClusterCount();

			FbxCluster::ELinkMode linkMode;

			// Skin Mesh üũ..
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

				// ���� joint(��)�� �̸��� ���� index�� ���ؿ´�
				int _boneIndex = m_pSkeleton->FindBoneIndex(_currJointName);
				if (_boneIndex < 0)
				{
					continue;
				}

				// Bone Index�� �ش��ϴ� Bone ����..
				JMParserData::Bone* _nowBone = m_pSkeleton->m_Bones[_boneIndex];

				FbxAMatrix matClusterTransformMatrix;
				FbxAMatrix matClusterLinkTransformMatrix;

				cluster->GetTransformMatrix(matClusterTransformMatrix);
				cluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

				// Bone Matrix ����..
				Matrix clusterMatrix = ConvertMatrix(matClusterTransformMatrix);
				Matrix clusterlinkMatrix = ConvertMatrix(matClusterLinkTransformMatrix);

				_nowBone->bindPoseTransform = clusterlinkMatrix;
				_nowBone->boneReferenceTransform = clusterMatrix;

				// BindPose ����� ���ϴ� ����
				const FbxVector4 lT = node->GetGeometricTranslation(FbxNode::eDestinationPivot);
				const FbxVector4 lR = node->GetGeometricRotation(FbxNode::eDestinationPivot);
				const FbxVector4 lS = node->GetGeometricScaling(FbxNode::eDestinationPivot);

				FbxAMatrix geometryTransform = FbxAMatrix(lT, lR, lS);
				Matrix geometryMatrix = ConvertMatrix(geometryTransform);

				// OffsetMatrix�� WorldBindPose�� �����
				Matrix offsetMatrix = clusterMatrix * clusterlinkMatrix.Invert() * geometryMatrix;

				m_BoneOffsets[_boneIndex] = offsetMatrix;


				// Bone Mesh üũ..
				// ��Ʈ������ ���� index�� �����ֱ� ���� �۾��ε�

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
				// ����ġ ���� 1.0���� ����� �۾�..
				for (int i = 0; i < (int)skinBoneWeights.size(); ++i)
					skinBoneWeights[i].Normalize();
			}
			break;

			case FbxCluster::eAdditive:
				// ����ġ�� ���̵� �׳� �ջ�
				break;

			case FbxCluster::eTotalOne:
				// �̹� ����ȭ�� �Ǿ��ִ� ���
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

		// �ؽ��� �̸��� �����ö� �ڿ� Ȯ���ڰ� �Ⱥٴ´�.
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













		// �ؽ��� �̸��� �����ö� �ڿ� Ȯ���ڰ� �Ⱥٴ´�.
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


// �Ž��� 3���� �������� �ε������� �־��ش�
unsigned int JMFBXParser::ProcessMeshVertexIndex()
{
	unsigned int _triCount = m_pFBXmesh->GetPolygonCount(); // �޽��� �ﰢ�� ������ �����´�. 
	unsigned int _vertexCount = 0; // ������ ���� 

	std::vector<JMParserData::Face*> _tempFaces;
	_tempFaces.resize(_triCount);

	JMParserData::Face* nowFaces = (JMParserData::Face*)malloc(sizeof(JMParserData::Face) * _triCount);
	memset(nowFaces, 0, sizeof(JMParserData::Face) * _triCount);


	for (unsigned int i = 0; i < _triCount; ++i) // �ﰢ���� ���� 
	{
		_tempFaces[i] = &nowFaces[i];

		for (unsigned int j = 0; j < 3; ++j) // �ﰢ���� �� ���� �������� ���� 
		{
			int _controlPointIndex = m_pFBXmesh->GetPolygonVertex(i, j); // ������ �ε����� �����´�.

			// ���� �̷���ִ� �ε�����
			_tempFaces[i]->vertexIndex_A[j] = _controlPointIndex;

			_vertexCount++; // �ε����� ����++ 
		}
	}

	m_pMesh->meshFace_V.swap(_tempFaces);

	return _vertexCount;
}

#pragma region MeshVertexInfo

// �Ž��� 3���� ��������  + ���, ���̳��, ź��Ʈ, UV
// ProcessMeshVertexIndex �� ���� �������ұ� ���λ����ұ�
void JMFBXParser::ProcessMeshVertexInfo()
{
	// ������ ī��Ʈ�� �����´�
	unsigned int _triCount = m_pFBXmesh->GetPolygonCount(); // �޽��� �ﰢ�� ������ �����´�. 
	unsigned int _vertexCount = 0; // ������ ���� 

	for (unsigned int i = 0; i < _triCount; ++i) // �ﰢ���� ���� 
	{
		for (unsigned int j = 0; j < 3; ++j) // �ﰢ���� �� ���� �������� ���� 
		{
			int _controlPointIndex = m_pFBXmesh->GetPolygonVertex(i, j); // ������ �ε����� �����´�.

			// ���� �̷���ִ� ���ؽ����� �븻
			if (m_pFBXmesh->GetElementNormalCount() > 0) // �븻�� ������ ����. 
			{
				m_pMesh->meshFace_V[i]->vertexNormal_A[j] = ReadNormal(m_pFBXmesh, _controlPointIndex, _vertexCount);
			}

			// ���� �̷���ִ� ���ؽ����� UV
			if (m_pFBXmesh->GetElementUVCount() > 0)
			{
				m_pMesh->meshFace_V[i]->vertexUV_A[j] = ReadUV(m_pFBXmesh, _controlPointIndex, m_pFBXmesh->GetTextureUVIndex(i, j));
			}

			// ���� �̷���ִ� ���ؽ����� ���̳븻
			if (m_pFBXmesh->GetElementBinormalCount() > 0)
			{
				m_pMesh->meshFace_V[i]->vertexBinormal_A[j] = RadeBinormal(m_pFBXmesh, _controlPointIndex, _vertexCount);
			}

			// ���� �̷���ִ� ���ؽ����� ź��Ʈ
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
	const FbxGeometryElementNormal* _pVertexNormal = mesh->GetElementNormal(0); // �븻 ȹ�� 
	JMParserData::Float3 _result; // �븻 ���͸� ������ ���� 

	// ���� ��� 
	// 3ds MAX�� ���� ��� 
	// ���ؽ� ���ø��� �Ͽ� ���� ��ǥ���� �鿡���� �ٸ��븻�� �������̳�
	// ���ؽ� ���ø��� �����ʰ� ��ǥ�� �ϳ��� �鿡���� �븻�� ����� �� ���� �븻���� �������̳ĸ� ������ ����
	switch (_pVertexNormal->GetMappingMode())
	{
		// control point mapping 
		// ������ �������� ��ְ��� ����
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
			int index = _pVertexNormal->GetIndexArray().GetAt(controlPointIndex); // �ε����� ���´�. 
			_result.x = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[0]);
			_result.z = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[1]);
			_result.y = static_cast<float>(_pVertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// �ε����� �������� ��ְ��� ����
	// ���ؽ� ���ø��� �ʿ��ϴ�
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
			int index = _pVertexNormal->GetIndexArray().GetAt(vertexCounter); // �ε����� ���´�. 
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

	// ���� ��� 
	// 3ds MAX�� ���� ��� 
	// ���ؽ� ���ø��� �Ͽ� ���� ��ǥ���� �鿡���� �ٸ��븻�� �������̳�
	// ���ؽ� ���ø��� �����ʰ� ��ǥ�� �ϳ��� �鿡���� �븻�� ����� �� ���� �븻���� �������̳ĸ� ������ ����

	switch (_pVertexUV->GetMappingMode()) // ���� ��� 
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
			int index = _pVertexUV->GetIndexArray().GetAt(controlPointIndex); // �ε����� ���´�. 
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

	// ���� ��� 
	// 3ds MAX�� ���� ��� 
	// ���ؽ� ���ø��� �Ͽ� ���� ��ǥ���� �鿡���� �ٸ��븻�� �������̳�
	// ���ؽ� ���ø��� �����ʰ� ��ǥ�� �ϳ��� �鿡���� �븻�� ����� �� ���� �븻���� �������̳ĸ� ������ ����
	switch (_pVertexTanget->GetMappingMode())
	{
		// control point mapping 
		// ������ �������� ��ְ��� ����
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
			int index = _pVertexTanget->GetIndexArray().GetAt(controlPointIndex); // �ε����� ���´�. 
			result.x = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexTanget->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// �ε����� �������� ��ְ��� ����
	// ���ؽ� ���ø��� �ʿ��ϴ�
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
			int index = _pVertexTanget->GetIndexArray().GetAt(vertexCounter); // �ε����� ���´�. 
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

	// ���� ��� 
	// 3ds MAX�� ���� ��� 
	// ���ؽ� ���ø��� �Ͽ� ���� ��ǥ���� �鿡���� �ٸ��븻�� �������̳�
	// ���ؽ� ���ø��� �����ʰ� ��ǥ�� �ϳ��� �鿡���� �븻�� ����� �� ���� �븻���� �������̳ĸ� ������ ����
	switch (_pVertexBinormal->GetMappingMode())
	{
		// control point mapping 
		// ������ �������� ��ְ��� ����
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
			int index = _pVertexBinormal->GetIndexArray().GetAt(controlPointIndex); // �ε����� ���´�. 
			result.x = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[0]);
			result.y = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[1]);
			result.z = static_cast<float>(_pVertexBinormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
	}
	break;
	// polygon vertex mapping 
	// �ε����� �������� ��ְ��� ����
	// ���ؽ� ���ø��� �ʿ��ϴ�
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
			int index = _pVertexBinormal->GetIndexArray().GetAt(vertexCounter); // �ε����� ���´�. 
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
	// ������ ��ü�� ���� ������ ������ �ʾƼ� ���ϴ� �������� ���ؽ��� �����ϴ� ����� ����
	unsigned int _nowVertexIndex[3];
	for (unsigned int i = 0; i < 3; i++)
	{
		_nowVertexIndex[i] = m_pMesh->meshFace_V[polygonIndex]->vertexIndex_A[i];
	}

	// split�ϱ� �� ��� �޽����� ���ؽ��� ä�����ִٴ� ���� �Ͽ�
	auto _nowVertexPos0 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[0]]->pos);
	auto _nowVertexPos1 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[1]]->pos);
	auto _nowVertexPos2 = Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[2]]->pos);

	// Vertex�� �޽� �����Ͱ� �и��Ǿ��ְ�, meshFace�� ���� ������ ��� ���ִٴ� ���� �Ͽ�
	auto _nowTexPos0 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[0]]->u, m_pMesh->optVertex_V[_nowVertexIndex[0]]->v);
	auto _nowTexPos1 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[1]]->u, m_pMesh->optVertex_V[_nowVertexIndex[1]]->v);
	auto _nowTexPos2 =	Vector2(m_pMesh->optVertex_V[_nowVertexIndex[2]]->u, m_pMesh->optVertex_V[_nowVertexIndex[2]]->v);

	// �޽��� ������ ���� �޽� �ﰢ�� �� ���͸� ���Ѵ�
	DirectX::SimpleMath::Vector3  _edge1 = _nowVertexPos1 - _nowVertexPos0;
	DirectX::SimpleMath::Vector3  _edge2 = _nowVertexPos2 - _nowVertexPos0;

	float _X1 = _nowTexPos1.x - _nowTexPos0.x;
	float _X2 = _nowTexPos2.x - _nowTexPos0.x;
	float _Y1 = _nowTexPos1.y - _nowTexPos0.y;
	float _Y2 = _nowTexPos2.y - _nowTexPos0.y;

	// ���Լ��� ���ؼ� TBN�� ���Ѵ�. ��Ȯ���� T�� ������ ��
	float _inverseScalar = 1.0f / (_X1 * _Y2 - _X2 * _Y1);
	DirectX::SimpleMath::Vector3 _tangent = (_edge1 * _Y2 - _edge2 * _Y1) * _inverseScalar;
	DirectX::SimpleMath::Vector3 _result;
	_tangent.Normalize(_result);

	auto _prevTanget0 =	Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[0]]->tangent);
	auto _prevTanget1 =	Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[1]]->tangent);
	auto _prevTanget2 =	Float3ToVector3(m_pMesh->optVertex_V[_nowVertexIndex[2]]->tangent);

	_prevTanget0 += _result;	_prevTanget1 += _result;	_prevTanget2 += _result;

	m_pMesh->optVertex_V[_nowVertexIndex[0]]->tangent = Vector3ToFloat3(_prevTanget0);
	m_pMesh->optVertex_V[_nowVertexIndex[1]]->tangent = Vector3ToFloat3(_prevTanget1);
	m_pMesh->optVertex_V[_nowVertexIndex[2]]->tangent = Vector3ToFloat3(_prevTanget2);
}

#pragma endregion

// ���ؽ� ���ø�
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

		// ��ġ���� �ε����� �־��ش�.
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

	/// ���� : m_meshVertex_V(�׳� ���ؽ��� ����) == m_optVertex_V(���ø��� ���ؽ��� ����)

	// ���ø��� ���ؽ����� Face�� ���� * 3��ŭ �����ؾ� �Ѵ�.
	// ���ؽ��� �ϳ��� ���̽����� ������ ���ؽ��� �븻�� �����ϱ� �����̴�.
	// SplitVertex�� ù ���� ������ MeshVertex�� ������ ���ϱ� ������ Size�� �������ش�.

	size_t _faceCount = m_pMesh->meshFace_V.size();			// ���� ����
	size_t _vertexCount = m_pMesh->meshVertex_V.size();		// �Ž��� �̷�� ���ؽ��� ����(���� ���ؽ��� ����)
	size_t _splitVertexCount = 0;							// ������ ���ؽ��� ����

	size_t _resize_VertexIndex = m_pMesh->meshVertex_V.size();

	/// ���ؽ��� ������ ���� ��������
	bool _createNew = false;

	/// �޽��� ���̽�����Ʈ�� �������� * 3��ŭ ����.
	// ���� �����ϴ¸�ŭ 
	for (unsigned int i = 0; i < _faceCount; i++)
	{
		// �鿡�� ���ؽ� 3�� ���� 
		for (int j = 0; j < 3; j++)
		{
			// ���̽��� ���� * �ε���[0~2] ���� ���� optVertex�� ���� 
			int _vertexIndexMakeUpFace = m_pMesh->meshFace_V[i]->vertexIndex_A[j];
			/// _pNowVertex�� ������ ����ȭ ���Ѿ��� ���ؽ�
			JMParserData::Vertex* _pNowVertex = m_pMesh->optVertex_V[_vertexIndexMakeUpFace];

			/// //////////////////////////////////////
			/// �̹��� ������ ���ؽ��� �븻���� ���ٸ�(= �� ������ ó�� ������ optVertex��. �������� �Ȱ���.)
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
			/// ���ؽ��� ���ø����� ���� �����ϴ� �κ�(�븻�� UV�ϳ��� �ٸ��� false)
			/// //////////////////////////////////////

			/// 1. �̹��� ������ ���ؽ��� �븻���� �ְ�
			//if (_pNowVertex->isNormalSet == true)
			{
				// ���� ������ ���� normal[0~2]�� (Face�� Normal[0~2])
				// ���� ���ؽ��� ���� (Vertex�� Normal)
				// ������ �Ǵ�
				/// �׷��� �븻�� ��ġ���� �ʴ´ٸ�(�����ִ� �븻������ �ٸ����̴�! ���ؽ��� �÷�����)
				if (JMParserData::Float3Func(m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->normal)
					!= m_pMesh->meshFace_V[i]->vertexNormal_A[j])
				{
					_createNew = true;
				}
			}

			/// 2. �̹��� ������ ���ؽ��� UV�� �ְ�
			//if (_pNowVertex->isUVSet == true)
			{
				/// �׷��� UV�� ��ġ���� �ʴ´ٸ�
				if ((m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->u != m_pMesh->meshFace_V[i]->vertexUV_A[j].x) ||
					(m_pMesh->optVertex_V[_vertexIndexMakeUpFace]->v != m_pMesh->meshFace_V[i]->vertexUV_A[j].y))
				{
					_createNew = true;
				}
			}

			if (_createNew)
			{
				/// 3. ���θ��� ���ؽ��� ��������(��������)
				//if (_splitVertexCount > 0)
				if (_resize_VertexIndex > m_pMesh->meshVertex_V.size())
					//if (_vertexCount > m_pMesh->meshVertex_V.size())
				{
					/// 4. ���ø��� ���ؽ��� ������ŭ ���鼭 �˻縦 ���ش�.
					//for (size_t k = 0; k < _splitVertexCount; k++)
					for (unsigned int k = m_pMesh->meshVertex_V.size(); k < _resize_VertexIndex; k++)
						//for (size_t k = m_pMesh->meshVertex_V.size(); k < _vertexCount; k++)
					{
						/// 5. ���ø��� ���ؽ��� �ε����� �븻��, ���� ���� ���� face�� vertex �ε����� �븻�� ����
						//if (m_pMesh->optVertex_V[_vertexCount + k]->index == m_pMesh->meshFace_V[i]->vertexIndex_A[j] &&
						//	JMParserData::Float3Func(m_pMesh->optVertex_V[_vertexCount + k]->normal) == m_pMesh->meshFace_V[i]->vertexNormal_A[j])
						if (m_pMesh->optVertex_V[k]->index == m_pMesh->meshFace_V[i]->vertexIndex_A[j] &&
							JMParserData::Float3Func(m_pMesh->optVertex_V[k]->normal) == m_pMesh->meshFace_V[i]->vertexNormal_A[j])
						{
							/// 6. ���ø��� ���ؽ��� UV��, ���� ���� ���� face�� vertex�� UV�� ���ٸ�
							//if (m_pMesh->optVertex_V[_vertexCount + k]->u == m_pMesh->meshFace_V[i]->vertexUV_A[j].x &&
							//	m_pMesh->optVertex_V[_vertexCount + k]->v == m_pMesh->meshFace_V[i]->vertexUV_A[j].y)
							if (m_pMesh->optVertex_V[k]->u == m_pMesh->meshFace_V[i]->vertexUV_A[j].x &&
								m_pMesh->optVertex_V[k]->v == m_pMesh->meshFace_V[i]->vertexUV_A[j].y)
							{
								/// �ش� Face ������ �����ϴ� �ε��� ���۰��� �־��ش�.
								//m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(_vertexCount + k);
								m_pMesh->meshFace_V[i]->vertexIndex_A[j] = static_cast<int>(k);
								_createNew = false;	// �� ������ �ʴ´�.
								break;
							}
						}
					}
				}
			}

			/// //////////////////////////////////////
			/// ���ؽ��� ���ø��ϴ� �κ�
			/// //////////////////////////////////////
			if (_createNew)
			{
				// ���� ��ġ���� ���� �ε��� ���۸� ����ϴ� ���۸� ���� �����.
				// �׸��� ���ؽ��� Normal���� face�� vertexNormal(���ؽ��� �븻)�� ��ġ�ϵ��� ���ش�.
				JMParserData::Vertex* _pVertex = new JMParserData::Vertex;
				_pVertex->pos = _pNowVertex->pos;
				_pVertex->index = _pNowVertex->index;

				_pVertex->normal = m_pMesh->meshFace_V[i]->vertexNormal_A[j];
				_pVertex->isNormalSet = true;

				_pVertex->tangent = _pNowVertex->tangent;

				_pVertex->u = m_pMesh->meshFace_V[i]->vertexUV_A[j].x;
				_pVertex->v = m_pMesh->meshFace_V[i]->vertexUV_A[j].y;
				_pVertex->isUVSet = true;


				//��Ű��
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

// ���ؽ� ���ø�
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

		// ��ġ���� �ε����� �־��ش�.
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
	size_t _vSplitCount = 0;							// ������ ���ؽ��� ����
	//size_t _nowVCount = (int)m_pMesh->meshVertex_V.size();	// <- //size_t _nowVertexCount = m_pMesh->meshVertex_V.size();

	// ���� Face���� �����ϴ� 3���� Vertex ��..
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

			// �ؽ�ó�� �ְ�, �������� �ʾ����� �ؽ�ó u,v ����..
			if (_nowVertex->isUVSet == false)
			{
				_nowVertex->u = _nowUV.x;
				_nowVertex->v = _nowUV.y;
				_nowVertex->isUVSet = true;
			}

			// ���� �ε��� �븻�� �˻�ÿ� �־��ְ� ����..
			if (_nowVertex->isNormalSet == false)
			{
				_nowVertex->normal = _nowNormal;
				_nowVertex->isNormalSet = true;
			}

			// Normal, U, V ���� �Ѱ��� �ٸ��� Vertex ���� ����..
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
				// �߰��� Vertex�� �ִٸ� üũ..
				if (_vCount + _vSplitCount > _vCount)
				{
					for (size_t k = 0; k < _vSplitCount; k++)
					{
						JMParserData::Vertex* _checkVertex = m_pMesh->optVertex_V[k];

						// ���� �߰��� Vertex�� ������ �����͸� �����ִ� Face ���� Vertex Index ����..
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

			// ���� �߰��ؾ��� Vertex..
			if (_createNew == true)
			{
				JMParserData::Vertex* _newVertex = new JMParserData::Vertex();
				_newVertex->pos = _nowVertex->pos;
				_newVertex->index = _nowVertex->index;
				_newVertex->normal = _nowNormal;				// ������ �ڵ�

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

	// Tanget �� ����..
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
	//	// ���� �ȼ� ���̴����� ������ ���� T, B, N�� ��µ�
	//	// �ȼ� ���̴� ���� ������ ���ؽ� ���̴��� ���꿡 ���� ȣ�� Ƚ���� ������ �ٸ��� ũ�Ƿ� ���ϰ� �� �� �ִ�..
	//	// �������� �ȼ��� ���� �ȼ� ���̴� ���� �ƴϸ� ������ ���� ������ �ȼ� ���̴����� ������ �Ѵٰ� �Ѵ�..
	//	/// ���� ��������� ���ؽ� ���̴��� �Űܵ�
	//	DirectX::SimpleMath::Vector3 _tangent = (_ep1 * _uv2.y - _ep2 * _uv1.y) * den;
	//	_tangent.Normalize();
	//
	//	// ���� ������ ���� �����Ͽ� ���̴����� ��հ��� ����ϵ��� ����..
	//	_vertex0->tangent = _vertex0->tangent + _tangent;
	//	_vertex1->tangent = _vertex1->tangent + _tangent;
	//	_vertex2->tangent = _vertex2->tangent + _tangent;
	//}

	// �ε����� �׳� ����
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

	// Vertex ������ŭ Vertex ����..
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

		// Bounding Box üũ�� Position
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

				// FBX�� Bone Mesh Data �� �����Ƿ� ����� Mesh Vertex ���Ͽ� BoundingBox ����..
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
	// �޾ƿ� ����� ����TM�� �������ش�(yz�� ���� �����̶�� ����)
	DirectX::SimpleMath::Matrix _world = ConvertMatrix(_worldpos);
	DirectX::SimpleMath::Matrix _local = ConvertMatrix(_localpos);
	DirectX::SimpleMath::Matrix _newMat2 = ConvertMatrix(_newMat);
	DirectX::SimpleMath::Matrix _newMatGeom2 = ConvertMatrix(_newMatGeom);

	switch (node->GetNodeAttribute()->GetAttributeType())
	{
	case FbxNodeAttribute::eSkeleton:
	{
		// ��Ʈ ���� ó�����ش�?
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
	// �޾ƿ� ����� ����TM�� �������ش�(yz�� ���� �����̶�� ����)
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

	//��尡 ����� �Ž��Ŀ� ���� �־��ִ°� �ٸ�
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

	// 2��, 3�� ����..
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

	// 2��, 3�� ����..
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

