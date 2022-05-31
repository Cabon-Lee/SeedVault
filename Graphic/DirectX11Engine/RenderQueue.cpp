#include "RenderQueue.h"

#include "Renderer.h"
#include "NodeData.h"

#include "Shaders.h"

#include "ErrorLogger.h"
#include "COMException.h"


RenderQueue::RenderQueue()
	: m_NowIndex(0)
{


}

RenderQueue::~RenderQueue()
{
	m_InstancedBuffer.Reset();
	m_InstancedData.clear();

	free(m_RenderQueueObejctPool_V[0]);
	free(m_RenderQueueParticleObejctPool_V[0]);
}

void RenderQueue::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	m_RenderQueueObejctPool_V.resize(MAX_RENDER_NODEDATA);

	RenderQueueElement* _newArray = static_cast<RenderQueueElement*>(malloc(sizeof(RenderQueueElement) * MAX_RENDER_NODEDATA));
	memset(_newArray, 0, sizeof(RenderQueueElement) * MAX_RENDER_NODEDATA);

	m_RenderQueueParticleObejctPool_V.resize(MAX_RENDER_NODEDATA);

	RenderQueueParticleElement* _newEffectArray = static_cast<RenderQueueParticleElement*>(malloc(sizeof(RenderQueueParticleElement) * MAX_RENDER_NODEDATA));
	memset(_newEffectArray, 0, sizeof(RenderQueueParticleElement) * MAX_RENDER_NODEDATA);

	for (size_t i = 0; i < MAX_RENDER_NODEDATA; i++)
	{
		m_RenderQueueObejctPool_V[i] = &_newArray[i];
		m_RenderQueueParticleObejctPool_V[i] = &_newEffectArray[i];
	}


	// 실제 값을 넣는 곳
	m_InstancedData.resize(MAX_RENDER_NODEDATA);

	// 실제 값을 밀어넣을 버퍼
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * MAX_RENDER_NODEDATA;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	try
	{
		HRESULT _hr;

		_hr = pDevice->CreateBuffer(&vbd, 0, m_InstancedBuffer.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "RenderQueue CreateBuffer Fail");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

}

void RenderQueue::AddRenderQueue(
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	unsigned int reflectionProbe,
	unsigned int objectID,
	int modelIndex,
	bool isTransparent,
	std::shared_ptr<NodeData> pData,
	const DirectX::XMMATRIX& pWorldTM,
	DirectX::XMMATRIX* pBoneOFfsetTM)
{
	// 오브젝트 풀에서 하나를 가져온다
	RenderQueueElement* _nowRenderQueue = m_RenderQueueObejctPool_V[m_NowIndex];

	// 인덱스를 부여한다
	_nowRenderQueue->renderQueueIndex = m_NowIndex;

	_nowRenderQueue->isUsing = true;
	_nowRenderQueue->pVertexShader = pVertexShader;
	_nowRenderQueue->pPixelShader = pPixelShader;
	_nowRenderQueue->objectID = objectID;

	// -1이면 반사 없다
	_nowRenderQueue->reflectionProbe = reflectionProbe;

	_nowRenderQueue->modelMeshIndex = modelIndex;
	_nowRenderQueue->isTransparent = isTransparent;

	// 노드 데이터를 넣어준다
	_nowRenderQueue->pNodeData = pData;
	_nowRenderQueue->WorldTM = pWorldTM;

	// 본 오프셋 포인터를 넘긴다
	_nowRenderQueue->pBoneOffsetTM = pBoneOFfsetTM;

	if (isTransparent == true)
		m_TransparentCount++;
	else
		m_OpaqueCount++;

	// Temp포인터가 null 값이 아니면 큐에 넣는다
	if (pData->m_pTempOpaqueRenderQueueElement == nullptr && pData->m_pTempTrasparentRenderQueueElement == nullptr)
	{
		if (isTransparent != true)
		{
			pData->m_pTempOpaqueRenderQueueElement = _nowRenderQueue;
			m_OpaqueRenderQueue_Q.push(_nowRenderQueue);
		}
		else
		{
			pData->m_pTempTrasparentRenderQueueElement = _nowRenderQueue;
			m_TransparentRenderQueue_Q.push(_nowRenderQueue);
		}
	}
	else
	{
		if (isTransparent != true)
		{
			// 불투명 링크드 리스트를 만든다
			// 헤드의 Next가 null이면
			if (pData->m_pTempOpaqueRenderQueueElement->pNext == nullptr)
			{
				// 헤드의 링크드 리스트의 Next, 현재 만들어진 포인터의 prev를 헤드로
				pData->m_pTempOpaqueRenderQueueElement->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempOpaqueRenderQueueElement->pNext;
			}
			else
			{
				// 헤드의 Prev는 마지막이므로, 현재 요소는 마지막의 다음 것
				pData->m_pTempOpaqueRenderQueueElement->pPrev->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempOpaqueRenderQueueElement->pPrev;
			}

			// 헤드의 Prev는 항상 현재 요소를 연결한다(환형)
			pData->m_pTempOpaqueRenderQueueElement->pPrev = _nowRenderQueue;
		}
		else
		{
			if (pData->m_pTempTrasparentRenderQueueElement->pNext == nullptr)
			{
				pData->m_pTempTrasparentRenderQueueElement->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempTrasparentRenderQueueElement;
			}
			else
			{
				pData->m_pTempTrasparentRenderQueueElement->pPrev->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempTrasparentRenderQueueElement->pPrev;
			}

			pData->m_pTempTrasparentRenderQueueElement->pPrev = _nowRenderQueue;
		}
	}

	m_NowIndex++;
}

