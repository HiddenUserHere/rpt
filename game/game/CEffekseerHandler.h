#pragma once

#include <filesystem>

#include "CPoint.h"
#include "ObjectOwner.h"

#include "CEffekSeerParticleScript.h"

class CEffekseerDistortion : public EffekseerRenderer::DistortingCallback
{
public:
	CEffekseerDistortion()
	{

	}

	virtual ~CEffekseerDistortion()
	{

	}

	bool			CopyRenderTarget();

	bool			OnDistorting( EffekseerRenderer::Renderer * renderer ) override;
};

class CEffekseerTextureLoader : public ::Effekseer::TextureLoader
{
private:
	::Effekseer::Backend::GraphicsDeviceRef pcGraphicsDeviceEffekseer;
	::Effekseer::FileInterface * pcFileInterface;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

public:
	CEffekseerTextureLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterface * fileInterface = nullptr )
	{
		pcGraphicsDeviceEffekseer = graphicsDevice;
		pcFileInterface = fileInterface;
		if ( pcFileInterface == nullptr )
		{
			pcFileInterface = &defaultFileInterface_;
		}
	};
	virtual ~CEffekseerTextureLoader() {};

	Effekseer::TextureRef			Load( const EFK_CHAR * path, Effekseer::TextureType textureType ) override;
	void							Unload( Effekseer::TextureRef data ) override;
};

class CustomModelLoader : public ::Effekseer::ModelLoader
{
private:
	::Effekseer::DefaultFileInterface defaultFileInterface_;
	::Effekseer::FileInterface * fileInterface_;

public:
	CustomModelLoader( ::Effekseer::FileInterface * fileInterface = nullptr )
		: fileInterface_( fileInterface )
	{
		if ( fileInterface == nullptr )
		{
			fileInterface_ = &defaultFileInterface_;
		}
	}

	~CustomModelLoader() override = default;

public:
	Effekseer::ModelRef Load( const char16_t * path ) override
	{
		std::filesystem::path p = (const WCHAR *)path;

		std::wstring strFile = std::wstring( L"game\\models\\particles\\" ) + std::wstring( p.filename().wstring() );

		auto reader = fileInterface_->OpenRead( (char16_t *)strFile.c_str() );
		if ( reader == nullptr )
		{
			return nullptr;
		}

		size_t size = reader->GetLength();
		std::unique_ptr<uint8_t[]> data( new uint8_t[size] );
		reader->Read( data.get(), size );

		auto model = Load( data.get(), (int32_t)size );

		return model;
	}

	Effekseer::ModelRef Load( const void * data, int32_t size ) override
	{
		auto model = ::Effekseer::MakeRefPtr<::Effekseer::Model>( (const uint8_t *)data, size );

		return model;
	}

	void Unload( Effekseer::ModelRef data ) override
	{
	}
};


class CEffekseerHandler
{
private:
	static const int				MAX_EFFEKSEER_SPRITES = 2000;

public:
	CEffekseerHandler();
	~CEffekseerHandler();

	EffekseerRendererDX9::RendererRef GetRenderer() { return pcRenderer; }
	Effekseer::ManagerRef			  GetManager() { return pcManager; };

	LPDIRECT3DSURFACE9				GetDistortionRenderTarget() { return lpRenderSurface; }
	DX::RenderTarget_ptr			GetRenderTarget(){ return pRenderTargetDistortion; }

	void							Init();

	CEffekSeerParticleScript		* LoadScript( const std::string strFileName );

	bool							AddScript( CEffekSeerParticleScript * pcScript );

	void							Kill( UINT uID );
	void							Kill( UINT uID, UINT uOwnerID );
	void							KillHandle( Effekseer::Handle iHandle );

	void							Update( float fTime );

	void							Render();

	void							OnLost( CEffekSeerParticleSystem * pcParticleSystem );

	void							OnLostDevice();
	void							OnResetDevice();

private:
	EffekseerRendererDX9::RendererRef	pcRenderer = nullptr;
	Effekseer::ManagerRef				pcManager = nullptr;

	DX::RenderTarget_ptr			pRenderTargetDistortion = nullptr;

	LPDIRECT3DSURFACE9				lpRenderSurface = nullptr;

	std::vector<CEffekSeerParticleScript *> vParticleScriptCache;
	std::vector<CEffekSeerParticleSystem *> vParticleSystems;

	CEffekSeerParticleScript		* GetScriptFromCache( const std::string strFileName );

	void							AddScriptToCache( CEffekSeerParticleScript * pcParticleScript );

	void							CreateRenderTarget();
};

