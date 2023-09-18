#include "PrecompiledHeader.h"
#include "Graphics.h"

#include "RenderTarget.h"
#include "DepthStencilBuffer.h"
#include "Texture.h"
#include "Sprite.h"
#include "Font.h"
#include "Shader.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexElements.h"
#include "VertexDeclaration.h"
#include "IndexBuffer.h"

template <class T>
inline void hash_combine( std::size_t& seed, const T& v )
{
	std::hash<T> hasher;
	seed ^= hasher( v ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
};

namespace Delta3D::Graphics
{

Graphics* Graphics::instance = nullptr;

Graphics::Graphics() :
	appHash( 0 ),
	windowed( false ),
	supportStencil32( true ),
	vsync( false ),
	antiAliasing( false ),
	isDeviceLost( false ),
	size( Vector2Int::Null ),
	stateBlock( StateBlock::None ),
	supportHardwareSkinning( false ),
	useSoftwareSkinning( false ),
	reduceQualityTexture( 0 ),
	pixelShaderVersionMajor( 0 ),
	vertexShaderVersionMajor( 0 ),
	presentParams{0},
	previousPresentParams{0},
	bSkipRendering( false )
{
	//Get Is Supported device
	CommonDevice.interfaced3d = nullptr;
	CommonDevice.device = nullptr;

	renderTargetFactory = std::make_unique<RenderTargetFactory>( this );
	depthStencilBufferFactory = std::make_unique<DepthStencilBufferFactory>( this );
	textureFactory = std::make_unique<TextureFactory>( this );
	spriteFactory = std::make_unique<SpriteFactory>( this );
	fontFactory = std::make_unique<FontFactory>( this );
	shaderFactory = std::make_unique<ShaderFactory>( this );
	attributeAnimationFactory = std::make_unique<Resource::AttributeAnimationFactory>();

	renderer = std::make_unique<Renderer>( this );
}

Graphics::~Graphics()
{
	if( GetInterface() )
	{
		GetInterface()->Release();
		
		CommonDevice.interfaced3d = nullptr;
	}

	if( GetDevice() )
	{
		GetDevice()->Release();
		CommonDevice.device = nullptr;
	}
}

std::shared_ptr<VertexDeclaration> Graphics::CreateVertexDeclaration( std::shared_ptr<VertexElements> vertexElements )
{
	IDirect3DVertexDeclaration9* vertexDeclarationd3d;
	if( FAILED( GetDevice()->CreateVertexDeclaration( vertexElements->GetElements().data(), &vertexDeclarationd3d ) ) )
	{
		DELTA3D_LOGERROR( "Could not create Vertex Declaration!" );
		return nullptr;
	}

	//Create Vertex Declaration
	auto vertexDeclaration = std::make_shared<VertexDeclaration>( vertexDeclarationd3d );

	//Remove our Reference to Vertex Declaration
	vertexDeclarationd3d->Release();

	//Return Vertex Declaration
	return vertexDeclaration;
}

std::shared_ptr<VertexBuffer> Graphics::CreateDynamicVertexBuffer( unsigned int elementSize, unsigned int elementCount )
{
	IDirect3DVertexBuffer9* vertexBufferd3d;
	if( FAILED( GetDevice()->CreateVertexBuffer( elementSize* elementCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vertexBufferd3d, NULL ) ) )
	{
		DELTA3D_LOGERROR( "Could not create Dynamic Vertex Buffer of size %d*%d", elementSize, elementCount );
		return nullptr;
	}

	//Create Vertex Buffer
	auto vertexBuffer = std::make_shared<VertexBuffer>( vertexBufferd3d, elementSize, elementCount, true );

	//Remove our Reference to Vertex Buffer
	vertexBufferd3d->Release();

	dynamicVertexBuffers.push_back( vertexBuffer );

	//Return Vertex Buffer
	return vertexBuffer;
}

std::shared_ptr<VertexBuffer> Graphics::CreateStaticVertexBuffer( unsigned int elementSize, unsigned int elementCount )
{
	IDirect3DVertexBuffer9* vertexBufferd3d;
	if( FAILED( GetDevice()->CreateVertexBuffer( elementSize* elementCount, D3DUSAGE_WRITEONLY, 0, bDeviceEx ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &vertexBufferd3d, NULL ) ) )
	{
		DELTA3D_LOGERROR( "Could not create Vertex Buffer of size %d*%d", elementSize, elementCount );
		return nullptr;
	}

	//Create Vertex Buffer
	auto vertexBuffer = std::make_shared<VertexBuffer>( vertexBufferd3d, elementSize, elementCount );

	//Remove our Reference to Vertex Buffer
	vertexBufferd3d->Release();

	//Return Vertex Buffer
	return vertexBuffer;
}

std::shared_ptr<IndexBuffer> Graphics::CreateIndexBuffer( unsigned int primitiveSize, unsigned int primitiveCount )
{
	IDirect3DIndexBuffer9* indexBufferd3d;
	if( FAILED( GetDevice()->CreateIndexBuffer( primitiveSize* primitiveCount, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, bDeviceEx ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &indexBufferd3d, NULL ) ) )
	{
		DELTA3D_LOGERROR( "Could not create Index Buffer of size %d*%d", primitiveSize, primitiveCount );
		return nullptr;
	}

	//Create Index Buffer
	auto indexBuffer = std::make_shared<IndexBuffer>( indexBufferd3d, primitiveSize, primitiveCount );

	//Remove our Reference to Index Buffer
	indexBufferd3d->Release();

	//Return Index Buffer
	return indexBuffer;
}

void Graphics::SetFillMode( FillMode fillMode )
{
	if( fillMode == FillMode::Point )
		GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_POINT );
	else if( fillMode == FillMode::Wireframe )
		GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else if( fillMode == FillMode::Solid )
		GetDevice()->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}

bool Graphics::SetRenderTarget( std::shared_ptr<RenderTarget> renderTarget )
{
	//Check if render target is valid to set
	if( (renderTarget) && renderTarget->Surface() )
	{
		//Put Render Target on top of stack
		PushRenderTarget();

		//Set Render Target
		HRESULT hr = GetDevice()->SetRenderTarget( 0, renderTarget->Surface() );
		if( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Unexpected Error on set Render Target [%08X]", hr );

			//Failed, so pop Render Target from stack
			PopRenderTarget();

			return false;
		}

		UpdateSurfacesDesc( true );
		return true;
	}

	return false;
}

void Graphics::UnsetRenderTarget()
{
	//Pop Render Target from stack
	PopRenderTarget();
	UpdateSurfacesDesc( true );
}

bool Graphics::SetDepthStencilBuffer( std::shared_ptr<DepthStencilBuffer> depthStencilBuffer )
{
	//Check if depth stencil buffer is valid to set
	if( (depthStencilBuffer) && depthStencilBuffer->Surface() )
	{
		//Put Depth Stencil Buffer on top of stack
		PushDepthStencilBuffer();

		//Set Depth Stencil Buffer
		HRESULT hr = GetDevice()->SetDepthStencilSurface( depthStencilBuffer->Surface() );
		if( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Unexpected Error on set Depth Stencil Buffer [%08X]", hr );

			//Failed, so pop Depth Stencil Buffer from stack
			PopDepthStencilBuffer();

			return false;
		}

		return true;
	}

	return false;
}

void Graphics::UnsetDepthStencilBuffer()
{
	//Pop Depth Stencil Buffer from stack
	PopDepthStencilBuffer();
}

void Graphics::Run()
{
	if( IsDeviceReady() )
	{
		renderer->Run();
	}
}

bool Graphics::IsDeviceReady()
{
	if ( bDeviceEx )
	{
		if ( bSkipRendering )
		{
			Sleep( 250 );

			bSkipRendering = false;
			return false;
		}

		return true;
	}

	HRESULT hr = GetDevice()->TestCooperativeLevel();
	switch( hr )
	{
		case D3DERR_DEVICELOST:
			OnLostDevice();

			//Yield CPU
			Sleep( 500 );
			DELTA3D_LOGINFO( "Device Lost!" );

			isDeviceLost = true;

			return false;
		case D3DERR_DEVICENOTRESET:
			OnLostDevice();

			isDeviceLost = true;

			if( FAILED( hr = GetDevice()->Reset( &presentParams ) ) )
			{
				//Yield CPU
				Sleep( 250 );
				DELTA3D_LOGERROR( "Device Reset Failed!" );

				return false;
			}

			isDeviceLost = false;

			OnResetDevice();
			break;
		default:
			if ( FAILED( hr ) )
				DELTA3D_LOGERROR( "Device Test Cooperative Level Error! [%08X]", hr );
			break;
	}

	return true;
}

bool Graphics::Begin()
{
	//Begin Scene
	HRESULT hr = GetDevice()->BeginScene();
	if( FAILED( hr ) )
	{
		DELTA3D_LOGERROR( "Begin Scene Failed!" );
		return false;
	}

	return true;
}

void Graphics::End()
{
	//End Scene
	GetDevice()->EndScene();

	//Present
	HRESULT hr = GetDeviceEx()->Present( NULL, NULL, NULL, NULL );
	if ( bDeviceEx )
	{
		//Handle Erros
		switch ( hr )
		{
			case S_PRESENT_OCCLUDED:
				GetDeviceEx()->Reset( &presentParams );
				break;
			
			case D3DERR_DEVICELOST:
				OnLostDevice();

				GetDeviceEx()->Reset( &presentParams );
				bSkipRendering = true;

				DELTA3D_LOGERROR( "Error on Present [D3DERR_DEVICELOST][%08X]", hr );

				OnResetDevice();
				break;

			case S_PRESENT_MODE_CHANGED:
				GetDeviceEx()->Reset( &presentParams );
				break;
			
			case D3DERR_DEVICEHUNG:
				bSkipRendering = true;
				break;
			
			default:
				bSkipRendering = false;
				break;
		}
	}
}

void Graphics::Clear( ClearFlags flags, const Math::Color& color, bool stencil )
{
	GetDevice()->Clear( 0, nullptr, (DWORD)flags, color.ToUInt(), 1.0f, stencil );
}

void Graphics::StateBlockBegin( StateBlock stateBlock_ )
{
	if( stateBlock != stateBlock_ )
	{
		StateBlockEnd();

		stateBlock = stateBlock_;

		switch( stateBlock )
		{
			case StateBlock::Blend_None:
				GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Blend_Color:
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Blend_AddColor:
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCCOLOR );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Blend_Shadow:
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Blend_Lamp:
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Blend_InvShadow:
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::SelectGlow:
				GetDevice()->SetRenderState( D3DRS_STENCILENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT );
				GetDevice()->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT );
				GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT );
				GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
				GetDevice()->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
				GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, 0 );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
				break;
			case StateBlock::SelectGlowMask:
				GetDevice()->SetRenderState( D3DRS_STENCILENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESS );
				GetDevice()->SetRenderState( D3DRS_STENCILREF, 0 );
				GetDevice()->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
				GetDevice()->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
				break;
			case StateBlock::SelectGlowSprite:
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				GetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
				GetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
				GetDevice()->SetRenderState( D3DRS_STENCILENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
				GetDevice()->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL );
				GetDevice()->SetRenderState( D3DRS_STENCILREF, 0 );
				GetDevice()->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF );
				GetDevice()->SetRenderState( D3DRS_STENCILWRITEMASK, 0xFFFFFFFF );
				break;
			case StateBlock::ObjectSky:
				GetDevice()->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				break;
			case StateBlock::Particle3D:
				GetDevice()->SetRenderState( D3DRS_DITHERENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				break;
			case StateBlock::Default:
				GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
				GetDevice()->SetRenderState( D3DRS_CLIPPING, FALSE );
				GetDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0, 0, 0, 0 ) );
				GetDevice()->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				GetDevice()->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
				GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
				GetDevice()->SetRenderState( D3DRS_LASTPIXEL, FALSE );
				GetDevice()->SetRenderState( D3DRS_DITHERENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
				GetDevice()->SetRenderState( D3DRS_ALPHAREF, 0 );
				GetDevice()->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
				GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				GetDevice()->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

				for( int i = 0; i < 8; i++ )
				{
					GetDevice()->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
					GetDevice()->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
					GetDevice()->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
					GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTEXF_LINEAR );
					GetDevice()->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTEXF_POINT );
					GetDevice()->SetTexture( i, NULL );
				}
				break;
		}
	}
}

