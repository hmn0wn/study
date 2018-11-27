#pragma once
#include "d3dUtil.h"


class RenderStates
{
public:
    static void InitAll(ID3D11Device* device);
    static void DestroyAll();

    // Rasterizer states
    static ID3D11RasterizerState* WireframeRS;
    static ID3D11RasterizerState* SolidRS;

};

