#pragma once
#include "d3dUtil.h"
#pragma region Effect
class Effect
{
public:
    Effect(ID3D11Device* device, const std::wstring& filename);
    virtual ~Effect();

private:
    Effect(const Effect& rhs);
    Effect& operator=(const Effect& rhs);

protected:
    ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
    BasicEffect(ID3D11Device* device, const std::wstring& filename);
    ~BasicEffect();
    void SetWorldViewProj(DirectX::CXMMATRIX M) {       WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }


    ID3DX11EffectTechnique* BaseTech;

    ID3DX11EffectMatrixVariable* WorldViewProj;


};
#pragma endregion


#pragma region ParticleSpriteEffect
class ParticleSpriteEffect : public Effect
{
public:
    ParticleSpriteEffect(ID3D11Device* device, const std::wstring& filename);
    ~ParticleSpriteEffect();

    void SetViewProj(DirectX::CXMMATRIX M) { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
    void SetParticleCount(int n) { ParticleCount->SetInt(n); }
    void SetLayersCount(float m) { LayersCount->SetInt(m); }

    void SetPosTex(ID3D11ShaderResourceView* tex) { PosTex->SetResource(tex); }

    ID3DX11EffectTechnique* BaseTech;

    ID3DX11EffectMatrixVariable* ViewProj;
    ID3DX11EffectScalarVariable* ParticleCount;
    ID3DX11EffectScalarVariable* LayersCount;
    ID3DX11EffectShaderResourceVariable* PosTex;

};
#pragma endregion

class Effects
{
public:
    static void InitAll(ID3D11Device* device);
    static void DestroyAll();

    static BasicEffect* BasicFX;
    static ParticleSpriteEffect* ParticleSpriteFX;
};
#pragma endregion