void Graphics::StateBlockEnd( StateBlock stateBlock_ )
{
	if( (stateBlock == StateBlock::SelectGlow || stateBlock == StateBlock::Particle3D) && stateBlock_ == StateBlock::None )
		return;

	if( stateBlock_ != StateBlock::None )
		stateBlock = stateBlock_;

	switch( stateBlock )
	{
		case StateBlock::Blend_None:
		case StateBlock::Blend_Alpha:
		case StateBlock::Blend_Color:
		case StateBlock::Blend_AddColor:
		case StateBlock::Blend_Shadow:
		case StateBlock::Blend_Lamp:
		case StateBlock::Blend_InvShadow:
			GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			break;
		case StateBlock::SelectGlow:
			GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			GetDevice()->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
			GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			break;
		case StateBlock::SelectGlowMask:
			GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			break;
		case StateBlock::SelectGlowSprite:
			GetDevice()->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			break;
		case StateBlock::Particle3D:
		case StateBlock::ObjectSky:
			GetDevice()->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
			break;
		default:
			GetDevice()->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
			break;
	}

	StateBlockReset();
}

void Graphics::StateBlockReset()
{
	stateBlock = StateBlock::None;
}

HRESULT Graphics::DrawPrimitive( D3DPRIMITIVETYPE primitiveType, DWORD vertexTypeDesc, DWORD primitiveCount, LPVOID vertices, DWORD vertexCount, DWORD flags )
{
	HRESULT hr = S_OK;
	DWORD oldFVF = 0;

	auto GetVertexSize = [&]()
	{
		//Transformed Vertices (RHW)
		if( vertexTypeDesc == 324 )
			return 28;
		else if( vertexTypeDesc == 452 )
			return 32;
		else if( vertexTypeDesc == 708 )
			return 40;
		else if( vertexTypeDesc == 964 )
			return 48;
		else if( vertexTypeDesc == 1220 )
			return 56;
		//Not transformed vertices
		else if( vertexTypeDesc == 322 )
			return 24;
		else if( vertexTypeDesc == 450 )
			return 28;
		else if( vertexTypeDesc == 706 )
			return 36;
		else if( vertexTypeDesc == 962 )
			return 44;
		else if( vertexTypeDesc == 1218 )
			return 52;

		return 0;
	};

	if( FAILED( hr = GetDevice()->GetFVF( &oldFVF ) ) )
		return hr;

	if( FAILED( hr = GetDevice()->SetFVF( vertexTypeDesc ) ) )
		return hr;

	if( FAILED( hr = GetDevice()->DrawPrimitiveUP( primitiveType, primitiveCount, vertices, GetVertexSize() ) ) )
		return hr;

	if( FAILED( hr = GetDevice()->SetFVF( oldFVF ) ) )
		return hr;

	return hr;
}

