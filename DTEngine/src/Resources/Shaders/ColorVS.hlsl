cbuffer PerFrameBuffer : register(b0)
{
	matrix World2ViewMatrix;
	matrix View2ProjectionMatrix;
};

cbuffer PerObjectBuffer : register(b1)
{
	matrix Model2WorldMatrix;
};

cbuffer ColorPerFrameBuffer : register(b2)
{
	float4 Color;
}

struct VertexInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float2 UVs : TEXCOORD0;
};

struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UVs : TEXCOORD0;
	float4 Color : COLOR;
};

PixelInput main(VertexInput input)
{
	PixelInput output;
	output.Position = mul(float4(input.Position, 1.0f), Model2WorldMatrix);
	output.Position = mul(output.Position, World2ViewMatrix);
	output.Position = mul(output.Position, View2ProjectionMatrix);

	output.Normal = (mul(float4(input.Normal, 0.0f), Model2WorldMatrix)).xyz;

	output.UVs = input.UVs;

	output.Color = Color;
	
	return output;
}