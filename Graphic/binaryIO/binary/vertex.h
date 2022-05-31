#pragma once

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

#pragma pack(push, 1)
struct pack1
{
	int a;
	char b;
	float c;
};
#pragma pack(pop)

struct notpack
{
	int a;
	char b;
	float c;
};

struct Vertex
{
	Vertex()
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;

		normal.x = 0;
		normal.y = 0;
		normal.z = 0;

		texCoord.x = 0;
		texCoord.y = 0;
	}

	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;

		texCoord.x = u;
		texCoord.y = v;

		normal.x = nx;
		normal.y = ny;
		normal.z = nz;
	}

	Float3 pos;
	Float3 normal;
	Float2 texCoord;
};

struct VertexSkinned
{
public:
	VertexSkinned()
	{
		pos.x = 0;
		pos.y = 0;
		pos.z = 0;

		texCoord.x = 0;
		texCoord.y = 0;

		normal.x = 0;
		normal.y = 0;
		normal.z = 0;

		weights.x = 0;
		weights.y = 0;
		weights.z = 0;

		boneIndices[0] = 0;
		boneIndices[1] = 0;
		boneIndices[2] = 0;
		boneIndices[3] = 0;
	};

	VertexSkinned(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v,
		float w1, float w2, float w3,
		unsigned int indx1, unsigned int indx2, unsigned int indx3, unsigned int indx4)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;

		texCoord.x = u;
		texCoord.y = v;

		normal.x = nx;
		normal.y = ny;
		normal.z = nz;

		weights.x = w1;
		weights.y = w2;
		weights.z = w3;

		boneIndices[0] = indx1;
		boneIndices[1] = indx2;
		boneIndices[2] = indx3;
		boneIndices[3] = indx4;
	};

	Float3 pos;
	Float2 texCoord;
	Float3 normal;
	Float3 weights;
	unsigned int boneIndices[4];
};
