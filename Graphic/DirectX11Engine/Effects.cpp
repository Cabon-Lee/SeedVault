//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename, std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HRESULT hr = D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device, &mFX);
	COM_ERROR_IF_FAILED(hr, "이펙트 클래스를 메모리로부터 생성하는데 실패했습니다");
}

Effect::~Effect()
{
	//ReleaseCOM(mFX);
	if (mFX)
	{
		mFX->Release();
		mFX = 0;
	}
}
#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

LightTexEffect::LightTexEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");


	/// 텍스쳐
	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

LightTexEffect::~LightTexEffect()
{
}

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{

}
#pragma endregion

#pragma region Effects

BasicEffect* Effects::BasicFX = 0;
LightTexEffect* Effects::BasicTexFX = 0;
SkyEffect* Effects::SkyFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	std::wstring shaderfolder = L"";;
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG
#ifdef _WIN64
		shaderfolder = L"../x64/Debug/";
#else
		shaderfolder = L"../Debug/";
#endif
#else
#ifdef _WIN64
		shaderfolder = L"../x64/Release/";
#else
		shaderfolder = L"../Release/";
#endif
#endif
	}

	BasicFX = new BasicEffect(device, shaderfolder + L"Basic.cso");
	BasicTexFX = new LightTexEffect(device, shaderfolder + L"BasicTex.cso");
	SkyFX = new SkyEffect(device, shaderfolder + L"SkySphere.cso");
#pragma endregion
}

void Effects::DestroyAll()
{
	//SafeDelete(BasicFX);
	//SafeDelete(BasicTexFX);

	delete BasicFX;
	BasicFX = 0;
	delete BasicTexFX;
	BasicTexFX = 0;
	delete SkyFX;
	SkyFX = 0;
}
#pragma endregion