void RenderQueue::AddRenderQueueParticle(
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	unsigned int spriteIndex,
	std::shared_ptr<SpriteData> pSpriteData,
	const DirectX::XMMATRIX& pWorldTM)
{
	RenderQueueParticleElement* _nowEffectElement = m_RenderQueueParticleObejctPool_V[m_NowParticleIndex];

	_nowEffectElement->renderQueueIndex = m_NowParticleIndex;
	_nowEffectElement->spriteIndex = spriteIndex;
	_nowEffectElement->pVertexShader = pVertexShader;
	_nowEffectElement->pPixelShader = pPixelShader;
	_nowEffectElement->WorldTM = pWorldTM;

	if (pSpriteData->m_pTempRenderQueueEffectElement != nullptr)
	{
		// 이미 해당 spriteData를 사용중이므로 링크드 리스트로 관리하자
		RenderQueueParticleElement* _pOrigin = pSpriteData->m_pTempRenderQueueEffectElement;

		// 또 다른 것이 사용중인가?
		if (_pOrigin->pNext == nullptr)
		{
			_pOrigin->pNext = _nowEffectElement;
			_nowEffectElement->pPrev = _pOrigin;
		}
		else
		{
			// 헤드의 가장 마지막 링크를 가져온다
			_pOrigin->pPrev->pNext = _nowEffectElement;
			_nowEffectElement->pPrev = _pOrigin->pPrev;
		}

		_pOrigin->pPrev = _nowEffectElement;
	}
	else
	{
		pSpriteData->m_pTempRenderQueueEffectElement = _nowEffectElement;
		m_ParticleRenderQueue_Q.push(_nowEffectElement);
	}

	m_NowParticleIndex++;
}

