#include "TextureCapture.h"

#include "DirectXTex.h"

#include "COMException.h"
#include "ErrorLogger.h"

TextureCapture::TextureCapture()
{

}

TextureCapture::~TextureCapture()
{

}

void TextureCapture::SaveTextureToDDSFile(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice, 
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr<ID3D11Resource> pTexture, LPCWSTR filename)
{
	DirectX::ScratchImage image;

	HRESULT _hr;

	try
	{
		_hr = DirectX::CaptureTexture(pDevice.Get(), pDeviceContext.Get(), pTexture.Get(), image);
		COM_ERROR_IF_FAILED(_hr, "Capture Texture Fail");

		_hr = DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DDS_FLAGS_NONE, filename);
		COM_ERROR_IF_FAILED(_hr, "SaveToDDSFail Fail");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
	
}
