#include "stdafx.h"
#include "CGameScreen.h"

CGameScreen::CGameScreen()
{
	pcActiveScreen = NULL;
	bActiveScreen = FALSE;
	iActiveScreen = SCREEN_None;
	iNewScreen = SCREEN_None;

	fScreenChangeTimeUp = 0.0f;
	fScreenChangeTimeDown = 0.0f;
}

CGameScreen::~CGameScreen()
{
	if ( bActiveScreen )
		DELET( pcActiveScreen );
}

void CGameScreen::Init()
{
	pMiscTexture = GetGraphicsDevice()->GetTextureFactory()->MakeTexture( "game\\textures\\misc\\black.png", FALSE );
}

void CGameScreen::Shutdown()
{
	if ( bActiveScreen )
		SHUTDOWN( pcActiveScreen );
}

void CGameScreen::Frame()
{
	if ( bActiveScreen )
	{
		if ( iNewScreen == iActiveScreen )
		{
			EScreen iNewActiveScreen = pcActiveScreen->Frame();

			if ( iNewActiveScreen != iActiveScreen )
				ChangeScreen( iNewActiveScreen );
		}
	}


	if ( iNewScreen != iActiveScreen )
	{
		fScreenChangeTimeUp += 1000.0f / 60.0f;
		if ( fScreenChangeTimeUp >= SCREEN_TIME_TO_CHANGE )
		{
			SetScreen( iNewScreen );

			fScreenChangeTimeUp = SCREEN_TIME_TO_CHANGE;
		}
	}

	fScreenChangeTimeDown += 1000.0f / 60.0f;
	if ( fScreenChangeTimeDown >= SCREEN_TIME_TO_CHANGE )
	{
		fScreenChangeTimeDown = SCREEN_TIME_TO_CHANGE;
	}
}

void CGameScreen::Update( float fTime )
{
	CAMERA->Update( (double)fTime );

	if ( bActiveScreen )
		pcActiveScreen->Update( fTime );
}

void CGameScreen::Render3D()
{
	if ( bActiveScreen )
		pcActiveScreen->Render3D();
}

void CGameScreen::Render2D()
{
	if ( bActiveScreen )
		pcActiveScreen->Render2D();

	if ( fScreenChangeTimeUp < SCREEN_TIME_TO_CHANGE )
	{
		UI::ImageRender::Render( pMiscTexture->Get(), 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0, DX::Color( (int)(fScreenChangeTimeUp * 255.0f / SCREEN_TIME_TO_CHANGE), 255, 255, 255 ).Get(), FALSE );
	}
	
	if ( fScreenChangeTimeDown < SCREEN_TIME_TO_CHANGE )
	{
		UI::ImageRender::Render( pMiscTexture->Get(), 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0, DX::Color( 255 - (int)(fScreenChangeTimeDown * 255.0f / SCREEN_TIME_TO_CHANGE), 255, 255, 255 ).Get(), FALSE );
	}
}

void CGameScreen::OnMouseMove( CMouse * pcMouse )
{
	if ( bActiveScreen )
		pcActiveScreen->OnMouseMove( pcMouse );
}

BOOL CGameScreen::OnKeyPress( CKeyboard * pcKeyboard )
{
	if( bActiveScreen )
		return pcActiveScreen->OnKeyPress( pcKeyboard );

	return FALSE;
}

BOOL CGameScreen::OnMouseClick( CMouse * pcMouse )
{
	if ( bActiveScreen )
		return pcActiveScreen->OnMouseClick( pcMouse );

	return FALSE;
}

BOOL CGameScreen::OnMouseScroll( CMouse * pcMouse )
{
	if( bActiveScreen )
		return pcActiveScreen->OnMouseScroll( pcMouse );

	return FALSE;
}

BOOL CGameScreen::OnKeyChar( CKeyboard * pcKeyboard )
{
	if( bActiveScreen )
		return pcActiveScreen->OnKeyChar( pcKeyboard );

	return FALSE;
}

void CGameScreen::OnResolutionChanged()
{
	if( bActiveScreen )
		pcActiveScreen->OnResolutionChanged();
}

void CGameScreen::ChangeScreen( EScreen iNewScreen )
{
	this->iNewScreen = iNewScreen;

	fScreenChangeTimeUp = 0.0f;
}

void CGameScreen::SetScreen( EScreen iNewScreen )
{
	iActiveScreen = iNewScreen;

	bActiveScreen = FALSE;

	SHUTDOWN( pcActiveScreen );
	DELET( pcActiveScreen );

	switch ( iNewScreen )
	{
		case SCREEN_Login:
			pcActiveScreen = new CLoginScreen();
			break;
		case SCREEN_Character:
			pcActiveScreen = new CCharacterScreen();
			break;
		case SCREEN_World:
			pcActiveScreen = new CWorldScreen();
			break;
		case SCREEN_Test:
			pcActiveScreen = new CTestScreen();
			break;
		default:
			pcActiveScreen = NULL;
			break;
	}

	if ( pcActiveScreen )
	{
		bActiveScreen = pcActiveScreen->Init();
		if ( bActiveScreen )
			fScreenChangeTimeDown = 0.0f;
	}
}
