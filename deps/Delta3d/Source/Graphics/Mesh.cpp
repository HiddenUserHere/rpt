#include "PrecompiledHeader.h"
#include "Mesh.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "MeshPart.h"
#include "Renderer.h"
#include "VertexDeclaration.h"
#include "Material.h"
#include "Texture.h"
#include "Model.h"

namespace Delta3D::Graphics
{
Mesh::Mesh() :
	GraphicsImpl(),
	modelParent( nullptr ),
	postRender( false ),
	verticesCount( 0 ),
	facesCount( 0 ),
	texturesCoordCount( 0 )
{

}

Mesh::Mesh( int verticesCount_, int facesCount_ ) : 
	GraphicsImpl(), 
	modelParent( nullptr ), 
	postRender( false )
{
}

Mesh::~Mesh()
{
	for( auto& p : meshParts )
	{
		delete p.second;
		p.second = nullptr;
	}

	meshParts.clear();
	textureCoordsBuffer.clear();
}

int Mesh::AddVertex( int x, int y, int z )
{
	return -1;
}

int Mesh::AddFace( int a, int b, int c, int materialID, Math::Vector2 texA, Math::Vector2 texB, Math::Vector2 texC )
{
	return -1;
}

int Mesh::AddTextureCoord( int faceIndex, Math::Vector2 texA, Math::Vector2 texB, Math::Vector2 texC )
{
	return -1;
}

void Mesh::ApplyRotationTransform( Math::Matrix4& out, Math::Matrix4* previousRotation_, IO::SMD::KeyRotation* frameRotation_, int frame_, IO::SMD::FrameInfo* frameInfo )
{
	if( frameRotation_[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = frameRotation_[i].frame;
		currentFrame = frameRotation_[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	if( frameInfo && currentFrame > (int)frameInfo->endFrame )
		out = previousRotation_[i];

	auto sQuaternion = Math::Quaternion().Slerp( Math::Quaternion( frameRotation_[i + 1].x, frameRotation_[i + 1].y, frameRotation_[i + 1].z, frameRotation_[i + 1].w ), alpha );
	
	out = sQuaternion.ToMatrix();
	out = previousRotation_[i] * out;

	vLastQuaternion = out.ToQuaternion();
}

void Mesh::ApplyScalingTransform( Math::Matrix4& out, IO::SMD::KeyScale* frameScaling_, int frame_ )
{
	if( legacyMesh->TmScale[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = legacyMesh->TmScale[i].frame;
		currentFrame = legacyMesh->TmScale[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	out._11 = (legacyMesh->TmScale[i].x / 256.0f) + (((legacyMesh->TmScale[i + 1].x / 256.0f) - (legacyMesh->TmScale[i].x / 256.0f)) * alpha);
	out._22 = (legacyMesh->TmScale[i].y / 256.0f) + (((legacyMesh->TmScale[i + 1].y / 256.0f) - (legacyMesh->TmScale[i].y / 256.0f)) * alpha);
	out._33 = (legacyMesh->TmScale[i].z / 256.0f) + (((legacyMesh->TmScale[i + 1].z / 256.0f) - (legacyMesh->TmScale[i].z / 256.0f) )* alpha );

	vLastScale = out.GetScale();
}

void Mesh::ApplyTranslationTransform( Math::Matrix4& out, IO::SMD::KeyPosition* framePosition_, int frame_ )
{
	auto lerpf = []( float a, float b, float t ) -> float
	{
		return ((1.f - t) * a) + (t * b);
	};

	if( framePosition_[0].frame > frame_ )
		return;

	int i = 0;
	int previousFrame, currentFrame;
	while( true )
	{
		previousFrame = framePosition_[i].frame;
		currentFrame = framePosition_[i + 1].frame;

		if( previousFrame <= frame_ && currentFrame > frame_ )
			break;

		i++;
	}

	int frameDiff = currentFrame - previousFrame;
	int frameDelta = frame_ - previousFrame;

	float alpha = (float)frameDelta / (float)frameDiff;

	out._41 = lerpf( framePosition_[i].x, framePosition_[i + 1].x, alpha );
	out._42 = lerpf( framePosition_[i].y, framePosition_[i + 1].y, alpha );
	out._43 = lerpf( framePosition_[i].z, framePosition_[i + 1].z, alpha );

	vLastPosition = out.GetTranslation();
}

int Mesh::FindAnimationPosition( int frame )
{
	for ( auto & sKeyFinder : vKeyPositionFinder )
	{
		if ( (sKeyFinder.iBeginFrame >= frame) && (sKeyFinder.iEndFrame <= frame) )
			return sKeyFinder.iKeyIndex;
	}

	if( legacyMesh->TmFrameCnt > 0 )
	{
		for( int i = 0; i < legacyMesh->TmFrameCnt; i++ )
		{
			if( i > _countof( legacyMesh->TmPosFrame ) )
				break;

			if( legacyMesh->TmPosFrame[i].keyFrameCount > 0 && legacyMesh->TmPosFrame[i].startFrame <= frame && legacyMesh->TmPosFrame[i].endFrame > frame )
				return legacyMesh->TmPosFrame[i].keyFrameStartIndex;
		}
	}

	return -1;
}

int Mesh::FindAnimationRotation( int frame )
{
    for ( auto & sKeyFinder : vKeyRotationFinder )
    {
        if ( (sKeyFinder.iBeginFrame >= frame) && (sKeyFinder.iEndFrame <= frame) )
            return sKeyFinder.iKeyIndex;
    }

	if( legacyMesh->TmFrameCnt > 0 )
	{
		for( int i = 0; i < legacyMesh->TmFrameCnt; i++ )
		{
			if( i > _countof( legacyMesh->TmRotFrame ) )
				break;

			if( legacyMesh->TmRotFrame[i].keyFrameCount > 0 && legacyMesh->TmRotFrame[i].startFrame <= frame && legacyMesh->TmRotFrame[i].endFrame > frame )
				return legacyMesh->TmRotFrame[i].keyFrameStartIndex;
		}
	}

	return -1;
}

int Mesh::FindAnimationScaling( int frame )
{
    for ( auto & sKeyFinder : vKeyScaleFinder )
    {
        if ( (sKeyFinder.iBeginFrame >= frame) && (sKeyFinder.iEndFrame <= frame) )
            return sKeyFinder.iKeyIndex;
    }

	if( legacyMesh->TmFrameCnt > 0 )
	{
		for( int i = 0; i < legacyMesh->TmFrameCnt; i++ )
		{
			if( i > _countof( legacyMesh->TmScaleFrame ) )
				break;

			if( legacyMesh->TmScaleFrame[i].keyFrameCount > 0 && legacyMesh->TmScaleFrame[i].startFrame <= frame && legacyMesh->TmScaleFrame[i].endFrame > frame )
				return legacyMesh->TmScaleFrame[i].keyFrameStartIndex;
		}
	}

	return -1;
}

void Mesh::Animate( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo * frameInfo )
{
    auto PTDegreeToRadians = []( const int deg ) { return (float)(deg & 4095) * D3DX_PI / 2048.0f; };

    Math::Matrix4 result = Math::Matrix4::Identity;

	{
		auto sRotation = Matrix4::Identity;
		auto sScale = Matrix4::Identity;
		auto sTranslation = Matrix4::Identity;

		//Animation was found?
		if ( GetMatricesAnimation( frame_, rotation_, frameInfo, sRotation, sScale, sTranslation ) )
		{
			result = sRotation * sScale * sTranslation;

			if ( (fAnimationBlendingTime > 0.0f) && (fAnimationBlendingTime <= 1.0f) && (vMeshAnimationOld.vLastScale.LengthSquared() != 0.0f) )
			{
				vLastScale = vMeshAnimationOld.vLastScale.Lerp( vLastScale, fAnimationBlendingTime );
				vLastQuaternion = vMeshAnimationOld.vLastQuaternion.Slerp( vLastQuaternion, fAnimationBlendingTime );
				vLastPosition = vMeshAnimationOld.vLastPosition.Lerp( vLastPosition, fAnimationBlendingTime );

				result = vLastQuaternion.ToMatrix() * vLastScale.ToMatrixScale() * vLastPosition.ToMatrixTranslation();
			}
			vMeshAnimationOld.vLastScale = vLastScale;
			vMeshAnimationOld.vLastQuaternion = vLastQuaternion;
			vMeshAnimationOld.vLastPosition = vLastPosition;
		}
		else if ( legacyMesh->pParent )
		{
			auto parentMesh = (Mesh *)legacyMesh->pParent->newMesh;
			result = transformationMatrix * parentMesh->inverseMatrix;
		}
		else
			result = transformationMatrix;
	}

    //Multiply by Parent Animation Matrix
    if ( legacyMesh->pParent )
    {
        auto parentMesh = (Mesh *)legacyMesh->pParent->newMesh;
        legacyMesh->TmResult = result * parentMesh->legacyMesh->TmResult;
    }
    else
        memcpy( &legacyMesh->TmResult, &result, sizeof( Math::Matrix4 ) );

    //Scaling Matrix
    static Math::Matrix4 scaling;
    static bool scaleModel = false;

    if ( modelParent->scaling != Math::Vector3( 1.0f, 1.0f, 1.0f ) )
    {
        scaling.Scale( modelParent->scaling );
        scaleModel = true;
    }
    else
        scaleModel = false;

    //Rotate Final World Matrix
    if ( rotation_ == Math::Vector3Int::Null )
    {
        if ( legacyMesh->bUseInfluences )
            world = inverseMatrix * legacyMesh->TmResult;
        else
            world = legacyMesh->TmResult;

        if ( scaleModel )
            world = world * scaling;
    }
	else
	{
		Math::Matrix4 rotateX, rotateY, rotateZ;

		rotateX.RotateX( PTDegreeToRadians( rotation_.x ) );
		rotateY.RotateZ( PTDegreeToRadians( rotation_.y ) );
		rotateZ.RotateY( PTDegreeToRadians( rotation_.z ) );

		local = rotateZ * rotateX * rotateY;

		if ( legacyMesh->bUseInfluences )
			world = inverseMatrix * legacyMesh->TmResult * local;
		else
			world = legacyMesh->TmResult * local;

		if ( scaleModel )
			world = world * scaling;
	}

    _mm_store_si128( (__m128i *) & legacyMesh->mWorld._11, _mm_cvttps_epi32( _mm_mul_ps( _mm_load_ps( &world._11 ), _mm_set1_ps( 256.f ) ) ) );
    _mm_store_si128( (__m128i *) & legacyMesh->mWorld._21, _mm_cvttps_epi32( _mm_mul_ps( _mm_load_ps( &world._21 ), _mm_set1_ps( 256.f ) ) ) );
    _mm_store_si128( (__m128i *) & legacyMesh->mWorld._31, _mm_cvttps_epi32( _mm_mul_ps( _mm_load_ps( &world._31 ), _mm_set1_ps( 256.f ) ) ) );
    _mm_store_si128( (__m128i *) & legacyMesh->mWorld._41, _mm_cvttps_epi32( _mm_mul_ps( _mm_load_ps( &world._41 ), _mm_set1_ps( 256.f ) ) ) );
}

void Mesh::SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ )
{
	if( position_ && rotation_ )
	{
		translation = Math::Matrix4::Identity;
		translation.Translate( *position_ );
	}
}

void Mesh::UpdateBoundingVolumes()
{
	if( modelParent == nullptr )
		return;

	if ( vertexData.size() <= 0 )
		return;

	bool first = true;

	//Building Bounding Box Volume
	for( size_t i = 0; i < vertexData.size(); i++ )
	{
		Math::Matrix4 matrix;

		Math::Vector3 transformedVector = Math::Vector3( 0.0f, 0.0f, 0.0f );

		//Skinned Mesh
		if ( modelParent->skeleton )
		{
			const auto & bones = modelParent->skeleton->orderedMeshes;

			if ( bones.size() )
			{
				int iCountBones = 0;

				for ( int j = 0; j < 4; j++ )
					if ( (int)vertexData[i].faBoneIndice[j] >= 0 )
						iCountBones++;

				float fLastWeight = 1.0f;
				for ( int j = 0; j < (iCountBones - 1); j++ )
				{
					Vector3 tmp = (bones[(int)vertexData[i].faBoneIndice[j]]->world.FlippedYZ() * vertexData[i].sVertex);
					transformedVector += (tmp * vertexData[i].faWeight[j]);

					fLastWeight -= vertexData[i].faWeight[j];
				}

				{
					Vector3 tmp = (bones[(int)vertexData[i].faBoneIndice[iCountBones - 1]]->world.FlippedYZ() * vertexData[i].sVertex);
					transformedVector += (tmp * fLastWeight);
				}
			}
		}
		else
		{
			matrix = world;

			transformedVector = matrix.FlippedYZ() * vertexData[i].sVertex;
		}

		//Update World Bounding Box
		if( first )
		{
			worldBoundingBox.min = transformedVector;
			worldBoundingBox.max = transformedVector;

			first = false;
		}
		else
		{
			if( transformedVector.x < worldBoundingBox.min.x )
				worldBoundingBox.min.x = transformedVector.x;
			if( transformedVector.y < worldBoundingBox.min.y )
				worldBoundingBox.min.y = transformedVector.y;
			if( transformedVector.z < worldBoundingBox.min.z )
				worldBoundingBox.min.z = transformedVector.z;

			if( transformedVector.x > worldBoundingBox.max.x )
				worldBoundingBox.max.x = transformedVector.x;
			if( transformedVector.y > worldBoundingBox.max.y )
				worldBoundingBox.max.y = transformedVector.y;
			if( transformedVector.z > worldBoundingBox.max.z )
				worldBoundingBox.max.z = transformedVector.z;
		}
	}

	//Update Bounding Sphere
	boundingSphere.radius = std::max( { worldBoundingBox.Size().x,  worldBoundingBox.Size().y, worldBoundingBox.Size().z } )* 0.5f;
	boundingSphere.center = worldBoundingBox.Center();
}

inline int Mesh::FindSimilarVertex( IO::SMD::PackedVertex& packed, std::map<IO::SMD::PackedVertex, unsigned int>& cache, unsigned short& out )
{
	auto it = cache.find( packed );

	//Not Found
	if( it == cache.end() )
		return false;
	else
	{
		out = it->second;
		return true;
	}

	return false;
}

MeshRenderResult Mesh::CanRender()
{
	MeshRenderResult ret = MeshRenderResult::Undefined;

	for( const auto& p : meshParts )
	{
		ret = p.second->CanRender();

		if( ret == MeshRenderResult::NotRender )
			break;
	}

	return ret;
}

inline void Mesh::Skinning()
{
	if( modelParent == nullptr )
		return;

	if( modelParent->skeleton == nullptr )
		return;

	if( vertexPositionBuffer && !vertexData.empty() )
	{
		float* vertexPositionData = (float*)vertexPositionBuffer->Lock();
		const auto& bones = modelParent->skeleton->orderedMeshes;

		if( vertexPositionData && bones.size() )
		{
			for( const auto& v : vertexData )
			{
				//Output Vertex
				Math::Vector3 out = Math::Vector3( 0.0f, 0.0f, 0.0f );

				int iCountBones = 0;

				for ( int i = 0; i < 4; i++ )
					if ( (int)v.faBoneIndice[i] >= 0 )
						iCountBones++;

				float fLastWeight = 1.0f;
				for ( int i = 0; i < (iCountBones - 1); i++ )
				{
					Vector3 tmp = (bones[(int)v.faBoneIndice[i]]->world.FlippedYZ() * v.sVertex);
					out += (tmp * v.faWeight[i]);

					fLastWeight -= v.faWeight[i];
				}

                {
                    Vector3 tmp = (bones[(int)v.faBoneIndice[iCountBones - 1]]->world.FlippedYZ() * v.sVertex);
                    out += (tmp * fLastWeight);
                }

				*(vertexPositionData++) = out.x;
				*(vertexPositionData++) = out.y;
				*(vertexPositionData++) = out.z;
			}

			vertexPositionBuffer->Unlock();
		}
	}
}

void Mesh::Update( float timeElapsed )
{
}

int Mesh::Render()
{
	if( vertexPositionBuffer && vertexNormalBuffer && vertexTangentBuffer && vertexBitangentBuffer && !textureCoordsBuffer.empty() && modelParent )
	{
		bool skinnedMesh = false;

		//Skinned Mesh
		if( modelParent->skeleton && skinnedVerticesIndex.size() )
			skinnedMesh = true;

		//Frustum Culling
		if( renderer->GetCamera()->Frustum().IsInside( worldBoundingBox.Transformed( translation ) ) == Intersection::Outside )
			return false;

		//Apply Camera Transformations
		renderer->ApplyTransformations();
		
		//Draw Bounding Box on Debug Mode
		if( renderer->IsDebugGeometry( DebugGeometry::DebugMesh ) )
			renderer->DrawDebugAABB( worldBoundingBox.Transformed( translation ) );

		//Set Vertex Position Buffer to Stream
		if( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 0, vertexPositionBuffer->Get(), 0, vertexPositionBuffer->ElementSize() ) ) )
			return false;

        //Set Vertex Normals Buffer to Stream
        if ( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 1, vertexNormalBuffer->Get(), 0, vertexNormalBuffer->ElementSize() ) ) )
            return false;

        //Set Vertex Tangent Buffer to Stream
        if ( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 3, vertexTangentBuffer->Get(), 0, vertexTangentBuffer->ElementSize() ) ) )
            return false;

        //Set Vertex Bitangent Buffer to Stream
        if ( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 4, vertexBitangentBuffer->Get(), 0, vertexBitangentBuffer->ElementSize() ) ) )
            return false;

		//Set Vertex Blend Indices Buffer to Stream
		if ( skinnedMesh && vertexBlendIndicesBuffer && vertexBlendWeightsBuffer )
		{
            if ( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 5, vertexBlendIndicesBuffer->Get(), 0, vertexBlendIndicesBuffer->ElementSize() ) ) )
                return false;
            if ( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 6, vertexBlendWeightsBuffer->Get(), 0, vertexBlendWeightsBuffer->ElementSize() ) ) )
                return false;
		}

		//Set Texture Coordinates Buffer to Stream
		for( size_t i = 0; i < textureCoordsBuffer.size(); i++ )
		{
			if( FAILED( Graphics::GetInstance()->GetDevice()->SetStreamSource( 5 + i + (skinnedMesh ? 2 : 0), textureCoordsBuffer[i]->Get(), 0, textureCoordsBuffer[i]->ElementSize() ) ) )
				return false;
		}

		//Scaling Matrix
		static Math::Matrix4 scalingMesh;
		static bool scaleMesh = false;

		if( modelParent->scaling != Math::Vector3( 1.0f, 1.0f, 1.0f ) )
		{
			scalingMesh.Scale( modelParent->scaling );
			scaleMesh = true;
		}
		else
			scaleMesh = false;

		//Draw Mesh Parts (per material)
		for( const auto& p : meshParts )
		{
			if( p.second )
			{
				//Push World Matrix
				renderer->PushWorldMatrix( skinnedMesh ? translation : world.FlippedYZ()* translation );

				//Push Scaling Matrix
				if( scaleMesh )
					renderer->PushWorldMatrix( scalingMesh );

				//Update Bones Transformations and set it to Texture Data
				if( skinnedMesh )
				{
					//Software Skinning
					if( graphics->useSoftwareSkinning )
						Skinning();
					//Hardware Skinning
					else if( modelParent->skeleton )
					{
						if ( Renderer::UpdateBonesTexture( modelParent->skeleton ) )
							modelParent->skeleton->lastFrameUsedBones = modelParent->skeleton->lastFrame;

						//Update Bones Texture Fetch
						if( p.second->material->GetEffect() )
						{
							p.second->material->GetEffect()->SetTexture( "BonesMap", Renderer::lpaBonesTextures[Renderer::iLastBoneModelWheel] );
						}
					}
				}

				//Render It!
				p.second->Render( vertexPositionBuffer, skinnedMesh );

				//Pop Scaling Matrix
				if( scaleMesh )
					renderer->PopWorldMatrix();

				renderer->PopWorldMatrix();
			}
		}

		return true;
	}

	return false;
}

