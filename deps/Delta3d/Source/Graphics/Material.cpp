#include "PrecompiledHeader.h"
#include "Material.h"

#include "Texture.h"

namespace Delta3D::Graphics
{

const std::vector<std::string> materialType = { "DIFFUSEMAP", "SELFILLUMINATIONMAP" };
const std::vector<std::string> techniqueType = { "LitSolid", "LitSolid_Shadow" };

bool Material::Prepare()
{
	//Prepare Material effect based on Current Scene
	if( renderer->Prepare( effect ) )
	{
		Math::Color finalDiffuseColor;
		Math::Vector2 finalDiffuseTranslation;

		//Material Settings
		finalDiffuseColor.a = 1.0f - legacyMaterial->Transparency;

		//Apply Attribute Animations for Material
		for( const auto& attributeAnimation : attributeAnimations )
		{
			if( attributeAnimation->GetType() == Resource::AttributeAnimationType::Color )
				Color::Blend( finalDiffuseColor, finalDiffuseColor, attributeAnimation->GetValue().color_ );
			else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::ScrollUV )
				finalDiffuseTranslation = attributeAnimation->GetValue().vector2_;
		}
		
		//Set Material for Shader
		if( effect )
		{
			effect->SetInt( "SelfIlluminationBlendingMode", selfIllumBlendingMode );
			effect->SetBool( "UseBlendingMaterial", useBlendingMaterial );
			effect->SetBool( "UseBlendingMap", useBlendingMap );
			effect->SetFloatArray( "DiffuseColor", &finalDiffuseColor.r, 4 );
			effect->SetFloatArray( "DiffuseScrolling", &finalDiffuseTranslation.x, 2 );
			effect->SetFloatArray( "GlobalLightDirection", renderer->GetGlobalLightDirection().f, 3 );
			effect->SetFloatArray( "AddColor", &addColor.r, 4 );
            effect->SetFloat( "DiffuseLightIntensity", renderer->GetDiffuseLightIntensity() );
            effect->SetFloat( "SpecularLightIntensity", renderer->GetSpecularLightIntensity() );
            effect->SetFloat( "SpecularLightPower", renderer->GetSpecularLightPower() );

			if ( renderer->IsRenderingShadowMap() )
			{
                effect->SetFloatArray( "ShadowOffsets", renderer->GetShadowOffsets(), MAX_SHADOWMAP_SAMPLES );
                effect->SetFloatArray( "ShadowWeights", renderer->GetShadowWeights(), MAX_SHADOWMAP_SAMPLES );
			}

			effect->SetTechnique( techniqueType[renderer->IsRenderingShadowMap() ? 1 : 0] );

			//Check if Self Illumination Map has scrolling
			if( legacyMaterial->TextureFormState[1] == TextureTransform::Scrolling || legacyMaterial->TextureFormState[1] == TextureTransform::Scrolling2x || legacyMaterial->TextureFormState[1] == TextureTransform::Scrolling4x )
				effect->SetBool( "ApplyScrollingSelfIllumination", true );
			else
				effect->SetBool( "ApplyScrollingSelfIllumination", false );

			iHasNormalMap = -1;
			iHasDisplacementMap = -1;
			for ( int i = 0; i < _countof( legacyMaterial->TextureFormState ); i++ )
			{
				if ( legacyMaterial->TextureFormState[i] == TextureTransform::NormalMapping )
					iHasNormalMap = i;
				else if ( legacyMaterial->TextureFormState[i] == TextureTransform::Displacement )
					iHasDisplacementMap = i;
			}

			if ( (iHasNormalMap >= 0) && (iHasNormalMap < (int)textures.size()) )
				effect->SetTexture( "NormalMapTexture", textures[iHasNormalMap]->Get() );

			if ( (iHasDisplacementMap >= 0) && (iHasDisplacementMap < (int)textures.size()) )
				effect->SetTexture( "ParallaxMapTexture", textures[iHasDisplacementMap]->Get() );


			bool bNormalMapUse = iHasNormalMap >= 0 ? true : false;
			if ( bNormalMapUse )
				bNormalMapUse = renderer->GetUseTextureEffects();

			effect->SetBool( "HasNormalMap", bNormalMapUse );

			bool bDisplacementMapUse = iHasDisplacementMap >= 0 ? true : false;
			if ( bDisplacementMapUse )
				bDisplacementMapUse = renderer->GetUseTextureEffects();

			effect->SetBool( "HasParallaxMap", bDisplacementMapUse );

			//Has Self Illumination Map?
			if ( textures.size() >= 2 )
			{
				bool bSelfIllum = true;
				if ( textures.size() == 2 )
					if ( (legacyMaterial->TextureFormState[1] == TextureTransform::NormalMapping) || (legacyMaterial->TextureFormState[1] == TextureTransform::Displacement) )
						bSelfIllum = false;

				if ( bSelfIllum )
					effect->SetFloat( "SelfIlluminationAmount", 1.f - (legacyMaterial->SelfIllum == 1.f ? 0.f : legacyMaterial->SelfIllum) );
				else
					effect->SetFloat( "SelfIlluminationAmount", 0.0f );
			}

			//Inner Glow
			renderer->SetInnerGlow( useInnerGlow, innerGlowColor, innerGlowPower, rainbowInnerGlow );
			effect->SetBool( "ApplyInnerGlow", false );

			//Prepare Blending Material
			if( blendingMaterial && useBlendingMaterial )
				PrepareBlendingMaterial();

			//Commit Changes to Effect
			effect->CommitChanges();
		}

		//Apply Material to Device
		Apply();	

		return true;
	}

