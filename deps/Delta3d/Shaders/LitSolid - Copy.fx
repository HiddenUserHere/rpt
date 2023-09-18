#include "Uniforms.fx"
#include "Samplers.fx"
#include "Lightning.fx"
#include "Transform.fx"
#include "Functions.fx"

float Contrast = 1.0f;

struct VS_INPUT
{
	float4 pos						: POSITION0;
	float3 normal					: NORMAL0;
	float4 color					: COLOR0;
	float4 tangent					: TANGENT;
	float4 bitangent				: BINORMAL;
#ifdef SKINNED		
	float4 blendIndices				: BLENDINDICES;
	float4 blendWeights 			: BLENDWEIGHT;
#endif		
	float2 tex1						: TEXCOORD0;
#ifdef SELFILLUMINATIONMAP		
	float2 tex2						: TEXCOORD1;
#endif
};

struct PS_INPUT
{
	float4 pos						: POSITION0;
	float3 normal					: COLOR0;
	float4 color					: COLOR1;
	float4 colorLight				: COLOR2;
	float2 tex1						: TEXCOORD1;
#ifdef SELFILLUMINATIONMAP		
	float2 tex2						: TEXCOORD2;
#endif		
	float4 viewPosition				: TEXCOORD3;
	float3 viewDirection			: TEXCOORD4;
	float3 tangent					: TEXCOORD5;
	float3 bitangent				: TEXCOORD6;
};

struct PS_INPUT_SHADOW
{
	float4 pos						: POSITION0;
	float2 tex1						: TEXCOORD1;
	float depth						: TEXCOORD2;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output;
	
#ifdef SKINNED
	float4 position = mul(input.pos, GetSkinMatrix(input.blendIndices, input.blendWeights));
	float4 worldPosition = mul( position, World );
	
	float4 normalPosition = mul(input.normal, GetSkinMatrix(input.blendIndices, input.blendWeights));
	float4 normalWorldPosition = mul(normalPosition, World);
	
	float4 tangentPosition = mul(input.tangent, GetSkinMatrix(input.blendIndices, input.blendWeights));
	float4 tangentWorldPosition = mul(tangentPosition, World);
	
	float4 bitangentPosition = mul(input.bitangent, GetSkinMatrix(input.blendIndices, input.blendWeights));
	float4 bitangentWorldPosition = mul(bitangentPosition, World);
#else
	float4 worldPosition = mul(input.pos, World);
	float4 normalWorldPosition = mul(input.normal, World);
	
	float4 tangentWorldPosition = mul(input.tangent, World);
	float4 bitangentWorldPosition = mul(input.bitangent, World);
#endif
	
	float4 viewPosition = mul( worldPosition, View );
	
	//Apply Distortion on Mesh
	if( ApplyDistortionFlag )
		viewPosition += ApplyDistortion(viewPosition, WorldTime);

	output.pos = mul( viewPosition, Projection );
    output.normal = normalWorldPosition;
	output.tangent = tangentWorldPosition;
	output.bitangent = bitangentWorldPosition;

#ifdef VERTEXCOLOR
	output.color = input.color;
#else
	output.color = input.color;
#endif

	//Lightning
	if (NumActiveLights > 0) {
		if (viewPosition.z < FogEnd) {
			output.colorLight = ApplyLightsLitSolid(worldPosition.xyz);
		}
	}

	output.tex1 = input.tex1;
	output.viewPosition = viewPosition;
	output.viewDirection = normalize( CameraEye.xyz - worldPosition.xyz );
	
#ifdef SELFILLUMINATIONMAP
	output.tex2 = input.tex2;
#endif

	if( DiffuseScrolling.x != 0.0f )
	{
		float2 coordTmp = DiffuseScrolling * WorldTime;
		output.tex1.xy = input.tex1.xy + coordTmp;
	
#ifdef SELFILLUMINATIONMAP
		if( ApplyScrollingSelfIllumination )
			output.tex2.xy = input.tex2.xy + coordTmp;
#endif
	}

	return output;
}

PS_INPUT_SHADOW VS_Shadow( VS_INPUT input )
{
	PS_INPUT_SHADOW output;
	
#ifdef SKINNED
	float4 position = mul(input.pos, GetSkinMatrix(input.blendIndices, input.blendWeights));
	float4 worldPosition = mul( position, World );
#else
	float4 worldPosition = mul( input.pos, World );
#endif
	
	float4 viewPosition = mul( worldPosition, View );

	output.pos = mul( viewPosition, Projection );
    output.depth = output.pos.z / output.pos.w;
	output.tex1 = input.tex1;

	return output;
}

