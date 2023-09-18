#include "PrecompiledHeader.h"
#include "MaterialCollection.h"

#include "Material.h"
#include "Texture.h"

namespace Delta3D::Graphics
{
MaterialCollection::MaterialCollection() : 
	GraphicsImpl(), 
	materials{},
	legacyMaterialCollection( nullptr )
{
}

MaterialCollection::MaterialCollection( int materialsCount_ ) : 
	GraphicsImpl(), 
	legacyMaterialCollection( nullptr )
{
	materials.reserve( materialsCount_ );
}

MaterialCollection::~MaterialCollection()
{
	for( auto& material : materials )
		delete material;

	legacyMaterialCollection = nullptr;
	materials.clear();
}

void MaterialCollection::SetBlendingMaterial( Material* blendingMaterial, bool useBlendingMap )
{
	if( materials.size() )
		for( const auto& material : materials )
			material->SetBlendingMaterial( blendingMaterial, useBlendingMap );
}

bool MaterialCollection::Build( FILE* file, bool skinned, bool loadTextures, bool useVertexColor, unsigned int mipMapsDefault, bool temporaryTextures, bool use3D, Legacy::MaterialCollection* legacyMaterialCollection_ )
{
	if( file )
	{
		fseek( file, sizeof( Legacy::MaterialCollection ), SEEK_CUR );

		//Allocate Materials
		legacyMaterialCollection = legacyMaterialCollection_;
		materials.reserve(legacyMaterialCollection->MaterialCount);

		//Read Each Material
		for( unsigned int i = 0; i < legacyMaterialCollection->MaterialCount; i++ )
		{
			auto material = new Material();

			if( auto material = new Material(); material )
			{
				material->Build( file, skinned, loadTextures, useVertexColor, mipMapsDefault, temporaryTextures, use3D, &legacyMaterialCollection->smMaterial[i] );
				materials.push_back( material );
			}
		}

		return true;
	}

	return false;
}
}