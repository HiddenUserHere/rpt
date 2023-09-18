#pragma once

#include "../Math/Matrix4.h"
#include "../Math/BoundingBox.h"
#include "../Math/Quad.h"
#include "../Math/Color.h"

#include "Graphics.h"
#include "Light.h"
#include "Camera.h"
#include "Viewport.h"
#include "Shader.h"
#include "RenderTarget.h"

namespace Delta3D::Graphics
{

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const int MAX_SHADOWMAP_SAMPLES = 32;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;

class Mesh;
class MeshPart;
class Model;

enum class RendererEvents
{
	BeginRendering,
	Rendering3D,
	EndRendering,
	Rendering2D,
};

enum DebugGeometry
{
	DebugNone = 0,

	DebugModel = 1 << 0,
	DebugMesh = 1 << 1,
	DebugCameraFrustum = 1 << 2,
	DebugTerrainQuadtree = 1 << 3,
};	DEFINE_ENUM_FLAG_OPERATORS( DebugGeometry );

struct RenderLight
{
	Math::Vector3 position;
	float range;
	Math::Color color;
};

class Renderer : public Core::EventsImpl<RendererEvents>
{
public:
	//! Default Constructor for Renderer.
	Renderer( Graphics * graphics_ );

	//! Deconstructor.
	~Renderer();

	//! Begin Rendering.
	bool Begin();

	//! Render Shadow Map.
	void RenderShadowMap();

	//! Render 3D.
	void Render();

	//! End Rendering.
	void End();

	//! Run Renderer.
	void Run();

	//! Update Renderer.
	void Update( float elapsedTime );

	//! Push Mesh to Render after everything.
	void PushPostRenderMesh( Mesh * mesh ) { postRenderMeshes.push_back( mesh ); }

	//! Clear Dynamic Lights.
	void ClearLights() { lights.clear(); }

	//! Push Dynamic Light to Renderer.
	void PushLight( const Light & light );

	//! Push Dynamic Light to Renderer.
	void PushLight( const RenderLight & light );

	//! Prepare Lights to Shader.
	void PrepareLights( std::shared_ptr<Shader> effect );

	//! Push World Matrix to Renderer.
	void PushWorldMatrix( const Math::Matrix4 & world );

	//! Pop World Matrix.
	void PopWorldMatrix();

	//! Get World Matrix from Top of stack.
	const D3DXMATRIX & WorldMatrix() const;

	//! Prepare Effect to Render.
	bool Prepare( std::shared_ptr<Shader> effect );

	//! Prepare Shadow Map to Effect.
	bool PrepareShadowMap( std::shared_ptr<Shader> effect );

	//! Viewport Setter.
	void SetViewport( const Viewport & viewport_ ) { viewport = viewport_; }

	//! Push Camera to Stack.
	void PushCamera( Camera * camera_ );

	//! Pop Camera from Stack.
	void PopCamera();

	//! Apply Camera Transformations to Scene.
	void ApplyTransformations();

	//! Reset Scene Transformations.
	void ResetTransformations();

	//! Flag to Renderer Draw Debug Geometries.
	const bool IsDebugGeometry( const DebugGeometry & type ) const { return (debugGeometry & type); }

	//! Set Debug Geometry
	void SetDebugGeometry( const DebugGeometry & type ) { debugGeometry = type; }

	//! Ambient Color Getter.
	const Math::Color & AmbientColor() const { return ambientColor; }

	//! Viewport Getter.
	const Viewport & GetViewport() const { return viewport; }

	//! Set Inner Glow Effect.
	void SetInnerGlow( bool apply, Math::Color color = Math::Color::White, float power = 0.f, bool rainbow = false )
	{
		applyInnerGlowFlag = apply;
		innerGlowColor = color;
		innerGlowPower = power;
		rainbowInnerGlowFlag = rainbow;
	}

	//! Get Dissolving Effect.
	inline float GetDissolveAmount() { return dissolveAmount; }

	//! Set Dissolving Effect.
	void SetDissolveAmount( float value )
	{
		dissolveAmount = value;
	}

	//! Set an Ambient Color for Terrain.
	void SetAmbientColor( Math::Color color ) { ambientColor = color; }

	//! Ambient Color Getter.
	const Math::Color & GetAmbientColor() const { return ambientColor; }

	//! Set Apply Distortion Flag.
	void SetApplyDistortionFlag( bool value ) { applyDistortionFlag = value; }

	//! Set Fog End Value.
	void SetFogEnd( float value ) { fogEnd = value; }

