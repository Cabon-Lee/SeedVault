#pragma once
#include "IHelperClass.h"

class VertexShader;
class PixelShader;

class Quad :
    public IHelperClass
{
public:
	Quad();
	virtual ~Quad();

	virtual void BuildHelper(ID3D11Device* pDevice) override;
	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj) override;


};

