//***************************************************************************************
// 
//
// Demonstrates ...
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "Vertex.h"
#include "Effects.h"
#include "RenderStates.h"
#include "smyth_particle_hydro_system.h"
#include <vector>


enum RenderOptions
{
    Wireframe = 0,
    Solid = 1,
    WireframeLighting = 2,
    SolidLighting = 3
};

class EofFutureSPHApp : public D3DApp
{
public:
    EofFutureSPHApp(HINSTANCE hInstance);
    ~EofFutureSPHApp();

    bool Init();
    void OnResize();
    void UpdateScene(float dt);
    void DrawScene();

    void OnMouseDown(WPARAM btnState, int x, int y);
    void OnMouseUp(WPARAM btnState, int x, int y);
    void OnMouseMove(WPARAM btnState, int x, int y);

private:
    float GetHeight(float x, float z)const;
    void BuildGridGeometryBuffers();
    void BuildParticlesBuffers();
    void CreateParticlePositionTexture2DSRV() {}
    void UpdateParticleBuffers();
    void DrawParticleSprites(DirectX::CXMMATRIX viewProj);

   
private:
    ID3D11Buffer* mGridVB;
    ID3D11Buffer* mGridIB;

    ID3D11Buffer* mParticleSpritesVB;
    ID3D11Texture2D* mParticlePositionsTex;
    ID3D11ShaderResourceView* mParticlePositions2DSRV;

    //Particles

    shps *shps0;
    

    float particleRadius;
    float layerHeight;

    // Define transformations from local spaces to world space.
    DirectX::XMFLOAT4X4 mGridWorld;

    UINT mGridIndexCount;
    RenderOptions mRenderOptions;

    DirectX::XMFLOAT4X4 mView;
    DirectX::XMFLOAT4X4 mProj;

    float mTheta;
    float mPhi;
    float mRadius;

    POINT mLastMousePos;

    UINT frameCount;
    float animDt;


};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    EofFutureSPHApp theApp(hInstance);

    if (!theApp.Init())
        return 0;

    return theApp.Run();
}


EofFutureSPHApp::EofFutureSPHApp(HINSTANCE hInstance)
    : D3DApp(hInstance), mGridVB(0), mGridIB(0), mGridIndexCount(0), mRenderOptions(RenderOptions::Solid),
    mTheta(1.5f*MathHelper::Pi), mPhi(0.1f*MathHelper::Pi), mRadius(500.0f),
    shps0(new shps), frameCount(0), animDt(0.f)
{
    mMainWndCaption = L"Edge of future";

    mLastMousePos.x = 0;
    mLastMousePos.y = 0;

    DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
    XMStoreFloat4x4(&mGridWorld, I);
    XMStoreFloat4x4(&mView, I);
    XMStoreFloat4x4(&mProj, I);

}

EofFutureSPHApp::~EofFutureSPHApp()
{
    ReleaseCOM(mGridIB);
    ReleaseCOM(mParticleSpritesVB);
    ReleaseCOM(mParticlePositionsTex);

    
    shps0->free_state();
    delete shps0;
}

bool EofFutureSPHApp::Init()
{
    if (!D3DApp::Init())
        return false;

    Effects::InitAll(md3dDevice);
    InputLayouts::InitAll(md3dDevice);
    RenderStates::InitAll(md3dDevice);

    BuildGridGeometryBuffers();

    
    shps0->init();

    BuildParticlesBuffers();

    return true;
}

