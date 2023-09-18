#pragma once

#include "LegacyMaterial.h"

namespace Delta3D::Legacy
{
class MaterialCollection
{
public:
	DWORD	Head;
	Material* smMaterial;
	DWORD MaterialCount;

	int ReformTexture;

	int MaxMaterial;

	int LastSearchMaterial;
	char szLastSearchName[64];
};
};