struct PixelInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 uvs : TEXCOORD0;
	float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
	return input.color;
}