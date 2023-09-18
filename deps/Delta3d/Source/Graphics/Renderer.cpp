#include "PrecompiledHeader.h"
#include "Renderer.h"

#include "MeshPart.h"
#include "Mesh.h"
#include "Model.h"

namespace Delta3D::Graphics
{

std::shared_ptr<Texture> Renderer::lpaBonesTextures[8];
unsigned int Renderer::iLastBoneModelWheel = 0;

Renderer::Renderer( Graphics * graphics_ ) :
	worldTime( 0.0f ),
	graphics( graphics_ ),
	sphereMesh( nullptr ),
	worldMatrixd3dStack( nullptr ),
	debugGeometry( DebugGeometry::DebugNone ),
	fogEnd( 1500.0f ),
	maxLights( 64 ),
	applyDistortionFlag( false ),
	renderShadowMap( false ),
	shadowCamera{},
	shadowRenderTarget{},
	dissolveAmount( 0.f ),
	applyInnerGlowFlag( false ),
	innerGlowPower( 0.f ),
	innerGlowColor( Math::Color::White ),
	rainbowInnerGlowFlag( false ),
	fDiffuseLightIntensity( 0.75f ),
	fSpecularLightIntensity( 0.75f ),
	fSpecularLightPower( 1.0f ),
	bTextureEffects( true )
{
	for( int i = 0; i < _countof( shadowCamera ); i++ )
		shadowCamera[i] = new Camera();

	D3DXCreateMatrixStack( 0, &worldMatrixd3dStack );
	worldMatrixd3dStack->Push();
	worldMatrixd3dStack->LoadIdentity();

	//Create Sun Light
	sunLight.GenerateOrthoMatrix( 20.f, SHADOWMAP_DEPTH, SHADOWMAP_NEAR );

	globalLightDirection = Math::Vector3( 1.0f, 1.0f, -0.75f );

	lights.reserve( 64 );

	ResetLightSettings();

	GaussianFuncShadow();
}

Renderer::~Renderer()
{
	for( int i = 0; i < _countof( shadowCamera ); i++ )
	{
		delete shadowCamera[i];
		shadowCamera[i] = nullptr;
	}


	if( worldMatrixd3dStack )
	{
		worldMatrixd3dStack->Release();
		worldMatrixd3dStack = nullptr;
	}

	if( sphereMesh )
	{
		sphereMesh->Release();
		sphereMesh = nullptr;
	}
}

bool Renderer::Begin()
{
	//Begin Scene
	if( !graphics->Begin() )
		return false;

	//Clear Scene
	graphics->Clear();

	//Push Identity Matrix for World Transform
	PushWorldMatrix( Math::Matrix4::Identity );

	//Fire Rendering Event
	FireEvent( RendererEvents::BeginRendering );

	return true;
}

void Renderer::RenderShadowMap()
{
	isRenderingShadowMap = true;

	static int iShadowsMapSize[3] = { 256, 512, 1024 };
	static float fShadowMapDistances[3] = { 400.f, 400.f, 400.f };

	for( int i = 2; i < _countof( shadowRenderTarget ); i++ )
	{
		if( shadowRenderTarget[i] == nullptr )
			shadowRenderTarget[i] = graphics->GetRenderTargetFactory()->Create( false );

		if( shadowCamera[i] && shadowRenderTarget[i] )
		{
			auto vLightDir = GetGlobalLightDirection();
			vLightDir.y = 1.0f;
			Math::Vector3 vLookAtShadow = GetCamera()->LookAt() + (vLightDir * fShadowMapDistances[i]);
			vLookAtShadow.y += 200.0f;
			shadowCamera[i]->SetOrthoProjection( (float)iShadowsMapSize[i], (float)iShadowsMapSize[i], 20.0f, 3000.f );
			shadowCamera[i]->SetPosition( vLookAtShadow, GetCamera()->LookAt() );

			PushCamera( shadowCamera[i] );

			if( graphics->SetRenderTarget( shadowRenderTarget[i] ) )
			{
				graphics->Clear();
				Render();
				graphics->UnsetRenderTarget();
			}

			PopCamera();
			graphics->Clear();
		}
	}

	isRenderingShadowMap = false;
}

void Renderer::Render()
{
	ApplyTransformations();

	//Draw Camera Frustum
	if( IsDebugGeometry( DebugGeometry::DebugCameraFrustum ) )
		DrawDebugFrustum( GetCamera()->Frustum() );

	//Fire Rendering Event
	FireEvent( RendererEvents::Rendering3D );
}

void Renderer::End()
{
	PopWorldMatrix();

	lights.clear();
	postRenderMeshes.clear();

	FireEvent( RendererEvents::EndRendering );

	graphics->End();
}

void Renderer::Run()
{
	if( Begin() )
	{
		//Render shadow Map?
		if( renderShadowMap )
			RenderShadowMap();

		//Post Render Meshes
		for( const auto& mesh : postRenderMeshes )
			if( mesh )
				mesh->Render();

		//Fire Rendering 2D Event
		FireEvent( RendererEvents::Rendering2D );

		End();
	}
}

void Renderer::Update( float elapsedTime )
{
	//Update Sun Light Direction
	float radians = 270.f * 0.0174532925f;
	sunLight.direction = Math::Vector3( sinf( radians ), cosf( radians ), 0.0f );
	sunLight.position = Math::Vector3( 9.0f, 8.f, -0.1f );
	sunLight.lookAt = Math::Vector3( -9.0f, 0.f, 0.f );

	//Update World Time
	worldTime += elapsedTime / 1000.0f;

	if( worldTime >= 100.0f )
		worldTime = 0.0f;
}

void Renderer::PushLight( const Light& light )
{
	if( lights.size() < maxLights )
		lights.push_back( RenderLight{ light.position, light.range, light.color } );
}

void Renderer::PushLight( const RenderLight& light )
{
	if( lights.size() < maxLights )
		lights.push_back( light );
}

void Renderer::PrepareLights( std::shared_ptr<Shader> effect )
{
	if( effect )
	{
		if( !lights.empty() )
			effect->SetData( "Lights", (const char*)&lights[0], lights.size() * sizeof( RenderLight ) );

		effect->SetInt( "NumActiveLights", lights.size() );
	}
}

void Renderer::PushWorldMatrix( const Math::Matrix4& world )
{
	//D3DX Matrix Stack
	{
		worldMatrixd3dStack->Push();
		worldMatrixd3dStack->MultMatrixLocal( &world.Get() );
	}

	//Set new World Matrix to Device
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, worldMatrixd3dStack->GetTop() );
}

