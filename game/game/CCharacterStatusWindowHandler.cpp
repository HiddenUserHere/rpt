#include "stdafx.h"
#include "CCharacterStatusWindowHandler.h"

CCharacterStatusWindowHandler::CCharacterStatusWindowHandler()
{
	pWindow = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 793, 275 ) );

	for ( UINT i = 0; i < _countof( saStatusRecordBuffer ); i++ )
	{
		saStatusRecordBuffer[i].iID				= BUTTON_Invalid;
		saStatusRecordBuffer[i].iStatusCount	= 0;
	}

	for( UINT i = 0; i < _countof( dwaStatusBaseColor ); i++ )
	{
		dwaStatusBaseColor[i] = DX::Color( 255, 255, 255, 255 ).Get();
	}

	pWindow->SetFocusWindow( FALSE );
}

CCharacterStatusWindowHandler::~CCharacterStatusWindowHandler()
{
	pWindow = nullptr;
}

void CCharacterStatusWindowHandler::Init()
{
	pFont = GetGraphicsDevice()->GetDefaultFont();

	pWindow->SetImage( UI::ImageLoader::LoadImage( "game\\images\\UI\\hud\\Status.png" ) );
	pWindow->SetBeforeRenderChildrenEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnBeforeRenderChildren, this, std::placeholders::_1 ) ) );

	BuildButtonsStatus();
}

void CCharacterStatusWindowHandler::Render()
{
	if ( (WINDOW_CHARACTERINFO || (WINDOW_CHARACTERINFOMOVE > 0)) && (WINDOW_INVENTORY == FALSE) && (WINDOW_SKILLS == FALSE) )
	{
		pWindow->Show();
		RenderStatusWindow( 0, RESOLUTION_HEIGHT - WINDOW_CHARACTERINFOMOVE - 19 );
	}
	else
	{
		pWindow->Hide();
	}
}

void CCharacterStatusWindowHandler::Update( float fTime )
{
	if ( (WINDOW_CHARACTERINFO || (WINDOW_CHARACTERINFOMOVE > 0)) && (WINDOW_INVENTORY == FALSE) && (WINDOW_SKILLS == FALSE) )
	{
		if ( UNITDATA->sCharacterData.iStatPoints > 0 )
		{
			if ( bIsButtonsEnabled == false )
			{
				EnableButtons();
			}
		}
		else
		{
			if ( bIsButtonsEnabled == true )
			{
				DisableButtons();
			}
		}

		if ( iCharacterStatusPointOld != UNITDATA->sCharacterData.iStatPoints )
		{
			iCharacterStatusPointOld = UNITDATA->sCharacterData.iStatPoints;
			
			UpdateRequiredStatus();
		}
	}
}

