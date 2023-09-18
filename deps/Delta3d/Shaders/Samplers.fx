//Samplers
sampler DiffuseMap : register( s0 ) = sampler_state {
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler SelfIlluminationMap : register( s1 );
sampler OverlayMap : register( s2 );



//Normal Map Sampler
texture NormalMapTexture;
sampler NormalMapSampler = sampler_state
{
	Texture = (NormalMapTexture);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//Bones Sampler
texture BonesMap;
sampler BonesMapSampler = sampler_state
{
	Texture   = (BonesMap);
	MipFilter = None;
	MinFilter = Point;
	MagFilter = Point;
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//Shadow Map Sampler
texture ShadowMapTexture1;
sampler ShadowMapSampler1 = sampler_state
{
	Texture = (ShadowMapTexture1);
	MipFilter		= None;
	MinFilter		= Point;
	MagFilter		= Point;
	AddressU		= Clamp;
	AddressV		= Clamp;
};

texture ShadowMapTexture2;
sampler ShadowMapSampler2 = sampler_state
{
	Texture = (ShadowMapTexture2);
	MipFilter		= None;
	MinFilter		= Point;
	MagFilter		= Point;
	AddressU		= Clamp;
	AddressV		= Clamp;
};

texture ShadowMapTexture3;
sampler ShadowMapSampler3 = sampler_state
{
	Texture = (ShadowMapTexture3);
	MipFilter		= None;
	MinFilter		= Point;
	MagFilter		= Point;
	AddressU		= Clamp;
	AddressV		= Clamp;
};

//Noise Map Sampler
texture NoiseMapTexture;
sampler NoiseMapSampler = sampler_state
{
	Texture = (NoiseMapTexture);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