void Renderer::PopWorldMatrix()
{
	//D3DX Matrix Stack
	worldMatrixd3dStack->Pop();

	//Set new World Matrix to Device
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, worldMatrixd3dStack->GetTop() );
}

const D3DXMATRIX& Renderer::WorldMatrix() const
{
	return *worldMatrixd3dStack->GetTop();
}

bool Renderer::Prepare( std::shared_ptr<Shader> effect )
{
	if( effect )
	{
		//Prepare Lights
		if( !lights.empty() )
			effect->SetData( "Lights", (const char*)&lights[0], lights.size()* sizeof( RenderLight ) );
		
		Vector2 size( (float)graphics->GetSize().width, (float)graphics->GetSize().height );

		effect->SetInt( "NumActiveLights", lights.size() );
		effect->SetBool( "ApplyDistortionFlag", applyDistortionFlag );
		effect->SetFloat( "WorldTime", worldTime );
		effect->SetFloat( "FogEnd", fogEnd );
		effect->SetMatrix( "World", *worldMatrixd3dStack->GetTop() );
		effect->SetMatrix( "View", GetCamera()->View().Get() );
		effect->SetMatrix( "Projection", GetCamera()->Projection().Get() );
		effect->SetFloatArray( "ScreenSize", &size.x, 2 );
		effect->SetFloatArray( "CameraEye", GetCamera()->Eye().Get(), 3 );
		effect->SetFloatArray( "CameraLookAt", GetCamera()->LookAt().Get(), 3 );
		effect->SetFloatArray( "CameraUp", GetCamera()->Up().Get(), 3 );
		effect->SetFloatArray( "CameraDirection", GetCamera()->Direction().Get(), 3 );
		effect->SetFloatArray( "AmbientColor", &ambientColor.r, 4 );
		effect->SetFloat( "DissolveAmount", dissolveAmount > 1.0f ? 1.0f : dissolveAmount );

		//Set Dissolving Effect
		if( dissolveAmount > 0.f )
		{
			static auto noiseTexture = graphics->GetTextureFactory()->Create( "game\\textures\\dissolveMap.png", false, false );

			if( noiseTexture )
				effect->SetTexture( "NoiseMapTexture", noiseTexture->Get() );
		}

		return true;
	}

	return false;
}

