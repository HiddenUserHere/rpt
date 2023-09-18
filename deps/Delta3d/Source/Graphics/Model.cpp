#include "PrecompiledHeader.h"
#include "Model.h"

#include "Renderer.h"

namespace Delta3D::Graphics
{
std::function<void( Mesh* )> Model::customRenderer( nullptr );
std::function<void( MaterialCollection* )> Model::customMaterialCollection( nullptr );

Model::Model() : 
	GraphicsImpl::GraphicsImpl(), 
	Core::TimerImpl::TimerImpl(), 
	skeleton( nullptr ), 
	materialCollection( nullptr ), 
	useFrustumCulling( true ), 
	lastRotation( -1, -1, -1 ), 
	scaling( 1.0f, 1.0f, 1.0f ), 
	autoAnimate( false ), 
	version( ModelVersion::SMDModelHeader62 ),
	bonesWorldMatrices( nullptr ), 
	bonesTransformations( nullptr ), 
	forceUpdate( false )
{
}

Model::~Model()
{
	if( !meshes.empty() )
	{
		for( auto& mesh : meshes )
		{
			delete mesh;
			mesh = nullptr;
		}
	}

	meshes.clear();
	orderedMeshes.clear();

	if( materialCollection )
	{
		delete materialCollection;
		materialCollection = nullptr;
	}

	if( bonesTransformations )
	{
		delete[] bonesTransformations;
		bonesTransformations = nullptr;
	}

	if( bonesWorldMatrices )
	{
		delete[] bonesWorldMatrices;
		bonesWorldMatrices = nullptr;
	}

	Core::Timer::DeleteTimer( this );
}

bool Model::AddMesh( Mesh* mesh )
{
	//Mesh is valid?
	if( mesh )
	{
		//First mesh?
		if( meshes.empty() )
			position = Math::Vector3( mesh->legacyMesh->Tm._41 / 256.f, mesh->legacyMesh->Tm._42 / 256.f, mesh->legacyMesh->Tm._43 / 256.f );

		meshes.push_back( mesh );

		//If bounding box its bigger than of Model, so set new bounding box based on Mesh
		if( size.x < mesh->legacyMesh->maxX / 256.f )
			size.x = mesh->legacyMesh->maxX / 256.f;

		if( size.x < mesh->legacyMesh->maxZ / 256.f )
			size.x = mesh->legacyMesh->maxZ / 256.f;

		if( size.y < mesh->legacyMesh->maxY / 256.f )
			size.y = mesh->legacyMesh->maxY / 256.f;

		return true;
	}

	return false;
}

void Model::ReorderMeshes()
{
	if( meshes.empty() )
		return;

	for( auto& mesh : meshes )
	{
		if( mesh->legacyMesh->NodeParent[0] != 0 )
		{
			for( auto& parentMesh : meshes )
			{
				if( _strnicmp( mesh->legacyMesh->NodeParent, parentMesh->legacyMesh->NodeName, 32 ) == 0 )
				{
					mesh->legacyMesh->pParent = parentMesh->legacyMesh;
					break;
				}
			}
		}
	}

	//Clear List of Ordered Meshes
	orderedMeshes.clear();

	//Build a Reserve
	orderedMeshes.reserve( meshes.size() );

	//Put Root Meshes on Top of the List
	for( auto mesh : meshes )
	{
		if( mesh->legacyMesh->pParent == nullptr )
			orderedMeshes.push_back( mesh );
	}

	//Put Child Meshes in the List
	for( auto mesh : meshes )
	{
		if( mesh->legacyMesh->pParent == nullptr )
			continue;

		std::list<Mesh*> parentList;

		auto parentMesh = (Mesh*)mesh->legacyMesh->pParent->newMesh;
		while( parentMesh )
		{
			bool existsInList = false;

			for( auto orderedMesh : orderedMeshes )
			{
				if( orderedMesh == parentMesh )
				{
					existsInList = true;
					break;
				}
			}

			if( existsInList )
				break;

			parentList.push_front( parentMesh );
			parentMesh = (Mesh*)parentMesh->legacyMesh->newMesh;
		}

		for( auto mesh : parentList )
			orderedMeshes.push_back( mesh );

		orderedMeshes.push_back( mesh );
	}

	//Offset Matrix
	for ( auto mesh : orderedMeshes )
	{
		mesh->offsetMatrix = mesh->transformationMatrix.Inverse();
		if ( mesh->legacyMesh )
		{
			for ( auto parent = mesh->legacyMesh->pParent; parent != nullptr; parent = parent->pParent )
				mesh->offsetMatrix = ((Mesh*)parent->newMesh)->transformationMatrix.Inverse() * mesh->offsetMatrix;
		}
	}
}

void Model::SetParent( Model* modelParent, Mesh* meshParent )
{
	skeleton = modelParent;
}

Mesh* Model::GetMesh( std::string meshName )
{
	if( meshes.empty() )
		return nullptr;

	for( auto& mesh : meshes )
		if( mesh )
			if( _strcmpi( mesh->legacyMesh->NodeName, meshName.c_str() ) == 0 )
				return mesh;

	return nullptr;
}

int Model::GetOrderedMeshIndex( const std::string& meshName )
{
	if( orderedMeshes.empty() )
		return -1;

	for( size_t i = 0; i < orderedMeshes.size(); i++ )
	{
		if( orderedMeshes[i] )
			if( _strcmpi( orderedMeshes[i]->legacyMesh->NodeName, meshName.c_str() ) == 0 )
				return i;
	}

	return -1;
}

int Model::GetOrderedMeshIndex( int iMeshID )
{
    if ( orderedMeshes.empty() )
        return -1;

    if ( meshes.empty() )
        return -1;

	if ( (iMeshID >= 0) && ((unsigned int)iMeshID < meshes.size()) )
	{
		for ( size_t i = 0; i < orderedMeshes.size(); i++ )
		{
			if ( orderedMeshes[i] )
				if ( orderedMeshes[i] == meshes[iMeshID] )
					return i;
		}
	}

	return -1;
}

int Model::GetMeshIndex( const std::string& meshName )
{
	if( meshes.empty() )
		return -1;

	for( size_t i = 0; i < meshes.size(); i++ )
	{
		if( meshes[i] )
			if( _strcmpi( meshes[i]->legacyMesh->NodeName, meshName.c_str() ) == 0 )
				return i;
	}

	return -1;
}

std::vector<Mesh*> Model::GetMeshes( std::string meshName )
{
	std::vector<Mesh*> ret;

	for( auto& mesh : meshes )
		if( mesh )
			if( _strcmpi( mesh->legacyMesh->NodeName, meshName.c_str() ) == 0 )
				ret.push_back( mesh );

	return ret;
}

void Model::SetFrame( int frame_, IO::SMD::FrameInfo* frameInfo )
{
	if( frame_ >= 0 )
	{
		if( skeleton )
		{
			skeleton->Animate( frame_, rotation, frameInfo );

			//Update World Matrices
			if( !meshes.empty() )
				for( auto& mesh : meshes )
					if( mesh->legacyMesh->pParent )
					{
						auto parentMesh = (Mesh*)mesh->legacyMesh->pParent->newMesh;
						mesh->world = parentMesh->world;
					}
		}
		else
			Animate( frame_, rotation, frameInfo );
	}

	//Update Bounding Volumes not Forced
	UpdateBoundingVolumes();
}

void Model::Animate( int frame_, Math::Vector3Int rotation_, IO::SMD::FrameInfo* frameInfo )
{
	if( (forceUpdate == false) && legacyModel->TmLastFrame == frame_ && lastRotation == rotation_ )
		return;

	legacyModel->TmLastFrame = frame_;
	lastRotation = rotation_;
	lastFrame = frame_;

	if ( vMeshAnimationOld.size() == orderedMeshes.size() )
	{
		for ( unsigned i = 0; i < orderedMeshes.size(); i++ )
		{
			orderedMeshes[i]->fAnimationBlendingTime = fAnimationBlendingTime;
			orderedMeshes[i]->vMeshAnimationOld = vMeshAnimationOld[i];
		}
	}

    if ( frameInfo )
    {
        lastFrameBaseStartFrame = frameInfo->startFrame;
        lastFrameBaseEndFrame = frameInfo->endFrame;
    }

	//Animate Meshes
	int boneIndex = 0;
	for( const auto& mesh : orderedMeshes )
	{
		mesh->Animate( frame_, rotation_, frameInfo );

		if( bonesWorldMatrices )
			bonesWorldMatrices[boneIndex++] = mesh->world;
	}

	if ( vMeshAnimationOld.size() == orderedMeshes.size() )
	{
		for ( unsigned i = 0; i < orderedMeshes.size(); i++ )
			vMeshAnimationOld[i] = orderedMeshes[i]->vMeshAnimationOld;
	}

	//Update Bones Transformations
	UpdateBonesTransformations();
}

void Model::SetPositionRotation( Math::Vector3* position_, Math::Vector3Int* rotation_ )
{
	if( position_ && rotation_ )
	{
		position.x = position_->x;
		position.y = position_->y;
		position.z = position_->z;

		rotation.x = rotation_->x;
		rotation.y = rotation_->y;
		rotation.z = rotation_->z;

		if( !meshes.empty() )
			for( const auto& mesh : meshes )
				if( mesh )
					mesh->SetPositionRotation( position_, rotation_ );
	}
}

void Model::UpdateBoundingVolumes( bool forceUpdate )
{
	if( boundingSphere.radius == 0.0f || forceUpdate == true )
	{
		//Reset Bounding Box
		BoundingBox boundingBox = BoundingBox();

		for( const auto& mesh : orderedMeshes )
		{
			if( mesh )
			{
				mesh->UpdateBoundingVolumes();
				boundingBox.Merge( mesh->worldBoundingBox );
			}
		}

		//Update Bounding Sphere Radius and Center
		boundingSphere.radius = std::max( { boundingBox.Size().x,  boundingBox.Size().y, boundingBox.Size().z } )* 0.5f;
		boundingSphere.center = boundingBox.Center();

		worldBoundingBox.min = boundingBox.min;
		worldBoundingBox.max = boundingBox.max;
	}
}

void Model::UpdateBonesTransformations()
{
	return;
}

void Model::SetDiffuseColor( Math::Color color )
{
	if( materialCollection )
	{
		if( materialCollection->materials.size() )
		{
			for( const auto& material : materialCollection->materials )
			{
				if( material )
				{
					material->legacyMaterial->DiffuseColor[0] = color.r;
					material->legacyMaterial->DiffuseColor[1] = color.g;
					material->legacyMaterial->DiffuseColor[2] = color.b;
					material->legacyMaterial->Transparency = 1.f - color.a;
				}
			}
		}
	}
}

void Model::SetAddColor( Math::Color color )
{
	if( materialCollection )
	{
		if( materialCollection->materials.size() )
		{
			for( const auto& material : materialCollection->materials )
			{
				if( material )
					material->addColor = color;
			}
		}
	}
}

void Model::Update( float timeElapsed )
{
	//Update Meshes
	for( const auto& mesh : meshes )
		if( mesh )
			mesh->Update( timeElapsed );

	//Auto Animate Model?
	if( autoAnimate )
	{
		legacyModel->Frame += (int)((legacyModel->MaxFrame / 4800.0f )* timeElapsed);

		//Repeat Animation
		if( legacyModel->Frame > legacyModel->MaxFrame )
			legacyModel->Frame = 160;
	}
}

bool Model::Render( IO::SMD::FrameInfo* frameInfo, ModelGroup* modelGroup )
{
	bool useCustomRenderer = (materialCollection) && materialCollection->legacyMaterialCollection->ReformTexture && !skeleton && customRenderer;

	//Apply Material Collection to old renderer
	if( customMaterialCollection )
		customMaterialCollection( materialCollection );

	//Check Frustum Culling
	if( !useCustomRenderer && useFrustumCulling && boundingSphere.radius != 0.0f && renderer->GetCamera()->Frustum().IsInside( boundingSphere.Transformed( position ) ) == Intersection::Outside )
		return false;

	//Set Model Frame
	SetFrame( legacyModel->Frame, frameInfo );

	//Draw Bounding Sphere on Debug Mode
	if( renderer->IsDebugGeometry( DebugGeometry::DebugModel ) )
		renderer->DrawDebugSphere( boundingSphere.Transformed( position ) );

	//Drawable Meshes
	auto CanRenderMesh = [&]( const Mesh* p )
	{
		if( modelGroup == nullptr )
			return true;

		if( modelGroup->meshes.empty() )
			return true;
		else
		{
			for ( const auto & meshGroup : modelGroup->meshes )
				if ( p == meshGroup )
					return true;
		}

		return false;
	};

	//Render Meshes
	for( auto& mesh : meshes )
	{
		if( useCustomRenderer )
			customRenderer( mesh );
		else if( mesh->postRender )
			graphics->renderer->PushPostRenderMesh( mesh );
		else if( CanRenderMesh( mesh ) )
			mesh->Render();
	}

	return true;
}

bool Model::Load( std::string filePath, Model* skeleton_, bool temporaryTextures, Legacy::Model* legacyModel_ )
{
	FILE* file = nullptr;
	fopen_s( &file, filePath.c_str(), "rb" );

	if( file )
	{
		//Set Legacy Model
		legacyModel = legacyModel_;

		//Read Header
		fseek( file, sizeof( IO::SMD::Header ) + sizeof( IO::SMD::ObjectInfo ) * legacyModel->nObj3d, SEEK_CUR );

		//Load Materials
		if( legacyModel->smMaterialGroup )
		{
			if( materialCollection = new MaterialCollection(); materialCollection )
				materialCollection->Build( file, skeleton_ ? true : false, true, false, 0, temporaryTextures, true, legacyModel ? legacyModel->smMaterialGroup : nullptr );
		}

		fAnimationBlendingTime = 1.0f;

		//Add Objects to Pattern
		for( int i = 0; i < legacyModel->nObj3d; i++ )
		{
			Legacy::Mesh* legacyMesh = legacyModel->obj3d[i];

			if( legacyMesh )
			{
				Mesh* mesh = new Mesh();
				if( mesh )
				{
					if ( i == 0 )
					{
						localMatrix = legacyMesh->Tm;
					}

					mesh->inverseMatrix = localMatrix.Inverse();

					mesh->modelParent = this;
					mesh->Build( skeleton_, legacyMesh );
					AddMesh( mesh );

					mesh->fAnimationBlendingTime = fAnimationBlendingTime;

					meshesByName[mesh->legacyMesh->NodeName] = mesh;
				}
			}
		}

		//Link Objets Parent
		ReorderMeshes();

		//Set Model Skeleton
		skeleton = skeleton_;

		//Create Texture for bones fetch
		if( skeleton && graphics->useSoftwareSkinning == false )
		{
			skeleton->bonesWorldMatrices = new Math::Matrix4[skeleton->orderedMeshes.size()];
			skeleton->bonesTransformations = new float[skeleton->orderedMeshes.size() * 16];
		}

		SetFrame( 0 );

		//Create Old Animations Data
		for ( auto mesh : orderedMeshes )
			vMeshAnimationOld.push_back( mesh->vMeshAnimationOld );

		fclose( file );

		return true;
	}

	return false;
}

void Model::CleanMemory()
{
	for( auto& mesh : meshes )
	{
		if( mesh->legacyMesh )
		{
			if( mesh->legacyMesh->Vertex )
			{
				delete[] mesh->legacyMesh->Vertex;
				mesh->legacyMesh->Vertex = nullptr;
				mesh->legacyMesh->nVertex = 0;
			}

			if( mesh->legacyMesh->Face )
			{
				delete[] mesh->legacyMesh->Face;
				mesh->legacyMesh->Face = nullptr;
				mesh->legacyMesh->nFace = 0;
			}

			if( mesh->legacyMesh->TexLink )
			{
				delete[] mesh->legacyMesh->TexLink;
				mesh->legacyMesh->TexLink = nullptr;
				mesh->legacyMesh->nTexLink = 0;
			}

			if( mesh->legacyMesh->Physique )
			{
				delete[] mesh->legacyMesh->Physique;
				mesh->legacyMesh->Physique = nullptr;
			}
		}
	}
}

void Model::GetBlendingAnimationValues( std::vector<MeshAnimationDataOld> & vMeshAnimations )
{
	if ( vMeshAnimations.size() == vMeshAnimationOld.size() )
		memcpy( vMeshAnimations.data(), vMeshAnimationOld.data(), sizeof( MeshAnimationDataOld ) * vMeshAnimationOld.size() );
}

void Model::SetBlendingAnimationValues( float _fAnimationBlendingTime, std::vector<MeshAnimationDataOld> & vMeshAnimations )
{
	if ( vMeshAnimations.size() == vMeshAnimationOld.size() )
	{
		memcpy( vMeshAnimationOld.data(), vMeshAnimations.data(), sizeof( MeshAnimationDataOld ) * vMeshAnimations.size() );

		fAnimationBlendingTime = _fAnimationBlendingTime;

		//Fix Time Below
		if ( fAnimationBlendingTime < 0.0f )
			fAnimationBlendingTime = 0.0f;

		//Fix Time Above
		if ( fAnimationBlendingTime > 1.0f )
			fAnimationBlendingTime = 1.0f;
	}
}

}