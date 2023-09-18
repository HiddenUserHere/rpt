#include "stdafx.h"
#include "CEffekseerHandler.h"

#include "stb_image_utils.h"


void __stdcall RemoveCallBack( Effekseer::Manager * manager, Effekseer::Handle handle, bool isRemovingManager )
{
	if ( EFFEKSEERHANDLER )
		EFFEKSEERHANDLER->KillHandle( handle );
}


Effekseer::TextureRef CEffekseerTextureLoader::Load( const EFK_CHAR * path, ::Effekseer::TextureType textureType )
{
	std::filesystem::path p = (const WCHAR *)path;

	std::wstring strFile = std::wstring( L"game\\textures\\particles\\" ) + std::wstring( p.filename().wstring() );

	auto reader = pcFileInterface->OpenRead( (char16_t*)strFile.c_str() );
	if ( reader != nullptr )
	{
		size_t size_texture = reader->GetLength();
		std::vector<char> data_texture;
		data_texture.resize( size_texture );
		reader->Read( data_texture.data(), size_texture );

		// load an image with the helper function
		// ヘルパー関数で画像を読み込む
		int width;
		int height;
		int bpp;
		uint8_t * pixels = (uint8_t *)EffekseerUtils::stbi_load_from_memory(
			(EffekseerUtils::stbi_uc const *)data_texture.data(), static_cast<int>(size_texture), &width, &height, &bpp, 0 );

		if ( width == 0 || bpp < 2 )
		{
			// Not supported
			EffekseerUtils::stbi_image_free( pixels );
			return nullptr;
		}

		// Load a image to GPU actually. Please see a code of each backends if you want to know what should be returned
		// 実際にGPUに画像を読み込む。何を返すべきか知りたい場合、、各バックエンドのコードを読んでください。
		::Effekseer::Backend::TextureParameter param;
		param.Format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		param.MipLevelCount = 1;
		param.Size = {width, height, 0};
		param.Dimension = 2;
		Effekseer::CustomVector<uint8_t> initialData;

		if ( bpp == 4 )
		{
			initialData.assign( pixels, pixels + width * height * 4 );
		}
		else if ( bpp == 3 )
		{
			initialData.resize( width * height * 4 );
			for ( int y = 0; y < height; y++ )
			{
				for ( int x = 0; x < width; x++ )
				{
					initialData[(x + y * width) * 4 + 0] = pixels[(x + y * width) * 3 + 0];
					initialData[(x + y * width) * 4 + 1] = pixels[(x + y * width) * 3 + 1];
					initialData[(x + y * width) * 4 + 2] = pixels[(x + y * width) * 3 + 2];
					initialData[(x + y * width) * 4 + 3] = 255;
				}
			}
		}
		else if ( bpp == 2 )
		{
			initialData.resize( width * height * 4 );
			
			uint16_t * p = (uint16_t *)pixels;

			//Convert 16 bpp to 32 bpp
			for ( int y = 0; y < height; y++ )
			{
				for ( int x = 0; x < width; x++ )
				{
					uint16_t c = p[x + y * width];
					uint8_t r = (c & 0xF800) >> 8;
					uint8_t g = (c & 0x07E0) >> 3;
					uint8_t b = (c & 0x001F) << 3;
					uint8_t a = (c & 0x8000) >> 8;

					initialData[(x + y * width) * 4 + 0] = r;
					initialData[(x + y * width) * 4 + 1] = g;
					initialData[(x + y * width) * 4 + 2] = b;
					initialData[(x + y * width) * 4 + 3] = a;
				}
			}
		}

		auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
		texture->SetBackend( pcGraphicsDeviceEffekseer->CreateTexture( param, initialData ) );
		return texture;
	}

	return nullptr;
}

void CEffekseerTextureLoader::Unload( Effekseer::TextureRef psTextureData )
{

}


CEffekseerHandler::CEffekseerHandler()
{
}

CEffekseerHandler::~CEffekseerHandler()
{
	pRenderTargetDistortion = nullptr;

	for ( auto pcParticleSystem : vParticleSystems )
		DELET( pcParticleSystem );

	vParticleSystems.clear();

	for ( auto pcParticleScript : vParticleScriptCache )
		DELET( pcParticleScript );

	vParticleScriptCache.clear();
}

