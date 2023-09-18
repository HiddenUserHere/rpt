float4 AmbientColor;
float3 GlobalLightDirection = float3(1.0f, 1.0f, -0.75f);

float DiffuseLightIntensity = 1.0f;
float SpecularLightIntensity = 1.0f;
float SpecularLightPower = 1.0f;

bool HasParallaxMap = false;

static const float MagicLightingValue = 0.2313725f;
static const float MagicLightingLimitValue = 1.517647058823529f;

struct DynamicLight
{
	float4 pos;
	float4 color;
};

DynamicLight Lights[64];
int NumActiveLights;

float3 ShadowMapSize = float3( 0.0078125, 0.001953125, 0.00048828125 );

static const float ShadowBias1 = 0.0010f;
static const float ShadowBias2 = 0.0005f;
static const float ShadowBias3 = 0.0002f;
static const float ShadowStrength = 0.5f;
static const float ShadowFadeOffset = 50.f;

#define MAX_SHADOW_SAMPLES	32

static const float3 SoftShadowKernels9[9] = {
	{ 0, -1, 0.08125f }, 
	{ -1, 0, 0.08125f }, 
	{ 0, 0, 0.35f }, 
	{ 1, 0, 0.08125f }, 
	{ 0, 1, 0.08125f }, 
	{ -1, -1, 0.08125f }, 
	{ 1, -1, 0.08125f }, 
	{ -1, 1, 0.08125f }, 
	{ 1, 1, 0.08125f },
};

static const float3 SoftShadowKernels5[5] = {
	{ 0, -1, 0.15f }, 
	{ -1, 0, 0.15f }, 
	{ 0, 0, 0.4f }, 
	{ 1, 0, 0.15f }, 
	{ 0, 1, 0.15f }
};


float ShadowOffsets[MAX_SHADOW_SAMPLES];
float ShadowWeights[MAX_SHADOW_SAMPLES];

float DiffuseLightningFromView(const float3 normal, const float3 lightDir)
{
	float product = dot(normal, lightDir);
	float brightness = pow(product * 0.5f + 0.5f, 1.0f);

	return brightness * DiffuseLightIntensity;
}

float4 SpecularLightningFromView(const float4 diffuseColor, const float3 normal, const float3 viewDirection, const float3 globalLight)
{
	float4 specularColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightReflect = normalize(reflect(globalLight, normal));
	float specularFactor = dot(viewDirection, lightReflect);
	if (specularFactor > 0.0f)
	{
		specularFactor = pow(abs(specularFactor), 0.5f);
		specularColor = float4(diffuseColor.rgb * specularFactor, 1.0f);
	}
	
	return specularColor * SpecularLightIntensity;
}

float4 SpecularLightningFromViewNoReflection(const float3 normal, const float3 lightDirection)
{
	float4 specularColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float specularFactor = dot(normal, lightDirection);
	if (specularFactor > 0.0f)
	{
		specularFactor = pow(abs(specularFactor), 1.0f);
		specularColor = float4(specularFactor, specularFactor, specularFactor, 1.0f);
	}
	
	return specularColor * SpecularLightIntensity;
}

float GetSpecularFromPosition(const float3 normal, const float3 viewPosition, const float3 globalLightDirection)
{
	float3 halfVec = normalize(normalize(viewPosition) + globalLightDirection);
    return saturate(pow(dot(normal, halfVec), 0.5f)) * SpecularLightIntensity;
}

float4 ApplyLights(in const float3 pos, float4 color)
{
	if (NumActiveLights > 0) {

		[loop]
		for (int i = 0; i < NumActiveLights; i++) {
			if (Lights[i].pos.w > 0.f) {
				const float dx = pos.x - Lights[i].pos.x;
				const float dy = pos.y - Lights[i].pos.y;
				const float dz = pos.z - Lights[i].pos.z;
				
				const float distanceSq = (dx * dx) + (dy * dy) + (dz * dz);
				
				if (distanceSq < Lights[i].pos.w) {
					const float intensity = ((Lights[i].pos.w - distanceSq) / Lights[i].pos.w) * 0.4f;

					color += (Lights[i].color * intensity);
				}
			}
		}
	}
	
	return color;
}

float4 ApplyLightsLitSolid(in const float3 pos)
{
	float4 finalColor = 0.0f;

	if (NumActiveLights > 0) {
		[loop]
		for (int i = 0; i < NumActiveLights; i++) {
			if (Lights[i].pos.w > 0.f) {
				const float dx = pos.x - Lights[i].pos.x;
				const float dy = pos.y - Lights[i].pos.y;
				const float dz = pos.z - Lights[i].pos.z;
				
				const float distanceSq = (dx * dx) + (dy * dy) + (dz * dz);
				
				if (distanceSq < Lights[i].pos.w) {
					const float intensity = ((Lights[i].pos.w - distanceSq) / Lights[i].pos.w);
					
					finalColor += Lights[i].color * intensity;
				}
			}
		}
	}
	
	return finalColor;
}


