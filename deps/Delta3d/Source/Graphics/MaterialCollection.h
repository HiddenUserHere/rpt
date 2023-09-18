#pragma once

#include "Graphics.h"

#include "../Legacy/LegacyMaterialCollection.h"

namespace Delta3D::Graphics
{
class Material;
class MaterialCollection : public GraphicsImpl
{
public:
	//! Default Constructor for Material Collection.
	MaterialCollection();

	//! Constructor for Material Collection with a specified number of materials.
	MaterialCollection( int materialsCount_ );

	//! Deconstructor.
	~MaterialCollection();

	//! Set Blending Material.
	void SetBlendingMaterial( Material* blendingMaterial, bool useBlendingMap = false );

	//! Build Material Collection.
	bool Build( FILE* file, bool skinned = false, bool loadTextures = true, bool useVertexColor = false, unsigned int mipMapsDefault = 0, bool temporaryTextures = false, bool use3D = false, Legacy::MaterialCollection* legacyMaterialCollection_ = nullptr );
public:
	std::vector<Material*> materials;	//!< Materials List
	Legacy::MaterialCollection* legacyMaterialCollection;
};
}