void CCharacterStatusWindowHandler::RenderText( int iBaseX, int iBaseY )
{
	int DEFAULT_POSITIONX = iBaseX;
	int DEFAULT_POSITIONY = iBaseY;

	DrawTextCharacter( GetCharacterClassData( UNITDATA->sCharacterData.iClass, UNITDATA->sCharacterData.iRank )->pszNameTranslated, Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 117, 87, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( UNITDATA->sCharacterData.szName, Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 137, 87, 18 ), DX::Color( 255, 255, 255, 255 ) );

	auto psTextureClanIcon = (EXESurfaceOld *)READDWORD( 0x03B18BC4 );
	if ( psTextureClanIcon && psTextureClanIcon->sTexture.pcD3DTexture && UNITDATA->sCharacterData.iClanID != 0 )
	{
		UI::ImageRender::Render( psTextureClanIcon->sTexture.pcD3DTexture, DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 157 - 2, 32, 32, 0, 0, -1, 0.5f );
	}

	DrawTextCharacter( (char *)0x03B17984, Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 157, 87, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iLevel ), Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 177, 53, 18 ), DX::Color( 255, 255, 255, 255 ) );

	auto GetFormattedEXP = []( INT64 iEXP ) -> std::string
	{
		std::string strEXP = "";
		if ( iEXP >= 1000000000 )
		{
			strEXP = FormatNumber( iEXP / 1000000 );
			strEXP += "M";
		}
		else
		{
			strEXP = FormatNumber( iEXP );
		}

		return strEXP;
	};

	auto GetFormattedBars = []( CurMax sValue ) -> std::string
	{
		return std::to_string( sValue.sCur ) + "/" + std::to_string( sValue.sMax );
	};

	//EXP
	INT64 iEXP = CHARACTERGAME->GetEXP();
	std::string strEXP = GetFormattedEXP( iEXP );
	std::string strNextEXP = GetFormattedEXP( UNITGAME->GetEXPFromLevel( UNITDATA->sCharacterData.iLevel + 1 ) );
	
	DrawTextCharacter( strEXP, Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 217, 85, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( strNextEXP, Rectangle2D( DEFAULT_POSITIONX + 75, DEFAULT_POSITIONY + 237, 85, 18 ), DX::Color( 255, 255, 255, 255 ) );

	//Elements
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.sElementalDef[ELEMENTID_Organic] ), Rectangle2D( DEFAULT_POSITIONX + 217, DEFAULT_POSITIONY + 132, 31, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.sElementalDef[ELEMENTID_Fire] ), Rectangle2D( DEFAULT_POSITIONX + 217, DEFAULT_POSITIONY + 169, 31, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.sElementalDef[ELEMENTID_Ice] ), Rectangle2D( DEFAULT_POSITIONX + 217, DEFAULT_POSITIONY + 206, 31, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.sElementalDef[ELEMENTID_Poison] ), Rectangle2D( DEFAULT_POSITIONX + 284, DEFAULT_POSITIONY + 132, 31, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.sElementalDef[ELEMENTID_Lighting] ), Rectangle2D( DEFAULT_POSITIONX + 284, DEFAULT_POSITIONY + 169, 31, 18 ), DX::Color( 255, 255, 255, 255 ) );

	//HP, MP and STM
	DrawTextCharacter( GetFormattedBars( CurMax( (short)CHARACTERGAME->GetCurrentHP(), UNITDATA->sCharacterData.sHP.sMax ) ), Rectangle2D( DEFAULT_POSITIONX + 385, DEFAULT_POSITIONY + 103, 60, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_HP ) ) );
	DrawTextCharacter( GetFormattedBars( UNITDATA->sCharacterData.sMP ), Rectangle2D( DEFAULT_POSITIONX + 385, DEFAULT_POSITIONY + 128, 60, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_MP ) ) );
	DrawTextCharacter( GetFormattedBars( UNITDATA->sCharacterData.sSP ), Rectangle2D( DEFAULT_POSITIONX + 385, DEFAULT_POSITIONY + 153, 60, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_SP ) ) );

	//Regens
	DrawTextCharacter( FormatString( "%.2f", UNITDATA->sCharacterData.fHPRegen ), Rectangle2D( DEFAULT_POSITIONX + 449, DEFAULT_POSITIONY + 103, 34, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( FormatString( "%.2f", UNITDATA->sCharacterData.fMPRegen ), Rectangle2D( DEFAULT_POSITIONX + 449, DEFAULT_POSITIONY + 128, 34, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( FormatString( "%.2f", UNITDATA->sCharacterData.fSPRegen ), Rectangle2D( DEFAULT_POSITIONX + 449, DEFAULT_POSITIONY + 153, 34, 18 ), DX::Color( 255, 255, 255, 255 ) );

	//Status
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iStrength ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 107, 48, 18 ), DX::Color( dwaStatusBaseColor[0] ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iSpirit ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 127, 48, 18 ), DX::Color( dwaStatusBaseColor[1] ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iTalent ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 147, 48, 18 ), DX::Color( dwaStatusBaseColor[2] ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iAgility ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 167, 48, 18 ), DX::Color( dwaStatusBaseColor[3] ) );
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iHealth ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 187, 48, 18 ), DX::Color( dwaStatusBaseColor[4] ) );

	if ( UNITDATA->sCharacterData.iStatPoints > 0 )
		DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iStatPoints ), Rectangle2D( DEFAULT_POSITIONX + 584, DEFAULT_POSITIONY + 207, 36, 18 ), DX::Color( 255, 255, 255, 255 ) );


	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iAttackRating ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 33, 67, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_AttackRating ) ) );

	const IMinMax * psDamageFake = ((IMinMax *)0x035EB600);
	std::string strAttackPower = std::to_string( psDamageFake->iMin + UNITDATA->sCharacterData.iMinDamage ) + "-" + std::to_string( psDamageFake->iMax + UNITDATA->sCharacterData.iMaxDamage );
	DrawTextCharacter( strAttackPower, Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 53, 67, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_Damage ) ) );
	
	const int * piCritical = (int *)0x04B0C5EC;
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iAttackSpeed ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 73, 67, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( *piCritical ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 93, 67, 18 ), DX::Color( 255, 255, 255, 255 ) );

	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iDefenseRating ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 129, 67, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_Defense ) ) );
	DrawTextCharacter( std::to_string( UNITGAME->GetCharacterAbsorption() ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 149, 67, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_Absorb ) ) );

	const int * piBlock = (int *)0x04B0C4B0;
	const int iEvade = *(int *)0x04B0C210;
	DrawTextCharacter( std::to_string( *piBlock ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 169, 67, 18 ), DX::Color( 255, 255, 255, 255 ) );
	DrawTextCharacter( std::to_string( iEvade ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 189, 67, 18 ), DX::Color( 255, 255, 255, 255 ) );
	
	DrawTextCharacter( std::to_string( UNITDATA->sCharacterData.iMovementSpeed ), Rectangle2D( DEFAULT_POSITIONX + 710, DEFAULT_POSITIONY + 209, 67, 18 ), DX::Color( GetStatusColor( CHARSTATUSTYPECOLOR_Speed ) ) );
}

