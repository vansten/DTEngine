cbuffer PerFrameBuffer : register(b0)
{
	matrix world2ViewMatrix;
	matrix view2ProjectionMatrix;
};

cbuffer PerObjectBuffer : register(b1)
{
	matrix model2WorldMatrix;
};

cbuffer ColorPerFrameBuffer : register(b2)
{
	float4 color;
}

struct VertexInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 uvs : TEXCOORD0;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uvs : TEXCOORD0;
	float4 color : COLOR;
};

PixelInput main(VertexInput input)
{
	PixelInput output;
	output.position = mul(float4(input.position, 1.0f), model2WorldMatrix);
	output.position = mul(output.position, world2ViewMatrix);
	output.position = mul(output.position, view2ProjectionMatrix);

	output.normal = (mul(float4(input.normal, 0.0f), model2WorldMatrix)).xyz;

	output.uvs = input.uvs;

	output.color = color;
	
	return output;
}