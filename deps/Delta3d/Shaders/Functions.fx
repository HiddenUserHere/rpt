//Constant for Fresnel computation
static const float R0 = 0.02037f;

float4 SplitSpecularFromDiffuse(float4 diffuse)
{
	float4 specular = float4(0.f, 0.f, 0.f, 0.f);
	
	[unroll]
	for (int i = 0; i < 4; i++) {
		if (diffuse[i] > 1.f) {
			specular[i] = diffuse[i] - 1.f;
			diffuse[i] = 1.f;
			
			if (specular[i] > 1.f) {
				specular[i] = 1.f;
			}
		}
	}
	
	return specular;
}

float ApplyDistanceFade(float4 viewPosition, float maxViewDistance)
{
	return clamp((viewPosition.z - maxViewDistance) / 512.f, 0.f, 1.f);
}

float4 ApplyDistortion(float4 position, float time)
{
	float4 distortion;
	distortion.x = sin((position.x * 128.f * 0.0015339807878857f) + time) * 2.f;
	distortion.y = sin((position.y * 128.f * 0.0015339807878857f) + time) * 2.f;
	distortion.z = cos((position.z * 128.f * 0.0015339807878857f) + time) * 2.f;
	distortion.w = 0.f;
	return distortion;
}

float ComputeFresnelTerm(float3 pos, float3 cameraPosition)
{
	//We'll just use the y unit vector for spec reflection.
	float3 up = float3(0, 1, 0);

	//Compute the fresnel term to blend reflection and refraction maps
	float angle = saturate(dot(-pos, up));
	float f = R0 + (1.0f - R0) * pow(1.0f - angle, 5.0);

	//Also blend based on distance
	f = min(1.0f, f + 0.007f * cameraPosition.y);
	
	return f;
}

float3 ComputeHueToRGB(float hue) 
{
    hue = frac(hue); //only use fractional part of hue, making it loop
    float r = abs(hue * 6 - 3) - 1; //red
    float g = 2 - abs(hue * 6 - 2); //green
    float b = 2 - abs(hue * 6 - 4); //blue
    float3 rgb = float3(r,g,b); //combine components
    rgb = saturate(rgb); //clamp between 0 and 1
    return rgb;
}

float4 BlendColor(float4 color1, float4 color2, int ColorBlendOp, int AlphaBlendOp)
{
	float3 color;
	float alpha;
	
	if (ColorBlendOp == 4) {
		color = float3(color1.rgb) * float3(color2.rgb);
	} else if (ColorBlendOp == 7) {
		color = float3(color1.rgb) + float3(color2.rgb);
	} else if (ColorBlendOp == 13) {
		color = (float3(color1.rgb) * color1.a) + (float3(color2.rgb) * (1.f - color1.a));
	} else {
		color = float3(color1.rgb) * float3(color2.rgb);
	}
	
	if (AlphaBlendOp == 4) {
		alpha = color1.a * color2.a;
	} else if (AlphaBlendOp == 15) {
		alpha = color1.a + (color2.a * (1.f - color1.a));
	} else {
		alpha = color1.a * color2.a;
	}
	
	return float4(color, alpha);
}