void CCharacterStatusWindowHandler::RenderStatusWindow( int iBaseX, int iBaseY )
{
	//UI::ImageRender::Render( pImage, iBaseX, iBaseY, pImage->GetWidth(), pImage->GetHeight(), -1 );

	pWindow->SetPosition( iBaseX, iBaseY );
	pWindow->Render( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

BOOL CCharacterStatusWindowHandler::OnMouseClick( CMouse * pcMouse )
{
	if ( WINDOW_CHARACTERINFO || (WINDOW_CHARACTERINFOMOVE > 0) )
	{
		if ( pWindow->IsOpen() )
			return pWindow->OnMouseClick( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
	}
	return FALSE;
}

void CCharacterStatusWindowHandler::OnMouseMove( CMouse * pcMouse )
{
	if ( WINDOW_CHARACTERINFO || (WINDOW_CHARACTERINFOMOVE > 0) )
	{
		if ( pWindow->IsOpen() )
			pWindow->OnMouseMove( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
	}
}

void CCharacterStatusWindowHandler::BuildButtonsStatus()
{
	{
		UI::Button_ptr pButtonStrength = std::make_shared<UI::Button>( Rectangle2D( 565, 107, 18, 17 ) );
		pButtonStrength->SetID( BUTTON_Strength );
		pButtonStrength->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
		pButtonStrength->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
		pButtonStrength->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
		pButtonStrength->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_Strength, std::placeholders::_1 ) ) );
		pButtonStrength->SetAdditionalEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_RequiredStrength, std::placeholders::_1 ) ), EMouseEvent::ClickUpR );
		pButtonStrength->SetMouseEvent( EMouseEvent::ClickUpL );
		pButtonStrength->Disable();
		pWindow->AddElement( pButtonStrength );

		UI::Tooltip_ptr pTooltipRequiredStrength = std::make_shared<UI::Tooltip>( Rectangle2D( 0, 0, 0, 0 ) );
		pTooltipRequiredStrength->Init();
		pTooltipRequiredStrength->SetID( TOOLTIP_RequiredStrength );
		pTooltipRequiredStrength->SetFollowMouse( FALSE );
		pTooltipRequiredStrength->SetPosition( TOOLTIPPOSITION_Top );
		pTooltipRequiredStrength->SetFont( "Arial", 14, -1 );
		pTooltipRequiredStrength->SetText( "Required 0" );
		pTooltipRequiredStrength->SetLinkObject( pButtonStrength );
		pWindow->AddElement( pTooltipRequiredStrength );
	}

	{
		UI::Button_ptr pButtonSpirit = std::make_shared<UI::Button>( Rectangle2D( 565, 127, 18, 17 ) );
		pButtonSpirit->SetID( BUTTON_Spirit );
		pButtonSpirit->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
		pButtonSpirit->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
		pButtonSpirit->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
		pButtonSpirit->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_Spirit, std::placeholders::_1 ) ) );
		pButtonSpirit->SetAdditionalEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_RequiredSpirit, std::placeholders::_1 ) ), EMouseEvent::ClickUpR );
		pButtonSpirit->SetMouseEvent( EMouseEvent::ClickUpL );
		pButtonSpirit->Disable();
		pWindow->AddElement( pButtonSpirit );

		UI::Tooltip_ptr pTooltipRequiredSpirit = std::make_shared<UI::Tooltip>( Rectangle2D( 0, 0, 0, 0 ) );
		pTooltipRequiredSpirit->Init();
		pTooltipRequiredSpirit->SetID( TOOLTIP_RequiredSpirit );
		pTooltipRequiredSpirit->SetFollowMouse( FALSE );
		pTooltipRequiredSpirit->SetPosition( TOOLTIPPOSITION_Top );
		pTooltipRequiredSpirit->SetFont( "Arial", 14, -1 );
		pTooltipRequiredSpirit->SetText( "Required 0" );
		pTooltipRequiredSpirit->SetLinkObject( pButtonSpirit );
		pWindow->AddElement( pTooltipRequiredSpirit );
	}
	
	{
		UI::Button_ptr pButtonTalent = std::make_shared<UI::Button>( Rectangle2D( 565, 147, 18, 17 ) );
		pButtonTalent->SetID( BUTTON_Talent );
		pButtonTalent->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
		pButtonTalent->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
		pButtonTalent->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
		pButtonTalent->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_Talent, std::placeholders::_1 ) ) );
		pButtonTalent->SetAdditionalEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_RequiredTalent, std::placeholders::_1 ) ), EMouseEvent::ClickUpR );
		pButtonTalent->SetMouseEvent( EMouseEvent::ClickUpL );
		pButtonTalent->Disable();
		pWindow->AddElement( pButtonTalent );

		UI::Tooltip_ptr pTooltipRequiredTalent = std::make_shared<UI::Tooltip>( Rectangle2D( 0, 0, 0, 0 ) );
		pTooltipRequiredTalent->Init();
		pTooltipRequiredTalent->SetID( TOOLTIP_RequiredTalent );
		pTooltipRequiredTalent->SetFollowMouse( FALSE );
		pTooltipRequiredTalent->SetPosition( TOOLTIPPOSITION_Top );
		pTooltipRequiredTalent->SetFont( "Arial", 14, -1 );
		pTooltipRequiredTalent->SetText( "Required 0" );
		pTooltipRequiredTalent->SetLinkObject( pButtonTalent );
		pWindow->AddElement( pTooltipRequiredTalent );
	}

	{
		UI::Button_ptr pButtonAgility = std::make_shared<UI::Button>( Rectangle2D( 565, 167, 18, 17 ) );
		pButtonAgility->SetID( BUTTON_Agility );
		pButtonAgility->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
		pButtonAgility->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
		pButtonAgility->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
		pButtonAgility->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_Agility, std::placeholders::_1 ) ) );
		pButtonAgility->SetAdditionalEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_RequiredAgility, std::placeholders::_1 ) ), EMouseEvent::ClickUpR );
		pButtonAgility->SetMouseEvent( EMouseEvent::ClickUpL );
		pButtonAgility->Disable();
		pWindow->AddElement( pButtonAgility );
		
		UI::Tooltip_ptr pTooltipRequiredAgility = std::make_shared<UI::Tooltip>( Rectangle2D( 0, 0, 0, 0 ) );
		pTooltipRequiredAgility->Init();
		pTooltipRequiredAgility->SetID( TOOLTIP_RequiredAgility );
		pTooltipRequiredAgility->SetFollowMouse( FALSE );
		pTooltipRequiredAgility->SetPosition( TOOLTIPPOSITION_Top );
		pTooltipRequiredAgility->SetFont( "Arial", 14, -1 );
		pTooltipRequiredAgility->SetText( "Required 0" );
		pTooltipRequiredAgility->SetLinkObject( pButtonAgility );
		pWindow->AddElement( pTooltipRequiredAgility );
	}

	{
		UI::Button_ptr pButtonHealth = std::make_shared<UI::Button>( Rectangle2D( 565, 187, 18, 17 ) );
		pButtonHealth->SetID( BUTTON_Health );
		pButtonHealth->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
		pButtonHealth->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
		pButtonHealth->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
		pButtonHealth->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_Health, std::placeholders::_1 ) ) );
		pButtonHealth->SetAdditionalEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_RequiredHealth, std::placeholders::_1 ) ), EMouseEvent::ClickUpR );
		pButtonHealth->SetMouseEvent( EMouseEvent::ClickUpL );
		pButtonHealth->Disable();
		pWindow->AddElement( pButtonHealth );
		
		UI::Tooltip_ptr pTooltipRequiredHealth = std::make_shared<UI::Tooltip>( Rectangle2D( 0, 0, 0, 0 ) );
		pTooltipRequiredHealth->Init();
		pTooltipRequiredHealth->SetID( TOOLTIP_RequiredHealth );
		pTooltipRequiredHealth->SetFollowMouse( FALSE );
		pTooltipRequiredHealth->SetPosition( TOOLTIPPOSITION_Top );
		pTooltipRequiredHealth->SetFont( "Arial", 14, -1 );
		pTooltipRequiredHealth->SetText( "Required 0" );
		pTooltipRequiredHealth->SetLinkObject( pButtonHealth );
		pWindow->AddElement( pTooltipRequiredHealth );
	}

	UI::Button_ptr pButtonBackStatus = std::make_shared<UI::Button>( Rectangle2D( 565, 207, 18, 17 ) );
	pButtonBackStatus->SetID( BUTTON_BackStatus );
	pButtonBackStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\StatusPoint.bmp" ) );
	pButtonBackStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow2.bmp" ) );
	pButtonBackStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
	pButtonBackStatus->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_BackStatus, std::placeholders::_1 ) ) );
	pButtonBackStatus->SetMouseEvent( EMouseEvent::ClickUpL );
	pButtonBackStatus->Disable();
	pWindow->AddElement( pButtonBackStatus );

	UI::Button_ptr pButtonChangeStatus = std::make_shared<UI::Button>( Rectangle2D( 622, 207, 18, 17 ) );
	pButtonChangeStatus->SetID( BUTTON_ChangeStatus );
	pButtonChangeStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
	pButtonChangeStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
	pButtonChangeStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
	pButtonChangeStatus->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonStatusClick, this, BUTTON_ChangeStatus, std::placeholders::_1 ) ) );
	pButtonChangeStatus->SetMouseEvent( EMouseEvent::ClickUpL );
	pButtonChangeStatus->Disable();
	pWindow->AddElement( pButtonChangeStatus );

	UI::Button_ptr pButtonClose = std::make_shared<UI::Button>( Rectangle2D( 250, 240, 20, 20 ) );
	pButtonClose->SetActiveImage( UI::ImageLoader::LoadActiveImage( "game\\images\\misc\\btnclose.png", "game\\images\\misc\\btnclose_.png" ) );
	pButtonClose->SetEvent( UI::Event::Build( std::bind( &CCharacterStatusWindowHandler::OnButtonCloseClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pButtonClose );
}

void CCharacterStatusWindowHandler::DrawTextCharacter( const std::string & strText, const Rectangle2D & rRect, const DX::Color & cColor, UINT uAlign )
{
	RECT r;
	r.left = rRect.iX;
	r.top = rRect.iY;
	r.right = r.left + rRect.iWidth;
	r.bottom = r.top + rRect.iHeight;

	pFont->Draw( strText, r, uAlign, cColor );
}

DWORD CCharacterStatusWindowHandler::GetStatusColor( int iStatus )
{
	if ( (iStatus < 0) || (iStatus >= CHARSTATUSTYPECOLOR_Max) )
		return GetColor( 0 );

	return GetColor( ((int *)0x0370713C)[iStatus] );
}

DWORD CCharacterStatusWindowHandler::GetColor( int iColorStatus )
{
	if ( (iColorStatus < 0) || (iColorStatus >= CHARSTATUSCOLOR_Max) )
		return D3DCOLOR_XRGB( 255, 255, 255 );

	const std::vector<DWORD> vColors =
	{
		D3DCOLOR_XRGB( 255,255,255 ),
		D3DCOLOR_XRGB( 255,0,0 ), 
		D3DCOLOR_XRGB( 255,220,0 ),
		D3DCOLOR_XRGB( 142,230,254 ),
		D3DCOLOR_XRGB( 128,255,128 ), 
		D3DCOLOR_XRGB( 255,225,0 ),
		D3DCOLOR_XRGB( 255,180,200 ),
		D3DCOLOR_XRGB( 255,150,60 ),
		D3DCOLOR_XRGB( 255,220,0 ),
		D3DCOLOR_XRGB( 255,220,0 )
	};

	return vColors[iColorStatus];
}

void CCharacterStatusWindowHandler::OnButtonStatusClick( int iButtonID, UIEventArgs eArgs )
{
	RecordStatusCharacterBuffer sStatus;
	sStatus.iID = BUTTON_Invalid;

	switch ( iButtonID )
	{
		case CCharacterStatusWindowHandler::BUTTON_Strength:
		case CCharacterStatusWindowHandler::BUTTON_Spirit:
		case CCharacterStatusWindowHandler::BUTTON_Talent:
		case CCharacterStatusWindowHandler::BUTTON_Agility:
		case CCharacterStatusWindowHandler::BUTTON_Health:
			RecordBuffer( iButtonID, GetStatusCounter( eCurrentStatusCounter ) );

			AddStatus( iButtonID, GetStatusCounter( eCurrentStatusCounter ) );
			break;
			
		case CCharacterStatusWindowHandler::BUTTON_BackStatus:
			if ( GetInvalidatedBuffer( sStatus ) )
				SubStatus( sStatus.iID, sStatus.iStatusCount );
			break;
			
		case CCharacterStatusWindowHandler::BUTTON_ChangeStatus:
			ChangeStatusCounter();
			break;

		case CCharacterStatusWindowHandler::BUTTON_RequiredStrength:
			RecordBuffer( BUTTON_Strength, sRequiredCharacterStatusBaseData.iStrength );

			AddStatus( BUTTON_Strength, sRequiredCharacterStatusBaseData.iStrength );
			break;
		case CCharacterStatusWindowHandler::BUTTON_RequiredSpirit:
			RecordBuffer( BUTTON_Spirit, sRequiredCharacterStatusBaseData.iSpirit );

			AddStatus( BUTTON_Spirit, sRequiredCharacterStatusBaseData.iSpirit );
			break;
		case CCharacterStatusWindowHandler::BUTTON_RequiredTalent:
			RecordBuffer( BUTTON_Talent, sRequiredCharacterStatusBaseData.iTalent );

			AddStatus( BUTTON_Talent, sRequiredCharacterStatusBaseData.iTalent );
			break;
		case CCharacterStatusWindowHandler::BUTTON_RequiredAgility:
			AddStatus( BUTTON_Agility, sRequiredCharacterStatusBaseData.iAgility );

			RecordBuffer( BUTTON_Agility, sRequiredCharacterStatusBaseData.iAgility );
			break;
		case CCharacterStatusWindowHandler::BUTTON_RequiredHealth:
			RecordBuffer( BUTTON_Health, sRequiredCharacterStatusBaseData.iHealth );

			AddStatus( BUTTON_Health, sRequiredCharacterStatusBaseData.iHealth );
			break;

		default:
			break;
	}
}

void CCharacterStatusWindowHandler::OnButtonCloseClick( UIEventArgs eArgs )
{
	WINDOW_CHARACTERINFO = FALSE;
}

void CCharacterStatusWindowHandler::RecordBuffer( int iButtonID, int iPoints )
{
	for ( UINT i = 0; i < _countof( saStatusRecordBuffer ) - 1; i++ )
	{
		saStatusRecordBuffer[i].iID				= saStatusRecordBuffer[i + 1].iID;
		saStatusRecordBuffer[i].iStatusCount	= saStatusRecordBuffer[i + 1].iStatusCount;
	}

	saStatusRecordBuffer[_countof( saStatusRecordBuffer ) - 1].iID			= iButtonID;
	saStatusRecordBuffer[_countof( saStatusRecordBuffer ) - 1].iStatusCount = iPoints;
}

void CCharacterStatusWindowHandler::SubStatus( int iButtonID, int iStatusCount )
{
	if ( iStatusCount <= 0 )
		return;

	if( iButtonID == BUTTON_Invalid )
		return;

	auto & sCharacterStatusBase = GetCharacterStatusBaseData( UNITDATA->sCharacterData.iClass );

	switch ( iButtonID )
	{
		case CCharacterStatusWindowHandler::BUTTON_Strength:
			if ( UNITDATA->sCharacterData.iStrength <= sCharacterStatusBase.iStrength )
				return;

			if ( (UNITDATA->sCharacterData.iStrength - iStatusCount) < sCharacterStatusBase.iStrength )
				iStatusCount = abs( UNITDATA->sCharacterData.iStrength - sCharacterStatusBase.iStrength );

			UNITDATA->sCharacterData.iStrength -= iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Spirit:
			if ( UNITDATA->sCharacterData.iSpirit <= sCharacterStatusBase.iSpirit )
				return;

			if ( (UNITDATA->sCharacterData.iSpirit - iStatusCount) < sCharacterStatusBase.iSpirit )
				iStatusCount = abs( UNITDATA->sCharacterData.iSpirit - sCharacterStatusBase.iSpirit );

			UNITDATA->sCharacterData.iSpirit -= iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Talent:
			if ( UNITDATA->sCharacterData.iTalent <= sCharacterStatusBase.iTalent )
				return;

			if ( (UNITDATA->sCharacterData.iTalent - iStatusCount) < sCharacterStatusBase.iTalent )
				iStatusCount = abs( UNITDATA->sCharacterData.iTalent - sCharacterStatusBase.iTalent );

			UNITDATA->sCharacterData.iTalent -= iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Agility:
			if ( UNITDATA->sCharacterData.iAgility <= sCharacterStatusBase.iAgility )
				return;

			if ( (UNITDATA->sCharacterData.iAgility - iStatusCount) < sCharacterStatusBase.iAgility )
				iStatusCount = abs( UNITDATA->sCharacterData.iAgility - sCharacterStatusBase.iAgility );

			UNITDATA->sCharacterData.iAgility -= iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Health:
			if ( UNITDATA->sCharacterData.iHealth <= sCharacterStatusBase.iHealth )
				return;

			if ( (UNITDATA->sCharacterData.iHealth - iStatusCount) < sCharacterStatusBase.iHealth )
				iStatusCount = abs( UNITDATA->sCharacterData.iHealth - sCharacterStatusBase.iHealth );
			
			UNITDATA->sCharacterData.iHealth -= iStatusCount;
			break;
		default:
			break;
	}

	UNITDATA->sCharacterData.iStatPoints += iStatusCount;
	iCharacterStatusPointOld = UNITDATA->sCharacterData.iStatPoints;
	
	UPDATE_CHARACTER_CHECKSUM;

	ITEMHANDLER->UpdateItemCharacterStatus();
}

void CCharacterStatusWindowHandler::ChangeStatusCounter()
{
	switch ( eCurrentStatusCounter )
	{
		case EStatusCounter::STATUSCOUNTER_One:
			eCurrentStatusCounter = EStatusCounter::STATUSCOUNTER_Ten;
			break;
		case EStatusCounter::STATUSCOUNTER_Ten:
			eCurrentStatusCounter = EStatusCounter::STATUSCOUNTER_Hundred;
			break;
		case EStatusCounter::STATUSCOUNTER_Hundred:
			eCurrentStatusCounter = EStatusCounter::STATUSCOUNTER_One;
			break;
		default:
			break;
	}

	ChangeStatusCounterButton( eCurrentStatusCounter );
}

bool CCharacterStatusWindowHandler::GetInvalidatedBuffer( RecordStatusCharacterBuffer & sOut )
{
	for ( int i = _countof( saStatusRecordBuffer ) - 1; i >= 0; i-- )
	{
		if ( saStatusRecordBuffer[i].iID != BUTTON_Invalid )
		{
			sOut.iID			= saStatusRecordBuffer[i].iID;
			sOut.iStatusCount	= saStatusRecordBuffer[i].iStatusCount;

			saStatusRecordBuffer[i].iID = BUTTON_Invalid;

			return true;
		}
	}
	
	return false;
}

void CCharacterStatusWindowHandler::AddStatus( int iButtonID, int iStatusCount )
{
	if ( iStatusCount <= 0 )
		return;

	if( UNITDATA->sCharacterData.iStatPoints <= 0 )
		return;

	if ( UNITDATA->sCharacterData.iStatPoints < iStatusCount )
	{
		iStatusCount = UNITDATA->sCharacterData.iStatPoints;
	}

	switch ( iButtonID )
	{
		case CCharacterStatusWindowHandler::BUTTON_Strength:
			UNITDATA->sCharacterData.iStrength += iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Spirit:
			UNITDATA->sCharacterData.iSpirit += iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Talent:
			UNITDATA->sCharacterData.iTalent += iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Agility:
			UNITDATA->sCharacterData.iAgility += iStatusCount;
			break;
		case CCharacterStatusWindowHandler::BUTTON_Health:
			UNITDATA->sCharacterData.iHealth += iStatusCount;
			break;
		default:
			break;
	}

	UNITDATA->sCharacterData.iStatPoints -= iStatusCount;
	iCharacterStatusPointOld = UNITDATA->sCharacterData.iStatPoints;

	UPDATE_CHARACTER_CHECKSUM;

	ITEMHANDLER->UpdateItemCharacterStatus();

	UpdateRequiredStatus();
}

int CCharacterStatusWindowHandler::GetStatusCounter( EStatusCounter eStatusCounter )
{
	int iStatusCounterNum = 1;

	switch ( eStatusCounter )
	{
		case CCharacterStatusWindowHandler::STATUSCOUNTER_One:
			iStatusCounterNum = 1;
			break;
		case CCharacterStatusWindowHandler::STATUSCOUNTER_Ten:
			iStatusCounterNum = 10;
			break;
		case CCharacterStatusWindowHandler::STATUSCOUNTER_Hundred:
			iStatusCounterNum = 100;
			break;
		default:
			break;
	}

	return iStatusCounterNum;
}

void CCharacterStatusWindowHandler::ChangeStatusCounterButton( EStatusCounter eStatusCounter )
{
	auto SetChangeStatusButton = []( UI::Button_ptr pButton, EStatusCounter _eStatusCounter ) -> void
	{
		pButton->Clear();

		switch ( _eStatusCounter )
		{
			case CCharacterStatusWindowHandler::STATUSCOUNTER_One:
				pButton->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
				pButton->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
				pButton->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
				break;
				
			case CCharacterStatusWindowHandler::STATUSCOUNTER_Ten:
				pButton->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangeArrowRed.bmp" ) );
				pButton->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectArrowRed.bmp" ) );
				pButton->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
				break;
				
			case CCharacterStatusWindowHandler::STATUSCOUNTER_Hundred:
				pButton->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangeArrowPurple.bmp" ) );
				pButton->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectArrowPurple.bmp" ) );
				pButton->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
				break;
				
			default:
				break;
		}
	};

	auto pButtonBackStatus = pWindow->GetElement<UI::Button>( BUTTON_BackStatus );
	auto pButtonChangeStatus = pWindow->GetElement<UI::Button>( BUTTON_ChangeStatus );

	pButtonBackStatus->Clear();
	pButtonChangeStatus->Clear();

	switch ( eStatusCounter )
	{
		case CCharacterStatusWindowHandler::STATUSCOUNTER_One:
			pButtonBackStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\StatusPoint.bmp" ) );
			pButtonBackStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow2.bmp" ) );
			pButtonBackStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
			
			pButtonChangeStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow.bmp" ) );
			pButtonChangeStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\SelectArrow.bmp" ) );
			pButtonChangeStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrow_disabled.bmp" ) );
			break;
			
		case CCharacterStatusWindowHandler::STATUSCOUNTER_Ten:
			pButtonBackStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangeArrowRed2.bmp" ) );
			pButtonBackStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectArrowRed2.bmp" ) );
			pButtonBackStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrowRed_disabled.bmp" ) );

			pButtonChangeStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangePower.bmp" ) );
			pButtonChangeStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectPower.bmp" ) );
			pButtonChangeStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangePower_disabled.bmp" ) );
			break;
			
		case CCharacterStatusWindowHandler::STATUSCOUNTER_Hundred:
			pButtonBackStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangeArrowPurple2.bmp" ) );
			pButtonBackStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectArrowPurple2.bmp" ) );
			pButtonBackStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangeArrowPurple_disabled.bmp" ) );

			pButtonChangeStatus->SetImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\ChangePowerPurple.bmp" ) );
			pButtonChangeStatus->SetHoldImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\Custom\\SelectPowerPurple.bmp" ) );
			pButtonChangeStatus->SetDisabledImage( UI::ImageLoader::LoadImage( "image\\Sinimage\\Status\\ChangePowerPurple_disabled.bmp" ) );
			break;
		default:
			break;
	}

	SetChangeStatusButton( pWindow->GetElement<UI::Button>( BUTTON_Strength ), eStatusCounter );
	SetChangeStatusButton( pWindow->GetElement<UI::Button>( BUTTON_Spirit ), eStatusCounter );
	SetChangeStatusButton( pWindow->GetElement<UI::Button>( BUTTON_Talent ), eStatusCounter );
	SetChangeStatusButton( pWindow->GetElement<UI::Button>( BUTTON_Agility ), eStatusCounter );
	SetChangeStatusButton( pWindow->GetElement<UI::Button>( BUTTON_Health ), eStatusCounter );
}

void CCharacterStatusWindowHandler::EnableButtons()
{
	pWindow->GetElement<UI::Button>( BUTTON_Strength )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_Spirit )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_Talent )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_Agility )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_Health )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_BackStatus )->Enable();
	pWindow->GetElement<UI::Button>( BUTTON_ChangeStatus )->Enable();

	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredStrength )->Open();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredSpirit )->Open();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredTalent )->Open();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredAgility )->Open();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredHealth )->Open();

	UpdateRequiredStatus();

	bIsButtonsEnabled = true;
}

