#include "pch.h"

#include "InputLayout.h"

HRESULT InputLayout::CrateInputLayoutFromVertexShader(
	ID3D11Device* pD3DDevice,
	ID3DBlob* pShaderBlob,
	ID3D11InputLayout** pInputLayout)
{
	HRESULT _hr;

	try
	{
		ID3D11ShaderReflection* _pVertexShaderReflection = NULL;

		_hr = D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&_pVertexShaderReflection);

		COM_ERROR_IF_FAILED(_hr, "D3DReflect Fail");

		// Get shader info
		D3D11_SHADER_DESC _shaderDesc;
		_pVertexShaderReflection->GetDesc(&_shaderDesc);
		
		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> _inputLayoutDesc;
		for (UINT i = 0; i < _shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC _paramDesc;
			_pVertexShaderReflection->GetInputParameterDesc(i, &_paramDesc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC _elementDesc;
			_elementDesc.SemanticName = _paramDesc.SemanticName;
			_elementDesc.SemanticIndex = _paramDesc.SemanticIndex;
			_elementDesc.InputSlot = 0;
			_elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			_elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			_elementDesc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (_paramDesc.Mask == 1)
			{
				if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) _elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) _elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) _elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (_paramDesc.Mask <= 3)
			{
				if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) _elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) _elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) _elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (_paramDesc.Mask <= 7)
			{
				if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (_paramDesc.Mask <= 15)
			{
				if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (_paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) _elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			//save element desc
			_inputLayoutDesc.push_back(_elementDesc);
		}

		// Try to create Input Layout
		_hr = pD3DDevice->CreateInputLayout(&_inputLayoutDesc[0], static_cast<UINT>(_inputLayoutDesc.size()), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

		//Free allocation shader reflection memory
		_pVertexShaderReflection->Release();
		return _hr;
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
	}



	
}
