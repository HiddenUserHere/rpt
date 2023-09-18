#define MAX_BONES 			128

static const float BonesColumnsPerRowCount = 4.0f;
static const float BonesTexFetchStep = 1.0f / (BonesColumnsPerRowCount * MAX_BONES);


#ifdef SKINNED

float4x4 GetMatrixBlend(float blendIndice, float blendWeight)
{
	float fColumn 	= (float)((int)blendIndice % MAX_BONES);
	float4 c0 = tex2Dlod(BonesMapSampler, float4(((fColumn * BonesColumnsPerRowCount) + 0.5f) * BonesTexFetchStep, 0.5f, 0, 0));
	float4 c1 = tex2Dlod(BonesMapSampler, float4(((fColumn * BonesColumnsPerRowCount) + 1.5f) * BonesTexFetchStep, 0.5f, 0, 0));
	float4 c2 = tex2Dlod(BonesMapSampler, float4(((fColumn * BonesColumnsPerRowCount) + 2.5f) * BonesTexFetchStep, 0.5f, 0, 0));
	float4 c3 = tex2Dlod(BonesMapSampler, float4(((fColumn * BonesColumnsPerRowCount) + 3.5f) * BonesTexFetchStep, 0.5f, 0, 0));

	return float4x4(c0.x, c1.x, c2.x, 0.f,
					c0.y, c1.y, c2.y, 0.f,
					c0.z, c1.z, c2.z, 0.f,
					c0.w, c1.w, c2.w, 1.f) * blendWeight;
}

float4x4 GetSkinMatrix(float4 blendIndices, float4 blendWeights)
{
	float4x4 mResult = GetMatrixBlend(blendIndices.x, blendWeights.x) + GetMatrixBlend(blendIndices.y, blendWeights.y) +
					   GetMatrixBlend(blendIndices.z, blendWeights.z) + GetMatrixBlend(blendIndices.w, blendWeights.w);
	
	return mResult;
}

float4 GetTransformedPosition(float4 pos, float4 blendIndices, float4 blendWeights)
{
	float4 position = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
	float faBlendWeights[4] = (float[4])blendWeights;
    float faBlendIndices[4] = (float[4])blendIndices;
    
	int iBoneCount = 0;
	for( int i = 0; i < 4; i++ )
		if ( (int)faBlendIndices[i] >= 0 )
			iBoneCount++;

	float fLastWeight = 1.0f;
    for ( int i = 0; i < iBoneCount-1; i++ )
    {
        fLastWeight -= faBlendWeights[i];
        
        position += mul(pos, GetMatrixBlend(faBlendIndices[i], 1.0f)) * faBlendWeights[i];
    }

    position += mul(pos, GetMatrixBlend(faBlendIndices[iBoneCount-1], 1.0f)) * fLastWeight;
	
	return position;
}

#endif