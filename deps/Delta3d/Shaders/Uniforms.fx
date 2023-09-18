//Render Scene
uniform float WorldTime;
uniform float FogEnd;
uniform float2 ScreenSize;

//Scene Transformations
uniform float4x4 World;
uniform float4x4 View;
uniform float4x4 Projection;

#ifdef REFLECTION
uniform float4x4 ReflectionView;
#endif

//Lights
uniform float4x4 LightProjection;
uniform float4x4 LightView;

//Camera
uniform float3 CameraEye;
uniform float3 CameraLookAt;
uniform float3 CameraUp;
uniform float3 CameraDirection;

//Materials
uniform int SelfIlluminationBlendingMode;
uniform bool ApplyScrollingSelfIllumination;
uniform bool UseBlendingMaterial;
uniform bool UseBlendingMap;
uniform bool ApplyDistortionFlag;
uniform float4 DiffuseColor;
uniform float2 DiffuseScrolling;
uniform float4 AddColor;
uniform float4 OverlayColor;
uniform float4 OverlayBlinkingColor;
uniform float2 OverlayScrolling;
uniform float DissolveAmount;

uniform bool ApplyInnerGlow;
uniform bool InnerRainbowColor;
uniform float InnerGlowPower;
uniform float4 InnerGlowColor;

uniform bool UseDepthMap;
uniform float SelfIlluminationAmount;

uniform int MagicValue;

uniform bool HasNormalMap;

//Blending
int Tex1ColorBlendOp;
int Tex2ColorBlendOp;

int Tex1AlphaBlendOp;
int Tex2AlphaBlendOp;