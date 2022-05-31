#pragma once


#include <vector>

class InputLayout
{

public:
	static HRESULT CrateInputLayoutFromVertexShader(
		ID3D11Device* pD3DDevice, 
		ID3DBlob* pShaderBlob, 
		ID3D11InputLayout** pInputLayout);

};