bool Graphics::SetMode( int width_, int height_, bool windowed_, int colorDepth_, bool antiAliasing_, bool useSoftwareSkinning_, HWND hWnd_, RendererVersion eVersion )
{
	bool resetGraphics = false;
	bool firstTime = false;

	//First Time
	if( !GetInterface() && !GetDevice() )
	{
		firstTime = true;

		bDeviceEx = (eVersion == RendererVersion::RENDERERVERSION_DirectX9Ex) ? (FAILED( Is9EXCompatible() ) ? false : true) : false;

		hWnd = hWnd_;
		useSoftwareSkinning = useSoftwareSkinning_;

		char szWindowTitle[128] = { 0 };
		GetWindowText( hWnd, szWindowTitle, 128 );

		char szWindowClass[128] = { 0 };
		GetClassName( hWnd, szWindowClass, 128 );

		//Hashing
		if( szWindowTitle[0] && szWindowClass[0] )
		{
			std::size_t seed = szWindowTitle[0] + szWindowClass[0];
			hash_combine( seed, std::string(szWindowTitle) );
			hash_combine( seed, std::string(szWindowClass) );

			//Set Final Hash
			appHash = seed;
			//DELTA3D_LOGERROR( "Game Hash -> %d", seed );
		}
	}

	//Check if would be necessary reset graphics
	if( GetInterface() && GetDevice() && (size.width != width_ || size.height != height_ || windowed != windowed_ || colorDepth != colorDepth_) )
	{
		memcpy( &previousPresentParams, &presentParams, sizeof( D3DPRESENT_PARAMETERS ) );
		resetGraphics = true;
	}

	//Window Settings
	size.width = width_;
	size.height = height_;
	windowed = windowed_;
	colorDepth = colorDepth_;
	antiAliasing = antiAliasing_;

	//Create D3D Interface
	if( !CreateInterface() )
	{
		DELTA3D_LOGERROR( "Could not create Direct3D Interface" );
		return false;
	}

	//Check if all features needed is supported
	if( !CheckSupport() )
	{
		DELTA3D_LOGERROR( "Your graphics hardware doest not support all needed features" );
		return false;
	}

	//Multi Sample
	presentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
	presentParams.MultiSampleQuality = 0;

	presentParams.BackBufferWidth = size.width;
	presentParams.BackBufferHeight = size.height;

	if ( bDeviceEx )
	{
		EXDevice.displayMode.Width = size.width;
		EXDevice.displayMode.Height = size.height;
	}

	presentParams.BackBufferFormat = colorDepth == 16 ? D3DFMT_X4R4G4B4 : D3DFMT_X8R8G8B8;
	presentParams.AutoDepthStencilFormat = supportStencil32 ? D3DFMT_D24S8 : D3DFMT_D15S1;

	//Anti Aliasing
	if( antiAliasing )
	{
		D3DMULTISAMPLE_TYPE d3daMultipleSampleType[] = {
			D3DMULTISAMPLE_16_SAMPLES,
			D3DMULTISAMPLE_15_SAMPLES,
			D3DMULTISAMPLE_14_SAMPLES,
			D3DMULTISAMPLE_13_SAMPLES,
			D3DMULTISAMPLE_12_SAMPLES,
			D3DMULTISAMPLE_11_SAMPLES,
			D3DMULTISAMPLE_10_SAMPLES,
			D3DMULTISAMPLE_9_SAMPLES,
			D3DMULTISAMPLE_8_SAMPLES,
			D3DMULTISAMPLE_7_SAMPLES,
			D3DMULTISAMPLE_6_SAMPLES,
			D3DMULTISAMPLE_5_SAMPLES,
			D3DMULTISAMPLE_4_SAMPLES,
			D3DMULTISAMPLE_3_SAMPLES,
			D3DMULTISAMPLE_2_SAMPLES,
		};

		for( int i = 0; i < _countof( d3daMultipleSampleType ); i++ )
		{
			D3DMULTISAMPLE_TYPE t = d3daMultipleSampleType[i];
			DWORD q;

			if ( bDeviceEx )
			{
				if ( SUCCEEDED( GetInterfaceEx()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, presentParams.BackBufferFormat, windowed,
																			t, &q ) ) && SUCCEEDED( GetInterfaceEx()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, presentParams.AutoDepthStencilFormat,
																																				windowed, t, &q ) ) )
				{
			//		presentParams.MultiSampleType = t;
			//		presentParams.MultiSampleQuality = q - 1;
					break;
				}

			}
			else
			{
				if ( SUCCEEDED( GetInterface()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, presentParams.BackBufferFormat, windowed,
																			t, &q ) ) && SUCCEEDED( GetInterface()->CheckDeviceMultiSampleType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, presentParams.AutoDepthStencilFormat,
																																				windowed, t, &q ) ) )
				{
					presentParams.MultiSampleType = t;
					presentParams.MultiSampleQuality = q - 1;
					break;
				}
			}
		}
	}

	if ( !resetGraphics )
	{
		//Presentation Parameters Setup
		presentParams.PresentationInterval = vsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;
		presentParams.SwapEffect = bDeviceEx ? D3DSWAPEFFECT_FLIPEX : D3DSWAPEFFECT_DISCARD;
		presentParams.hDeviceWindow = hWnd;
		presentParams.BackBufferCount = 1;
		presentParams.EnableAutoDepthStencil = TRUE;
		presentParams.Flags = 0;

		//Force Software Skinning if HW Skinning isn't supported
		if ( !supportHardwareSkinning )
			useSoftwareSkinning = true;
	}

	presentParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if ( bDeviceEx )
		EXDevice.displayMode.RefreshRate = D3DPRESENT_RATE_DEFAULT;
	else
		CommonDevice.displayMode.RefreshRate = D3DPRESENT_RATE_DEFAULT;

	//Window Mode?
	presentParams.Windowed = windowed ? TRUE : FALSE;

	//Create Device
	if( !CreateDevice() )
	{
		DELTA3D_LOGERROR( "Could not create Direct3D Device" );
		return false;
	}

	if( resetGraphics )
	{
		if ( bDeviceEx )
		{
			HRESULT hr = GetDeviceEx()->Reset( &presentParams );
			if ( FAILED( hr ) )
			{
				//Failed, so we should try with old presentation parameters and restore graphics
				memcpy( &presentParams, &previousPresentParams, sizeof( D3DPRESENT_PARAMETERS ) );
				Sleep( 100 );

				OnLostDevice();

				//Try Reset device again
				HRESULT hr = GetDeviceEx()->Reset( &presentParams );
				if ( FAILED( hr ) )
				{
					//Error
					return false;
				}
			}

			OnResetDevice();
		}
		else
		{
			OnLostDevice();

			HRESULT hr = GetDevice()->Reset( &presentParams );
			if ( FAILED( hr ) )
			{
				//Failed, so we should try with old presentation parameters and restore graphics
				memcpy( &presentParams, &previousPresentParams, sizeof( D3DPRESENT_PARAMETERS ) );
				Sleep( 100 );

				OnLostDevice();

				//Try Reset device again
				HRESULT hr = GetDevice()->Reset( &presentParams );
				if ( FAILED( hr ) )
				{
					//Error
					return false;
				}
			}

			OnResetDevice();
		}

		DELTA3D_LOGINFO( "Graphics has been reseted!" );
	}

	//Set Default Render States
	StateBlockBegin();

	//Get Surfaces Description
	UpdateSurfacesDesc();

	//Mode Settings
	DELTA3D_LOGINFO( "Multi Sample: %d %d", presentParams.MultiSampleType, presentParams.MultiSampleQuality );
	DELTA3D_LOGINFO( "Resolution: %dx%d", width_, height_ );
	DELTA3D_LOGINFO( "%s", windowed ? "Window Mode" : "Fullscreen Mode" );
	DELTA3D_LOGINFO( "Color Depth: %dBPP", colorDepth );
	DELTA3D_LOGINFO( "%d bit Back Buffer", presentParams.BackBufferFormat == D3DFMT_X4R4G4B4 ? 16 : 32 );

	if( firstTime )
	{
		//Create Default Objects
		Texture::Default = textureFactory->CreateSolidColor();
		Sprite::Default = spriteFactory->Create( false );
		Font::Default = fontFactory->Create( Sprite::Default, "Arial", 16 );

		//Create Default Vertex Declarations
		for( int i = 0; i < 2; i++ )
		{
			for( int j = 0; j < _countof( VertexDeclaration::Tex ); j++ )
			{
				auto pElements = std::make_shared<VertexElements>();
				pElements->AddElement( 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 );
				pElements->AddElement( 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 );
                pElements->AddElement( 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 );
                pElements->AddElement( 3, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 );
                pElements->AddElement( 4, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 );

				//Skinned Texture (Bone Index)
				if ( i == 1 )
				{
                    pElements->AddElement( 5, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 );
                    pElements->AddElement( 6, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 );
				}

				//Textures Coordinate Buffer
				for( int k = 0; k < j; k++ )
					pElements->AddElement( i == 1 ? 7 + k : 5 + k, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, k );

				//Not Skinned Texture
				if( i == 0 )
					VertexDeclaration::Tex[j] = CreateVertexDeclaration( pElements );
				else
					VertexDeclaration::SkinnedTex[j] = CreateVertexDeclaration( pElements );
			}
		}

		//Texture Bones
		if ( useSoftwareSkinning == false )
		{
			for ( size_t i = 0; i < _countof( Renderer::lpaBonesTextures ); i++ )
				Renderer::lpaBonesTextures[i] = GetTextureFactory()->CreateDynamicTexture( 512, 1 );
		}

		PrintDeviceCaps();
	}

	return true;
}

