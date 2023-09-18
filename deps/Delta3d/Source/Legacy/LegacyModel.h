#pragma once

#include "../Math/Vector3.h"
#include "../IO/SMD/Frame.h"

#include "LegacyMesh.h"
#include "LegacyMaterialCollection.h"

namespace Delta3D::Legacy
{
class Model
{
public:
	DWORD Head;
	Mesh* obj3d[128];
	BYTE TmSort[128];

	Model* TmParent;

	MaterialCollection* smMaterialGroup;

	int MaxFrame;
	int Frame;

	int SizeWidth, SizeHeight;

	int nObj3d;
	void* hD3DTexture;

	Math::Vector3Int Posi;
	Math::Vector3Int Angle;
	Math::Vector3Int CameraPosi;

	int dBound;
	int Bound;

	IO::SMD::Frame TmFrame[32];
	int TmFrameCnt;

	int TmLastFrame;
	Math::Vector3Int TmLastAngle;

	Graphics::Model * pcNewModel;
};
};