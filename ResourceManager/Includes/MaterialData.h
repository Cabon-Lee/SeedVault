#pragma once

/// PixelShader에서 쓰기 위해 사용하는 머트리얼
/// 텍스쳐, 픽셀 쉐이더가 들어간다
/// 픽셀 쉐이더가 들어가는 이유는 각 모델마다 별도의 쉐이더로 돌려야할 수 있기 때문
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

	// 텍스쳐 타입(알베도, 노말맵), 실제 들어갈 이름
	std::vector<std::string> textureName_V;

	DirectX::SimpleMath::Vector3 diffuse;
	DirectX::SimpleMath::Vector3 emissive;
	DirectX::SimpleMath::Vector3 ambient;
	DirectX::SimpleMath::Vector3 fresnelR0;			// 베이스 컬러(반사에 사용됨)
	DirectX::SimpleMath::Vector3 specular;

	//bool isTransparency;

	float transparentFactor;
	float specularPower;
	float shineness;			// 블렌더 상 Roughness를 의미 
	float reflactionFactor;
};