bool Mesh::Build( Model* skeleton, Legacy::Mesh* legacyMesh_ )
{
	//Set legacy mesh
	legacyMesh = legacyMesh_;

	if( legacyMesh )
	{
		strName = legacyMesh->NodeName;

		std::vector<IO::SMD::VertexColor> verticesColor;
		bool hasVertexColor = false;

		auto ConvertMatrixToFloat = []( const Math::Matrix4Int b )
		{
			Math::Matrix4 m;

			for( int i = 0; i < 4; i++ )
				for( int j = 0; j < 4; j++ )
					m.m[i][j] = b.m[i][j] / 256.0f;

			return m;
		};

		verticesCount = legacyMesh->nVertex;
		facesCount = legacyMesh->nFace;
		texturesCoordCount = legacyMesh->nTexLink;

		world = ConvertMatrixToFloat( legacyMesh->mWorld );
		local = ConvertMatrixToFloat( legacyMesh->mLocal );

		if ( legacyMesh->bUseInfluences )
			transformationMatrix = legacyMesh->sTransformationMatrixF;
		else
			transformationMatrix = ConvertMatrixToFloat( legacyMesh->Tm );

		rotationMatrix = legacyMesh->TmRotate;
		inverseMatrix = transformationMatrix.Inverse();

		skinnedVerticesIndex.clear();
		skinnedVerticesIndex.reserve( verticesCount );

		struct VertexBoneBlendData { float saOrderedIndex[4]; float faWeight[4]; };
		std::vector<VertexBoneBlendData> vSkinWeightData;
		vSkinWeightData.reserve( verticesCount );

		//Skinned Object? Generate Bones List and Skinned Indices
		if( skeleton && legacyMesh->Physique )
		{
			for( unsigned int i = 0; i < verticesCount; i++ )
			{
				if( legacyMesh->Physique[i] == nullptr )
					break;

				auto boneMesh = legacyMesh->Physique[i];
				int boneIndex = skeleton->GetOrderedMeshIndex( boneMesh->NodeName );

				if ( legacyMesh->bUseInfluences )
				{
					VertexBoneBlendData sDataBlend;
                    for ( int j = 0; j < 4; j++ )
                    {
						sDataBlend.saOrderedIndex[j] = -1.0f;

						int iBoneIndex = skeleton->GetOrderedMeshIndex( legacyMesh->vInfluences[i].saBoneIndex[j] );

						if ( iBoneIndex >= 0 )
							skeleton->orderedMeshes[iBoneIndex]->legacyMesh->bUseInfluences = TRUE;

						sDataBlend.saOrderedIndex[j] = (float)iBoneIndex;
						sDataBlend.faWeight[j] = legacyMesh->vInfluences[i].faWeight[j];
                    }

					vSkinWeightData.push_back( sDataBlend );
				}

				if ( boneIndex != -1 )
					skinnedVerticesIndex.push_back( boneIndex );
			}
		}

		//Do not build Mesh if it's a Skeleton Mesh
		if( facesCount > 0 && _strnicmp( legacyMesh->NodeName, "Bip", 5 ) != 0 )
		{
			//Skinned Mesh Flag
			bool skinnedMesh = skeleton && skinnedVerticesIndex.size() > 0;

			//Create Texture to store Bones Matrices
			if ( skinnedMesh && !graphics->useSoftwareSkinning )
			{
                vertexBlendIndicesBuffer = graphics->CreateStaticVertexBuffer( sizeof( float ) * 4, facesCount * 3 );
                vertexBlendWeightsBuffer = graphics->CreateStaticVertexBuffer( sizeof( float ) * 4, facesCount * 3 );
			}

			//Create Vertex Buffer and Texture Coordinates Buffer
			if( skinnedMesh && graphics->useSoftwareSkinning )
				vertexPositionBuffer = graphics->CreateDynamicVertexBuffer( sizeof( Math::Vector3 ), facesCount * 3 );
			else
				vertexPositionBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount * 3 );

            vertexNormalBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount * 3 );
            vertexTangentBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount * 3 );
            vertexBitangentBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector3 ), facesCount * 3 );

			//Vertex Buffer and Texture Coordinates Buffer was created successfully?
			if( vertexPositionBuffer && vertexNormalBuffer && vertexTangentBuffer && vertexBitangentBuffer )
			{
				float* vertexPositionData = (float*)vertexPositionBuffer->Lock();
                float * vertexNormalData = (float *)vertexNormalBuffer->Lock();
                float * vertexTangentData = (float *)vertexTangentBuffer->Lock();
                float * vertexBitangentData = (float *)vertexBitangentBuffer->Lock();

				float* textureCoordData[8] = { 0 };
                float * vertexBlendIndicesData = nullptr;
                float * vertexBlendWeightsData = nullptr;

				//Skinned Mesh
                if ( vertexBlendIndicesBuffer )
                    vertexBlendIndicesData = (float *)vertexBlendIndicesBuffer->Lock();

				if ( vertexBlendWeightsBuffer )
					vertexBlendWeightsData = (float *)vertexBlendWeightsBuffer->Lock();

				//Was locked successfully?
				if( vertexPositionData && vertexNormalData )
				{
					unsigned int curVertexIndex = 0;
					std::map<IO::SMD::PackedVertex, unsigned int> verticesIndex;

					//Build Texture Coordinates Before proceed
					for( unsigned int i = 0; i < facesCount; i++ )
					{
						IO::SMD::Face* face = legacyMesh->Face + i;

						//First Face?
						if( textureCoordsBuffer.empty() )
						{
							auto texCoord = face->textureLink;

							while( texCoord )
							{
								if( textureCoordsBuffer.size() >= 8 )
									break;

								auto textureCoordBuffer = graphics->CreateStaticVertexBuffer( sizeof( Math::Vector2 ), facesCount * 3 );
								if( textureCoordBuffer )
								{
									textureCoordData[textureCoordsBuffer.size()] = (float*)textureCoordBuffer->Lock();
									textureCoordsBuffer.push_back( textureCoordBuffer );
								}

								texCoord = texCoord->next;
							}
						}
						else
							break;
					}

					//Loop through Mesh Faces
					for( unsigned int i = 0; i < facesCount; i++ )
					{
						IO::SMD::Face* face = legacyMesh->Face + i;
						Material* material = (modelParent) && modelParent->materialCollection ? modelParent->materialCollection->materials[face->v[3]] : nullptr;

						//Material not found or face without textures coordinates? Do nothing!
						if( !material || !face->textureLink )
							continue;

						MeshPart* meshPart = nullptr;

						//Find Mesh Part by current face Material
						if( meshParts.find( material ) != meshParts.end() )
							meshPart = meshParts[material];
						else
						{
							//Not Found? So create it and put on mesh parts vector
							meshPart = new MeshPart();
							meshPart->material = material;
							meshPart->mesh = this;
							meshParts[material] = meshPart;
						}
						
						//Loop through Face Vertices (A,B,C)
						for( int j = 0; j < 3; j++ )
						{
							//Temporary Variables used to find Vertex
							//Math::Vector3 position( legacyMesh->Vertex[face->v[j]].x / 256.0f, legacyMesh->Vertex[face->v[j]].y / 256.0f, legacyMesh->Vertex[face->v[j]].z / 256.0f );
							
							Math::Vector3 position( legacyMesh->Vertex[face->v[j]].x / 256.0f, legacyMesh->Vertex[face->v[j]].y / 256.0f, legacyMesh->Vertex[face->v[j]].z / 256.0f );
                            Math::Vector3 normal( legacyMesh->Vertex[face->v[j]].nx / 256.0f, legacyMesh->Vertex[face->v[j]].ny / 256.0f, legacyMesh->Vertex[face->v[j]].nz / 256.0f );
                            Math::Vector3 tangentpos( 0.0f, 0.0f, 0.0f );
                            Math::Vector3 bitangentpos( 0.0f, 0.0f, 0.0f );

							if ( legacyMesh->vFloatVertices.size() > 0 )
							{
                                position = Math::Vector3( legacyMesh->vFloatVertices[face->v[j]].sVertex.x, legacyMesh->vFloatVertices[face->v[j]].sVertex.y, legacyMesh->vFloatVertices[face->v[j]].sVertex.z );
                                normal = Math::Vector3( legacyMesh->vFloatVertices[face->v[j]].sNormal.x, legacyMesh->vFloatVertices[face->v[j]].sNormal.y, legacyMesh->vFloatVertices[face->v[j]].sNormal.z );

                                tangentpos = Math::Vector3( legacyMesh->vFloatVertices[face->v[j]].sTangent.x, legacyMesh->vFloatVertices[face->v[j]].sTangent.y, legacyMesh->vFloatVertices[face->v[j]].sTangent.z );
                                bitangentpos = Math::Vector3( legacyMesh->vFloatVertices[face->v[j]].sBitangent.x, legacyMesh->vFloatVertices[face->v[j]].sBitangent.y, legacyMesh->vFloatVertices[face->v[j]].sBitangent.z );
							}

							Math::Vector2 uv1( face->textureLink->u[j], face->textureLink->v[j] );
							D3DCOLOR vertexColor = hasVertexColor ? Math::Color( verticesColor[face->v[j]].r, verticesColor[face->v[j]].g, verticesColor[face->v[j]].b ).ToUInt() : -1;

							IO::SMD::PackedVertex packedVertex = { 0 };
							packedVertex.position = position;
							packedVertex.color = vertexColor;
							packedVertex.boneIndex = skinnedMesh ? skinnedVerticesIndex[face->v[j]] : -1;
							
							auto texCoord = face->textureLink;

							//Fill Texture Coordinates Buffer Data
							for( int k = 0; k < _countof( textureCoordData ); k++ )
							{
								if( texCoord == nullptr )
									break;

								if( textureCoordData[k] )
								{
									packedVertex.uv[k].u = texCoord->u[j];
									packedVertex.uv[k].v = texCoord->v[j];

									*(textureCoordData[k]++) = texCoord->u[j];
									*(textureCoordData[k]++) = texCoord->v[j];
								}

								texCoord = texCoord->next;
							}

							//Find Similar Vertex to Build Index Buffer
							unsigned short index = 0;
							bool found = FindSimilarVertex( packedVertex, verticesIndex, index );

							//Found? Push vertex index found, else, current vertex index
							if( found )
								meshPart->indices.push_back( index );
							else
								meshPart->indices.push_back( verticesIndex[packedVertex] = curVertexIndex );

							//Fill Vertex Buffer Data
							*(vertexPositionData++) = position.x;
							*(vertexPositionData++) = position.y;
							*(vertexPositionData++) = position.z;
							*(vertexNormalData++) = normal.x;
							*(vertexNormalData++) = normal.y;
							*(vertexNormalData++) = normal.z;

                            *(vertexTangentData++) = tangentpos.x;
                            *(vertexTangentData++) = tangentpos.y;
                            *(vertexTangentData++) = tangentpos.z;
                            *(vertexBitangentData++) = bitangentpos.x;
                            *(vertexBitangentData++) = bitangentpos.y;
                            *(vertexBitangentData++) = bitangentpos.z;


							MeshSkinningVertex sNewVertex;
                            float faWeight[4] = {1.0f,0.0f,0.0f,0.0f};
                            float faBone[4] = { skinnedMesh ? (float)skinnedVerticesIndex[face->v[j]] : -1.0f, -1.0f,-1.0f,-1.0f};
                            sNewVertex = MeshSkinningVertex( position, faBone, faWeight );

                            if ( legacyMesh->bUseInfluences )
                                sNewVertex = MeshSkinningVertex( position, vSkinWeightData[face->v[j]].saOrderedIndex, vSkinWeightData[face->v[j]].faWeight );

							//Fill structure for Software Skinning
							vertexData.push_back( sNewVertex );

							//Skinned Mesh? So push the bone index
							if ( vertexBlendIndicesData )
							{
								if ( legacyMesh->bUseInfluences )
								{
									*(vertexBlendIndicesData++) = sNewVertex.faBoneIndice[0];
									*(vertexBlendIndicesData++) = sNewVertex.faBoneIndice[1];
									*(vertexBlendIndicesData++) = sNewVertex.faBoneIndice[2];
									*(vertexBlendIndicesData++) = sNewVertex.faBoneIndice[3];

                                    *(vertexBlendWeightsData++) = sNewVertex.faWeight[0];
									*(vertexBlendWeightsData++) = sNewVertex.faWeight[1];
									*(vertexBlendWeightsData++) = sNewVertex.faWeight[2];
									*(vertexBlendWeightsData++) = sNewVertex.faWeight[3];
								}
								else
								{
									*(vertexBlendIndicesData++) = (float)skinnedVerticesIndex[face->v[j]];
                                    *(vertexBlendIndicesData++) = (float)0;
                                    *(vertexBlendIndicesData++) = (float)0;
                                    *(vertexBlendIndicesData++) = (float)0;

									*(vertexBlendWeightsData++) = 1.0f;
                                    *(vertexBlendWeightsData++) = 0.0f;
                                    *(vertexBlendWeightsData++) = 0.0f;
                                    *(vertexBlendWeightsData++) = 0.0f;
								}
							}

							curVertexIndex++;
						}	
					}

					vertexPositionBuffer->Unlock();
                    vertexNormalBuffer->Unlock();
                    vertexTangentBuffer->Unlock();
                    vertexBitangentBuffer->Unlock();

                    if ( vertexBlendIndicesBuffer )
                        vertexBlendIndicesBuffer->Unlock();

					if ( vertexBlendWeightsBuffer )
						vertexBlendWeightsBuffer->Unlock();

					for( auto& texCoordBuffer : textureCoordsBuffer )
						texCoordBuffer->Unlock();
				}
			}
		}

		//Set the new mesh for legacy mesh
		legacyMesh->newMesh = this;

		//Update Finders
        if ( legacyMesh->TmPosCnt >= MAX_MESHKEY_FINDER )
        {
            int iKeysDivided = legacyMesh->TmPosCnt / MAX_MESHKEY_FINDER;

			int iCount = iKeysDivided <= MAX_MESHKEY_FINDER ? 1 : MAX_MESHKEY_FINDER;

            int iIndex = 0;
            int iNextIndex = iKeysDivided;
            for ( int i = 0; i < iCount; i++ )
            {
                if ( (i + 1) == iCount )
                    iNextIndex = legacyMesh->TmPosCnt;

                vKeyPositionFinder.push_back( MeshKeyIndexFinder( iIndex, legacyMesh->TmPos[iIndex].frame, legacyMesh->TmPos[iNextIndex - 1].frame ) );

                iIndex += iKeysDivided;
                iNextIndex += iKeysDivided;
            }
        }

        if ( legacyMesh->TmRotCnt >= MAX_MESHKEY_FINDER )
        {
            int iKeysDivided = legacyMesh->TmRotCnt / MAX_MESHKEY_FINDER;

			int iCount = iKeysDivided <= MAX_MESHKEY_FINDER ? 1 : MAX_MESHKEY_FINDER;

            int iIndex = 0;
            int iNextIndex = iKeysDivided;
            for ( int i = 0; i < iCount; i++ )
            {
                if ( (i + 1) == iCount )
                    iNextIndex = legacyMesh->TmRotCnt;

                vKeyRotationFinder.push_back( MeshKeyIndexFinder( iIndex, legacyMesh->TmRot[iIndex].frame, legacyMesh->TmRot[iNextIndex - 1].frame ) );

                iIndex += iKeysDivided;
                iNextIndex += iKeysDivided;
            }
        }

        if ( legacyMesh->TmScaleCnt >= MAX_MESHKEY_FINDER )
        {
            int iKeysDivided = legacyMesh->TmScaleCnt / MAX_MESHKEY_FINDER;

			int iCount = iKeysDivided <= MAX_MESHKEY_FINDER ? 1 : MAX_MESHKEY_FINDER;

            int iIndex = 0;
            int iNextIndex = iKeysDivided;
            for ( int i = 0; i < iCount; i++ )
            {
				if ( (i + 1) == iCount )
					iNextIndex = legacyMesh->TmScaleCnt;

                vKeyScaleFinder.push_back( MeshKeyIndexFinder( iIndex, legacyMesh->TmScale[iIndex].frame, legacyMesh->TmScale[iNextIndex - 1].frame ) );

                iIndex += iKeysDivided;
                iNextIndex += iKeysDivided;       
            }
        }

		legacyMesh->vInfluences.clear();
		legacyMesh->vFloatVertices.clear();

		vSkinWeightData.clear();

		return true;
	}

	return false;
}
void Mesh::ClipInfluenceEdges( MeshInfluenceVertex & sVertex1, MeshInfluenceVertex & sVertex2 )
{
	
}
bool Mesh::GetMatricesAnimation( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo * frameInfo, Math::Matrix4 & sRotation, Math::Matrix4 & sScale, Math::Matrix4 & sTranslation )
{
	auto PTDegreeToRadians = []( const int deg ) { return (float)(deg & 4095) * D3DX_PI / 2048.0f; };

	int rotationIndex = legacyMesh->TmFrameCnt ? FindAnimationRotation( frame_ ) : 0;
	int scalingIndex = legacyMesh->TmFrameCnt ? FindAnimationScaling( frame_ ) : 0;
	int positionIndex = legacyMesh->TmFrameCnt ? FindAnimationPosition( frame_ ) : 0;

	//Animation was found?
	if ( (!legacyMesh->TmFrameCnt && (legacyMesh->TmRotCnt > 0 || legacyMesh->TmPosCnt > 0 || legacyMesh->TmScaleCnt > 0)) || (legacyMesh->TmFrameCnt && (rotationIndex >= 0 || positionIndex >= 0 || scalingIndex > 0)) )
	{
		bool hasRotationAnimation = legacyMesh->TmRotCnt > 0 && (legacyMesh->TmFrameCnt ? rotationIndex >= 0 : legacyMesh->TmRot[legacyMesh->TmRotCnt - 1].frame > frame_);
		bool hasScalingAnimation = legacyMesh->TmScaleCnt > 0 && (legacyMesh->TmFrameCnt ? scalingIndex >= 0 : legacyMesh->TmScale[legacyMesh->TmScaleCnt - 1].frame > frame_);
		bool hasPositionAnimation = legacyMesh->TmPosCnt > 0 && (legacyMesh->TmFrameCnt ? positionIndex >= 0 : legacyMesh->TmPos[legacyMesh->TmPosCnt - 1].frame > frame_);

		sRotation = Matrix4::Identity;
		sScale = Matrix4::Identity;
		sTranslation = Matrix4::Identity;

		//Rotation
		if ( hasRotationAnimation && (rotationIndex >= 0) && (rotationIndex < legacyMesh->TmRotCnt) )
			ApplyRotationTransform( sRotation, &legacyMesh->TmPrevRot[rotationIndex], &legacyMesh->TmRot[rotationIndex], frame_, frameInfo );
		else
			sRotation = legacyMesh->TmRotate;

		//Scaling
		if ( hasScalingAnimation && (scalingIndex >= 0) && (scalingIndex < legacyMesh->TmScaleCnt) )
			ApplyScalingTransform( sScale, &legacyMesh->TmScale[scalingIndex], frame_ );


		//Position
		if ( hasPositionAnimation && (positionIndex >= 0) && (positionIndex < legacyMesh->TmPosCnt) )
			ApplyTranslationTransform( sTranslation, &legacyMesh->TmPos[positionIndex], frame_ );
		else
		{
			sTranslation._41 = float( legacyMesh->px ) / 256.0f;
			sTranslation._42 = float( legacyMesh->py ) / 256.0f;
			sTranslation._43 = float( legacyMesh->pz ) / 256.0f;
		}

		return true;
	}

	return false;
}
}