void RenderQueue::CopyOutLineMesh(
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	int modelIndex,
	std::shared_ptr<NodeData> pData,
	const DirectX::XMMATRIX& worldTM,
	DirectX::XMMATRIX* pBoneOffsetTM)
{
	// 오브젝트 풀에서 하나를 가져온다
	RenderQueueElement* _nowRenderQueue = m_RenderQueueObejctPool_V[m_NowIndex];

	// 인덱스를 부여한다
	_nowRenderQueue->renderQueueIndex = m_NowIndex;

	_nowRenderQueue->isUsing = true;
	_nowRenderQueue->pVertexShader = pVertexShader;
	_nowRenderQueue->pPixelShader = pPixelShader;
	_nowRenderQueue->modelMeshIndex = modelIndex;
	_nowRenderQueue->pNodeData = pData;

	_nowRenderQueue->WorldTM = worldTM;
	_nowRenderQueue->pBoneOffsetTM = pBoneOffsetTM;

	m_SelectedObjectIndex = m_NowIndex;
	m_NowIndex++;

	m_OutLineQueue_Q.push(_nowRenderQueue);

}

void RenderQueue::ProcessOpaqueQueue(Renderer* pRenderer)
{
	while (m_OpaqueRenderQueue_Q.empty() != true)
	{
		auto* _nowElement = m_OpaqueRenderQueue_Q.front();
		m_OpaqueRenderQueue_Q.pop();

		RenderQueueElement* _pTempNode;
		RenderQueueElement* _pNextNode;
		_pTempNode = _nowElement;
		_pNextNode = _nowElement;
		// 이 작업을 처리함으로해서 다음에 렌더큐를 넣을 때 이미 렌더큐 안에 
		// 같은 노드데이터가 있는지 알 수 있다
		_pTempNode->pNodeData->m_pTempOpaqueRenderQueueElement = nullptr;

		// 어차피 다 같은 리소스를 사용하니까 여기서 한번만 해준다
		pRenderer->SetMaterial(_pTempNode->pNodeData);

		unsigned int _nowReflectionProbe = _pTempNode->reflectionProbe;
		if (_nowReflectionProbe != 9999)
			pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{

			ConstantBufferManager::GetPSObjectID()->data.ObjectID = _pTempNode->objectID;
			ConstantBufferManager::GetPSObjectID()->ApplyChanges();
			pRenderer->GetDeviceContext()->PSSetConstantBuffers(3, 1, ConstantBufferManager::GetPSObjectID()->GetAddressOf());

			// 노드데이터마다 참조하는 리플렉션 프로브가 다를 것으로 예상
			// 그래서 다를 때마다 바꿔준다
			if (_nowReflectionProbe != _pTempNode->reflectionProbe && _pTempNode->reflectionProbe != 9999)
			{
				_nowReflectionProbe = _pTempNode->reflectionProbe;
				pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);
			}

			pRenderer->RenderProcess(
				_pTempNode->modelMeshIndex,
				_pTempNode->pNodeData,
				_pTempNode->WorldTM,
				_pTempNode->pBoneOffsetTM);

			_pNextNode = _pTempNode->pNext;

			_pTempNode->pNext = nullptr;
			_pTempNode->pPrev = nullptr;
			_pTempNode->pNodeData.reset();	// RenderQueueElement가 오브젝트 풀이므로 이렇게 RefCnt를 줄인다

			_pTempNode = _pNextNode;

		}

	}
}