	return false;
}

void Material::PrepareBlendingMaterial()
{
	if( blendingMaterial == nullptr )
		return;

	if( blendingMaterial->attributeAnimations.empty() )
		return;

	Math::Color finalDiffuseColor( 0.0f, 0.0f, 0.0f, 0.0f );
	Math::Color finalBlinkingColor( 0.0f, 0.0f, 0.0f, 0.0f );
	Math::Vector2 finalDiffuseTranslation( 0.0f, 0.0f );

	//Apply Attribute Animations for Material
	for( const auto& attributeAnimation : blendingMaterial->attributeAnimations )
	{
		if ( attributeAnimation->GetType() == Resource::AttributeAnimationType::Color )
			Color::Blend( finalDiffuseColor, finalDiffuseColor, attributeAnimation->GetValue().color_ );
		else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::BlinkingColor )
			finalBlinkingColor = attributeAnimation->GetValue().color_;
		else if( attributeAnimation->GetType() == Resource::AttributeAnimationType::ScrollUV )
			finalDiffuseTranslation = attributeAnimation->GetValue().vector2_;
	}

	//Set to Effect
	if( effect )
	{
		effect->SetFloatArray( "OverlayColor", &finalDiffuseColor.r, 4 );
		effect->SetFloatArray( "OverlayBlinkingColor", &finalBlinkingColor.r, 4 );
		effect->SetFloatArray( "OverlayScrolling", &finalDiffuseTranslation.x, 2 );
	}

	//Inner Glow
	renderer->SetInnerGlow( blendingMaterial->useInnerGlow, blendingMaterial->innerGlowColor, blendingMaterial->innerGlowPower, blendingMaterial->rainbowInnerGlow );

	//TO-DO: Fix -> Set Inner Glow Color 
	effect->SetBool( "ApplyInnerGlow", blendingMaterial->useInnerGlow );
	effect->SetBool( "InnerRainbowColor", blendingMaterial->rainbowInnerGlow );
	effect->SetFloat( "InnerGlowPower", blendingMaterial->innerGlowPower );
	effect->SetFloatArray( "InnerGlowColor", &blendingMaterial->innerGlowColor.r, 4 );
}