void CCharacterStatusWindowHandler::DisableButtons()
{
	pWindow->GetElement<UI::Button>( BUTTON_Strength )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_Spirit )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_Talent )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_Agility )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_Health )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_BackStatus )->Disable();
	pWindow->GetElement<UI::Button>( BUTTON_ChangeStatus )->Disable();
	
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredStrength )->Close();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredSpirit )->Close();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredTalent )->Close();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredAgility )->Close();
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredHealth )->Close();

	bIsButtonsEnabled = false;
}

void CCharacterStatusWindowHandler::UpdateRequiredStatus()
{
	GetRequiredStatus( sRequiredCharacterStatusBaseData );

	dwaStatusBaseColor[0] = (sRequiredCharacterStatusBaseData.iStrength > 0 ? DX::Color( 255, 255, 150, 140 ) : DX::Color( 255, 255, 255, 255 )).Get();
	dwaStatusBaseColor[1] = (sRequiredCharacterStatusBaseData.iSpirit > 0 ? DX::Color( 255, 255, 150, 140 ) : DX::Color( 255, 255, 255, 255 )).Get();
	dwaStatusBaseColor[2] = (sRequiredCharacterStatusBaseData.iTalent > 0 ? DX::Color( 255, 255, 150, 140 ) : DX::Color( 255, 255, 255, 255 )).Get();
	dwaStatusBaseColor[3] = (sRequiredCharacterStatusBaseData.iAgility > 0 ? DX::Color( 255, 255, 150, 140 ) : DX::Color( 255, 255, 255, 255 )).Get();
	dwaStatusBaseColor[4] = (sRequiredCharacterStatusBaseData.iHealth > 0 ? DX::Color( 255, 255, 150, 140 ) : DX::Color( 255, 255, 255, 255 )).Get();

	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredStrength )->SetText( "Required " + std::to_string( sRequiredCharacterStatusBaseData.iStrength ) );
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredSpirit )->SetText( "Required " + std::to_string( sRequiredCharacterStatusBaseData.iSpirit ) );
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredTalent )->SetText( "Required " + std::to_string( sRequiredCharacterStatusBaseData.iTalent ) );
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredAgility )->SetText( "Required " + std::to_string( sRequiredCharacterStatusBaseData.iAgility ) );
	pWindow->GetElement<UI::Tooltip>( TOOLTIP_RequiredHealth )->SetText( "Required " + std::to_string( sRequiredCharacterStatusBaseData.iHealth ) );

	if ( sRequiredCharacterStatusBaseData.iStrength > 0 )
	{
		sRequiredCharacterStatusBaseData.iStrength -= UNITDATA->sCharacterData.iStrength;
	}
	
	if ( sRequiredCharacterStatusBaseData.iSpirit > 0 )
	{
		sRequiredCharacterStatusBaseData.iSpirit -= UNITDATA->sCharacterData.iSpirit;
	}

	if ( sRequiredCharacterStatusBaseData.iTalent > 0 )
	{
		sRequiredCharacterStatusBaseData.iTalent -= UNITDATA->sCharacterData.iTalent;
	}

	if ( sRequiredCharacterStatusBaseData.iAgility > 0 )
	{
		sRequiredCharacterStatusBaseData.iAgility -= UNITDATA->sCharacterData.iAgility;
	}

	if ( sRequiredCharacterStatusBaseData.iHealth > 0 )
	{
		sRequiredCharacterStatusBaseData.iHealth -= UNITDATA->sCharacterData.iHealth;
	}
}