void CEffekseerHandler::Init()
{
	//Create Renderer
	pcRenderer = EffekseerRendererDX9::Renderer::Create( GetGraphicsDevice()->Get(), CEffekseerHandler::MAX_EFFEKSEER_SPRITES );

	//Create Manager
	pcManager = Effekseer::Manager::Create( CEffekseerHandler::MAX_EFFEKSEER_SPRITES );

	//Create Renderers
	pcManager->SetSpriteRenderer( pcRenderer->CreateSpriteRenderer() );
	pcManager->SetRibbonRenderer( pcRenderer->CreateRibbonRenderer() );
	pcManager->SetRingRenderer( pcRenderer->CreateRingRenderer() );
	pcManager->SetTrackRenderer( pcRenderer->CreateTrackRenderer() );
	pcManager->SetModelRenderer( pcRenderer->CreateModelRenderer() );
	
	//Create Texture Loader (Create a custom Loader since we can load from a new directory)
	pcManager->SetTextureLoader( ::Effekseer::MakeRefPtr<CEffekseerTextureLoader>( pcRenderer->GetGraphicsDevice() ) );
	pcManager->SetModelLoader( ::Effekseer::MakeRefPtr<CustomModelLoader>() );

	//Set Coordinate System
	pcManager->SetCoordinateSystem( Effekseer::CoordinateSystem::LH );

	//Set Distortion
	pRenderTargetDistortion = GetGraphicsDevice()->MakeRenderTarget( 777, 0 );

	//Set Distortion
	pcRenderer->SetDistortingCallback( new CEffekseerDistortion() );

	//Create Culling
	//pcManager->CreateCullingWorld( 1000.0f, 1000.0f, 1000.0f, 6 );

	CreateRenderTarget();
}

CEffekSeerParticleScript * CEffekseerHandler::LoadScript( const std::string strFileName )
{
	CEffekSeerParticleScript * pcRet = nullptr;

	CEffekSeerParticleScript * pcParticleScript = GetScriptFromCache( strFileName );
	if ( pcParticleScript == nullptr )
	{
		//Convert File Name to UTF-16
		char16_t szFileName[MAX_PATH] = { 0 };
		Effekseer::ConvertUtf8ToUtf16( szFileName, MAX_PATH, strFileName.c_str() );

		FILE * pFile = nullptr;
		if ( fopen_s( &pFile, strFileName.c_str(), "rb" ) == 0 )
		{
			//Get File Size
			fseek( pFile, 0, SEEK_END );
			int32_t iFileSize = ftell( pFile );

			//Reset
			fseek( pFile, 0, SEEK_SET );

			//Read Buffer
			BYTE * pbBuffer = new BYTE[iFileSize];
			fread( pbBuffer, iFileSize, 1, pFile );

			//Close
			fclose( pFile );

			//New Script Cache
			pcParticleScript = new CEffekSeerParticleScript( strFileName, Effekseer::Effect::Create( pcManager, pbBuffer, iFileSize, 14.0f ) );

			//Add to Cache
			AddScriptToCache( pcParticleScript );

			//Delete Buffer
			DELETA( pbBuffer );
		}
	}

	//Has a Script?
	if ( pcParticleScript )
	{
		//Add New One
		pcRet = new CEffekSeerParticleScript( *pcParticleScript );
	}

	return pcRet;
}

bool CEffekseerHandler::AddScript( CEffekSeerParticleScript * pcScript )
{
#if defined(_DEBUG)
	//return false;
#endif
	bool bRet = false;

	CPoint<int> sPosition = CPoint<int>( 0, 0, 0 );
	if ( pcScript->GetParticleSystems().size() > 0 )
	{
		if ( pcScript->GetParticleSystems()[0]->GetOwner() )
		{
			pcScript->GetParticleSystems()[0]->GetOwner()->Update();

			sPosition = pcScript->GetParticleSystems()[0]->GetOwner()->GetPosition();
		}
	}

	if ( auto iHandle = pcManager->Play( pcScript->GetEffect(), (float)sPosition.GetX() / 256.0f, (float)sPosition.GetY() / 256.0f, (float)sPosition.GetZ() / 256.0f ); iHandle >= 0 )
	{
		for ( auto pcParticleSystem : pcScript->GetParticleSystems() )
		{
			//Set Handle
			pcParticleSystem->SetHandle( iHandle );

			//Add Particle System
			vParticleSystems.push_back( pcParticleSystem );
		}

		if ( pcScript->GetParticleSystems().size() > 0 )
		{
			if ( pcScript->GetParticleSystems()[0]->GetOwner() )
			{
				auto sRot = pcScript->GetParticleSystems()[0]->GetOwner()->GetRotation();
				X3D::Vector3 vRot = X3D::Vector3( PTAngleToRadians( sRot.iX ), PTAngleToRadians( sRot.iY ), PTAngleToRadians( sRot.iZ ) );
				pcManager->SetRotation( iHandle, vRot.x, vRot.y, vRot.z );
			}
		}

		pcManager->SetRemovingCallback( iHandle, RemoveCallBack );

		bRet = true;
	}

	pcScript->Clear();
	
	DELET( pcScript );

	return bRet;
}