void Material::Apply()
{
	//Animated Texture?
	if( legacyMaterial->TextureType && animatedTextures.size() )
	{
		//Loop
		if( legacyMaterial->AnimationFrame == 0x100 )
			legacyMaterial->AnimationFrame = 0;

		//Step texture frame
		legacyMaterial->AnimationFrame = (int)(16.0f * renderer->GetWorldTime()) % animatedTextures.size();

		//Set texture frame
		if( legacyMaterial->AnimationFrame >= 0 && legacyMaterial->AnimationFrame < (int)animatedTextures.size() )
			Graphics::GetInstance()->GetDevice()->SetTexture( 0, animatedTextures[legacyMaterial->AnimationFrame]->Get() );
	}
	else
	{
		for( int i = 0; i < 8; i++ )
		{
			Graphics::GetInstance()->GetDevice()->SetTexture( i, nullptr );

			//Set Texture
			if( (i < (int)textures.size()) && textures[i] && (i != iHasNormalMap) )
			{
				Graphics::GetInstance()->GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
				Graphics::GetInstance()->GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
				Graphics::GetInstance()->GetDevice()->SetTexture( i, textures[i]->Get() );
			}
		}

		//Set Blending Material for Device
		if( useBlendingMaterial && blendingMaterial )
			if( !blendingMaterial->textures.empty() )
				Graphics::GetInstance()->GetDevice()->SetTexture( 2, blendingMaterial->textures[0]->Get() );
	}

	//Reset Render State
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DBLEND_DESTALPHA );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	//Texture Blending
	graphics->StateBlockBegin( (StateBlock)(legacyMaterial->BlendType + StateBlock::Blend_None) );

	//Two Sided
	if( legacyMaterial->TwoSide )
		Graphics::GetInstance()->Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	else
		Graphics::GetInstance()->Graphics::GetInstance()->GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

