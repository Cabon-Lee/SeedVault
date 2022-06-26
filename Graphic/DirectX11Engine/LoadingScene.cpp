#include "LoadingScene.h"
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include "Renderer.h"

#include "Shaders.h"
#include "InputLayout.h"

LoadingScene::~LoadingScene()
{
	delete m_VertexShader;
	m_VertexShader = nullptr;

	delete m_pixelShader;
	m_pixelShader = nullptr;
}

void LoadingScene::SetImageAndFps(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::string& fileName, const std::string& vertexShaderName, const std::string& pixelShaderName, float fps)
{
	m_fps = fps;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _tempBitmap;
	Microsoft::WRL::ComPtr<ID3D11Resource> _texResource;
	
	std::wstring _FileName;
	_FileName.assign(fileName.begin(), fileName.end());

	HRESULT _hr = DirectX::CreateWICTextureFromFile(pDevice.Get(), _FileName.c_str(), &_texResource, &_tempBitmap);

	std::wstring _vertexShaderName;
	_vertexShaderName.assign(vertexShaderName.begin(), vertexShaderName.end());

	std::wstring _pixelShaderName;
	_pixelShaderName.assign(pixelShaderName.begin(), pixelShaderName.end());

	m_VertexShader = new VertexShader();
	m_pixelShader = new PixelShader();

	m_VertexShader->Initialize(pDevice, _vertexShaderName);
	
	InputLayout::CrateInputLayoutFromVertexShader(pDevice.Get(), m_VertexShader->GetVertexShaderBuffer(), &m_VertexShader->inputLayout);

	m_pixelShader->Initialize(pDevice, _pixelShaderName);

	m_ImageInfo_V.push_back(_tempBitmap);

	m_worldTM =
		DirectX::XMMatrixScaling(1920.0f, 1080.0f, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f + DirectX::XM_PI, 0.f + DirectX::XM_PI) *
		DirectX::XMMatrixTranslation(1920.0f / 2, 1080.0f / 2, 0.f);

}

void LoadingScene::Draw(Renderer* renderer)
{
	renderer->DrawSprite(m_ImageInfo_V[0], m_worldTM, m_VertexShader, m_pixelShader);
}
