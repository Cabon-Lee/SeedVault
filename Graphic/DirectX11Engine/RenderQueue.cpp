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


	// ���� ���� �ִ� ��
	m_InstancedData.resize(MAX_RENDER_NODEDATA);

	// ���� ���� �о���� ����
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
	// ������Ʈ Ǯ���� �ϳ��� �����´�
	RenderQueueElement* _nowRenderQueue = m_RenderQueueObejctPool_V[m_NowIndex];

	// �ε����� �ο��Ѵ�
	_nowRenderQueue->renderQueueIndex = m_NowIndex;

	_nowRenderQueue->isUsing = true;
	_nowRenderQueue->pVertexShader = pVertexShader;
	_nowRenderQueue->pPixelShader = pPixelShader;
	_nowRenderQueue->objectID = objectID;

	// -1�̸� �ݻ� ����
	_nowRenderQueue->reflectionProbe = reflectionProbe;

	_nowRenderQueue->modelMeshIndex = modelIndex;
	_nowRenderQueue->isTransparent = isTransparent;

	// ��� �����͸� �־��ش�
	_nowRenderQueue->pNodeData = pData;
	_nowRenderQueue->WorldTM = pWorldTM;

	// �� ������ �����͸� �ѱ��
	_nowRenderQueue->pBoneOffsetTM = pBoneOFfsetTM;

	if (isTransparent == true)
		m_TransparentCount++;
	else
		m_OpaqueCount++;

	// Temp�����Ͱ� null ���� �ƴϸ� ť�� �ִ´�
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
			// ������ ��ũ�� ����Ʈ�� �����
			// ����� Next�� null�̸�
			if (pData->m_pTempOpaqueRenderQueueElement->pNext == nullptr)
			{
				// ����� ��ũ�� ����Ʈ�� Next, ���� ������� �������� prev�� ����
				pData->m_pTempOpaqueRenderQueueElement->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempOpaqueRenderQueueElement->pNext;
			}
			else
			{
				// ����� Prev�� �������̹Ƿ�, ���� ��Ҵ� �������� ���� ��
				pData->m_pTempOpaqueRenderQueueElement->pPrev->pNext = _nowRenderQueue;
				_nowRenderQueue->pPrev = pData->m_pTempOpaqueRenderQueueElement->pPrev;
			}

			// ����� Prev�� �׻� ���� ��Ҹ� �����Ѵ�(ȯ��)
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
		// �̹� �ش� spriteData�� ������̹Ƿ� ��ũ�� ����Ʈ�� ��������
		RenderQueueParticleElement* _pOrigin = pSpriteData->m_pTempRenderQueueEffectElement;

		// �� �ٸ� ���� ������ΰ�?
		if (_pOrigin->pNext == nullptr)
		{
			_pOrigin->pNext = _nowEffectElement;
			_nowEffectElement->pPrev = _pOrigin;
		}
		else
		{
			// ����� ���� ������ ��ũ�� �����´�
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
	// ������Ʈ Ǯ���� �ϳ��� �����´�
	RenderQueueElement* _nowRenderQueue = m_RenderQueueObejctPool_V[m_NowIndex];

	// �ε����� �ο��Ѵ�
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
		// �� �۾��� ó���������ؼ� ������ ����ť�� ���� �� �̹� ����ť �ȿ� 
		// ���� ��嵥���Ͱ� �ִ��� �� �� �ִ�
		_pTempNode->pNodeData->m_pTempOpaqueRenderQueueElement = nullptr;

		// ������ �� ���� ���ҽ��� ����ϴϱ� ���⼭ �ѹ��� ���ش�
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

			// ��嵥���͸��� �����ϴ� ���÷��� ���κ갡 �ٸ� ������ ����
			// �׷��� �ٸ� ������ �ٲ��ش�
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
			_pTempNode->pNodeData.reset();	// RenderQueueElement�� ������Ʈ Ǯ�̹Ƿ� �̷��� RefCnt�� ���δ�

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

		// ������ �� ���� ���ҽ��� ����ϴϱ� ���⼭ �ѹ��� ���ش�
		pRenderer->SetMaterial(_pTempNode->pNodeData);

		unsigned int _nowReflectionProbe = _pTempNode->reflectionProbe;
		if (_nowReflectionProbe != 9999)
			pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{
			// ��嵥���͸��� �����ϴ� ���÷��� ���κ갡 �ٸ� ������ ����
			// �׷��� �ٸ� ������ �ٲ��ش�
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
			_pTempNode->pNodeData.reset();	// RenderQueueElement�� ������Ʈ Ǯ�̹Ƿ� �̷��� RefCnt�� ���δ�

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
		// �� �۾��� ó���������ؼ� ������ ����ť�� ���� �� �̹� ����ť �ȿ� 
		// ���� ��嵥���Ͱ� �ִ��� �� �� �ִ�
		_pTempNode->pNodeData->m_pTempTrasparentRenderQueueElement = nullptr;

		// ������ �� ���� ���ҽ��� ����ϴϱ� ���⼭ �ѹ��� ���ش�
		pRenderer->SetMaterial(_pTempNode->pNodeData);

		unsigned int _nowReflectionProbe = _pTempNode->reflectionProbe;
		if (_nowReflectionProbe != 9999)
			pRenderer->SetReflectionProbeBuffer(_nowReflectionProbe);

		pRenderer->SetVertexShader(_pTempNode->pVertexShader);
		pRenderer->SetPixelShader(_pTempNode->pPixelShader);

		while (_pTempNode != nullptr)
		{


			// ��嵥���͸��� �����ϴ� ���÷��� ���κ갡 �ٸ� ������ ����
			// �׷��� �ٸ� ������ �ٲ��ش�
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
			_pTempNode->pNodeData.reset();	// RenderQueueElement�� ������Ʈ Ǯ�̹Ƿ� �̷��� RefCnt�� ���δ�

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
		// �� �۾��� ó���������ؼ� ������ ����ť�� ���� �� �̹� ����ť �ȿ� 
		// ���� ��嵥���Ͱ� �ִ��� �� �� �ִ�
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
			_pTempNode->pNodeData.reset();	// RenderQueueElement�� ������Ʈ Ǯ�̹Ƿ� �̷��� RefCnt�� ���δ�

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