void CEffekseerHandler::Kill( UINT uID )
{
	for ( auto pcParticleSystem : vParticleSystems )
	{
		if ( pcParticleSystem->GetID() == uID )
			pcParticleSystem->Kill( TRUE );
	}
}

void CEffekseerHandler::Kill( UINT uID, UINT uOwnerID )
{
	for ( auto pcParticleSystem : vParticleSystems )
	{
		if ( (pcParticleSystem->GetID() == uID) && (pcParticleSystem->GetOwnerID() == uOwnerID) )
			pcParticleSystem->Kill( TRUE );
	}
}

void CEffekseerHandler::KillHandle( Effekseer::Handle iHandle )
{
	for ( auto it = vParticleSystems.begin(); it != vParticleSystems.end(); )
	{
		auto pcParticleSystem = (*it);
		if ( pcParticleSystem->GetHandle() == iHandle )
		{
			DELET( pcParticleSystem );

			it = vParticleSystems.erase( it );
		}
		else
			it++;
	}
}

void CEffekseerHandler::Update( float fTime )
{
	auto sCameraEye = X3D::Vector3( GRAPHICS->GetRenderer()->GetCamera()->Eye().x, GRAPHICS->GetRenderer()->GetCamera()->Eye().y, GRAPHICS->GetRenderer()->GetCamera()->Eye().z );

	auto GetAngle = []( X3D::Vector3 a, X3D::Vector3 b ) -> float
	{
		return acosf( a.dot( b ) / (a.length() * b.length()) );
	};

	for ( auto it = vParticleSystems.begin(); it != vParticleSystems.end(); )
	{
		auto pcParticleSystem = (*it);

		//Can Update?
		if ( pcParticleSystem->Update( fTime ) )
		{
			//Have an Owner?
			if ( pcParticleSystem->GetOwner() )
			{
				auto sOwnerPosition = pcParticleSystem->GetOwner()->GetPosition();
				Effekseer::Vector3D sPosition = Effekseer::Vector3D( (float)sOwnerPosition.GetX() / 256.0f, (float)sOwnerPosition.GetY() / 256.0f, (float)sOwnerPosition.GetZ() / 256.0f );

				//Rotate to View?
				if ( pcParticleSystem->IsRotateToView() )
				{
					//Get Rotation by inverse lookat
					auto sDirection = (sCameraEye - X3D::Vector3( sPosition.X, sPosition.Y, sPosition.Z )).normalize();

					//Get Rotation
					auto sRotation = X3D::Vector3( 0.0f, 0.0f, 0.0f );
					sRotation.x = GetAngle( X3D::Vector3( 1.0f, 0.0f, 0.0f ), sDirection );
					sRotation.y = GetAngle( X3D::Vector3( 0.0f, 1.0f, 0.0f ), sDirection );
					sRotation.z = GetAngle( X3D::Vector3( 0.0f, 0.0f, 1.0f ), sDirection );
					
					
					//Set Rotation
					pcManager->SetRotation( pcParticleSystem->GetHandle(), sRotation.x, sRotation.y, sRotation.z );
				}

				//Move Particle
				pcManager->SetLocation( pcParticleSystem->GetHandle(), sPosition );

				//Has Target?
				if ( pcParticleSystem->GetOwner()->HasTarget() )
				{
					auto sTargetPosition = pcParticleSystem->GetOwner()->GetTargetPosition();
					
					Effekseer::Vector3D sTPosition = Effekseer::Vector3D( (float)sTargetPosition.iX / 256.0f, (float)sTargetPosition.iY / 256.0f, (float)sTargetPosition.iZ / 256.0f );
					pcManager->SetTargetLocation( pcParticleSystem->GetHandle(), sTPosition );
				}
			}

			it++;
		}
		else
		{
			EFFEKSEERHANDLER->GetManager()->StopEffect( pcParticleSystem->GetHandle() );

			DELET( pcParticleSystem );

			it = vParticleSystems.erase( it );
		}
	}

	//Update Particles Effects
	pcManager->Update();
}

void CEffekseerHandler::Render()
{
	//Begin Renderer
	if ( pcRenderer->BeginRendering() )
	{
		//Reset Render States
		pcRenderer->ResetRenderState();

		GRAPHICS->GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		GRAPHICS->GetDevice()->SetRenderState( D3DRS_ALPHAREF, 128 );
		GRAPHICS->GetDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		GRAPHICS->GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		GRAPHICS->GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		GRAPHICS->GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );

		//Draw Particle Back
		for ( auto pcParticleSystem : vParticleSystems )
			pcParticleSystem->Render( pcManager, TRUE );

		//Draw Particle Front
		for ( auto pcParticleSystem : vParticleSystems )
			pcParticleSystem->Render( pcManager, FALSE );

		//End Renderer
		pcRenderer->EndRendering();
	}
}

