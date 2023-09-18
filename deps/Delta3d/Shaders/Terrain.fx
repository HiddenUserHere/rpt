#include "Uniforms.fx"
#include "Samplers.fx"
#include "Lightning.fx"
#include "Transform.fx"
#include "Functions.fx"

float Contrast = 1.0f;
float Brightness = 0.1f;

struct VS_INPUT
{
	float4 pos						: POSITION0;
	float3 normal					: NORMAL0;
	float4 color					: COLOR0;
	float2 tex1						: TEXCOORD0;
	float2 tex2						: TEXCOORD1;
	float3 tangent					: TEXCOORD2;
	float3 bitangent				: TEXCOORD3;
};

struct PS_INPUT
{
	float4 pos						: POSITION0;
	float3 normal					: NORMAL;
	float4 viewPosition				: TEXCOORD1;
	float4 lightPosition			: TEXCOORD2;
	float2 tex1						: TEXCOORD3;
	float2 tex2						: TEXCOORD4;
	float3 viewDirection			: TEXCOORD5;
	float4 color					: COLOR0;
	float4 colorLight				: COLOR2;
	float3 tangent					: TEXCOORD6;
	float3 bitangent				: TEXCOORD7;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;

	float4 viewPosition = mul(input.pos, View);

	output.pos = mul( viewPosition, Projection );
	output.normal = input.normal;
	output.tangent = input.tangent;
	output.bitangent = input.bitangent;
	output.viewPosition = viewPosition;
	output.lightPosition =  mul(mul(input.pos, LightView), LightProjection);
	output.tex1 = input.tex1;
	output.tex2 = input.tex2;
	output.color = input.color;
	
//	output.normal = (dot(output.normal,input.pos.xyz) < 0.0f) ? output.normal : -output.normal;
	
	output.viewDirection = normalize( CameraEye.xyz - input.pos.xyz );
	
	// float bias = max(0.05 * (1.0 - dot(InNorm.xyz, LightDir)), 0.005);

	//Lightning
	if (NumActiveLights > 0) {
		output.colorLight = ApplyLights(input.pos.xyz, 0.0f);
	}
	
	return output;
}

float4 PS( PS_INPUT input ) : COLOR0
{
	float4 textureColor1 = tex2D(DiffuseMap, input.tex1.xy);
	float4 textureColor2 = tex2D(SelfIlluminationMap, input.tex2.xy);

	textureColor1.rgb /= textureColor1.a;

	//Alpha Mask
	if(textureColor1.a < 0.5)
        discard;
		
	const float4 clampValueMin = float4(0.5f, 0.5f, 0.5f, 0.5f);
	const float4 clampValueMax = float4(1000.0f, 1000.0f, 1000.0f, 1000.0f);

	float4 diffuse = input.color * 0.5f + AddColor + input.colorLight;

	float4 textureColor = textureColor1;

	//Self Illumination
	if( SelfIlluminationAmount > 0.f )
		textureColor = BlendColor(textureColor2, textureColor1, Tex2ColorBlendOp, Tex2AlphaBlendOp);
	
	//Shaded Color
	float4 shadedColor = textureColor;

	input.normal = normalize(input.normal);

	float diffuseLight = DiffuseLightningFromView(input.normal, GlobalLightDirection) + DiffuseLightningFromView(input.normal, -GlobalLightDirection);

	float4 specularLight = SpecularLightningFromViewNoReflection(input.normal, GlobalLightDirection) + SpecularLightningFromViewNoReflection(input.normal, -GlobalLightDirection);

	input.normal = ComputeNormalMap(input.normal, input.tangent, input.bitangent, NormalMapSampler, input.tex1.xy);
	
	shadedColor *= ((diffuseLight + specularLight) + AmbientColor * 0.25f) * diffuse;

	//Shadow
	if (shadedColor.a > 0.1f) {
		const float shadow = ComputeShadow(input.viewPosition.z, input.lightPosition, input.normal);
		
		if (shadow > 0.f) 
		{
			shadedColor.rgb *= 1.f - shadow;
		}
	}

	//Distance Fade
	float transparency = DiffuseColor.a - ApplyDistanceFade(input.viewPosition, FogEnd);
	
	//Apply Diffuse Color
	shadedColor.rgb *= DiffuseColor.rgb;
	
	//Apply contrast
  	shadedColor.rgb = ((shadedColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;

	//Apply brightness
	shadedColor.rgb += Brightness;

	//Apply Alpha
	shadedColor.rgb *= textureColor1.a;

	return float4(shadedColor.rgb,transparency);
}

technique Terrain
{
	pass P0
	{
		PixelShader = compile ps_3_0 PS();
		VertexShader = compile vs_3_0 VS();
	}
}