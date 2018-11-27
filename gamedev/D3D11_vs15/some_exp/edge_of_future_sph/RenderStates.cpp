#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::SolidRS = 0;


void RenderStates::InitAll(ID3D11Device* device)
{
    //
    // WireframeRS
    //
    D3D11_RASTERIZER_DESC wireframeDesc;
    ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
    wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
    wireframeDesc.CullMode = D3D11_CULL_BACK;
    wireframeDesc.FrontCounterClockwise = false;
    wireframeDesc.DepthClipEnable = true;
    HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

    //
    // SolidRS
    //
    D3D11_RASTERIZER_DESC solidDesc;
    ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
    solidDesc.FillMode = D3D11_FILL_SOLID;
    solidDesc.CullMode = D3D11_CULL_BACK;
    solidDesc.FrontCounterClockwise = false;
    solidDesc.DepthClipEnable = true;
    HR(device->CreateRasterizerState(&solidDesc, &SolidRS));

}


void RenderStates::DestroyAll()
{
    ReleaseCOM(WireframeRS);
    ReleaseCOM(SolidRS);
}