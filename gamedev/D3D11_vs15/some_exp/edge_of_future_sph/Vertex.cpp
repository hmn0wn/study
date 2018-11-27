#include "Vertex.h"
#include "Effects.h"

#pragma region InputLayoutDesc

// Create the vertex input layout.
const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Basic32[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::ParticlePointSprite[3] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Basic32 = 0;
ID3D11InputLayout* InputLayouts::ParticlePointSprite = 0;


void InputLayouts::InitAll(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	//
	// Basic32
	//

	Effects::BasicFX->BaseTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &Basic32));

	//
	// TreePointSprite
	//

	Effects::ParticleSpriteFX->BaseTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::ParticlePointSprite, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &ParticlePointSprite));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Basic32);
	ReleaseCOM(ParticlePointSprite);
}

#pragma endregion