void EofFutureSPHApp::OnResize()
{
    D3DApp::OnResize();

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void EofFutureSPHApp::UpdateScene(float dt)
{
    frameCount++;
    // Convert Spherical to Cartesian coordinates.
    float x = mRadius*sinf(mPhi)*cosf(mTheta);
    float z = mRadius*sinf(mPhi)*sinf(mTheta);
    float y = mRadius*cosf(mPhi);

    // Build the view matrix.
    DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
    DirectX::XMVECTOR target = DirectX::XMVectorZero();
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, V);
    
    //
    // Update the particle vertex buffer with the new solution.
    //
    shps0->state->dt = 0.0005f;
    float k = 100.f;


    //shps0->state->dt = dt;


    animDt += dt;
    if (animDt > shps0->state->dt * k)
    {
        animDt = 0.f;
        //shps0->move_tentakles(shps_params->dt *80.f);
        shps0->update_all_layers();
        UpdateParticleBuffers();
    }
    


    if (GetAsyncKeyState('1') & 0x8000)
        mRenderOptions = RenderOptions::Wireframe;

    if (GetAsyncKeyState('2') & 0x8000)
        mRenderOptions = RenderOptions::Solid;

    if (GetAsyncKeyState('3') & 0x8000)
        mRenderOptions = RenderOptions::SolidLighting;

}

void EofFutureSPHApp::DrawScene()
{
    md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
    md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    
    // Set constants
    DirectX::XMMATRIX view = XMLoadFloat4x4(&mView);
    DirectX::XMMATRIX proj = XMLoadFloat4x4(&mProj);
    DirectX::XMMATRIX world = XMLoadFloat4x4(&mGridWorld);
    DirectX::XMMATRIX worldViewProj = world*view*proj;
    DirectX::XMMATRIX viewProj = view*proj;

    
    //
    // Draw the particle sprites
    //
    DrawParticleSprites(viewProj);
    //
    // DrawParticleSprites() changes InputLayout and PrimitiveTopology, so change it based on 
    // the geometry we draw next.
    //

    md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

    //
    // Set per frame constants for the rest of the objects.
    //

    //...        


    //
    // Figure out which technique to use.
    //

    ID3DX11EffectTechnique* gridTech = nullptr;

    ID3D11RasterizerState*  curRasterState = nullptr;

    switch (mRenderOptions)
    {
    case RenderOptions::Wireframe:
        gridTech = Effects::BasicFX->BaseTech;
        curRasterState = RenderStates::WireframeRS;
        break;

    case RenderOptions::Solid:
        gridTech = Effects::BasicFX->BaseTech;
        curRasterState = RenderStates::SolidRS;
        break;
    }

    D3DX11_TECHNIQUE_DESC techDesc;
    //
    //Draw the grid
    //

    if (gridTech)
    {
        gridTech->GetDesc(&techDesc);
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            md3dImmediateContext->IASetVertexBuffers(0, 1, &mGridVB, &stride, &offset);
            md3dImmediateContext->IASetIndexBuffer(mGridIB, DXGI_FORMAT_R32_UINT, 0);

            // Draw the grid.
            Effects::BasicFX->SetWorldViewProj(worldViewProj);

            md3dImmediateContext->RSSetState(curRasterState);
            gridTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
            md3dImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);
            md3dImmediateContext->RSSetState(0);

        }

        HR(mSwapChain->Present(0, 0));
    }
}

void EofFutureSPHApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x = x;
    mLastMousePos.y = y;

    SetCapture(mhMainWnd);
}

void EofFutureSPHApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();
}

void EofFutureSPHApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0)
    {
        // Make each pixel correspond to a quarter of a degree.
        float dx = DirectX::XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
        float dy = DirectX::XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.
        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0)
    {
        // Make each pixel correspond to 0.2 unit in the scene.
        float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
        float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = MathHelper::Clamp(mRadius, 50.0f, 500.0f);
    }

    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

float EofFutureSPHApp::GetHeight(float x, float z)const
{
    //return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
    return 0.0f;

}