void CEffekseerHandler::OnLost( CEffekSeerParticleSystem * pcParticleSystem )
{
	if ( pcParticleSystem->GetID() == PARTICLEID_MAGNETIC_DISCHARGE )
	{
		if ( auto pcUnit = UNITDATATOUNIT( UNITDATABYID( pcParticleSystem->GetOwnerID() ) ) )
			MAGNETICDISCHARGEHANDLER->OnCastEnd( pcUnit );
	}
}

void CEffekseerHandler::OnLostDevice()
{
	((EffekseerRendererDX9::RendererRef)EFFEKSEERHANDLER->GetRenderer())->SetBackground( NULL );

	pcRenderer->OnLostDevice();

	for ( auto pcParticleSystem : vParticleSystems )
		pcParticleSystem->GetEffect()->UnloadResources();

	RELEASE( lpRenderSurface );
}

void CEffekseerHandler::OnResetDevice()
{
	pcRenderer->OnResetDevice();

	for ( auto pcParticleSystem : vParticleSystems )
		pcParticleSystem->GetEffect()->ReloadResources();

	CreateRenderTarget();
}

CEffekSeerParticleScript * CEffekseerHandler::GetScriptFromCache( const std::string strFileName )
{
	for ( auto pcParticleScript : vParticleScriptCache )
	{
		//Same file in Cache? So return it
		if ( pcParticleScript->GetFileName().compare( strFileName ) == 0 )
			return pcParticleScript;
	}

	return nullptr;
}

void CEffekseerHandler::AddScriptToCache( CEffekSeerParticleScript * pcParticleScript )
{
	vParticleScriptCache.push_back( pcParticleScript );
}

void CEffekseerHandler::CreateRenderTarget()
{
	RELEASE( lpRenderSurface );

	if ( SUCCEEDED( DX::Graphic::GetDevice()->CreateRenderTarget( GRAPHICS->presentParams.BackBufferWidth, GRAPHICS->presentParams.BackBufferHeight,
		 GRAPHICS->presentParams.BackBufferFormat == D3DFMT_X4R4G4B4 ? D3DFMT_A4R4G4B4 : D3DFMT_A8R8G8B8,
		 GRAPHICS->presentParams.MultiSampleType, GRAPHICS->presentParams.MultiSampleQuality, FALSE, &lpRenderSurface, NULL ) ) )
	{

	}
}

bool CEffekseerDistortion::CopyRenderTarget()
{
	bool bRet = false;
	IDirect3DSurface9 * pBackBuffer				= nullptr;
	IDirect3DSurface9 * pDepthStencilSurface	= nullptr;

	if ( SUCCEEDED( GetGraphicsDevice()->Get()->GetRenderTarget( 0, &pBackBuffer ) ) )
	{
		if ( SUCCEEDED( GetGraphicsDevice()->Get()->GetDepthStencilSurface( &pDepthStencilSurface ) ) )
		{
			GetGraphicsDevice()->Get()->SetRenderTarget( 0, EFFEKSEERHANDLER->GetDistortionRenderTarget() );
			GetGraphicsDevice()->Get()->SetDepthStencilSurface( nullptr );

			D3DSURFACE_DESC desc;
			pBackBuffer->GetDesc( &desc );

			GetGraphicsDevice()->Get()->StretchRect( pBackBuffer, nullptr, EFFEKSEERHANDLER->GetDistortionRenderTarget(), nullptr, D3DTEXF_POINT );

			GetGraphicsDevice()->Get()->SetRenderTarget( 0, pBackBuffer );
			GetGraphicsDevice()->Get()->SetDepthStencilSurface( pDepthStencilSurface );

			GetGraphicsDevice()->Get()->StretchRect( EFFEKSEERHANDLER->GetDistortionRenderTarget(), nullptr, EFFEKSEERHANDLER->GetRenderTarget()->GetSurface(), nullptr, D3DTEXF_POINT );


			ES_SAFE_RELEASE( pDepthStencilSurface );

			bRet = true;
		}

		ES_SAFE_RELEASE( pBackBuffer );
	}

	return bRet;
}

bool CEffekseerDistortion::OnDistorting( EffekseerRenderer::Renderer * renderer )
{
	if ( EFFEKSEERHANDLER->GetRenderTarget()->GetTexture() == NULL )
	{
		reinterpret_cast<EffekseerRendererDX9::Renderer *>(renderer)->SetBackground( NULL );
		return false;
	}

	CopyRenderTarget();

	reinterpret_cast<EffekseerRendererDX9::Renderer *>(renderer)->SetBackground( EFFEKSEERHANDLER->GetRenderTarget()->GetTexture() );
	return true;
}
