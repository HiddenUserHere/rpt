#pragma once

namespace Delta3D::Legacy
{
struct Material
{
	DWORD InUse;
	DWORD TextureCounter;
	void* smTexture[8];

	DWORD TextureStageState[8];
	DWORD TextureFormState[8];
	int ReformTexture;

	int MapOpacity;

	DWORD TextureType;
	DWORD BlendType;

	DWORD Shade;
	DWORD TwoSide;
	DWORD SerialNum;

	float DiffuseColor[3];
	float Transparency;
	float SelfIllum;

	int TextureSwap;
	int MatFrame;
	int TextureClip;

	int UseState;
	int MeshState;
	int WindMeshBottom;

	void* smAnimTexture[32];
	DWORD AnimTexCounter;
	DWORD FrameMask;
	DWORD Shift_FrameSpeed;
	DWORD AnimationFrame;
};
};