bool Material::Build( FILE* file, bool skinned, bool loadTextures, bool useVertexColor, unsigned int mipMapsDefault, bool temporaryTextures, bool use3D, Legacy::Material* legacyMaterial_ )
{
	if( file )
	{
		unsigned int texturesCount = 0;
		unsigned int animTexturesCount = 0;

		fseek( file, sizeof( Legacy::Material ), SEEK_CUR );

		legacyMaterial = legacyMaterial_;
		texturesCount = legacyMaterial->TextureCounter;
		animTexturesCount = legacyMaterial->AnimTexCounter;

		if( loadTextures == true )
		{	
			std::vector<std::string> attributeAnimationsFile;
			std::vector<ShaderDefine> defines;

			if( legacyMaterial->InUse )
			{
				//Read Static and Animated Textures
				if( texturesCount != -1 && animTexturesCount != -1 )
				{
					int  stringLength = 0;
					char materialName[4096] = { 0 };

					//Read String Name and Length
					fread( &stringLength, sizeof( int ), 1, file );
					fread_s( materialName, sizeof( materialName ), stringLength, 1, file );

					char* curMaterialName = materialName;

					//Read Textures from Material
					for( unsigned int j = 0; j < texturesCount; j++ )
					{
						//Blending Mode
						if( legacyMaterial->TextureStageState[j] == D3DTOP_ADD )
							selfIllumBlendingMode = 1;

						//Texture Name
						char* name = curMaterialName;
						curMaterialName += strlen( name ) + 1;
						curMaterialName += strlen( curMaterialName ) + 1;

						//Add Texture to Texture Handler
						auto texture = graphics->GetTextureFactory()->Create( name, false, false, mipMapsDefault, temporaryTextures, use3D ? graphics->reduceQualityTexture : 0 );

						if( texture )
						{
							if ( j >= 0 && j < materialType.size() )
							{
								bool bCanAdd = true;
								if ( (j == 1) && ((legacyMaterial->TextureFormState[j] == TextureTransform::NormalMapping) || (legacyMaterial->TextureFormState[j] == TextureTransform::Displacement)) )
									bCanAdd = false;

								if ( bCanAdd )
									defines.push_back( ShaderDefine{materialType[j].c_str(), "1"} );
							}

							textures.push_back( texture );
						}
					}

					//Read Animated Textures from Material
					for( unsigned int j = 0; j < animTexturesCount; j++ )
					{
						//Texture Name
						char* name = curMaterialName;
						curMaterialName += strlen( name ) + 1;
						curMaterialName += strlen( curMaterialName ) + 1;

						//Add Texture to Texture Handler
						auto texture = graphics->GetTextureFactory()->Create( name, false, false, mipMapsDefault, temporaryTextures, use3D ? graphics->reduceQualityTexture : 0 );

						if( texture )
							animatedTextures.push_back( texture );
					}
				}
			}

			//Texture Transform
			for( int i = 0; i < _countof( legacyMaterial->TextureFormState ); i++ )
			{
				//Ignore Texture Transform Reflex
                if ( legacyMaterial->TextureFormState[i] == TextureTransform::Reflex )
                    continue;
				if ( legacyMaterial->TextureFormState[i] == TextureTransform::NormalMapping )
					continue;
				if ( legacyMaterial->TextureFormState[i] == TextureTransform::Displacement )
					continue;

				if( legacyMaterial->TextureFormState[i] == TextureTransform::Scrolling )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll1.xml" );
				else if( legacyMaterial->TextureFormState[i] == TextureTransform::Scrolling2x )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll2.xml" );
				else if( legacyMaterial->TextureFormState[i] == TextureTransform::Scrolling4x )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll4.xml" );
				else if( legacyMaterial->TextureFormState[i] >= TextureTransform::Scrolling )
					attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll2.xml" );
			}

			//Water Material
			if( legacyMaterial->WindMeshBottom & MeshTransform::Water )
				attributeAnimationsFile.push_back( "game\\scripts\\animations\\scroll1.xml" );

			//Load Attribute Animations File
			for( auto file : attributeAnimationsFile )
			{
				int index = 0;
				while( true )
				{
					auto animation = graphics->GetAttributeAnimationFactory()->Create( file, index );

					if( animation )
					{
						if( animation->GetType() == Resource::AttributeAnimationType::ScrollUV )
							defines.push_back( ShaderDefine{ "SCROLLUV", "1" } );

						attributeAnimations.push_back( animation );
					}
					else
						break;

					index++;
				}
			}

			//Prepare Effect Defines
			if( skinned && !graphics->useSoftwareSkinning )
				defines.push_back( ShaderDefine{ "SKINNED", "1" } );

			//Vertex Color Define
			if( useVertexColor )
				defines.push_back( ShaderDefine{ "VERTEXCOLOR", "1" } );

			//Supports Pixel Shader 3.0
			if( graphics->pixelShaderVersionMajor == 3 )
				defines.push_back( ShaderDefine{ "_PS_3_0", "1" } );

			//Create Effect
			effect = graphics->GetShaderFactory()->Create( "game\\scripts\\shaders\\LitSolid.fx", defines );
		}
		else
		{
			unsigned int offset = 0;
			fread( &offset, sizeof( int ), 1, file );
			fseek( file, offset, SEEK_CUR );
		}

		return true;
	}

	return false;
}