void Graphics::OnLostDevice()
{
	//Lost Factories
	renderTargetFactory->OnLostDevice();
	depthStencilBufferFactory->OnLostDevice();
	spriteFactory->OnLostDevice();
	fontFactory->OnLostDevice();
	shaderFactory->OnLostDevice();
	textureFactory->OnLostDevice();

	for( auto& vertexBuffer : dynamicVertexBuffers )
		vertexBuffer->Release();

	//Fire Event
	FireEvent( GraphicEvents::LostDevice );
}

void Graphics::OnResetDevice()
{
	//Get Surfaces Description Updated
	UpdateSurfacesDesc();

	//Reset Factories
	renderTargetFactory->OnResetDevice();
	depthStencilBufferFactory->OnResetDevice();
	spriteFactory->OnResetDevice();
	fontFactory->OnResetDevice();
	shaderFactory->OnResetDevice();
	textureFactory->OnResetDevice();

	for( auto& vertexBuffer : dynamicVertexBuffers )
	{
		//Create Vertex Buffer
		IDirect3DVertexBuffer9* vertexBufferd3d;

		if( FAILED( GetDevice()->CreateVertexBuffer( vertexBuffer->ElementSize()* vertexBuffer->ElementCount(), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vertexBufferd3d, NULL ) ) )
		{
			DELTA3D_LOGERROR( "Could not recreate Dynamic Vertex Buffer of size %d*%d", vertexBuffer->ElementSize(), vertexBuffer->ElementCount() );
			continue;
		}

		//Renew Texture (adds Reference)
		vertexBuffer->Renew( vertexBufferd3d );

		//Release our Reference
		if( vertexBufferd3d )
			vertexBufferd3d->Release();
	}

	//Reset Render State Blocks
	StateBlockReset();
	StateBlockBegin();

	//Fire Event
	FireEvent( GraphicEvents::ResetDevice );
}

