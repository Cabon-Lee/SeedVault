#pragma once

/// PixelShader���� ���� ���� ����ϴ� ��Ʈ����
/// �ؽ���, �ȼ� ���̴��� ����
/// �ȼ� ���̴��� ���� ������ �� �𵨸��� ������ ���̴��� �������� �� �ֱ� ����
#include <string>
#include <map>
#include "SimpleMath.h"


const unsigned int DIFFUSE_TEXUTRE = 0;
const unsigned int BUMP_TEXUTRE = 1;
const unsigned int SHININESS_TEXUTRE = 2;
const unsigned int EMISSIVE_TEXUTRE = 3;
const unsigned int SPECULAR_TEXUTRE = 4;
const unsigned int REFLECTION_TEXUTRE = 5;
const unsigned int TRANSPARENT_TEXUTRE = 6;
const unsigned int DISPLACEMENT_TEXUTRE = 7;


struct MaterialDesc
{
	MaterialDesc()
		: reflactionFactor(0)
		, shineness(0)
		, specularPower(0)
		, transparentFactor(0)
	{
		textureName_V.resize(8);
	};

	std::string materialName;

	// �ؽ��� Ÿ��(�˺���, �븻��), ���� �� �̸�
	std::vector<std::string> textureName_V;

	DirectX::SimpleMath::Vector3 diffuse;
	DirectX::SimpleMath::Vector3 emissive;
	DirectX::SimpleMath::Vector3 ambient;
	DirectX::SimpleMath::Vector3 fresnelR0;			// ���̽� �÷�(�ݻ翡 ����)
	DirectX::SimpleMath::Vector3 specular;

	//bool isTransparency;

	float transparentFactor;
	float specularPower;
	float shineness;			// ���� �� Roughness�� �ǹ� 
	float reflactionFactor;
};