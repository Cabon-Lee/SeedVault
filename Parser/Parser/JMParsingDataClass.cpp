#include "JMParsingDataClass.h"

#include "CATrace.h"
#include "DataRelease.h"

namespace JMParserData
{
	///-----------------------------------------
	/// Float2
	///-----------------------------------------
	Float2Func::Float2Func(const Float2& vec2)
	{
		this->x = vec2.x;
		this->y = vec2.y;
	}

	Float2Func::Float2Func(float f1, float f2)
	{
		this->x = f1;
		this->y = f2;
	}

	Float2& Float2Func::operator=(const Float2& vec2)
	{
		this->x = vec2.x;
		this->y = vec2.y;

		return *this;
	}

	Float2 Float2Func::operator+(Float2& vec2)
	{
		this->x += vec2.x;
		this->y += vec2.y;

		return *this;
	}

	Float2 Float2Func::operator-(Float2& vec2)
	{
		this->x -= vec2.x;
		this->y -= vec2.y;

		return *this;
	}

	bool Float2Func::operator==(Float2& vec2)
	{
		if (this->x == vec2.x && this->y == vec2.y)
		{
			return true;
		}
		return false;
	}

	bool Float2Func::operator!=(Float2& vec2)
	{
		if (this->x != vec2.x || this->y != vec2.y)
		{
			return true;
		}
		return false;
	}

	///-----------------------------------------
	/// Float3
	///-----------------------------------------
	Float3Func::Float3Func(const Float3& vec3)
	{
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;
	}

	Float3Func::Float3Func(float f1, float f2, float f3)
	{
		this->x = f1;
		this->y = f2;
		this->z = f3;
	}

	Float3& Float3Func::operator=(const Float3& vec3)
	{
		this->x = vec3.x;
		this->y = vec3.y;
		this->z = vec3.z;

		return *this;
	}

	Float3 Float3Func::operator+(Float3& vec3)
	{
		this->x += vec3.x;
		this->y += vec3.y;
		this->z += vec3.z;

		return *this;
	}

	Float3 Float3Func::operator-(Float3& vec3)
	{
		this->x -= vec3.x;
		this->y -= vec3.y;
		this->z -= vec3.z;

		return *this;
	}

	bool Float3Func::operator==(Float3& vec3)
	{
		if (this->x == vec3.x && this->y == vec3.y && this->z == vec3.z)
		{
			return true;
		}
		return false;
	}

	bool Float3Func::operator!=(Float3& vec3)
	{
		if (this->x != vec3.x || this->y != vec3.y || this->z != vec3.z)
		{
			return true;
		}
		return false;
	}

	///-----------------------------------------
	/// Float4x4
	///-----------------------------------------

	


	Vertex::Vertex()
		: pos()
		, normal()
		, binomral()
		, tangent()
		, index(0)
		, u(0.0f)
		, v(0.0f)
		, isNormalSet(false)
		, isBinormalSet(false)
		, isTangentSet(false)
		, isUVSet(false)
	{

	}

	Vertex::~Vertex()
	{
		CA_TRACE("���ؽ� ����");
	}

	IndexList::IndexList()
	{
		index_A[0] = 0;
		index_A[1] = 0;
		index_A[2] = 0;
	}

	Face::Face()
	{
		vertexIndex_A[0] = 0;
		vertexIndex_A[1] = 0;
		vertexIndex_A[2] = 0;

		vertexNormal_A[0] = Float3Func(0.0f, 0.0f, 0.0f);
		vertexNormal_A[1] = Float3Func(0.0f, 0.0f, 0.0f);
		vertexNormal_A[2] = Float3Func(0.0f, 0.0f, 0.0f);

		vertexUV_A[0] = Float2Func(0.0f, 0.0f);
		vertexUV_A[1] = Float2Func(0.0f, 0.0f);
		vertexUV_A[2] = Float2Func(0.0f, 0.0f);
	}

	Bone::Bone() :
		boneName(),
		boneIndex(0),
		pParentBone(nullptr),
		parentBoneName(),
		parentBoneIndex(-1),
		isBoneSet(false),
		isParentExist(false),
		minmum(0.0f, 0.0f, 0.0f),
		maximum(0.0f, 0.0f, 0.0f),
		tmRow0(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow1(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow2(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow3(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f))
	{
	}

	Skeleton::~Skeleton()
	{

	}



	Mesh::Mesh() :
		meshNumIndex(0),
		meshNumFace(0),
		pOptIndex(nullptr),
		optIndexNum(0),
		isParentExist(false),
		pNodeParentMesh(nullptr),
		isSkinningObject(false),
		tmRow0(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow1(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow2(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f)),
		tmRow3(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f))
	{

	}


	Mesh::~Mesh()
	{
		//CA_TRACE("Mesh ������");
		CA_TRACE("~�ס١� ������� �����(^��^)�� ���Ѵ�! ��Mesh��, ������!�� �ס١�~");
		
		VectorDataRelese(meshVertex_V);
		VectorDataRelese(meshFace_V);
		VectorDataRelese(optVertex_V);
		
		delete pOptIndex;
		pOptIndex = nullptr;
	}



}