void RenderQueue::ProcessQueueByInstance(Renderer* pRenderer)
{
	while (m_OpaqueRenderQueue_Q.empty() != true)
	{
		RenderQueueElement* _nowElement = m_OpaqueRenderQueue_Q.front();
		m_OpaqueRenderQueue_Q.pop();

		std::shared_ptr<NodeData> _nowNodeData = _nowElement->pNodeData;

		RenderQueueElement* _pTempNode;
		RenderQueueElement* _pNextNode;
		_pTempNode = _nowElement;
		_pNextNode = _nowElement;

		_pTempNode->pNodeData->m_pTempOpaqueRenderQueueElement = nullptr;

		unsigned int _nowInstanceCount = 0;

		// 어차피 다 같은 리소스를 사용하니까 여기서 한번만 해준다
		pRenderer->SetMaterial(_pTempNode->pNodeData);

		unsigned int _nowReflectionProbe = _pTempNode->reflectionProbe;
		if (_nowReflectionProbe != 9999)
			pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{
			// 노드데이터마다 참조하는 리플렉션 프로브가 다를 것으로 예상
			// 그래서 다를 때마다 바꿔준다
			if (_nowReflectionProbe != _pTempNode->reflectionProbe && _pTempNode->reflectionProbe != 9999)
			{
				_nowReflectionProbe = _pTempNode->reflectionProbe;
				pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);
			}

			ConstantBufferManager::GetPSObjectID()->data.ObjectID = _pTempNode->objectID;
			ConstantBufferManager::GetPSObjectID()->ApplyChanges();
			pRenderer->GetDeviceContext()->PSSetConstantBuffers(3, 1, ConstantBufferManager::GetPSObjectID()->GetAddressOf());

			m_InstancedData[_nowInstanceCount].world = _pTempNode->WorldTM;

			size_t s1 = sizeof(Vertex::Vertex);
			size_t s2 = sizeof(InstancedData);

			_pNextNode = _pTempNode->pNext;

			_pTempNode->pNext = nullptr;
			_pTempNode->pPrev = nullptr;
			_pTempNode->pNodeData.reset();	// RenderQueueElement가 오브젝트 풀이므로 이렇게 RefCnt를 줄인다

			_pTempNode = _pNextNode;

			_nowInstanceCount++;
		}

		D3D11_MAPPED_SUBRESOURCE mappedData;
		pRenderer->GetDeviceContext()->Map(m_InstancedBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

		for (UINT i = 0; i < _nowInstanceCount; ++i)
		{
			dataView[i] = m_InstancedData[i];
		}

		pRenderer->GetDeviceContext()->Unmap(m_InstancedBuffer.Get(), 0);

		pRenderer->RenderProcessInstance(
			_nowElement->modelMeshIndex,
			m_InstancedBuffer,
			_nowNodeData,
			_nowInstanceCount);
	}
}

void RenderQueue::ProcessTransparentQueue(Renderer* pRenderer)
{
	while (m_TransparentRenderQueue_Q.empty() != true)
	{
		auto* _nowElement = m_TransparentRenderQueue_Q.front();
		m_TransparentRenderQueue_Q.pop();

		RenderQueueElement* _pTempNode;
		RenderQueueElement* _pNextNode;
		_pTempNode = _nowElement;
		_pNextNode = _nowElement;
		// 이 작업을 처리함으로해서 다음에 렌더큐를 넣을 때 이미 렌더큐 안에 
		// 같은 노드데이터가 있는지 알 수 있다
		_pTempNode->pNodeData->m_pTempTrasparentRenderQueueElement = nullptr;

		// 어차피 다 같은 리소스를 사용하니까 여기서 한번만 해준다
		pRenderer->SetMaterial(_pTempNode->pNodeData);

		unsigned int _nowReflectionProbe = _pTempNode->reflectionProbe;
		if (_nowReflectionProbe != 9999)
			pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{


			// 노드데이터마다 참조하는 리플렉션 프로브가 다를 것으로 예상
			// 그래서 다를 때마다 바꿔준다
			if (_nowReflectionProbe != _pTempNode->reflectionProbe && _pTempNode->reflectionProbe != 9999)
			{
				_nowReflectionProbe = _pTempNode->reflectionProbe;
				pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);
			}

			ConstantBufferManager::GetPSObjectID()->data.ObjectID = _pTempNode->objectID;
			ConstantBufferManager::GetPSObjectID()->ApplyChanges();
			pRenderer->GetDeviceContext()->PSSetConstantBuffers(3, 1, ConstantBufferManager::GetPSObjectID()->GetAddressOf());

			pRenderer->RenderOITProcess(
				_pTempNode->modelMeshIndex,
				_pTempNode->pNodeData,
				_pTempNode->WorldTM,
				_pTempNode->pBoneOffsetTM);

			_pNextNode = _pTempNode->pNext;

			_pTempNode->pNext = nullptr;
			_pTempNode->pPrev = nullptr;
			_pTempNode->pNodeData.reset();	// RenderQueueElement가 오브젝트 풀이므로 이렇게 RefCnt를 줄인다

			_pTempNode = _pNextNode;
		}
	}
}

