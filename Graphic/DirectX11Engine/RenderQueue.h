#pragma once

/// 2022.03.25
/// 실제 렌더링을 할 친구들을 모아두는 클래스
/// 목적
/// 1. 같은 리소스를 사용하는 데이터를 소팅하여 리소스 등록 오버헤드를 줄인다
/// 2. Frustum Culling을 통해 그릴 필요가 없는 리소스를 제외한다
/// 3. Opaque와 Transparent를 리소스를 구분한다


#include "D3D11Define.h"
#include <queue>

class Renderer;
struct NodeData;
struct SpriteData;

const unsigned int MAX_RENDER_NODEDATA = 10000;

// 실제 씬에 그려질 NodeData의 개수만큼 필요하다
struct RenderQueueElement
{
	unsigned int renderQueueIndex;

	class VertexShader* pVertexShader;
	class PixelShader* pPixelShader;
	unsigned int reflectionProbe;
	unsigned int objectID;			// 피킹 때문에

	int modelMeshIndex;
	bool isTransparent;					// 같은 메쉬라도 Opaque가 다르면 따로 넣어줘야함

	std::shared_ptr<NodeData> pNodeData;
	DirectX::XMMATRIX WorldTM;
	DirectX::XMMATRIX* pBoneOffsetTM;

	RenderQueueElement* pNext;
	RenderQueueElement* pPrev;		// 환형 링크드 리스트로 만든다

	bool isUsing;
};

// 이펙트의 경우에는 어차피 쿼드로만 사용될 것이다
// + 빌보드만 사용된다
// + 리플렉션 프로브를 사용하지 않고, 모든 이펙트가 Transparent하다
// 같은 이미지를 사용하는 이펙트를 동시에 활용하기 위해 링크드 리스트로 만든다
struct RenderQueueParticleElement
{
	unsigned int renderQueueIndex;

	class VertexShader* pVertexShader;
	class PixelShader* pPixelShader;
	unsigned int spriteIndex;

	DirectX::XMMATRIX WorldTM;

	RenderQueueParticleElement* pNext;
	RenderQueueParticleElement* pPrev;
};

struct InstancedData
{
	DirectX::XMMATRIX world;
	//DirectX::XMFLOAT4 f1;
	//DirectX::XMFLOAT4 f2;
	//DirectX::XMFLOAT4 f3;
	//DirectX::XMFLOAT4 f4;
};

class RenderQueue
{
public:
	RenderQueue();
	~RenderQueue();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	void AddRenderQueue(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		unsigned int reflectionProbe,
		unsigned int objectID,
		int modelIndex,
		bool isOpaque,
		std::shared_ptr<NodeData> pData,
		const DirectX::XMMATRIX& pWorldTM,
		DirectX::XMMATRIX* pBoneOFfsetTM);

	void AddRenderQueueParticle(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		unsigned int spriteIndex,
		std::shared_ptr<SpriteData> pSpriteData,
		const DirectX::XMMATRIX& pWorldTM);


	void CopyOutLineMesh(
		class VertexShader* pVertexShader,
		class PixelShader* pPixelShader,
		int modelIndex,
		std::shared_ptr<NodeData> pData, 
		const DirectX::XMMATRIX& worldTM,
		DirectX::XMMATRIX* pBoneOffsetTM);

	void ProcessOpaqueQueue(Renderer* pRenderer);
	void ProcessQueueByInstance(Renderer* pRenderer);
	void ProcessTransparentQueue(Renderer* pRenderer);
	void ProcessParticleQueue(Renderer* pRenderer);
	void ProcessDebugQueue(Renderer* pRenderer);
	void ProcessOutLineQueue(Renderer* pRenderer, bool isQT);

	void GetObjectCount(unsigned int& __out opaque, unsigned int& __out transparent, unsigned int& __out particle);
	void Clear();
	
	void GetSelectedObjectTM(const DirectX::XMMATRIX& worldTM);
	const DirectX::XMMATRIX& GetSelectedObjectTM();

private:
	unsigned int m_OpaqueCount;
	unsigned int m_TransparentCount;

	// 렌더큐의 오브젝트 풀
	std::vector<RenderQueueElement*> m_RenderQueueObejctPool_V;
	std::vector<RenderQueueParticleElement*> m_RenderQueueParticleObejctPool_V;

	std::queue<RenderQueueElement*> m_OpaqueRenderQueue_Q;
	std::queue<RenderQueueElement*> m_TransparentRenderQueue_Q;
	std::queue<RenderQueueElement*> m_OutLineQueue_Q;

	std::queue<RenderQueueParticleElement*> m_ParticleRenderQueue_Q;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_InstancedBuffer;

	std::vector<InstancedData> m_InstancedData;


	// 현재 접근해야하는 배열 정보
	// 이게 없으면 항상 사용하지 않고 있는 것을 검색해야한다
	unsigned int m_NowIndex;
	unsigned int m_SelectedObjectIndex;
	unsigned int m_NowParticleIndex;

	DirectX::XMMATRIX m_SelectedWorldTM;

};