void EofFutureSPHApp::BuildGridGeometryBuffers()
{
    GeometryGenerator::MeshData grid;

    GeometryGenerator geoGen;

    geoGen.CreateGrid(160.0f, 160.0f, 5, 5, grid);

    mGridIndexCount = grid.Indices.size();

    //
    // Extract the vertex elements we are interested and apply the height function to
    // each vertex.  
    //

    std::vector<Vertex::Basic32> vertices(grid.Vertices.size());
    for (size_t i = 0; i < grid.Vertices.size(); ++i)
    {
        DirectX::XMFLOAT3 p = grid.Vertices[i].Position;

        p.y = GetHeight(p.x, p.z);

        vertices[i].Pos = p;

        // Color the vertex based on its height.
        if (p.y < -10.0f)
        {
            // Sandy beach color.
            vertices[i].Color = DirectX::XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
        }
        else if (p.y < 5.0f)
        {
            // Light yellow-green.
            vertices[i].Color = DirectX::XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
        }
        else if (p.y < 12.0f)
        {
            // Dark yellow-green.
            vertices[i].Color = DirectX::XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
        }
        else if (p.y < 20.0f)
        {
            // Dark brown.
            vertices[i].Color = DirectX::XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
        }
        else
        {
            // White snow.
            vertices[i].Color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * grid.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mGridVB));

    //
    // Pack the indices of all the meshes into one index buffer.
    //

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &grid.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mGridIB));
}

void EofFutureSPHApp::BuildParticlesBuffers()
{
    float maxRadius = shps0->state->size*100.f; //magic const
    particleRadius = maxRadius / shps0->state->n;

    float maxHeight = 300.f * shps0->state->max_height;
    layerHeight = maxHeight / shps0->state->layers_count;

    UINT vbSize = shps0->state->n * shps0->state->layers_count;
    std::vector<Vertex::ParticlePointSprite> v(vbSize);
    std::vector<DirectX::XMFLOAT3> particlesMemForTex(vbSize);

    for (UINT col = 0; col < shps0->state->n; ++col)
    {
        DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF(), 0.f);
        for (UINT row = 0; row < shps0->state->layers_count; ++row)
        {
            color.w = (float)row * 1.0f / shps0->state->layers_count;
            //float x = MathHelper::RandF(-80.0f, 80.0f);
            //float z = MathHelper::RandF(-80.0f, 80.0f);

            float x = (shps0->get_x(col, row) - 0.5f)*80.0f;
            float z = (shps0->get_y(col, row) - 0.5f)*80.0f; //from 2d to 3d
            float y = layerHeight * (row+1);
            
            

            // Move tree slightly above land height.
            

            /*float r = MathHelper::RandF(0.0f, 1.0f);
            float g = MathHelper::RandF(0.0f, 1.0f);
            float b = MathHelper::RandF(0.0f, 1.0f);*/


            v[row * shps0->state->n + col].Pos = DirectX::XMFLOAT3(x, y, z);
            v[row * shps0->state->n + col].Color = color; // DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f);
            v[row * shps0->state->n + col].Size = DirectX::XMFLOAT2(particleRadius, particleRadius);

            particlesMemForTex[row * shps0->state->n + col] = DirectX::XMFLOAT3(x, y, z);
        }
    }

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v.data();

    D3D11_BUFFER_DESC vbDesc;
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.ByteWidth = sizeof(Vertex::ParticlePointSprite) * vbSize;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbDesc.MiscFlags = 0;

   
    HR(md3dDevice->CreateBuffer(&vbDesc, &vinitData, &mParticleSpritesVB));

    //Create tex with particle positions
    D3D11_SUBRESOURCE_DATA tinitData;
    tinitData.pSysMem = particlesMemForTex.data();
    tinitData.SysMemPitch = shps0->state->n * sizeof(DirectX::XMFLOAT3);
    tinitData.SysMemSlicePitch = 0;

    //
    // Create the texture.
    //
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width = shps0->state->n;
    texDesc.Height = shps0->state->layers_count;
    texDesc.MipLevels = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    texDesc.Usage = D3D11_USAGE_DYNAMIC;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    texDesc.MiscFlags = 0;
    texDesc.ArraySize = 1;

    mParticlePositionsTex = 0;
    HR(md3dDevice->CreateTexture2D(&texDesc, &tinitData, &mParticlePositionsTex));

    //
    // Create the resource view.
    //
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MipLevels = texDesc.MipLevels;
    viewDesc.Texture2D.MostDetailedMip = 0;

    HR(md3dDevice->CreateShaderResourceView(mParticlePositionsTex, &viewDesc, &mParticlePositions2DSRV));

    


}

