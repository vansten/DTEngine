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
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInput main(VertexInput input)
{
	PixelInput output;
	output.position = mul(float4(input.position, 1.0f), model2WorldMatrix);
	output.position = mul(output.position, world2ViewMatrix);
	output.position = mul(output.position, view2ProjectionMatrix);

	output.color = color;
	
	return output;
}