void RenderQueue::ProcessParticleQueue(Renderer* pRenderer)
{
	while (m_ParticleRenderQueue_Q.empty() != true)
	{
		auto* _nowElement = m_ParticleRenderQueue_Q.front();
		m_ParticleRenderQueue_Q.pop();

		RenderQueueParticleElement* _pTempNode;
		RenderQueueParticleElement* _pNextNode;
		_pTempNode = _nowElement;
		_pNextNode = _nowElement;

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{
			pRenderer->RenderParticleProcess(
				_pTempNode->spriteIndex,
				_pTempNode->WorldTM);

			_pNextNode = _pTempNode->pNext;

			_pTempNode->pNext = nullptr;
			_pTempNode->pPrev = nullptr;

			_pTempNode = _pNextNode;
		}
	}
}

void RenderQueue::ProcessDebugQueue(Renderer* pRenderer)
{
	while (m_OpaqueRenderQueue_Q.empty() != true)
	{
		auto* _nowElement = m_OpaqueRenderQueue_Q.front();
		m_OpaqueRenderQueue_Q.pop();

		RenderQueueElement* _pTempNode;
		RenderQueueElement* _pNextNode;
		_pTempNode = _nowElement;
		_pNextNode = _nowElement;
		// 이 작업을 처리함으로해서 다음에 렌더큐를 넣을 때 이미 렌더큐 안에 
		// 같은 노드데이터가 있는지 알 수 있다
		_pTempNode->pNodeData->m_pTempOpaqueRenderQueueElement = nullptr;

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{
			pRenderer->RenderIDColor(
				_pTempNode->objectID,
				_pTempNode->modelMeshIndex,
				_pTempNode->pNodeData,
				_pTempNode->WorldTM,
				_pTempNode->pBoneOffsetTM);

			_pNextNode = _pTempNode->pNext;

			_pTempNode->pNext = nullptr;
			_pTempNode->pPrev = nullptr;
			_pTempNode->pNodeData.reset();	// RenderQueueElement가 오브젝트 풀이므로 이렇게 RefCnt를 줄인다

			_pTempNode = _pNextNode;

		}
	}
}

void RenderQueue::ProcessOutLineQueue(Renderer* pRenderer, bool isQT)
{	
	if (isQT == true)
	{
		while (m_OutLineQueue_Q.empty() != true)
		{
			auto* _nowElement = m_OutLineQueue_Q.front();
			m_OutLineQueue_Q.pop();

			m_SelectedWorldTM = _nowElement->WorldTM;
			pRenderer->SetVertexShader(_nowElement->pVertexShader);
			pRenderer->SetPixelShader(_nowElement->pPixelShader);

			pRenderer->RenderOutLineProcess(
				_nowElement->modelMeshIndex,
				_nowElement->pNodeData,
				_nowElement->WorldTM,
				_nowElement->pBoneOffsetTM);
		}

		pRenderer->RenderOutLine();
	}
}

void RenderQueue::GetObjectCount(unsigned int& __out opaque, unsigned int& __out transparent, unsigned int& __out particle)
{
	opaque = m_OpaqueCount;
	transparent = m_TransparentCount;
	particle = m_NowParticleIndex;
}

void RenderQueue::Clear()
{
	m_NowIndex = 0;
	m_NowParticleIndex = 0;

	m_OpaqueCount = 0;
	m_TransparentCount = 0;
}

void RenderQueue::GetSelectedObjectTM(const DirectX::XMMATRIX& worldTM)
{
	m_SelectedWorldTM = worldTM;
}

const DirectX::XMMATRIX& RenderQueue::GetSelectedObjectTM()
{
	return m_SelectedWorldTM;
}

