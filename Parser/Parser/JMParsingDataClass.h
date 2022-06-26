#pragma once
#include <vector>
#include <memory>
#include <string>

#include "SimpleMath.h"
#include "BoneWeights.h"

struct KeyFrame;

namespace JMParserData
{
	struct Float2
	{
		float x;
		float y;
	};

	struct Float3
	{
		float x;
		float y;
		float z;
	};

	struct Float4
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct Float2Func : public Float2
	{
		Float2Func() {};
		Float2Func(const Float2& origin);
		Float2Func(float f1, float f2);
		Float2& operator=(const Float2& vec2);
		Float2 operator+(Float2& vec2);
		Float2 operator-(Float2& vec2);
		bool operator==(Float2& vec2);
		bool operator!=(Float2& vec2);
	};

	struct Float3Func : public Float3
	{
		Float3Func() {};
		Float3Func(const Float3& origin);
		Float3Func(float f1, float f2, float f3);
		Float3& operator=(const Float3& vec3);
		Float3 operator+(Float3& vec3);
		Float3 operator-(Float3& vec3);
		bool operator==(Float3& vec3);
		bool operator!=(Float3& vec3);
	};

	struct Float4Func : public Float4 
	{
		Float4Func() {};
		Float4Func(const Float4& origin);
		Float4Func(float f1, float f2, float f3, float f4);
		Float4& operator=(const Float4& vec3);
		Float4& operator+=(const Float4& vec3);
		Float4 operator+(Float4& vec3);
		Float4 operator-(Float4& vec3);
		bool operator==(Float4& vec3);
		bool operator!=(Float4& vec3);
	};


	///-----------------------------------------
	/// 버텍스
	///-----------------------------------------
	struct Vertex
	{
		Vertex();
		~Vertex();

		Float3		pos;				// 좌표상의 위치값
		Float3		normal;				// 노말값
		Float4		tangent;			// 탄젠트
		Float3		binormal;			// 바이 노말

		// Skinned Mesh 에서 추가
		float boneWeight_Arr[4];
		UINT boneIndex_Arr[4];

		int index;							// 인덱스
		float u, v;							// 텍스쳐 좌표

		bool isNormalSet;					// 노말값이 셋팅됐는가?
		bool isBinormalSet;					// 바이 노말값이 셋팅됐는가?
		bool isTangentSet;					// 탄젠트값이 셋팅됐는가?
		bool isUVSet;						// 텍스쳐값이 셋팅됐는가?
	};

	///-----------------------------------------
	/// 인덱스 리스트
	///-----------------------------------------
	struct IndexList
	{
		IndexList();

		int index_A[3];
	};

	struct Face
	{
		Face();

		/// Face를 이루는 3개의 Vertex의 정보
		int			vertexIndex_A[3];		// 이 Face를 이루는 Vertex의 인덱스
		Float3		vertexNormal_A[3];		// Vertex의 Normal값	
		Float2		vertexUV_A[3];			// Vertex의 UV값
		Float3		vertexBinormal_A[3];	// Vertex의 Binormal값
		Float3		vertexTangent_A[3];		// Vertex의 Tangent값
	};

	///-----------------------------------------
	/// 
	///-----------------------------------------
	struct Bone
	{
		Bone();
		~Bone()
		{

		}

		std::string		boneName;								/// ☆
		int				boneIndex;								/// ☆

		/// 바이너리에 안넘겨줄 데이터
		Bone* pParentBone;
		std::string		parentBoneName;
		int				parentBoneIndex;						/// ☆

		bool			isBoneSet;
		bool			isParentExist;

		/// child Mesh 정보
		std::vector<Bone*> childBoneList_V;				// 자식노드 목록
		/// 바이너리에 넘겨줄 데이터
		std::vector<int> childBoneIndex_V;

		DirectX::SimpleMath::Vector3	minmum;
		DirectX::SimpleMath::Vector3	maximum;

		/// NodeTM
		DirectX::SimpleMath::Vector3	tmRow0;
		DirectX::SimpleMath::Vector3	tmRow1;
		DirectX::SimpleMath::Vector3	tmRow2;
		DirectX::SimpleMath::Vector3	tmRow3;


		DirectX::SimpleMath::Matrix		bindPoseTransform;		/// ☆
		DirectX::SimpleMath::Matrix		boneReferenceTransform;

		DirectX::SimpleMath::Matrix		worldTM;				/// ☆
		DirectX::SimpleMath::Matrix		localTM;				/// ☆

	};

	///-----------------------------------------
	/// 본들을 갖고 있는 스켈레톤
	///-----------------------------------------
	struct Skeleton
	{
		Skeleton() {};
		~Skeleton();

		std::vector<JMParserData::Bone*> m_Bones;
		std::vector<DirectX::SimpleMath::Matrix> m_BoneOffsets;

		void AddBone(JMParserData::Bone* bone)
		{
			m_Bones.push_back(bone);
		}

		JMParserData::Bone* FindBone(const std::string& boneName)
		{
			for (JMParserData::Bone* _now : m_Bones)
			{
				if (_now->boneName == boneName)
				{
					return _now;
				}
			}
			return nullptr;
		}

		int FindBoneIndex(const std::string& boneName)
		{
			for (int i = 0; i < m_Bones.size(); i++)
			{
				if (m_Bones[i]->boneName == boneName)
				{
					return i;
				}
			}
			return -1;
		}
	};


	///-----------------------------------------
	/// 매쉬
	///-----------------------------------------
	struct Mesh
	{
		Mesh();
		~Mesh();

		std::string nodeName;						// 노드이름
		std::string materialName;					// 머트리얼 이름

		/// 처음 읽어들인 vertex와 Face의 index 정보
		std::vector<Vertex*> meshVertex_V;			// 매쉬의 p버텍스들
		std::vector<Face*> meshFace_V;				// 매쉬의 p페이스들

		int	meshNumIndex;							// 매쉬의 인덱스 갯수
		int	meshNumFace;							// 매쉬의 페이스 갯수

		// 최적화(버텍스 스플릿)
		std::vector<Vertex*> optVertex_V;			// 최적화 후의 p버텍스들
		IndexList* pOptIndex;						// 최적화 후의 인덱스 리스트 포인터
		//std::vector<IndexList*> pOptIndex_V;
		
		int	optIndexNum;							// 최적화 후의 인덱스개수

		// 하이어라키
		std::string nodeParentName;					// 부모 노드 이름
		bool isParentExist;							// 부모 노드가 존재하는지
		std::shared_ptr <Mesh> pNodeParentMesh;						// 부모노드 포인터
		std::vector< std::shared_ptr<Mesh>> childList_V;				// 자식노드 목록

		std::vector<Float3> m_OriginVertexList;
		std::vector<int> m_OriginIndexList;	

		// 매쉬만 있을경우 부모자식관계를 알기위한 인덱스
		int meshIndex;
		
		// 스키닝 정보
		bool isSkinningObject;
		
		// NodeTM
		DirectX::SimpleMath::Vector3	tmRow0;
		DirectX::SimpleMath::Vector3	tmRow1;
		DirectX::SimpleMath::Vector3	tmRow2;
		DirectX::SimpleMath::Vector3	tmRow3;

		DirectX::SimpleMath::Matrix		worldTM;	// Mesh 의 WorldTM
		DirectX::SimpleMath::Matrix		localTM;	// Mesh 의 LocalTM



	};
}
