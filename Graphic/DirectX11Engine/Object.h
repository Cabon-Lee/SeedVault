#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <string>

struct SHADER_MATERIAL;
class Transform;
class MeshOIT;
class VShader;
class PShader;

__interface IResourceManager;
class VertexShader;
class PixelShader;

class Object
{
public:
	/*

	Object(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::string name, 
		std::shared_ptr<MeshOIT> shape, 
		std::string sVS, 
		const D3D11_INPUT_ELEMENT_DESC* iLayouts, 
		UINT layoutCount, 
		std::string sHS, 
		std::string sDS, 
		std::string sGS, 
		std::string sPS, 
		bool directRender = true);
	Object(
		Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
		std::string name, 
		std::shared_ptr<MeshOIT> shape,  
		ID3D11ShaderResourceView* diffSRV, 
		ID3D11ShaderResourceView* normalSRV, 
		bool directRender = true);
	virtual ~Object();

	virtual void Update();
	virtual void Render(
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
		const DirectX::XMMATRIX& v, 
		const DirectX::XMMATRIX& p, 
		UINT sceneDepth = 0) const;
	virtual void RenderGeom(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext) const;

	void SetEnabled(bool e) { enabled = e; }
	void SetShow(bool s) { show = s; }

	int Layer()const;
	void SetLayer(int l);
	void GetMaterial(SHADER_MATERIAL* pMaterial);
	void GetMainTex(ID3D11ShaderResourceView** pSRV);
	void GetNormal(ID3D11ShaderResourceView** pNormal);
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	std::shared_ptr<IResourceManager> m_pResourceManager;

	const std::string name;
	Transform* transform;
	std::shared_ptr <MeshOIT> mesh;
	
	//VShader* vs;
	//PShader* ps;
	
	std::shared_ptr<VertexShader> m_vs;
	std::shared_ptr<PixelShader> m_ps;

	ID3D11BlendState* m_dxBlendState;
	ID3D11RasterizerState* m_dxRState;
	ID3D11DepthStencilState* m_dxDSState;
	
	SHADER_MATERIAL* m_material;
	ID3D11ShaderResourceView* m_mainTex;
	ID3D11ShaderResourceView* m_normal;


protected:
	Object();
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)const;
	*/

	bool enabled = true;
	bool show = true;
	int layer;

};