void Graphics::PushRenderTarget()
{
	LPDIRECT3DSURFACE9 surface;
	GetDevice()->GetRenderTarget( 0, &surface );

	renderTargets.push( surface );
}

void Graphics::PopRenderTarget()
{
	if( renderTargets.empty() )
		return;

	LPDIRECT3DSURFACE9 surface = renderTargets.top();

	GetDevice()->SetRenderTarget( 0, surface );
	surface->Release();

	renderTargets.pop();
}

void Graphics::PushDepthStencilBuffer()
{
	LPDIRECT3DSURFACE9 surface;
	GetDevice()->GetDepthStencilSurface( &surface );

	depthStencilBuffers.push( surface );
}

void Graphics::PopDepthStencilBuffer()
{
	if( depthStencilBuffers.empty() )
		return;

	LPDIRECT3DSURFACE9 surface = depthStencilBuffers.top();

	GetDevice()->SetDepthStencilSurface( surface );
	surface->Release();

	depthStencilBuffers.pop();
}

void Graphics::UpdateSurfacesDesc( bool onlyRenderTarget )
{
	//Lambda Expression to get surface description
	auto GetSurfaceDesc = [&]( auto& surfaceInfo, bool depthStencil = false )
	{
		memset( &surfaceInfo, 0, sizeof( SurfaceInfo ) );

		LPDIRECT3DSURFACE9 surface;
		if( SUCCEEDED( depthStencil ? GetDevice()->GetDepthStencilSurface( &surface ) : GetDevice()->GetRenderTarget( 0, &surface ) ) )
		{
			D3DSURFACE_DESC surfaceDesc;
			if( SUCCEEDED( surface->GetDesc( &surfaceDesc ) ) )
			{
				surfaceInfo.width = surfaceDesc.Width;
				surfaceInfo.height = surfaceDesc.Height;
				surfaceInfo.format = surfaceDesc.Format;
			}

			surface->Release();
		}
	};
	
	//Check if will get only render target surface description
	if( !onlyRenderTarget )
	{
		GetSurfaceDesc( backBufferInfo );
		GetSurfaceDesc( depthStencilBufferInfo, true );
	}

	GetSurfaceDesc( renderTargetInfo );
}

