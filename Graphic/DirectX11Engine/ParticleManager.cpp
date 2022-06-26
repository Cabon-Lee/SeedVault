#include "ParticleManager.h"

#include "IRenderer.h"
#include "Shaders.h"

#include "ConstantBufferManager.h"
#include "RasterizerState.h"

void ParticleElement::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	try
	{
		{
			HRESULT _hr;

			D3D11_BUFFER_DESC _soDesc;
			_soDesc.Usage = D3D11_USAGE_DEFAULT;
			_soDesc.ByteWidth = sizeof(ParticleCore) * 1;
			_soDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			_soDesc.CPUAccessFlags = 0;
			_soDesc.MiscFlags = 0;
			_soDesc.StructureByteStride = 0;

			ParticleCore _particle;
			ZeroMemory(&_particle, sizeof(ParticleCore));
			_particle.age.x = 0.0f;
			_particle.age.y = 0.0f;
			_particle.Type = 0;

			D3D11_SUBRESOURCE_DATA _vinitData;
			_vinitData.pSysMem = &_particle;
			_vinitData.SysMemPitch = 0;
			_vinitData.SysMemSlicePitch = 0;

			_hr = pDevice->CreateBuffer(&_soDesc, &_vinitData, &pInitVB);

			COM_ERROR_IF_FAILED(_hr, "Initialize Buffer CreateBuffer Fail");

			_soDesc.ByteWidth = sizeof(ParticleCore) * MAX_PARTICLE;
			_soDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

			_hr = pDevice->CreateBuffer(&_soDesc, 0, &pDrawVB);

			COM_ERROR_IF_FAILED(_hr, "Draw Buffer CreateBuffer Fail");

			_hr = pDevice->CreateBuffer(&_soDesc, 0, &pSOBuffer);

			COM_ERROR_IF_FAILED(_hr, "StreamOut Buffer CreateBuffer Fail");
		}

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}
}

void ParticleElement::Reset()
{
	isRun = false;
	isFirstRun = true;
	index = 0;

	particleCore.age.x = 0.0f;
	particleCore.age.y = 0.0f;

	particleCore.initialPos = { 0.0f,0.0f, 0.0f };
	particleCore.initialVel = { 0.0f,0.0f, 0.0f };
	particleCore.size = { 1.0f,1.0f };
	particleCore.Type = 0;

	pTexture = nullptr;
}

ParticleManager::ParticleManager()
{

}

ParticleManager::~ParticleManager()
{

}

static float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
static float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

void ParticleManager::Initialize(
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	class GeometryShader* pShader,
	class VertexShader* pVShader)
{

	m_StreamOutGeometryShaderTest = std::make_shared<class GeometryShader>();

	try
	{	// GeometryShader로부터 StreamOutShader 생성

		HRESULT _hr;

		D3D11_SO_DECLARATION_ENTRY SODeclarationEntry[5] =
		{
			{ 0, "POSITION", 0, 0, 3, 0 },
			{ 0, "VELOCITY", 0, 0, 3, 0 },
			{ 0, "SIZE", 0, 0, 2, 0 },
			{ 0, "AGE", 0, 0, 2, 0 },
			{ 0, "TYPE", 0, 0, 1, 0 }
		};

		pShader->StreamOutputInitialize(pDevice, SODeclarationEntry, _countof(SODeclarationEntry));



		{	// 인풋 레이아웃 작성 및 생성
			D3D11_INPUT_ELEMENT_DESC _particle[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"AGE",      0, DXGI_FORMAT_R32G32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			_hr = pDevice->CreateInputLayout(
				_particle, _countof(_particle),
				pVShader->GetVertexShaderBuffer()->GetBufferPointer(),
				pVShader->GetVertexShaderBuffer()->GetBufferSize(),
				m_pParticleInputLayout.GetAddressOf());

			COM_ERROR_IF_FAILED(_hr, "CreateInputLayout Fail");
		}

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}

	BuildRandomTexture(pDevice);

	for (unsigned int i = 0; i < PARTICLE_EMITTER; ++i)
	{
		std::shared_ptr<ParticleElement> _newParticle = std::make_shared<ParticleElement>();
		_newParticle->Initialize(pDevice);

		m_WaitingParticle_Q.push(_newParticle);
	}
}

void ParticleManager::BuildRandomTexture(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
	try
	{
		HRESULT _hr;

		XMFLOAT4 randomValues[1024];

		for (int i = 0; i < 1024; ++i)
		{
			randomValues[i].x = RandF(-1.0f, 1.0f);
			randomValues[i].y = RandF(-1.0f, 1.0f);
			randomValues[i].z = RandF(-1.0f, 1.0f);
			randomValues[i].w = RandF(-1.0f, 1.0f);
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = randomValues;
		initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
		initData.SysMemSlicePitch = 0;

		//
		// Create the texture.
		//
		D3D11_TEXTURE1D_DESC texDesc;
		texDesc.Width = 1024;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_IMMUTABLE;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.ArraySize = 1;

		ID3D11Texture1D* randomTex = 0;
		_hr = pDevice->CreateTexture1D(&texDesc, &initData, &randomTex);


		COM_ERROR_IF_FAILED(_hr, "");

		//
		// Create the resource view.
		//
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
		viewDesc.Texture1D.MostDetailedMip = 0;

		_hr = pDevice->CreateShaderResourceView(randomTex, &viewDesc, m_pRandomTextureSRV.GetAddressOf());

		COM_ERROR_IF_FAILED(_hr, "");

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);

	}

}

unsigned int ParticleManager::AddParticle(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext, 
	ID3D11ShaderResourceView* pTexture,
	struct ParticleProperty* particle)
{
	auto _nowParticle = m_WaitingParticle_Q.front();
	m_WaitingParticle_Q.pop();

