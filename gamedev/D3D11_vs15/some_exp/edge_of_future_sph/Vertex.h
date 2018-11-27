#pragma once

#include "d3dUtil.h"

namespace Vertex
{
	// Basic 32-byte vertex structure.
	struct Basic32
	{
		Basic32() : Pos(0.0f, 0.0f, 0.0f),  Color(0.0f, 0.0f, 0.0f, 0.0f) {}
		Basic32(const DirectX::XMFLOAT3& p, const DirectX::XMFLOAT4& c)
			: Pos(p), Color(c) {}
		Basic32(float px, float py, float pz, float r, float g, float b, float a)
			: Pos(px, py, pz), Color(r, g, b, a) {}

		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	struct ParticlePointSprite
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT2 Size;
	};
}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Basic32[2];
	static const D3D11_INPUT_ELEMENT_DESC ParticlePointSprite[3];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* ParticlePointSprite;
};