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


	///-----------------------------------------
	/// ���ؽ�
	///-----------------------------------------
	struct Vertex
	{
		Vertex();
		~Vertex();

		Float3		pos;				// ��ǥ���� ��ġ��
		//Float2		tex;
		Float3		normal;				// �븻��
		Float3		tangent;			// ź��Ʈ
		Float3		binomral;			// ���� �븻

		// Skinned Mesh ���� �߰�
		float boneWeight_Arr[4];
		UINT boneIndex_Arr[4];

		int index;							// �ε���
		float u, v;							// �ؽ��� ��ǥ

		bool isNormalSet;					// �븻���� ���õƴ°�?
		bool isBinormalSet;					// ���� �븻���� ���õƴ°�?
		bool isTangentSet;					// ź��Ʈ���� ���õƴ°�?
		bool isUVSet;						// �ؽ��İ��� ���õƴ°�?
	};

	///-----------------------------------------
	/// �ε��� ����Ʈ
	///-----------------------------------------
	struct IndexList
	{
		IndexList();

		int index_A[3];
	};

	struct Face
	{
		Face();

		/// Face�� �̷�� 3���� Vertex�� ����
		int			vertexIndex_A[3];		// �� Face�� �̷�� Vertex�� �ε���
		Float3		vertexNormal_A[3];		// Vertex�� Normal��	
		Float2		vertexUV_A[3];			// Vertex�� UV��
		Float3		vertexBinormal_A[3];	// Vertex�� Binormal��
		Float3		vertexTangent_A[3];		// Vertex�� Tangent��
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

		std::string		boneName;								/// ��
		int				boneIndex;								/// ��

		/// ���̳ʸ��� �ȳѰ��� ������
		Bone* pParentBone;
		std::string		parentBoneName;
		int				parentBoneIndex;						/// ��

		bool			isBoneSet;
		bool			isParentExist;

		/// child Mesh ����
		std::vector<Bone*> childBoneList_V;				// �ڽĳ�� ���
		/// ���̳ʸ��� �Ѱ��� ������
		std::vector<int> childBoneIndex_V;

		DirectX::SimpleMath::Vector3	minmum;
		DirectX::SimpleMath::Vector3	maximum;

		/// NodeTM
		DirectX::SimpleMath::Vector3	tmRow0;
		DirectX::SimpleMath::Vector3	tmRow1;
		DirectX::SimpleMath::Vector3	tmRow2;
		DirectX::SimpleMath::Vector3	tmRow3;


		DirectX::SimpleMath::Matrix		bindPoseTransform;		/// ��
		DirectX::SimpleMath::Matrix		boneReferenceTransform;

		DirectX::SimpleMath::Matrix		worldTM;				/// ��
		DirectX::SimpleMath::Matrix		localTM;				/// ��

	};

	///-----------------------------------------
	/// ������ ���� �ִ� ���̷���
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
	/// �Ž�
	///-----------------------------------------
	struct Mesh
	{
		Mesh();
		~Mesh();

		std::string nodeName;						// ����̸�
		std::string materialName;					// ��Ʈ���� �̸�

		/// ó�� �о���� vertex�� Face�� index ����
		std::vector<Vertex*> meshVertex_V;			// �Ž��� p���ؽ���
		std::vector<Face*> meshFace_V;				// �Ž��� p���̽���

		int	meshNumIndex;							// �Ž��� �ε��� ����
		int	meshNumFace;							// �Ž��� ���̽� ����

		// ����ȭ(���ؽ� ���ø�)
		std::vector<Vertex*> optVertex_V;			// ����ȭ ���� p���ؽ���
		IndexList* pOptIndex;						// ����ȭ ���� �ε��� ����Ʈ ������
		//std::vector<IndexList*> pOptIndex_V;
		
		int	optIndexNum;							// ����ȭ ���� �ε�������

		// ���̾��Ű
		std::string nodeParentName;					// �θ� ��� �̸�
		bool isParentExist;							// �θ� ��尡 �����ϴ���
		std::shared_ptr <Mesh> pNodeParentMesh;						// �θ��� ������
		std::vector< std::shared_ptr<Mesh>> childList_V;				// �ڽĳ�� ���

		std::vector<Float3> m_OriginVertexList;
		std::vector<int> m_OriginIndexList;	

		// �Ž��� ������� �θ��ڽİ��踦 �˱����� �ε���
		int meshIndex;
		
		// ��Ű�� ����
		bool isSkinningObject;
		
		// NodeTM
		DirectX::SimpleMath::Vector3	tmRow0;
		DirectX::SimpleMath::Vector3	tmRow1;
		DirectX::SimpleMath::Vector3	tmRow2;
		DirectX::SimpleMath::Vector3	tmRow3;

		DirectX::SimpleMath::Matrix		worldTM;	// Mesh �� WorldTM
		DirectX::SimpleMath::Matrix		localTM;	// Mesh �� LocalTM



	};
}
