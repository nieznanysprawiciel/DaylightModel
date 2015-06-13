// Shadery dla kopuły nieba używającej modelu Hoseka i Wilkiego

// Bufory i tak zostaną przekazane
cbuffer ConstantPerFrame : register( b0 )
{
	matrix View;
	matrix Projection;
	float3 LightDir[2];
	float3 LightColor[2];
	float3 AmbientLight;
	float time;
	float time_lag;
}

cbuffer ConstantPerMesh : register( b1 )
{
	matrix World;
	float4 mesh_scale;
	float4 Diffuse;
	float3 Ambient;
	float3 Specular;
	float3 Emissive;
	float Power;
}

cbuffer SkyDomeConstants : register( b2 )
{
	matrix dither_mask;
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Color : COLOR;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
// Potrzebujemy zmodyfikowanej macierzy widoku, bo nie obchodzi nas translacja kamery,
// a jedynie jej obrót. Żeby nie zmieniać macierzy widoku, to podamy ją w macierzy świata,
// która nie jest nam potrzebna, bo i tak wolimy nie ruszać kopuły.
//--------------------------------------------------------------------------------------
PS_INPUT vertex_shader( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, Projection );
    output.Color = input.Color;
	
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float3 pixel_shader( PS_INPUT input) : SV_Target
{
	int2 pixelPos = input.Pos - float2( 0.5, 0.5 );
	pixelPos = pixelPos % int2( 4, 4 );
	
	float3 weight = dither_mask[pixelPos.x][pixelPos.y].xxx;
	weight = weight * float( 1.0 / 15 ).xxx;
	
    return input.Color + (input.Color * weight);
}