bool Renderer::PrepareShadowMap( std::shared_ptr<Shader> effect )
{
	if( effect )
	{
		if( renderShadowMap )
		{
			D3DXMATRIX lightProjections[3];
			D3DXMATRIX lightViews[3];

			for( int i = 2; i < _countof( shadowRenderTarget ); i++ )
			{
				if( shadowRenderTarget[i] && shadowCamera[i] )
				{
					lightProjections[i] = shadowCamera[i]->Projection().Get();
					lightViews[i] = shadowCamera[i]->View().Get();

					effect->SetTexture( "ShadowMapTexture" + std::to_string( i + 1 ), shadowRenderTarget[i]->Texture() );
				}
			}

			effect->SetMatrix( "LightProjection", lightProjections[2] );
			effect->SetMatrix( "LightView", lightViews[2] );
		}
	}

	return false;
}

void Renderer::PushCamera( Camera* camera_ )
{
	cameraStack.push( camera_ );
}

void Renderer::PopCamera()
{
	cameraStack.pop();
}

void Renderer::ApplyTransformations()
{
	graphics->GetDevice()->SetViewport( &viewport.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_VIEW, &GetCamera()->View().Get() );
	graphics->GetDevice()->SetTransform( D3DTS_PROJECTION, &GetCamera()->Projection().Get() );
}

void Renderer::ResetTransformations()
{
	graphics->GetDevice()->SetViewport( &viewport.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_VIEW, &Matrix4::Identity.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_WORLD, &Matrix4::Identity.Get() );
	graphics->GetDevice()->SetTransform( D3DTS_PROJECTION, &GetCamera()->Projection().Get() );
}

void Renderer::DrawDebugFrustum( const Math::Frustum& frustum )
{
	//Near Plane
	{
		Math::Quad q;
		q.p1 = frustum.vertices[0];
		q.p2 = frustum.vertices[2];
		q.p3 = frustum.vertices[3];
		q.p4 = frustum.vertices[1];
		DrawDebugQuad( q );
	}

	//Far Plane
	{
		Math::Quad q;
		q.p1 = frustum.vertices[4];
		q.p2 = frustum.vertices[6];
		q.p3 = frustum.vertices[7];
		q.p4 = frustum.vertices[5];
		DrawDebugQuad( q );
	}

	//Attach Corners
	DrawDebugLine( frustum.vertices[0], frustum.vertices[4] );
	DrawDebugLine( frustum.vertices[2], frustum.vertices[6] );
	DrawDebugLine( frustum.vertices[3], frustum.vertices[7] );
	DrawDebugLine( frustum.vertices[1], frustum.vertices[5] );
}

void Renderer::DrawDebugAABB( const Math::BoundingBox& box )
{
	Vector3 corners[8];
	box.ComputeCorners( corners );

	PushWorldMatrix( Math::Matrix4::Identity );

	DrawDebugQuad( Quad( corners[0], corners[4], corners[7], corners[3] ) ); //Left
	DrawDebugQuad( Quad( corners[5], corners[1], corners[2], corners[6] ) ); //Right
	DrawDebugQuad( Quad( corners[7], corners[6], corners[2], corners[3] ) ); //Bottom
	DrawDebugQuad( Quad( corners[0], corners[1], corners[5], corners[4] ) ); //Top
	DrawDebugQuad( Quad( corners[4], corners[5], corners[6], corners[7] ) ); //Front
	DrawDebugQuad( Quad( corners[1], corners[0], corners[3], corners[2] ) ); //Back

	PopWorldMatrix();
}

void Renderer::DrawDebugSphere( const Math::Sphere& sphere )
{
	if( !sphereMesh )
		D3DXCreateSphere( graphics->GetDevice(), 1.0f, 15, 15, &sphereMesh, nullptr );

	//Apply Transformations
	ApplyTransformations();

	//Matrix Transformed
	Math::Matrix4 translation;
	translation.Translate( sphere.center );

	Math::Matrix4 scaling;
	scaling.Scale( sphere.radius );

	//Push World Matrix
	PushWorldMatrix( scaling* translation );

	//Draw Sphere
	graphics->SetFillMode( FillMode::Wireframe );
	sphereMesh->DrawSubset( 0 );
	graphics->SetFillMode( FillMode::Solid );

	//Reset Scene Transformations and restore World Matrix
	ResetTransformations();
	PopWorldMatrix();
}

void Renderer::DrawDebugQuad( const Math::Quad& quad )
{
	DrawDebugLine( quad.p1, quad.p2 );
	DrawDebugLine( quad.p2, quad.p3 );
	DrawDebugLine( quad.p3, quad.p4 );
	DrawDebugLine( quad.p4, quad.p1 );
}

