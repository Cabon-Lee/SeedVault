#pragma once

#include "IHelperClass.h"
#include "SimpleMath.h"
#include "VertexHelper.h"

struct NodeData;
struct ID3D11Device;
struct ID3D11Buffer;

// ��Ʈ �������� �ٲ㺸��
const unsigned int WHITE = 0;
const unsigned int RED = 1;
const unsigned int GREEN = 2;
const unsigned int BLUE = 3;
const unsigned int DARKGRAY = 4;

class DynamicFace : public IHelperClass
{
public:
	DynamicFace();
	DynamicFace(
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2,
		const DirectX::SimpleMath::Vector3& v3,
		const DirectX::SimpleMath::Vector3& normal);
	virtual ~DynamicFace();

	virtual void BuildHelper(ID3D11Device* pDevice) override;
	virtual void HelperRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj) override;

	// ���̾� �����Ӱ� �ָ���� �����ؼ� �׺�޽��� ä�� �� �ְ� �Ѵ�
	void DynamicRender(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex,
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		bool isWire,
		const DirectX::SimpleMath::Vector3& v1,
		const DirectX::SimpleMath::Vector3& v2,
		const DirectX::SimpleMath::Vector3& v3,
		const DirectX::SimpleMath::Vector3& normal,
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj,
		const unsigned int color);

private:
	// ���Ե� �ٽ� ���� ���� DeviceContext�� �ʿ��ϴ�
	void RebuildBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex);

	std::vector<Vertex::VertexColorNormal> m_Vertices;
	DirectX::SimpleMath::Vector3 m_Vertices_Arr[3];
	DirectX::SimpleMath::Vector3 m_Normal;
	DirectX::SimpleMath::Vector4 m_Color;
};