	//! Shadow Map Getter and Setter.
	void SetRenderShadowMap( bool value ) { renderShadowMap = value; }
	bool GetRenderShadowMap() const { return renderShadowMap; }
	bool IsRenderingShadowMap() const { return isRenderingShadowMap; }

	//! Get/Set Global Light Direction
	Math::Vector3 GetGlobalLightDirection() { return globalLightDirection; }
	void SetGlobalLightDirection( const Math::Vector3 & vDir ) { globalLightDirection = vDir; }

	//! Debug Geometries.
	void DrawDebugFrustum( const Math::Frustum & frustum );
	void DrawDebugAABB( const Math::BoundingBox & box );
	void DrawDebugSphere( const Math::Sphere & sphere );
	void DrawDebugQuad( const Math::Quad & quad );
	void DrawDebugLine( const Math::Vector3 & point1, const Math::Vector3 & point2 );

	//! Bone Texture Cache
	static bool UpdateBonesTexture( Model * pBoneModel );

	//! World Time Getter.
	const float GetWorldTime() const { return worldTime; };

	//! Camera Getter.
	inline Camera * GetCamera() const { return cameraStack.top(); };
	inline Camera * GetShadowCamera( int i ) const { return shadowCamera[i]; }

	//! Bones Texture Cache
	static std::shared_ptr<Texture> lpaBonesTextures[8];
	static unsigned int iLastBoneModelWheel;

	//! Get Shadow Samples
	inline float * GetShadowOffsets() { return faShadowOffsets; }
	inline float * GetShadowWeights() { return faShadowWeights; }

	//! Light Settings
	const float GetDiffuseLightIntensity() { return fDiffuseLightIntensity; }
	const float GetSpecularLightIntensity() { return fSpecularLightIntensity; }
	const float GetSpecularLightPower() { return fSpecularLightPower; }

	void SetDiffuseLightIntensity( float f ) { fDiffuseLightIntensity = f; }
	void SetSpecularLightIntensity( float f ) { fSpecularLightIntensity = f; }
	void SetSpecularLightPower( float f ) { fSpecularLightPower = f; }

	void ResetLightSettings();

	//! Texture Effects
	bool GetUseTextureEffects() { return bTextureEffects; }
	void SetUseTextureEffects( bool b ) { bTextureEffects = b; }

private:
	Graphics* graphics;	//!< Graphics Pointer
	Viewport viewport;	//!< Renderer Viewport
	ID3DXMesh* sphereMesh;	//!< Only for debug bounding sphere

	std::stack<Camera*> cameraStack;	//!< Active Camera Pointer
	Camera* shadowCamera[3];	//!< Camera used to Shadow Map

	Math::Color ambientColor;	//!< Ambient Lightning
	float fogEnd;	//!< Fog End Value

	bool applyDistortionFlag;	//!< Apply Distortion Flag
	bool applyInnerGlowFlag;	//!< Inner Glow Flag
	bool rainbowInnerGlowFlag;	//!< Inner Rainbow Glow Flag
	float innerGlowPower;	//!< Inner Glow Power
	Color innerGlowColor;	//!< Inner Glow Color
	float dissolveAmount;	//!< Dissolve Effect Amount

	bool renderShadowMap;	//!< Flag to render shadow Map
	bool isRenderingShadowMap;	//!< Rendering shadow Map

	std::shared_ptr<RenderTarget> shadowRenderTarget[3];	//!< Render Target of Shadow Map

	Light sunLight;	//!< Sun Light

	Math::Vector3 globalLightDirection;

	std::vector<Mesh*> postRenderMeshes;	//!< Meshes to render after everything rendered
	std::vector<RenderLight> lights;	//!< Structure to hold renderer lights
	unsigned int maxLights;	//!< Max Lights

	ID3DXMatrixStack* worldMatrixd3dStack;	//!< Matrix D3D Stack
	float worldTime;	//!< Set Elapsed Time to Shader
	DebugGeometry debugGeometry;	//!< Render Debug Geometries (Frustum, Bounding, Skeleton etc)

	//Shadow Offsets and Weights
	float faShadowOffsets[MAX_SHADOWMAP_SAMPLES];
	float faShadowWeights[MAX_SHADOWMAP_SAMPLES];

	//Light Settings
    float fDiffuseLightIntensity;
    float fSpecularLightIntensity;
    float fSpecularLightPower;

	bool bTextureEffects;

	void GaussianFuncShadow();

	void GaussianFunc( float * faOffsets, float * faWeights, int iSamplesCount );

	float Gaussian( float x );
};
}