void Graphics::PrintDeviceCaps()
{
	DELTA3D_LOGINFO( "@ Device Capabilities" );
	DELTA3D_LOGINFO( "Vertex Shader Version: %d.%d", D3DSHADER_VERSION_MAJOR( deviceCaps.VertexShaderVersion ), D3DSHADER_VERSION_MINOR( deviceCaps.VertexShaderVersion ) );
	DELTA3D_LOGINFO( "Pixel Shader Version: %d.%d", D3DSHADER_VERSION_MAJOR( deviceCaps.PixelShaderVersion ), D3DSHADER_VERSION_MINOR( deviceCaps.PixelShaderVersion ) );
	DELTA3D_LOGINFO( "Max Vertex Blend Matrices: %d", deviceCaps.MaxVertexBlendMatrices );
	DELTA3D_LOGINFO( "Max Vertex Blend Matrix Index: %d", deviceCaps.MaxVertexBlendMatrixIndex );
	DELTA3D_LOGINFO( "Max Primitive Count: %d", deviceCaps.MaxPrimitiveCount );
	DELTA3D_LOGINFO( "Max Vertex Index: %d", deviceCaps.MaxVertexIndex );
	DELTA3D_LOGINFO( "Max Streams: %d", deviceCaps.MaxStreams );
	DELTA3D_LOGINFO( "Max Streams Stride: %d", deviceCaps.MaxStreamStride );
	DELTA3D_LOGINFO( "Max Vertex Shader Constant Registers: %d", deviceCaps.MaxVertexShaderConst );
	DELTA3D_LOGINFO( "Max VShader Instructions Executed: %d", deviceCaps.MaxVShaderInstructionsExecuted );
	DELTA3D_LOGINFO( "Max PShader Instructions Executed: %d", deviceCaps.MaxPShaderInstructionsExecuted );
	DELTA3D_LOGINFO( "Max Vertex Shader 30 Instruction Slots: %d", deviceCaps.MaxVertexShader30InstructionSlots );
	DELTA3D_LOGINFO( "Max Pixel Shader 30 Instruction Slots: %d", deviceCaps.MaxPixelShader30InstructionSlots );
	DELTA3D_LOGINFO( "Max Simultaneous Textures: %d", deviceCaps.MaxSimultaneousTextures );
	DELTA3D_LOGINFO( "Max Texture Blend Stages: %d", deviceCaps.MaxTextureBlendStages );
	DELTA3D_LOGINFO( "Max Texture Width: %d", deviceCaps.MaxTextureWidth );
	DELTA3D_LOGINFO( "Max Texture Height: %d", deviceCaps.MaxTextureHeight );
	DELTA3D_LOGINFO( "Max Volume Extent: %d", deviceCaps.MaxVolumeExtent );
	DELTA3D_LOGINFO( "Max Texture Repeat: %d", deviceCaps.MaxTextureRepeat );
	DELTA3D_LOGINFO( "Max Texture Aspect Ratio: %d", deviceCaps.MaxTextureAspectRatio );
	DELTA3D_LOGINFO( "Max Anisotropy: %d", deviceCaps.MaxAnisotropy );
	DELTA3D_LOGINFO( "Max Active Lights: %d", deviceCaps.MaxActiveLights );
	DELTA3D_LOGINFO( "Max User Clip Planes: %d", deviceCaps.MaxUserClipPlanes );
	DELTA3D_LOGINFO( "Max Point Size: %.6f", deviceCaps.MaxPointSize );
	DELTA3D_LOGINFO( "Max Npatch Tesselation Level: %.6f", deviceCaps.MaxNpatchTessellationLevel );
	DELTA3D_LOGINFO( "Num Simultaneous RTs: %d", deviceCaps.NumSimultaneousRTs );
	DELTA3D_LOGINFO( "Using Textures Non Pow2 Conditional: %s", (deviceCaps.TextureCaps& D3DPTEXTURECAPS_NONPOW2CONDITIONAL) ? "Yes" : "No" );
	DELTA3D_LOGINFO( "Using Textures Pow2: %s", (deviceCaps.TextureCaps& D3DPTEXTURECAPS_POW2) ? "Yes" : "No" );
	DELTA3D_LOGINFO( "Supports HW Skinning: %s", supportHardwareSkinning ? "Yes" : "No" );
	DELTA3D_LOGINFO( "%s Skinning", useSoftwareSkinning ? "Software" : "Hardware" );
}