	_nowParticle->particleProperty = particle;

	_nowParticle->isFirstRun = true;
	_nowParticle->particleCore.initialPos = *particle->emitterPos;
	_nowParticle->particleCore.initialVel = *particle->emitDir;
	_nowParticle->particleCore.size = *particle->Particlesize;
	_nowParticle->pTexture = pTexture;

	m_ActiveParticle_Q.push(_nowParticle);

	_nowParticle->index = m_ParticleCount;
	return m_ParticleCount++;
}

void ParticleManager::UpdateParticle(float dTime)
{
	m_DeltaTime = dTime;

	std::queue<std::shared_ptr<ParticleElement>> _tempQueue;
	while (m_ActiveParticle_Q.empty() != true)
	{
		auto _nowParticle = m_ActiveParticle_Q.front();
		m_ActiveParticle_Q.pop();

		if (_nowParticle->particleCore.age.x >= 
			_nowParticle->particleProperty->deadTime * 2)
			//&& _nowParticle->isContinue != false)
		{
			// 데드라인을 지난 파티클은 리셋 시키고 웨이팅 큐로 넣어버린다
			_nowParticle->Reset();
			m_WaitingParticle_Q.push(_nowParticle);
		}
		else
		{
			_nowParticle->particleCore.age.x += dTime;
			m_RenderParticle_Q.push(_nowParticle);
			_tempQueue.push(_nowParticle);
		}
	}

	if (_tempQueue.empty() == false)
		m_ActiveParticle_Q.swap(_tempQueue);

}

void ParticleManager::DrawParticle(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	class VertexShader* pStreamOutVS,
	class GeometryShader* pStreamOutGS,
	class VertexShader* pDrawVS,
	class GeometryShader* pDrawGS,
	class PixelShader* pDrawPS,
	const DirectX::XMFLOAT3& eyePos,
	const DirectX::XMMATRIX& viewTM,
	const DirectX::XMMATRIX& projTM)
{
	m_Total += m_DeltaTime;
	
	pDeviceContext->IASetInputLayout(m_pParticleInputLayout.Get());
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	ConstantBufferManager::GetGSParticlePerFrameBuffer()->data.EyePos = eyePos;
	ConstantBufferManager::GetGSParticlePerFrameBuffer()->data.viewProj = viewTM * projTM;
	ConstantBufferManager::GetGSParticlePerFrameBuffer()->ApplyChanges();
	pDeviceContext->GSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	UINT _stide = sizeof(ParticleCore);
	UINT _offset = 0;

	while (m_RenderParticle_Q.empty() != true)
	{
		auto _nowParticle = m_RenderParticle_Q.front();
		m_RenderParticle_Q.pop();

		{	// 스트림 출력 패스

			// 레스터화기 비활성화 방식
			pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDisabledDSS(), 1);

			// 첫 번째 도는 중이라면 초기화 버텍스 버퍼를 쓴다
			if (_nowParticle->isFirstRun == true)
				pDeviceContext->IASetVertexBuffers(0, 1, &_nowParticle->pInitVB, &_stide, &_offset);
			else
				pDeviceContext->IASetVertexBuffers(0, 1, &_nowParticle->pDrawVB, &_stide, &_offset);

			// 업데이트를 위해 스트림 출력 전용으로 Geometry Shader를 사용한다
			pDeviceContext->SOSetTargets(1, &_nowParticle->pSOBuffer, &_offset);

			pDeviceContext->VSSetShader(pStreamOutVS->GetVertexShader(), nullptr, 0);
			pDeviceContext->GSSetShader(pStreamOutGS->GetSteamOutputGeometryShader(), nullptr, 0);
			pDeviceContext->GSSetShaderResources(0, 1, m_pRandomTextureSRV.GetAddressOf());
			pDeviceContext->PSSetShader(nullptr, nullptr, 0);

#pragma region SteamOut Pass GeometryShader에 상수 버퍼 값 전달
			{
				auto& _nowCore = _nowParticle->particleCore;
				ConstantBufferManager::GetGSParticleBuffer()->data.emitPos = _nowCore.initialPos;
				ConstantBufferManager::GetGSParticleBuffer()->data.emitDir = _nowCore.initialVel;
				ConstantBufferManager::GetGSParticleBuffer()->data.gameTime = m_Total;
				ConstantBufferManager::GetGSParticleBuffer()->data.timeStep = m_DeltaTime;
				ConstantBufferManager::GetGSParticleBuffer()->data.size = _nowParticle->particleProperty->Particlesize->x;
				ConstantBufferManager::GetGSParticleBuffer()->data.randomWeight = _nowParticle->particleProperty->dirRandomWeight;
				ConstantBufferManager::GetGSParticleBuffer()->data.birthTime = _nowParticle->particleProperty->birthTime;
				ConstantBufferManager::GetGSParticleBuffer()->data.deadTime = _nowParticle->particleProperty->deadTime;
				ConstantBufferManager::GetGSParticleBuffer()->data.stopTime = _nowParticle->particleProperty->stopTime;
				ConstantBufferManager::GetGSParticleBuffer()->data.particleCount = _nowParticle->particleProperty->particeCount;

				ConstantBufferManager::GetGSParticleBuffer()->ApplyChanges();
				pDeviceContext->GSSetConstantBuffers(0, 1, ConstantBufferManager::GetGSParticleBuffer()->GetAddressOf());
			}
#pragma endregion SteamOut Pass GeometryShader에 상수 버퍼 값 전달

			// 첫 시작이라면 Draw를 통해 Vertex, PixelShader를 호출한다
			if (_nowParticle->isFirstRun == true)
			{
				pDeviceContext->Draw(1, 0);
				_nowParticle->isFirstRun = false;
			}
			else
			{
				pDeviceContext->DrawAuto();
			}

			// SteamOut Pass가 종료되면 unbind를 위해 아래 처리를 해준다
			ID3D11Buffer* bufferArray[1] = { nullptr };
			pDeviceContext->SOSetTargets(1, bufferArray, &_offset);
		}

		pDeviceContext->OMSetBlendState(RasterizerState::GetTransparentSetState(), nullptr, 0xffffffff);
		pDeviceContext->GSSetConstantBuffers(0, 1, ConstantBufferManager::GetGSParticlePerFrameBuffer()->GetAddressOf());
		pDeviceContext->OMSetDepthStencilState(RasterizerState::GetOITInitDS(), 0);

#pragma region  Render Pass VertexShader에 상수 버퍼 값 전달
		float _cbufferData[8] =
		{
			_nowParticle->particleProperty->constForce,
			_nowParticle->particleProperty->constZrot,

			_nowParticle->particleProperty->deltaAlpha,
			_nowParticle->particleProperty->deltaVelo,

			_nowParticle->particleProperty->deltaSize,
			_nowParticle->particleProperty->deltaRot,

			_nowParticle->particleProperty->deltaSizeWeight,
			_nowParticle->particleProperty->deltaRotWeight
		};

		ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, 
			CBufferType::VS, CBufferSize::SIZE_8, 0, _cbufferData);

