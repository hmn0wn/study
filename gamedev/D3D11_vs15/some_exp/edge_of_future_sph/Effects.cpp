
#include "Effects.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
    : mFX(0)
{
    std::ifstream fin(filename, std::ios::binary);

    fin.seekg(0, std::ios_base::end);
    int size = (int)fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    std::vector<char> compiledShader(size);

    fin.read(&compiledShader[0], size);
    fin.close();

    HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
        0, device, &mFX));
}

Effect::~Effect()
{
    ReleaseCOM(mFX);
}
#pragma endregion


#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
    : Effect(device, filename)
{
    BaseTech = mFX->GetTechniqueByName("Base");

    WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();

}


BasicEffect::~BasicEffect()
{
}
#pragma endregion



#pragma region ParticleSpriteEffect
ParticleSpriteEffect::ParticleSpriteEffect(ID3D11Device* device, const std::wstring& filename)
    : Effect(device, filename)
{
    BaseTech = mFX->GetTechniqueByName("Base");

    ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
    ParticleCount = mFX->GetVariableByName("gParticleCount")->AsScalar();
    LayersCount = mFX->GetVariableByName("gLayersCount")->AsScalar();

    PosTex = mFX->GetVariableByName("positionsTex")->AsShaderResource();

}


ParticleSpriteEffect::~ParticleSpriteEffect()
{
}
#pragma endregion


#pragma region Effects

BasicEffect*      Effects::BasicFX = 0;
ParticleSpriteEffect* Effects::ParticleSpriteFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
    BasicFX = new BasicEffect(device, L"FX/Basic_EdgeOfFutureSph.cso");
    ParticleSpriteFX = new ParticleSpriteEffect(device, L"FX/ParticleSprite_EdgeOfFutureSph.cso");
}

void Effects::DestroyAll()
{
    SafeDelete(BasicFX);
    SafeDelete(ParticleSpriteFX);
}
#pragma endregion