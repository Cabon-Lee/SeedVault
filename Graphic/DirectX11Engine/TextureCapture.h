#pragma once

#include "D3D11Define.h"

class TextureCapture
{
public:
	TextureCapture();
	~TextureCapture();


	

	static void SaveTextureToDDSFile(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr < ID3D11Resource> pTexture, LPCWSTR filename);



};

