#pragma once

// ������� �ϴµ� �ʿ��� �������� �����ִ� Ŭ����
// �����, �׸��带 �����Ѵ�. �������� �״�� ����Ǿ��ִ°� ���ζ�.
// ���⿡ ������ ���� ģ������ �߰��ϰ� ���ͷ� ������ ����
// radius�� üũ�ؼ� 
#include "D3D11Define.h"
#include "SimpleMath.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
class DynamicBox;
class DynamicSphere;
class DynamicFace;
class DynamicLine;

class Grid;
class Gizmo;
class DirectionalLightShape;
class PointLightShape;
class SpotLightShape;

class VertexShader;
class PixelShader;
class DebugShader;

enum class eHELPER_TYPE
{
	GRID = 0,
	GIZMO,
	BOX,
	SPHERE,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct LineDebugger
{
	LineDebugger(
		Microsoft::WRL::ComPtr<ID3D11Buffer> vb,
		Microsoft::WRL::ComPtr<ID3D11Buffer> ib,
		const DirectX::SimpleMath::Matrix& tm)
		: _vb(vb)
		, _ib(ib)
		, worldTM(tm) {}

	Microsoft::WRL::ComPtr<ID3D11Buffer> _vb;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _ib;
	DirectX::SimpleMath::Matrix worldTM;
};

class VisualDebugger
{
public:
	VisualDebugger();
	~VisualDebugger();

	void Initilze(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice, 
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContex);

	void DrawLine(
		const DirectX::SimpleMath::Vector3& origin,
		const DirectX::SimpleMath::Vector3& dir,
		float ditance,
		const DirectX::SimpleMath::Color& color,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

	void DrawLine(
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMFLOAT3& origin,
		const DirectX::XMFLOAT3& target);


	// �׸� Ÿ�� ����, PhsicsActor�� ������ �ε��� ��ȣ�� �ִ´�
	// Grid, Gizmo�� �ε��� ��ȣ�� ����
	void DebuggerRender(
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMFLOAT3& pos,
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM,
		eHELPER_TYPE type,
		const DirectX::SimpleMath::Vector3& size = 
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f));


	void DrawGrid(
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader,
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

	void DrawLine(
		std::shared_ptr<VertexShader> pVertexShader,
		std::shared_ptr<PixelShader> pPixelShader, 
		const DirectX::XMMATRIX& viewTM,
		const DirectX::XMMATRIX& projTM);

	void DrawLine(
		const DirectX::XMMATRIX& worldTM,
		const DirectX::XMFLOAT3& origin,
		const DirectX::XMFLOAT3& target,
		float r, float g, float b);

	void DrawSqure(
		const DirectX::XMMATRIX& worldTM,
		DirectX::SimpleMath::Vector3 m_Corners[]);
	
	std::shared_ptr<DynamicFace>& GetFaceRender();
	std::shared_ptr<DynamicLine>& GetLineRender();


private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;

	std::shared_ptr<DebugShader> m_pDebugShader;

private:
	std::shared_ptr<DynamicBox> m_DebugBox;
	std::shared_ptr<DynamicSphere> m_DebugSphere;
	std::shared_ptr<DynamicFace> m_DebugFace;
	std::shared_ptr<DynamicLine> m_DebugLine;

	std::shared_ptr<Grid> m_pGrid;
	std::shared_ptr<Gizmo> m_pGizmo;
	std::shared_ptr<DirectionalLightShape> m_pDirectionalLight;
	std::shared_ptr<PointLightShape> m_pPointLight;
	std::shared_ptr<SpotLightShape> m_pSpotLight;

	std::vector<std::pair<float, unsigned int>> m_Radius_V;		// ���� ũ��
	std::vector<std::pair<DirectX::SimpleMath::Vector3, unsigned int>> m_Vector3_V;	// �ڽ� 

	std::vector<std::shared_ptr<LineDebugger>> m_LineDebugger_V;

private:
	// ���� ũ�� ������ ����
	DirectX::XMMATRIX m_OrginSize;

};