		ConstantBufferManager::GetPSParticleEmissive()->data.particleEmissive.x = 2.0f;
		ConstantBufferManager::GetPSParticleEmissive()->data.particleEmissive.y = 2.0f;
		ConstantBufferManager::GetPSParticleEmissive()->data.particleEmissive.z = 2.0f;
		ConstantBufferManager::GetPSParticleEmissive()->data.particleEmissive.w = 1.0f;
		ConstantBufferManager::GetPSParticleEmissive()->ApplyChanges();
		pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSParticleEmissive()->GetAddressOf());
#pragma endregion  Render Pass VertexShader에 상수 버퍼 값 전달

		pDeviceContext->VSSetShaderResources(0, 1, m_pRandomTextureSRV.GetAddressOf());
		pDeviceContext->VSSetSamplers(0, 1, RasterizerState::GetLinearSamplerStateAddressOf());

		// 앞서 SOSetTargets에 묶어두었던 버퍼와 그릴 버퍼를 스왑한다
		std::swap(_nowParticle->pDrawVB, _nowParticle->pSOBuffer);
		pDeviceContext->IASetVertexBuffers(0, 1, &_nowParticle->pDrawVB, &_stide, &_offset);

		pDeviceContext->VSSetShader(pDrawVS->GetVertexShader(), nullptr, 0);
		pDeviceContext->GSSetShader(pDrawGS->GetGeometryShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pDrawPS->GetPixelShader(), nullptr, 0);
		pDeviceContext->PSSetShaderResources(0, 1, &_nowParticle->pTexture);

		pDeviceContext->DrawAuto();
		

	}

	ID3D11ShaderResourceView* _srv[1] = { nullptr };
	pDeviceContext->PSSetShaderResources(0, 1, _srv);

	ID3D11Buffer* _bf[2] = { nullptr };
	pDeviceContext->PSSetConstantBuffers(0, 1, _bf);
	pDeviceContext->GSSetConstantBuffers(0, 1, _bf);

	pDeviceContext->VSSetShader(nullptr, nullptr, 0);
	pDeviceContext->GSSetShader(nullptr, nullptr, 0);
	pDeviceContext->PSSetShader(nullptr, nullptr, 0);
}

void ParticleManager::ResetParticle()
{
	if (m_RenderParticle_Q.empty() != true)
	{
		while (m_RenderParticle_Q.empty() == false)
		{
			m_RenderParticle_Q.front();
		}
	}
}