float GetShadowSoft9Samples(in sampler2D sm, in const float shadowMapSize, in const float4 lightPosition, in const float shadowBias)
{
	float shadow = 0.f;
	
	//Calculate the projected texture coordinates
	float2 projectTexCoord;
	projectTexCoord.x = (( lightPosition.x / lightPosition.w) / 2.f) + 0.5f;
	projectTexCoord.y = ((-lightPosition.y / lightPosition.w) / 2.f) + 0.5f;
	
	//Determine if the projected coordinates are in the 0 to 1 range. If so then this pixel is in the view of the light
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y)) {
		//Calculate the depth of the light
		const float lightDepthValue = lightPosition.z / lightPosition.w;
		
		if ((lightDepthValue > 0.f) && (lightDepthValue < (1.f - shadowBias))) {
			[unroll]
			for (int i = 0; i < 9; i++) {
				float2 sampleTexCoord;
				sampleTexCoord.x = projectTexCoord.x + (SoftShadowKernels9[i].x * shadowMapSize);
				sampleTexCoord.y = projectTexCoord.y + (SoftShadowKernels9[i].y * shadowMapSize);
				
				//Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location
				const float depthValue = tex2D(sm, sampleTexCoord).r;
				
				//Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel
				//If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it
				if ((depthValue.r > 0.f) && (lightDepthValue >= (depthValue.r + shadowBias))) {
					shadow += SoftShadowKernels9[i].z;
				}
			}
		}
	}
	
	return shadow * ShadowStrength;
}

float GetShadowSoft5Samples(in sampler2D sm, in const float shadowMapSize, in const float4 lightPosition, in const float shadowBias)
{
	float shadow = 0.f;
	
	//Calculate the projected texture coordinates
	float2 projectTexCoord;
	projectTexCoord.x = (( lightPosition.x / lightPosition.w) / 2.f) + 0.5f;
	projectTexCoord.y = ((-lightPosition.y / lightPosition.w) / 2.f) + 0.5f;
	
	//Determine if the projected coordinates are in the 0 to 1 range. If so then this pixel is in the view of the light
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y)) {
		//Calculate the depth of the light
		const float lightDepthValue = lightPosition.z / lightPosition.w;
		
		if ((lightDepthValue > 0.f) && (lightDepthValue < (1.f - shadowBias))) {
			[unroll]
			for (int i = 0; i < 5; i++) {
				float2 sampleTexCoord;
				sampleTexCoord.x = projectTexCoord.x + (SoftShadowKernels5[i].x * shadowMapSize);
				sampleTexCoord.y = projectTexCoord.y + (SoftShadowKernels5[i].y * shadowMapSize);
				
				//Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location
				const float depthValue = tex2D(sm, sampleTexCoord).r;
				
				//Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel
				//If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it
				if ((depthValue.r > 0.f) && (lightDepthValue >= (depthValue.r + shadowBias))) {
					shadow += SoftShadowKernels5[i].z;
				}
			}
		}
	}
	
	return shadow * ShadowStrength;
}

float3 GetShadowPosOffset(in float nDotL, in float3 normal, in float shadowMapSize)
{
	const float OffsetScale = 0.00004f;
	
    float texelSize = 2.0f / shadowMapSize;
    float nmlOffsetScale = saturate(1.0f - nDotL);
    return texelSize * OffsetScale * nmlOffsetScale * normal;
}


float GetShadowHard(in sampler2D sm, in const float4 lightPosition, in const float shadowBias, const float3 normal)
{
	//Calculate the projected texture coordinates
	float2 projectTexCoord;
	projectTexCoord.x = ((( lightPosition.x / lightPosition.w) / 2.f) + 0.5f);
	projectTexCoord.y = (((-lightPosition.y / lightPosition.w) / 2.f) + 0.5f);
	
	//Determine if the projected coordinates are in the 0 to 1 range. If so then this pixel is in the view of the light
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y)) {
		//Calculate the depth of the light
		const float lightDepthValue = (lightPosition.z / lightPosition.w);
		
		if ((lightDepthValue > 0.f) && (lightDepthValue < (1.f - shadowBias))) {
			//Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location
			const float depthValue = tex2D(sm, projectTexCoord).r;
			
			//Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel
			//If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it
			if ((depthValue.r > 0.f) && (lightDepthValue >= (depthValue.r + shadowBias))) {
				return 1.f * ShadowStrength;
			}
		}
	}
	
	return 0.f;
}

float ComputeShadow(in const float clipZ, in const float4 lightPosition, const float3 normal)
{
#ifdef _PS_3_0

	float shadow = GetShadowSoft9Samples(ShadowMapSampler3, ShadowMapSize[2], lightPosition, ShadowBias3);
		
	if( clipZ > 4000.f - ShadowFadeOffset )
		shadow *= (4000.f - clipZ) / ShadowFadeOffset;
		
	return shadow;
/*
	if (clipZ < 300.f) {	
		return GetShadowHard(ShadowMapSampler1, lightPosition1, ShadowBias1, normal);
	} else if (clipZ < 400.f) {
		return GetShadowSoft5Samples(ShadowMapSampler2, ShadowMapSize[1], lightPosition2, ShadowBias2);
	} else if (clipZ < 1000.f) {
		float shadow = GetShadowSoft9Samples(ShadowMapSampler3, ShadowMapSize[2], lightPosition3, ShadowBias3);
		
		
		return shadow;
	}
*/
#endif
	
	return 0.f;
}

float3 ComputeNormalMap(float3 normal, float3 tangent, float3 bitangent, const sampler normalMapSampler, const float2 textureCoordinate)
{
	//Normal Mapping
	if (HasNormalMap)
	{		
		//Normalize Normal, Tangent and Bitangent
		normal = normalize(normal);
		tangent = normalize(tangent);
		bitangent = normalize(bitangent);

		//Get Pixel
	    float3 vTangentNormal = tex2D( normalMapSampler, textureCoordinate ).xyz;
		
		//Convert [0,1] to [-1,1]
	    vTangentNormal = normalize( vTangentNormal * 2.0f - 1.0f );

	    //Compute Tangent, Bitangent and Normal Matrix (TBN), which transform to Tangent Space
	    float3x3 TBN = float3x3( tangent.xyz, bitangent.xyz, normal );

		//Transpose to World
	    TBN = transpose( TBN );

		//New normalized Normal
	    normal = mul( TBN, vTangentNormal );
	}
	
	return normal;
}