bool Material::Load( const std::string& filePath, bool defaultSettings, bool use3D )
{
	legacyMaterial = new Legacy::Material();

	//Set Default Settings for material
	if( defaultSettings )
	{
		legacyMaterial->InUse = 1;
		legacyMaterial->Shade = false;
		legacyMaterial->TwoSide = false;
		legacyMaterial->UseState = 0;
		legacyMaterial->TextureStageState[0] = 0;
		legacyMaterial->TextureFormState[0] = TextureTransform::None;
		legacyMaterial->MapOpacity = false;
		legacyMaterial->Transparency = 0;
		legacyMaterial->BlendType = StateBlock::Blend_None;
	}

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file( filePath.c_str() );

	if( !result )
		return false;

	pugi::xml_node material = doc.child( "Material" );
	customMaterial = true;

	std::vector<std::string> definesString;

	//Read Effects from XML and prepare Defines structure
	if( !material.child( "Effect" ).attribute( "defines" ).empty() )
	{
		std::istringstream buffer( material.child( "Effect" ).attribute( "defines" ).value() );
		definesString = std::vector<std::string>{ std::istream_iterator<std::string>( buffer ), std::istream_iterator<std::string>() };
	}

	//Read Textures from XML
	for( pugi::xml_node textureNode : material.children( "Texture" ) )
	{
		if( textureNode.attribute( "blendMode" ) )
		{
			if( _strcmpi( textureNode.attribute( "blendMode" ).value(), "Add" ) == 0 )
				selfIllumBlendingMode = 1;
		}

		//Add Texture to Texture Handler
		auto texture = graphics->GetTextureFactory()->Create( textureNode.attribute("file").value(), false, false, 3, false, use3D ? graphics->reduceQualityTexture : 0 );

		if( texture )
			textures.push_back( texture );
	}

	//Read Animated Textures from XML
	for( pugi::xml_node textureNode : material.children( "AnimatedTexture" ) )
	{
		int count = atoi( textureNode.attribute( "count" ).value() );

		if( count > 0 )
		{
			for( int i = 0; i < count; i++ )
			{
				char fileName[256] = { 0 };
				sprintf_s( fileName, textureNode.attribute( "file" ).value(), i );

				//Add Texture to Texture Handler
				auto texture = graphics->GetTextureFactory()->Create( fileName, false, false, 3, false, use3D ? graphics->reduceQualityTexture : 0 );

				if( texture )
					animatedTextures.push_back( texture );
			}

			legacyMaterial->AnimationFrame = true;
		}
	}

	//Read Attributes
	for( pugi::xml_node attributeNode : material.children( "Attribute" ) )
	{
		//Diffuse Color
		if( _strcmpi( attributeNode.attribute( "type" ).value(), "DiffuseColor" ) == 0 )
			sscanf_s( attributeNode.attribute( "value" ).value(), "%f %f %f %f", &legacyMaterial->DiffuseColor[0], &legacyMaterial->DiffuseColor[1], &legacyMaterial->DiffuseColor[2], &legacyMaterial->Transparency );
		//Inner Glow
		else if( _strcmpi( attributeNode.attribute( "type" ).value(), "InnerGlow" ) == 0 )
		{
			useInnerGlow = true;
			BOOL rainbowInnerGlowB = rainbowInnerGlow ? TRUE : FALSE;
			sscanf_s( attributeNode.attribute( "useRainbow" ).value(), "%d", &rainbowInnerGlowB );
			sscanf_s( attributeNode.attribute( "power" ).value(), "%f", &innerGlowPower );
			sscanf_s( attributeNode.attribute( "color" ).value(), "%f %f %f %f", &innerGlowColor.r, &innerGlowColor.g, &innerGlowColor.b, &innerGlowColor.a );
		}
	}

	//Read Attribute Animations from File
	for( pugi::xml_node attributeAnimationNode : material.children( "AttributeAnimation" ) )
	{
		std::string animationFilePath = attributeAnimationNode.attribute( "file" ).value();

		if( !animationFilePath.empty() )
		{
			int index = 0;
			while( true )
			{
				auto animation = graphics->GetAttributeAnimationFactory()->Create( animationFilePath, index );

				if( animation )
				{
					if( animation->GetType() == Resource::AttributeAnimationType::ScrollUV )
						definesString.push_back( "SCROLLUV" );

					attributeAnimations.push_back( animation );
				}
				else
					break;

				index++;
			}
		}
	}

	//Prepare Effect Defines
	std::vector<ShaderDefine> defines;

	for( const auto& str : definesString )
		defines.push_back( ShaderDefine{ str.c_str(), "1" } );

	//Create Effect
	if( material.child( "Effect" ) )
		effect = graphics->GetShaderFactory()->Create( material.child( "Effect" ).attribute( "file" ).value(), defines );

	return true;
}

void Material::SetBlendingMaterial( Material* material, bool forceUseBlendingMap )
{
	blendingMaterial = material;
	useBlendingMap = forceUseBlendingMap;

	if( material == nullptr )
		useBlendingMaterial = false;
	else
		useBlendingMaterial = true;
}
}