float4 PS( PS_INPUT input ) : COLOR0
{	
	//Texture Color
	float2 texCoord = input.tex1.xy;

	float4 textureColor = tex2D(DiffuseMap, texCoord);
	textureColor.rgb /= textureColor.a;
	float blendingIntensity = 1.f;

	//Alpha Mask
	if(textureColor.a < 0.5)
        discard;
	
	//Apply contrast
  	textureColor.rgb = ((textureColor.rgb - 0.5f) * max(Contrast, 0)) + 0.5f;
	
	//Using Blending Map?
	if( UseBlendingMap )
	{
		//Get Luma from Pixel Color
		blendingIntensity = 1.f - float(dot(textureColor.rgb, float3(0.2125, 0.7154, 0.0721)));

		//Contrast Blending Map
		blendingIntensity = saturate(((blendingIntensity - 0.5f) * 2.25f) + 0.5f);
	}

	input.normal = normalize(input.normal);

	float diffuseLight = DiffuseLightningFromView(input.normal, GlobalLightDirection) + DiffuseLightningFromView(input.normal, -GlobalLightDirection);

	float4 specularLight = SpecularLightningFromViewNoReflection(input.normal, GlobalLightDirection) + SpecularLightningFromViewNoReflection(input.normal, -GlobalLightDirection);
	//float specularLight = GetSpecularFromPosition(input.normal, input.viewPosition, GlobalLightDirection) + GetSpecularFromPosition(input.normal, -input.viewPosition, GlobalLightDirection);

	input.normal = ComputeNormalMap(input.normal, input.tangent, input.bitangent, NormalMapSampler, input.tex1.xy);
	
	float4 blendingColor = (UseBlendingMaterial ? (OverlayColor + OverlayBlinkingColor) * blendingIntensity * (UseBlendingMap ? 0.35f : 1.f): 0.0f );

	const float4 clampValueMin = float4(0.6f, 0.6f, 0.6f, 0.6f);
	const float4 clampValueMax = float4(1000.0f, 1000.0f, 1000.0f, 1000.0f);

	float4 diffuse = saturate(input.color) * 0.5f + saturate(input.colorLight) * 0.5f + AddColor * 0.5f;

	//Blending Material
	if( UseBlendingMaterial )
		textureColor.rgb += tex2D(OverlayMap, input.tex1.xy + (OverlayScrolling * WorldTime)).rgb * blendingIntensity * (UseBlendingMap ? 0.65f : 1.f);
	
#ifdef SELFILLUMINATIONMAP
	if( SelfIlluminationBlendingMode == 1 )
		textureColor.rgb += (saturate(tex2D(SelfIlluminationMap, input.tex2.xy)) * SelfIlluminationAmount);
	else
		textureColor.rgb *= (saturate(tex2D(SelfIlluminationMap, input.tex2.xy)) * SelfIlluminationAmount);
#endif

	//Shaded Color
	float4 shadedColor = textureColor;

	shadedColor *= ((diffuseLight + specularLight) + AmbientColor * 0.25f) * diffuse;

	float transparency = DiffuseColor.a - ApplyDistanceFade(input.viewPosition, FogEnd);
	
	//Apply Diffuse Color
	shadedColor *= DiffuseColor;

	//Inner Glow
	if (ApplyInnerGlow) {
		float innerGlowValue = 1.f - saturate(dot(input.viewDirection, input.normal));

		//Apply Contrast to adjust inner glow size
		if (InnerGlowPower > 0.f)
			innerGlowValue = ((innerGlowValue - 0.5f) * max(InnerGlowPower, 0)) + 0.5f;
			
		float4 finalInnerGlowColor = InnerGlowColor;

		//Rainbow
		if( InnerRainbowColor )
			finalInnerGlowColor = float4(ComputeHueToRGB(input.tex1.y + (0.3f * WorldTime)), 1.f);

		shadedColor += (finalInnerGlowColor * innerGlowValue * InnerGlowColor.a);
	}

	//Dissolving Effect
	if (DissolveAmount > 0.f) {
		float4 dissolveColor = tex2D(NoiseMapSampler, input.tex1.xy);

		if (dissolveColor.r < DissolveAmount + 0.06) {
			float delta = DissolveAmount + 0.06 - dissolveColor.r;
			shadedColor.r += delta * 60.0;
			shadedColor.g += delta * 30.0;
		}

		if (dissolveColor.r < DissolveAmount)
            discard;
	}

	shadedColor += blendingColor;

	return float4(shadedColor.rgb,transparency);
}

float4 PS_Shadow( PS_INPUT_SHADOW input ) : COLOR0
{	
	//Texture Color
	float4 textureColor = tex2D(DiffuseMap, input.tex1.xy);
	
	//Alpha Mask
	if(textureColor.a < 0.5)
        discard;
		
	float depthSlopeBias = max(
        abs(ddx(input.depth)), 
        abs(ddy(input.depth)));

	depthSlopeBias += input.depth;

	//Dissolving Effect
	if (DissolveAmount > 0.f) {
		float4 dissolveColor = tex2D(NoiseMapSampler, input.tex1.xy);

		if (dissolveColor.r < DissolveAmount + 0.06) {
			float delta = DissolveAmount + 0.06 - dissolveColor.r;
			depthSlopeBias += delta * 60.0;
		}

		if (dissolveColor.r < DissolveAmount)
            discard;
	}

	return float4( depthSlopeBias, depthSlopeBias, depthSlopeBias, textureColor.a );
}

technique LitSolid
{
	pass P0
	{
		PixelShader = compile ps_3_0 PS();
		VertexShader = compile vs_3_0 VS();
	}
}

technique LitSolid_Shadow
{
	pass P0
	{
		PixelShader = compile ps_3_0 PS_Shadow();
		VertexShader = compile vs_3_0 VS_Shadow();
	}
}