void EofFutureSPHApp::DrawParticleSprites(DirectX::CXMMATRIX viewProj)
{
    //
    // Set constants.
    //
    Effects::ParticleSpriteFX->SetViewProj(viewProj);
    Effects::ParticleSpriteFX->SetPosTex(mParticlePositions2DSRV);
    Effects::ParticleSpriteFX->SetParticleCount(shps0->state->n);
    Effects::ParticleSpriteFX->SetLayersCount(shps0->state->layers_count);

    //
    // Set IA stage.
    //
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    md3dImmediateContext->IASetInputLayout(InputLayouts::ParticlePointSprite);

    UINT stride = sizeof(Vertex::ParticlePointSprite);
    UINT offset = 0;

    ID3DX11EffectTechnique* particleTech = nullptr;

    ID3D11RasterizerState*  curRasterState = nullptr;

    switch (mRenderOptions)
    {
    case RenderOptions::Wireframe:
        particleTech = Effects::ParticleSpriteFX->BaseTech;
        curRasterState = RenderStates::WireframeRS;
        break;

    case RenderOptions::Solid:
        particleTech = Effects::ParticleSpriteFX->BaseTech;
        curRasterState = RenderStates::SolidRS;
        break;
    }

    D3DX11_TECHNIQUE_DESC techDesc;
    if (particleTech)
    {
        particleTech->GetDesc(&techDesc);
        for (UINT p = 0; p < techDesc.Passes; ++p)
        {
            md3dImmediateContext->IASetVertexBuffers(0, 1, &mParticleSpritesVB, &stride, &offset);
            particleTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
            md3dImmediateContext->RSSetState(curRasterState);
            md3dImmediateContext->Draw(shps0->state->n * shps0->state->layers_count, 0);
            md3dImmediateContext->RSSetState(0);
        }
    }

}


void EofFutureSPHApp::UpdateParticleBuffers()
{
    D3D11_MAPPED_SUBRESOURCE mappedData, mappedTexture;
    HR(md3dImmediateContext->Map(mParticleSpritesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
    Vertex::ParticlePointSprite* vBuffer = reinterpret_cast<Vertex::ParticlePointSprite*>(mappedData.pData);

    HR(md3dImmediateContext->Map(mParticlePositionsTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTexture));
    DirectX::XMFLOAT3* particlePosTex = reinterpret_cast<DirectX::XMFLOAT3*>(mappedTexture.pData);
    BYTE* pTexelsB = (BYTE*)mappedTexture.pData;


    D3D11_TEXTURE2D_DESC desc;
    mParticlePositionsTex->GetDesc(&desc);

    int rowPitch = mappedTexture.RowPitch;
    const UINT WIDTH = desc.Width;
    const UINT HEIGHT = desc.Height;
    //Update vertex 
    float y = layerHeight;
    for (UINT row = 0; row < shps0->state->layers_count; ++row)
    {
        for (UINT column = 0; column < shps0->state->n; ++column)
        {

            float x = (shps0->get_x(column, row) - 0.5f)*80.0f;
            float z = (shps0->get_y(column, row) - 0.5f)*80.0f; //from 2d to 3d

           

            vBuffer[row * shps0->state->n + column].Pos = DirectX::XMFLOAT3(x, y, z);
            //vBuffer[row * shps0->state->n + column].Color = DirectX::XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f);
            vBuffer[row * shps0->state->n + column].Size = DirectX::XMFLOAT2(particleRadius, particleRadius);

            
            //particlePosTex[rowStart + column] = DirectX::XMFLOAT3(x, y, z);
            FLOAT* pTexetsF = (FLOAT*)pTexelsB;
            pTexetsF[column*3 + 0] = x;
            pTexetsF[column*3 + 1] = y;
            pTexetsF[column*3 + 2] = z;

        }
        pTexelsB += rowPitch;
        y += layerHeight;
    }

    if (frameCount > 50000)
    {
        int tmp  =  frameCount;
    }

    md3dImmediateContext->Unmap(mParticleSpritesVB, 0);
    md3dImmediateContext->Unmap(mParticlePositionsTex, 0);


}