void CCharacterStatusWindowHandler::GetRequiredStatus( CharacterStatusBaseData & sCharacterStatusBaseData )
{
	sCharacterStatusBaseData.eCharacterClass = UNITDATA->sCharacterData.iClass;
	sCharacterStatusBaseData.iStrength = 0;
	sCharacterStatusBaseData.iSpirit = 0;
	sCharacterStatusBaseData.iTalent = 0;
	sCharacterStatusBaseData.iAgility = 0;
	sCharacterStatusBaseData.iHealth = 0;

	auto UpdateStatusRequired = [&sCharacterStatusBaseData]( ItemData * pcItemData ) -> void
	{
		if ( pcItemData && pcItemData->bValid )
		{
			sCharacterStatusBaseData.iStrength	= max( sCharacterStatusBaseData.iStrength, pcItemData->sItem.iStrength );
			sCharacterStatusBaseData.iSpirit	= max( sCharacterStatusBaseData.iSpirit, pcItemData->sItem.iSpirit );
			sCharacterStatusBaseData.iTalent	= max( sCharacterStatusBaseData.iTalent, pcItemData->sItem.iTalent );
			sCharacterStatusBaseData.iAgility	= max( sCharacterStatusBaseData.iAgility, pcItemData->sItem.iAgility );
			sCharacterStatusBaseData.iHealth	= max( sCharacterStatusBaseData.iHealth, pcItemData->sItem.iHealth );
		}
	};

	for ( int i = 0; i < ITEMSLOT_Max; i++ )
	{
		if ( INVENTORYITEMSLOT[i].iItemIndex )
		{
			UpdateStatusRequired( &INVENTORYITEMS[INVENTORYITEMSLOT[i].iItemIndex - 1] );
		}
	}

	//Earrings
	UpdateStatusRequired( HUDHANDLER->GetInventoryWindow()->GetEarring1() );
	UpdateStatusRequired( HUDHANDLER->GetInventoryWindow()->GetEarring2() );

	if ( sCharacterStatusBaseData.iStrength <= UNITDATA->sCharacterData.iStrength )
	{
		sCharacterStatusBaseData.iStrength = 0;
	}

	if ( sCharacterStatusBaseData.iSpirit <= UNITDATA->sCharacterData.iSpirit )
	{
		sCharacterStatusBaseData.iSpirit = 0;
	}
	
	if ( sCharacterStatusBaseData.iTalent <= UNITDATA->sCharacterData.iTalent )
	{
		sCharacterStatusBaseData.iTalent = 0;
	}

	if ( sCharacterStatusBaseData.iAgility <= UNITDATA->sCharacterData.iAgility )
	{
		sCharacterStatusBaseData.iAgility = 0;
	}

	if ( sCharacterStatusBaseData.iHealth <= UNITDATA->sCharacterData.iHealth )
	{
		sCharacterStatusBaseData.iHealth = 0;
	}
}

void CCharacterStatusWindowHandler::OnBeforeRenderChildren( UIEventArgs eArgs )
{
	RenderText( 0, RESOLUTION_HEIGHT - WINDOW_CHARACTERINFOMOVE - 19 );
}