HRESULT Graphics::Is9EXCompatible()
{
	HRESULT hr = E_FAIL;
	HMODULE libHandle = NULL;

	if ( !IsWindows7OrGreater() )
		return hr;

	//Manually load the d3d9.dll library.
	libHandle = LoadLibraryA( "d3d9.dll" );

	if ( libHandle != NULL )
	{
		//Define a function pointer to the Direct3DCreate9Ex function.
		typedef HRESULT( WINAPI * LPDIRECT3DCREATE9EX )(UINT, void **);

		//Obtain the address of the Direct3DCreate9Ex function. 
		LPDIRECT3DCREATE9EX Direct3DCreate9ExPtr = NULL;

		Direct3DCreate9ExPtr = (LPDIRECT3DCREATE9EX)GetProcAddress( libHandle, "Direct3DCreate9Ex" );

		if ( Direct3DCreate9ExPtr != NULL )
		{
			//Direct3DCreate9Ex is supported.
			hr = S_OK;
		}
		else
		{
			//Direct3DCreate9Ex is not supported on this
			//operating system.
			hr = ERROR_NOT_SUPPORTED;
		}

		//Free the library
		FreeLibrary( libHandle );
	}

	return hr;
}

bool Graphics::CreateInterface()
{
	//Already exists
	if( GetInterface() )
		return true;

	//Create Direct3D Interface
	if ( bDeviceEx )
		Direct3DCreate9Ex( D3D_SDK_VERSION, &EXDevice.interfaced3d );
	else
		CommonDevice.interfaced3d = Direct3DCreate9( D3D_SDK_VERSION );
	if( !GetInterface() )
	{
		DELTA3D_LOGERROR( "Error on Direct3D Interface creation" );
		return false;
	}

	//Get Device Capabilities
	HRESULT hr = GetInterface()->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps );
	if( FAILED( hr ) )
	{
		DELTA3D_LOGERROR( "Error on Device Capabilities Getter [%08X]", hr );
		return false;
	}

	//Get Dispaly Mode
	if ( bDeviceEx )
	{
		EXDevice.displayMode.Size = sizeof( D3DDISPLAYMODEEX );
		hr = GetInterfaceEx()->GetAdapterDisplayModeEx( D3DADAPTER_DEFAULT, &EXDevice.displayMode, NULL );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Error on Adapter Display Mode Getter [%08X]", hr );
			return FALSE;
		}
	}
	else
	{
		hr = GetInterface()->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &CommonDevice.displayMode );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Error on Adapter Display Mode Getter [%08X]", hr );
			return FALSE;
		}
	}


	DELTA3D_LOGINFO( "Direct3D 9 Interface Created" );

	return true;
}

bool Graphics::CreateDevice()
{
	//Already exists
	if( GetDevice() )
		return true;

	DWORD dwBehaviourFlags = D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED;

	DELTA3D_LOGINFO( "FPU Preserve" );

	//Check create device Flags
	if( deviceCaps.DevCaps& D3DDEVCAPS_HWTRANSFORMANDLIGHT )
	{
		dwBehaviourFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;

		DELTA3D_LOGINFO( "Vertex Processing: Hardware" );

		if( deviceCaps.DevCaps& D3DDEVCAPS_PUREDEVICE )
		{
			dwBehaviourFlags |= D3DCREATE_PUREDEVICE;
			DELTA3D_LOGINFO( "Pure Device" );
		}
	}
	else
	{
		dwBehaviourFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		DELTA3D_LOGINFO( "Vertex Processing: Software" );
	}

	//Create Device
	if ( bDeviceEx )
	{
		HRESULT hr = GetInterfaceEx()->CreateDeviceEx( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwBehaviourFlags, &presentParams, !windowed ? &EXDevice.displayMode : nullptr, &EXDevice.device );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Error %s on Direct3D Device Creation: (%s)[%08X]", DXGetErrorString( hr ), DXGetErrorDescription( hr ), hr );
			return false;
		}
	}
	else
	{
		HRESULT hr = GetInterface()->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwBehaviourFlags, &presentParams, &CommonDevice.device );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Error %s on Direct3D Device Creation: (%s)[%08X]", DXGetErrorString( hr ), DXGetErrorDescription( hr ), hr );
			return false;
		}
	}

	DELTA3D_LOGINFO( "Device Created!" );

	return true;
}

