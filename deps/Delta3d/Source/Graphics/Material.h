#pragma once

#include "Graphics.h"
#include "Shader.h"
#include "Renderer.h"

#include "../Math/Vector3.h"
#include "../Resource/AttributeAnimation.h"

#include "../Legacy/LegacyMaterial.h"

namespace Delta3D::Graphics
{

class Texture;
class Material : public GraphicsImpl
{
public:
	enum TextureTransform
	{
		None,

		Scrolling = 4,
		Reflex = 5,
		Scrolling2x = 6,
        Scrolling4x = 8,
		NormalMapping = 19,
		Displacement = 20,
	};

	enum MeshTransform
	{
		Water = 0x200,
	};

	//! Default Constructor for Material.
	Material() : 
		GraphicsImpl(), 
		effect( nullptr ), 
		blendingMaterial( nullptr ), 
		useBlendingMaterial( false ), 
		selfIllumBlendingMode( 0 ), 
		addColor( 0.0f, 0.0f, 0.0f, 0.0f ), 
		useInnerGlow( false ),
		rainbowInnerGlow( false ),
		innerGlowPower( 0.f ),
		innerGlowColor( Math::Color::White ),
		legacyMaterial( nullptr ),
		customMaterial( false ),
		iHasNormalMap( -1 ),
		iHasDisplacementMap( -1 )
	{
	}

	//! Deconstructor.
	~Material() { if( customMaterial ) delete legacyMaterial; legacyMaterial = nullptr; textures.clear(); animatedTextures.clear(); attributeAnimations.clear(); }

	//! Prepare Blending Material.
	void PrepareBlendingMaterial();

	//! Prepare Material to be used on Renderer.
	bool Prepare();

	//! Apply Material to Device.
	void Apply();

	//! Build Material.
	bool Build( FILE* file, bool skinned, bool loadTextures = true, bool useVertexColor = false, unsigned int mipMapsDefault = 0, bool temporaryTextures = false, bool use3D = false, Legacy::Material* legacyMaterial_ = nullptr );

	//! Load Material from XML File.
	bool Load( const std::string& filePath, bool defaultSettings = false, bool use3D = false );

	//! Set a Blending Material.
	void SetBlendingMaterial( Material* material, bool forceUseBlendingMap = false );

	//! Effect Getter.
	std::shared_ptr<Shader> GetEffect() const { return effect; }
public:
	bool useBlendingMaterial;	//!< Flag to determinate if Material will be blended with other
	int selfIllumBlendingMode;

	int iHasNormalMap;
	int iHasDisplacementMap;

	std::vector<std::shared_ptr<Texture>> textures;
	std::vector<std::shared_ptr<Texture>> animatedTextures;
	std::vector<std::shared_ptr<Resource::AttributeAnimation>> attributeAnimations;

	Material* blendingMaterial;	//!< Extra Material to combine with other (blending)
	bool useBlendingMap;	//!< Use Blending Map
	bool useInnerGlow;	//!< Use Inner Glow
	bool rainbowInnerGlow;	//!< Rainbow Inner Glow
	float innerGlowPower;	//!< Inner Glow Power
	Math::Color innerGlowColor;	//!< Inner Glow Color
	Math::Color addColor;	//!< Add Color

	bool customMaterial;

	std::shared_ptr<Shader> effect;

	Legacy::Material* legacyMaterial;
};
}