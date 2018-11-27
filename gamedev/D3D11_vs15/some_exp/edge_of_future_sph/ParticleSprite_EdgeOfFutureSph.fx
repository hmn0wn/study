//***************************************************************************************
// color.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject
{
    float4x4 gViewProj;
};

cbuffer cbPerFrame
{
    int gParticleCount;
    int gLayersCount;

};

Texture2D positionsTex;

struct VertexIn
{
    float3 PosW   : POSITION;
    float4 Color : COLOR;
    float2 SizeW  : SIZE;
};

struct VertexOut
{
    float3 CenterW :POSITION;
    float4 Color : COLOR;
    float2 SizeW  : SIZE;
};


struct GeoOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float4 Color : COLOR;
    uint   PrimID  : SV_PrimitiveID;
};


VertexOut VS(VertexIn vin, uint vertID : SV_VertexID)
{
    VertexOut vout;

    // Just pass data over to geometry shader.
    uint2 ij;

    ij[1] = vertID / gParticleCount ;
    ij[0] = vertID - ij[1] * gParticleCount;

    vout.CenterW = positionsTex[ij];

    vout.CenterW = vin.PosW;
    vout.Color = vin.Color;
    vout.SizeW = vin.SizeW;

    
    


    return vout;
}

[maxvertexcount(4)]
void GS_particle(point VertexOut gin[1],
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream)
{


    //
    // Compute the local coordinate system of the sprite relative to the world
    //
    float3 up    = float3(0.0f, 1.0f, 0.0f);
    float3 right = float3(-1.0f, 0.0f, 0.0f);
    float3 forward = float3(0.0f, 0.0f, 1.0f);


    //
    // Compute triangle strip vertices (quad) in world space.
    //
    float halfWidth = 0.5f*gin[0].SizeW.x;
    float halfHeight = 0.5f*gin[0].SizeW.y;

    float4 v[4];
    v[0] = float4(gin[0].CenterW + halfWidth*right - halfHeight*forward, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth*right + halfHeight*forward, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth*right - halfHeight*forward, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth*right + halfHeight*forward, 1.0f);

    //
    // Transform quad vertices to world space and output 
    // them as a triangle strip.
    //
    GeoOut gout;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        gout.PosH = mul(v[i], gViewProj);
        gout.PosW = v[i].xyz;
        gout.Color = gin[0].Color;
        gout.PrimID = primID;

        triStream.Append(gout);
    }
}

[maxvertexcount(16)]
void GS_volume(point VertexOut gin[1],
    uint primID : SV_PrimitiveID,
    inout TriangleStream<GeoOut> triStream)
{
    if (primID / gParticleCount == gLayersCount - 1)
    {
        return;
    }

    uint2 ij_next;

    ij_next[1] = (primID + gParticleCount) / gParticleCount;
    ij_next[0] = (primID + gParticleCount) - ij_next[1] * gParticleCount;
    //
    float3 NextCenterW = positionsTex[ij_next];
    // Compute the local coordinate system of the sprite relative to the world
    //
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = float3(-1.0f, 0.0f, 0.0f);
    float3 forward = float3(0.0f, 0.0f, 1.0f);


    //
    // Compute triangle strip vertices (quad) in world space.
    //
    float halfWidth = 0.5f*gin[0].SizeW.x;
    float halfHeight = 0.5f*gin[0].SizeW.y;

    float4 v[8];
    v[0] = float4(gin[0].CenterW + halfWidth*right - halfHeight*forward, 1.0f);
    v[1] = float4(gin[0].CenterW + halfWidth*right + halfHeight*forward, 1.0f);
    v[2] = float4(gin[0].CenterW - halfWidth*right - halfHeight*forward, 1.0f);
    v[3] = float4(gin[0].CenterW - halfWidth*right + halfHeight*forward, 1.0f);

    v[4] = float4(NextCenterW + halfWidth*right - halfHeight*forward, 1.0f);
    v[5] = float4(NextCenterW + halfWidth*right + halfHeight*forward, 1.0f);
    v[6] = float4(NextCenterW - halfWidth*right - halfHeight*forward, 1.0f);
    v[7] = float4(NextCenterW - halfWidth*right + halfHeight*forward, 1.0f);

    float4 v_strip[10];
    v_strip[0] = v[0];
    v_strip[1] = v[4];
    v_strip[2] = v[1];
    v_strip[3] = v[5];

    v_strip[4] = v[2];
    v_strip[5] = v[6];
    v_strip[6] = v[3];
    v_strip[7] = v[7];
    
    v_strip[8] = v[0];
    v_strip[9] = v[4];





    //
    // Transform quad vertices to world space and output 
    // them as a triangle strip.
    //
    GeoOut gout;
    [unroll]
    for (int i = 0; i < 10; ++i)
    {
        gout.PosH = mul(v_strip[i], gViewProj);
        gout.PosW = v_strip[i].xyz;
        gout.Color = gin[0].Color;
        gout.PrimID = primID;

        triStream.Append(gout);
    }
}


float4 PS(GeoOut pin) : SV_Target
{
    return pin.Color;
}

technique11 Base
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(CompileShader(gs_5_0, GS_particle()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