void Renderer::DrawDebugLine( const Math::Vector3& point1, const Math::Vector3& point2 )
{
	struct Vertex { Vector3 pos; int color; };

	Vertex vertex[2];
	vertex[0] = { Vector3( point1.x, point1.y, point1.z ), -1 };
	vertex[1] = { Vector3( point2.x, point2.y, point2.z ), -1 };

	graphics->GetDevice()->SetTexture( 0, nullptr );
	graphics->GetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	graphics->GetDevice()->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, vertex, sizeof( Vertex ) );
}

bool Renderer::UpdateBonesTexture( Model * pBoneModel )
{
	static Model * pLastBoneModel = nullptr;
	static int iLastBoneModelFrame = -1;
	static int iLastBoneModelFrameBaseStartFrame = -1;
	static int iLastBoneModelFrameBaseEndFrame = -1;
	static BYTE bLastBoneModelFrameUsedBones = 0;
    static unsigned int iBonesTexturesWheel = 0;

    if ( Graphics::GetInstance()->useSoftwareSkinning )
        return false;

    if ( (!pBoneModel) || (!pBoneModel->bonesTransformations) || !(pBoneModel->bonesWorldMatrices) )
        return false;

	/*
    if ( (pBoneModel == pLastBoneModel) &&
         (pBoneModel->lastFrame == iLastBoneModelFrame) &&
         (pBoneModel->lastFrameBaseStartFrame == iLastBoneModelFrameBaseStartFrame) &&
         (pBoneModel->lastFrameBaseEndFrame == iLastBoneModelFrameBaseEndFrame) &&
         ((pBoneModel->lastFrameUsedBones & bLastBoneModelFrameUsedBones) == pBoneModel->lastFrameUsedBones) )
        return true;
*/
    const auto iWheel = iBonesTexturesWheel % _countof( lpaBonesTextures );
    ++iBonesTexturesWheel;

    if ( !lpaBonesTextures[iWheel] )
        return false;

	for ( size_t i = 0; i < pBoneModel->orderedMeshes.size(); i++ )
	{
		memcpy( (unsigned char *)pBoneModel->bonesTransformations + ((i * 16 * 4)), &pBoneModel->bonesWorldMatrices[i].Transpose().f[0], sizeof( float ) * 16 );
	}

    //Set Bone Transformations Data to Texture Data
    if ( lpaBonesTextures[iWheel] )
    {
        if ( lpaBonesTextures[iWheel]->Lock() )
        {
			lpaBonesTextures[iWheel]->SetPixelData( pBoneModel->bonesTransformations, pBoneModel->orderedMeshes.size() * sizeof( float ) * 16 );
			lpaBonesTextures[iWheel]->Unlock();
        }
    }

    if ( pBoneModel )
    {
        pLastBoneModel = pBoneModel;
        iLastBoneModelFrame = pBoneModel->lastFrame;
        iLastBoneModelFrameBaseStartFrame = pBoneModel->lastFrameBaseStartFrame;
        iLastBoneModelFrameBaseEndFrame = pBoneModel->lastFrameBaseEndFrame;
        bLastBoneModelFrameUsedBones = pBoneModel->lastFrameUsedBones;
        iLastBoneModelWheel = iWheel;
    }
    else
        pLastBoneModel = nullptr;

    return true;
}

void Renderer::ResetLightSettings()
{
	fDiffuseLightIntensity = 1.0f;
	fSpecularLightIntensity = 1.0f;
	fSpecularLightPower = 1.0f;
}

void Renderer::GaussianFuncShadow()
{
	GaussianFunc( faShadowOffsets, faShadowWeights, MAX_SHADOWMAP_SAMPLES );
}

void Renderer::GaussianFunc( float * faOffsets, float * faWeights, int iSamplesCount )
{
    faWeights[0] = Gaussian( 0.0f );
    faOffsets[0] = 0.0f;

    float totalWeight = faWeights[0];

    for ( int i = 0; i < iSamplesCount / 2; i++ )
    {
        float weight = Gaussian( (float)(i + 1) );
        faWeights[(i * 2) + 1] = weight;
        faWeights[(i * 2) + 2] = weight;
        totalWeight += weight * 2.0f;

        float offset = ((float)i * 2.0f) + 1.5f;
        faOffsets[(i * 2) + 1] = +offset;
        faOffsets[(i * 2) + 2] = -offset;
    }

    for ( int i = 0; i < iSamplesCount; i++ )
        faWeights[i] /= totalWeight;
}

float Renderer::Gaussian( float x )
{
    float blur2 = 4.0f;
    float invSq = 1.0f / (float)sqrt( D3DX_PI * blur2 );
    float exp2 = (float)exp( -(x * x) / blur2 );
    return invSq * exp2;
}

}