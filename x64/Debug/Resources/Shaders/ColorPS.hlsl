struct PixelInput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 UVs : TEXCOORD0;
	float4 Color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET
{
	return input.Color;
}