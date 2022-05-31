#pragma once

#include "IHelperClass.h"
#include "SimpleMath.h"
#include "VertexHelper.h"

struct NodeData;
struct ID3D11Device;
struct ID3D11Buffer;

class DynamicBox : public  IHelperClass
{
public:
	DynamicBox();
	DynamicBox(const DirectX::SimpleMath::Vector3& size);
	virtual ~DynamicBox();


	DirectX::SimpleMath::Vector3 m_Shape;

	virtual void BuildHelper(ID3D11Device* pDevice) override;
	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj) override;

	// �׸��� ���� ��� �ܿ� ���� Geometry�� �ٲ� ���� �ʿ��ϴ�
	void DynamicRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::SimpleMath::Vector3& shape,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj);

private:
	// ���Ե� �ٽ� ���� ���� DeviceContext�� �ʿ��ϴ�
	void RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex);

	// ������ �� �� ��Ȱ���ϱ� ����
	std::vector<Vertex::VertexColor> m_Vertices;

};