bool Graphics::CheckSupport()
{
	HRESULT hr;

	//Check if Color Depth is supported
	if ( bDeviceEx )
	{
		hr = GetInterfaceEx()->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EXDevice.displayMode.Format, colorDepth == 16 ? D3DFMT_X4R4G4B4 : D3DFMT_X8R8G8B8, windowed );
		if ( FAILED( hr ) )
		{
			colorDepth = colorDepth == 16 ? 32 : 16;

			if ( FAILED( hr = GetInterfaceEx()->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EXDevice.displayMode.Format, colorDepth == 16 ? D3DFMT_X4R4G4B4 : D3DFMT_X8R8G8B8, windowed ) ) )
			{
				DELTA3D_LOGERROR( "Your graphics hardware doest not support a 16 or 32 bit Back Buffer [%08X]", hr );
				return false;
			}
		}

		//Check if Depth Stencil Format is Valid
		hr = GetInterfaceEx()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EXDevice.displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Your graphics hardware doest not support 32 bit DepthStencil Buffer [%08X]", hr );

			//Try with another Depth Stencil Format
			hr = GetInterfaceEx()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EXDevice.displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 );
			supportStencil32 = false;

			if ( FAILED( hr ) )
			{
				DELTA3D_LOGERROR( "Your graphics hardware doest not support a 16 or 32 bit DepthStencil Buffer [%08X]", hr );
				return false;
			}
		}
		else
			supportStencil32 = true;

		//Check if Supports Texture Fetch for Bones
		hr = GetInterfaceEx()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, EXDevice.displayMode.Format, D3DUSAGE_DYNAMIC, D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Your graphics hardware doest not support 32 bit Dynamic Texture [%08X]", hr );
			supportHardwareSkinning = false;
		}
		else
			supportHardwareSkinning = true;
	}
	else
	{
		hr = GetInterface()->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CommonDevice.displayMode.Format, colorDepth == 16 ? D3DFMT_X4R4G4B4 : D3DFMT_X8R8G8B8, windowed );
		if ( FAILED( hr ) )
		{
			colorDepth = colorDepth == 16 ? 32 : 16;

			if ( FAILED( hr = GetInterface()->CheckDeviceType( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CommonDevice.displayMode.Format, colorDepth == 16 ? D3DFMT_X4R4G4B4 : D3DFMT_X8R8G8B8, windowed ) ) )
			{
				DELTA3D_LOGERROR( "Your graphics hardware doest not support a 16 or 32 bit Back Buffer [%08X]", hr );
				return false;
			}
		}

		//Check if Depth Stencil Format is Valid
		hr = GetInterface()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CommonDevice.displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Your graphics hardware doest not support 32 bit DepthStencil Buffer [%08X]", hr );

			//Try with another Depth Stencil Format
			hr = GetInterface()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CommonDevice.displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D15S1 );
			supportStencil32 = false;

			if ( FAILED( hr ) )
			{
				DELTA3D_LOGERROR( "Your graphics hardware doest not support a 16 or 32 bit DepthStencil Buffer [%08X]", hr );
				return false;
			}
		}
		else
			supportStencil32 = true;

		//Check if Supports Texture Fetch for Bones
		hr = GetInterface()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, CommonDevice.displayMode.Format, D3DUSAGE_DYNAMIC, D3DRTYPE_TEXTURE, D3DFMT_A32B32G32R32F );
		if ( FAILED( hr ) )
		{
			DELTA3D_LOGERROR( "Your graphics hardware doest not support 32 bit Dynamic Texture [%08X]", hr );
			supportHardwareSkinning = false;
		}
		else
			supportHardwareSkinning = true;
	}

	//Check Vertex Shader Version
	if ( D3DSHADER_VERSION_MAJOR( deviceCaps.VertexShaderVersion ) == 0 )
	{
		DELTA3D_LOGERROR( "Your graphics hardware doest not support Hardware Skinning [%08X]", hr );
		supportHardwareSkinning = false;
	}

	//Check Vertex and Pixel Shader Version
    if ( (D3DSHADER_VERSION_MAJOR( deviceCaps.PixelShaderVersion ) < 3) || (D3DSHADER_VERSION_MAJOR( deviceCaps.VertexShaderVersion ) < 3) )
    {
        DELTA3D_LOGERROR( "Your graphic card does not support Shader Model 3.0 [%08X]", hr );

		MessageBoxA( 0, "Your graphic card does not support Shader Model 3.0\n\nPlease upgrade your graphic card!", "RegnumPT - ERROR", MB_ICONERROR | MB_OK );

		return false;
    }

	pixelShaderVersionMajor = D3DSHADER_VERSION_MAJOR( deviceCaps.PixelShaderVersion );
	vertexShaderVersionMajor = D3DSHADER_VERSION_MAJOR( deviceCaps.VertexShaderVersion );

	return true;
}

}