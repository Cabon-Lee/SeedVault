#pragma once

#include "IHelperClass.h"
#include "VertexHelper.h"
#include "SimpleMath.h"

struct NodeData;
struct ID3D11Device;
struct ID3D11Buffer;

class DynamicLine : public IHelperClass
{
public:
	DynamicLine();

	DirectX::SimpleMath::Vector3 m_Origin;
	DirectX::SimpleMath::Vector3 m_Target;

	virtual void BuildHelper(ID3D11Device* pDevice) override;
	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj) override;

	// 그리기 위한 행렬 외에 실제 Geometry의 바뀐 값이 필요하다
	void DynamicRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::SimpleMath::Vector3& origin,
		const DirectX::SimpleMath::Vector3& target,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj);

private:
	// 고맙게도 다시 만들 때는 DeviceContext만 필요하다
	void RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex);
	std::vector<Vertex::VertexColor> m_Vertices;
	
};

