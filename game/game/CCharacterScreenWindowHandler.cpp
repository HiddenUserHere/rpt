#include "stdafx.h"
#include "CCharacterScreenWindowHandler.h"


CCharacterScreenWindowHandler::CCharacterScreenWindowHandler()
{
	pWindow = std::make_shared<UI::Window>( Rectangle2D( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT ) );
	pWindow->SetFocusWindow( FALSE );
	
	pcCharacterRaceWindowHandler = new CCharacterRaceWindowHandler();

	pcCharacterScreenNewHandler = new CCharacterScreenNewHandler();
}


CCharacterScreenWindowHandler::~CCharacterScreenWindowHandler()
{
	DELET( pcCharacterScreenNewHandler );
	DELET( pcCharacterRaceWindowHandler );
}

void CCharacterScreenWindowHandler::Init()
{
	UI::ImageBox_ptr pBackgroundTop = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 260, 80 ) );
	pBackgroundTop->SetID( IMAGE_MainboxBackgroundTop );
	pBackgroundTop->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_background.png" ).c_str() ) );
	pWindow->AddElement( pBackgroundTop );

	UI::ImageBox_ptr pBackgroundBottom = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 260, 80 ) );
	pBackgroundBottom->SetID( IMAGE_MainboxBackgroundBottom );
	pBackgroundBottom->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_background.png" ).c_str() ) );
	pWindow->AddElement( pBackgroundBottom );

	UI::ImageBox_ptr pBackgroundMainboxLeft = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 8, 64 ) );
	pBackgroundMainboxLeft->SetID( IMAGE_MainboxLeft );
	pBackgroundMainboxLeft->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_L.png" ).c_str() ) );
	pBackgroundMainboxLeft->SetRepeatImageMode( UI::ImageBox::ERepeatImageMode::Vertical );
	pWindow->AddElement( pBackgroundMainboxLeft );

	UI::ImageBox_ptr pBackgroundMainboxRight = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 8, 64 ) );
	pBackgroundMainboxRight->SetID( IMAGE_MainboxRight );
	pBackgroundMainboxRight->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_R.png" ).c_str() ) );
	pBackgroundMainboxRight->SetRepeatImageMode( UI::ImageBox::ERepeatImageMode::Vertical );
	pWindow->AddElement( pBackgroundMainboxRight );

	UI::ImageBox_ptr pBackgroundMainboxTop = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 64, 8 ) );
	pBackgroundMainboxTop->SetID( IMAGE_MainboxTop );
	pBackgroundMainboxTop->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_T.png" ).c_str() ) );
	pBackgroundMainboxTop->SetRepeatImageMode( UI::ImageBox::ERepeatImageMode::Horizontal );
	pWindow->AddElement( pBackgroundMainboxTop );

	UI::ImageBox_ptr pBackgroundMainboxBottom = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 0, 64, 8 ) );
	pBackgroundMainboxBottom->SetID( IMAGE_MainboxBottom );
	pBackgroundMainboxBottom->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_D.png" ).c_str() ) );
	pBackgroundMainboxBottom->SetRepeatImageMode( UI::ImageBox::ERepeatImageMode::Horizontal );
	pWindow->AddElement( pBackgroundMainboxBottom );

	UI::ImageBox_ptr pImageMainboxTopLeft = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 80, 128, 64 ) );
	pImageMainboxTopLeft->SetID( IMAGE_MainboxTopLeft );
	pImageMainboxTopLeft->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_CN1.png" ).c_str() ) );
	pWindow->AddElement( pImageMainboxTopLeft );
	
	UI::ImageBox_ptr pImageMainboxTopRight = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 80, 128, 64 ) );
	pImageMainboxTopRight->SetID( IMAGE_MainboxTopRight );
	pImageMainboxTopRight->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_CN2.png" ).c_str() ) );
	pWindow->AddElement( pImageMainboxTopRight );
	
	UI::ImageBox_ptr pImageMainboxBottomLeft = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 80, 128, 64 ) );
	pImageMainboxBottomLeft->SetID( IMAGE_MainboxBottomLeft );
	pImageMainboxBottomLeft->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_CN3.png" ).c_str() ) );
	pWindow->AddElement( pImageMainboxBottomLeft );

	UI::ImageBox_ptr pImageMainboxBottomRight = std::make_shared<UI::ImageBox>( Rectangle2D( 0, 80, 128, 64 ) );
	pImageMainboxBottomRight->SetID( IMAGE_MainboxBottomRight );
	pImageMainboxBottomRight->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "mainbox_CN4.png" ).c_str() ) );
	pWindow->AddElement( pImageMainboxBottomRight );

	//Title
	{
		UI::Window_ptr pTitleWindow = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 162, 68 ) );
		pTitleWindow->SetID( WINDOW_Titlebox );
		pTitleWindow->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "title_box.png" ).c_str() ) );
		pWindow->AddElement( pTitleWindow );
		
		UI::ImageBox_ptr pImageSelectChar = std::make_shared<UI::ImageBox>( Rectangle2D( 26, 9, 110, 20 ) );
		pImageSelectChar->SetID( IMAGE_TitleboxSelectChar );
		pImageSelectChar->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "title_select_char.png" ).c_str() ) );
		pTitleWindow->AddElement( pImageSelectChar );

		UI::ImageBox_ptr pImageSelectTribe = std::make_shared<UI::ImageBox>( Rectangle2D( 26, 9, 110, 20 ) );
		pImageSelectTribe->SetID( IMAGE_TitleboxSelectTribe );
		pImageSelectTribe->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "title_select_tribe.png" ).c_str() ) );
		pImageSelectTribe->Hide();
		pTitleWindow->AddElement( pImageSelectTribe );

		UI::ImageBox_ptr pImageSelectClass = std::make_shared<UI::ImageBox>( Rectangle2D( 26, 9, 110, 20 ) );
		pImageSelectClass->SetID( IMAGE_TitleboxSelectClass );
		pImageSelectClass->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "title_select_class.png" ).c_str() ) );
		pImageSelectClass->Hide();
		pTitleWindow->AddElement( pImageSelectClass );

		UI::ImageBox_ptr pImageSelectFace = std::make_shared<UI::ImageBox>( Rectangle2D( 26, 9, 110, 20 ) );
		pImageSelectFace->SetID( IMAGE_TitleboxSelectFace );
		pImageSelectFace->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "title_select_face.png" ).c_str() ) );
		pImageSelectFace->Hide();
		pTitleWindow->AddElement( pImageSelectFace );
	}

	//New Character Button
	UI::Button_ptr pNewCharacterButton = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 256, 64 ) );
	pNewCharacterButton->SetID( BUTTON_NewCharacter );
	pNewCharacterButton->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "makechar.png" ).c_str() ) );
	pNewCharacterButton->SetAlphaHover( 180 );
	pNewCharacterButton->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonNewCharacterClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pNewCharacterButton );


	//Back
	UI::Button_ptr pButtonBack = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 32 ) );
	pButtonBack->SetID( BUTTON_Back );
	pButtonBack->SetAlphaHover( 180 );
	pButtonBack->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "back.png" ).c_str() ) );
	pButtonBack->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonBackClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pButtonBack );

	//Select
	UI::Button_ptr pButtonSelect = std::make_shared<UI::Button>( Rectangle2D( 132, 0, 128, 32 ) );
	pButtonSelect->SetID( BUTTON_Select );
	pButtonSelect->SetAlphaHover( 180 );
	pButtonSelect->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "select.png" ).c_str() ) );
	pButtonSelect->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonSelectClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pButtonSelect );
	
	//Create
	UI::Button_ptr pButtonCreate = std::make_shared<UI::Button>( Rectangle2D( 132, 0, 128, 32 ) );
	pButtonCreate->SetID( BUTTON_Create );
	pButtonCreate->SetAlphaHover( 180 );
	pButtonCreate->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "newchar.png" ).c_str() ) );
	pButtonCreate->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonCreateClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pButtonCreate );

	//Delete
	UI::Button_ptr pButtonDelete = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 32 ) );
	pButtonDelete->SetID( BUTTON_Delete );
	pButtonDelete->SetAlphaHover( 180 );
	pButtonDelete->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "delete.png" ).c_str() ) );
	pButtonDelete->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonDeleteClick, this, std::placeholders::_1 ) ) );
	pButtonDelete->Hide();
	pWindow->AddElement( pButtonDelete );

	//Exit
	UI::Button_ptr pButtonExit = std::make_shared<UI::Button>( Rectangle2D( 132, 0, 128, 32 ) );
	pButtonExit->SetID( BUTTON_Exit );
	pButtonExit->SetAlphaHover( 180 );
	pButtonExit->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "exit.png" ).c_str() ) );
	pButtonExit->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenWindowHandler::OnButtonExitClick, this, std::placeholders::_1 ) ) );
	pWindow->AddElement( pButtonExit );

	//Character Info Box
	{
		pcCharacterInfoImageNoClan = UI::ImageLoader::LoadImage( GetImagePath( "chbox_no_clan.png" ).c_str() );
		pcCharacterInfoImageClan = UI::ImageLoader::LoadImage( GetImagePath( "chbox.png" ).c_str() );

		UI::Window_ptr pWindowCharacterInfo = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 256, 128 ) );
		pWindowCharacterInfo->SetID( WINDOW_CharacterInfo );
		pWindowCharacterInfo->SetImage( pcCharacterInfoImageClan );
		pWindowCharacterInfo->Hide();
		pWindow->AddElement( pWindowCharacterInfo );

		UI::ImageBox_ptr pImageClan = std::make_shared<UI::ImageBox>( Rectangle2D( 76, 16, 16, 16 ) );
		pImageClan->SetID( IMAGE_CharacterClan );
		pImageClan->SetImage( nullptr );
		pImageClan->SetScale( 0.5f );
		pWindowCharacterInfo->AddElement( pImageClan );

		UI::Text_ptr pTextCharacterClanName = std::make_shared<UI::Text>( Rectangle2D( 95, 36, 133, 16 ) );
		pTextCharacterClanName->SetID( TEXT_CharacterClanName );
		pTextCharacterClanName->SetText( "" );
		pTextCharacterClanName->SetColor( DX::Color( 255, 255, 255, 255 ).Get() );
		pWindowCharacterInfo->AddElement( pTextCharacterClanName );

		UI::Text_ptr pTextCharacterName = std::make_shared<UI::Text>( Rectangle2D( 77, 56, 152, 16 ) );
		pTextCharacterName->SetID( TEXT_CharacterName );
		pTextCharacterName->SetText( "" );
		pTextCharacterName->SetColor( DX::Color( 255, 255, 255, 255 ).Get() );
		pWindowCharacterInfo->AddElement( pTextCharacterName );

		UI::Text_ptr pTextCharacterLevel = std::make_shared<UI::Text>( Rectangle2D( 77, 76, 152, 16 ) );
		pTextCharacterLevel->SetID( TEXT_CharacterLevel );
		pTextCharacterLevel->SetText( "" );
		pTextCharacterLevel->SetColor( DX::Color( 255, 205, 190, 155 ).Get() );
		pWindowCharacterInfo->AddElement( pTextCharacterLevel );

		UI::Text_ptr pTextCharacterTribe = std::make_shared<UI::Text>( Rectangle2D( 77, 96, 152, 16 ) );
		pTextCharacterTribe->SetID( TEXT_CharacterTribe );
		pTextCharacterTribe->SetText( "" );
		pTextCharacterTribe->SetColor( DX::Color( 255, 255, 255, 255 ).Get() );
		pWindowCharacterInfo->AddElement( pTextCharacterTribe );

		UI::Text_ptr pTextCharacterClass = std::make_shared<UI::Text>( Rectangle2D( 77, 116, 152, 16 ) );
		pTextCharacterClass->SetID( TEXT_CharacterClass );
		pTextCharacterClass->SetText( "" );
		pTextCharacterClass->SetColor( DX::Color( 255, 255, 255, 255 ).Get() );
		pWindowCharacterInfo->AddElement( pTextCharacterClass );
	}

	UpdateWindowPosition();

	UpdateButtonsMainbox( false, true, false, true );

	pcCharacterRaceWindowHandler->Init();
	pcCharacterScreenNewHandler->Init();
}

void CCharacterScreenWindowHandler::Render()
{
	pWindow->Render( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );

	pcCharacterRaceWindowHandler->Render();

	pcCharacterScreenNewHandler->Render();
}

void CCharacterScreenWindowHandler::Update( float fTime )
{
	if ( fTemporaryBlockActions > 0.0f )
	{
		fTemporaryBlockActions -= fTime;

		if ( fTemporaryBlockActions < 0.0f )
			fTemporaryBlockActions = 0.0f;
	}
}

BOOL CCharacterScreenWindowHandler::OnMouseClick( CMouse * pcMouse )
{
	if ( IsBlockedActions() )
		return FALSE;

	if ( pWindow->IsOpen() )
		if ( pWindow->OnMouseClick( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 ) )
			return TRUE;

	return pcCharacterRaceWindowHandler->OnMouseClick( pcMouse ) ||
		pcCharacterScreenNewHandler->OnMouseClick( pcMouse );
}

void CCharacterScreenWindowHandler::OnMouseMove( CMouse * pcMouse )
{
	pWindow->OnMouseMove( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
	pcCharacterRaceWindowHandler->OnMouseMove( pcMouse );
	pcCharacterScreenNewHandler->OnMouseMove( pcMouse );
}

BOOL CCharacterScreenWindowHandler::OnKeyPress( CKeyboard * pcKeyboard )
{
	if ( IsBlockedActions() )
		return FALSE;

	if ( (pcKeyboard->GetEvent() == KeyDown) && (pcKeyboard->GetKey() == VK_ESCAPE) )
	{
		OnButtonBackClick( UIEventArgs{} );
		return TRUE;
	}

	return pcCharacterScreenNewHandler->OnKeyPress( pcKeyboard );
}

BOOL CCharacterScreenWindowHandler::OnKeyChar( CKeyboard * pcKeyboard )
{
	if ( IsBlockedActions() )
		return FALSE;
	
	return pcCharacterScreenNewHandler->OnKeyChar( pcKeyboard );
}

void CCharacterScreenWindowHandler::OnResolutionChanged()
{
	UpdateWindowPosition();

	pcCharacterRaceWindowHandler->OnResolutionChanged();

	pcCharacterScreenNewHandler->OnResolutionChanged();
}

void CCharacterScreenWindowHandler::UpdateCharacterInfo( const CharacterScreenCharacterData & sCharacterData )
{
	int iX = 77;
	int iY = 36;

	auto pWindowCharacterInfo = pWindow->GetElement<UI::Window>( WINDOW_CharacterInfo );

	auto pTextCharacterName = pWindowCharacterInfo->GetElement<UI::Text>( TEXT_CharacterName );
	pTextCharacterName->SetText( sCharacterData.strName );
	
	auto pTextCharacterLevel = pWindowCharacterInfo->GetElement<UI::Text>( TEXT_CharacterLevel );
	pTextCharacterLevel->SetText( std::to_string( sCharacterData.iLevel ) );
	
	auto pTextCharacterTribe = pWindowCharacterInfo->GetElement<UI::Text>( TEXT_CharacterTribe );
	pTextCharacterTribe->SetText( GetCharacterRaceName( GetCharacterRace( sCharacterData.iClass ) ) );

	auto pTextCharacterClass = pWindowCharacterInfo->GetElement<UI::Text>( TEXT_CharacterClass );
	pTextCharacterClass->SetText( GetCharacterClassByFlag( CharacterClassToClassFlag( sCharacterData.iClass ) ) );

	auto pTextCharacterClan = pWindowCharacterInfo->GetElement<UI::Text>( TEXT_CharacterClanName );
	auto pImageClan = pWindowCharacterInfo->GetElement<UI::ImageBox>( IMAGE_CharacterClan );
	if ( sCharacterData.strClanName.length() > 0 )
	{
		pTextCharacterClan->SetText( sCharacterData.strClanName );
		pTextCharacterClan->SetPosition( iX + 18, iY - 20 );

		if ( sCharacterData.psClanSurface )
		{
			pImageClan->SetTexture( sCharacterData.psClanSurface->sTexture.pcD3DTexture );

			pImageClan->Show();
		}

		pWindowCharacterInfo->SetImage( pcCharacterInfoImageClan );

		//Update Y Position
		//iY += 20;
	}
	else
	{
		pWindowCharacterInfo->SetImage( pcCharacterInfoImageNoClan );
		pTextCharacterClan->SetText( "" );

		pImageClan->Hide();

		//Update Y Position
		iY = 26;
	}

	pTextCharacterName->SetPosition( iX, iY );
	pTextCharacterLevel->SetPosition( iX, iY + 20 );
	pTextCharacterTribe->SetPosition( iX, iY + 40 );
	pTextCharacterClass->SetPosition( iX, iY + 60 );

	pWindowCharacterInfo->Show();
}

void CCharacterScreenWindowHandler::HideCharacterInfo()
{
	auto pWindowCharacterInfo = pWindow->GetElement<UI::Window>( WINDOW_CharacterInfo );
	pWindowCharacterInfo->Hide();
}

void CCharacterScreenWindowHandler::OnCharacterStep( ECharacterScreenStep eStep )
{
	pcCharacterScreenNewHandler->UpdateNewCharacters( eStep );

	switch ( eStep )
	{
		case CHARACTERSCREENSTEP_None:
			UpdateButtonsMainbox( false, true, false, true );
			ShowTitle( IMAGE_TitleboxSelectChar );

			pcCharacterRaceWindowHandler->Close();
			break;
		case CHARACTERSCREENSTEP_NewCharacterTribe:
			UpdateButtonsMainbox( false, true, true, true );
			ShowTitle( IMAGE_TitleboxSelectTribe );

			pcCharacterRaceWindowHandler->Open();
			break;
		case CHARACTERSCREENSTEP_NewCharacterTempskronClass:
			ShowTitle( IMAGE_TitleboxSelectClass );

			pcCharacterRaceWindowHandler->Close();
			break;
		case CHARACTERSCREENSTEP_NewCharacterMorionClass:
			ShowTitle( IMAGE_TitleboxSelectClass );

			pcCharacterRaceWindowHandler->Close();
			break;
		case CHARACTERSCREENSTEP_NewCharacterHair:
			ShowTitle( IMAGE_TitleboxSelectFace );
			break;
		default:
			break;
	}
}

void CCharacterScreenWindowHandler::UpdateDeleteButton( bool bShow )
{
	auto pButtonDelete = pWindow->GetElement<UI::Button>( BUTTON_Delete );

	if ( bShow )
		pButtonDelete->Show();
	else
		pButtonDelete->Hide();
}

void CCharacterScreenWindowHandler::SetTemporaryBlockActions( float fDuration )
{
	fTemporaryBlockActions = fDuration;
}

const std::string CCharacterScreenWindowHandler::GetCharacterInitialBodyPath( ECharacterClass eCharacterClass )
{
	return pcCharacterScreenNewHandler->GetCharacterInitialModelPath( eCharacterClass );
}

void CCharacterScreenWindowHandler::ClearText()
{
	vTextLeft.clear();
	vTextRight.clear();
}

void CCharacterScreenWindowHandler::AddString( const std::string strText, BOOL bBold, DX::Color cColor, bool bLeft )
{
	CharacterInfoText sCharacterInfoText;
	sCharacterInfoText.strText	= strText;
	sCharacterInfoText.bBold	= bBold;
	sCharacterInfoText.dwColor	= cColor.Get();

	if ( bLeft )
		vTextLeft.push_back( sCharacterInfoText );
	else
		vTextRight.push_back( sCharacterInfoText );
}

void CCharacterScreenWindowHandler::BuildCharacterText()
{
}

void CCharacterScreenWindowHandler::OnButtonNewCharacterClick( UIEventArgs eArgs )
{
	if ( GetCharacterScreen()->GetModel()->GetUserCharacterCount() < GetCharacterScreen()->GetModel()->GetMaxUserCharacter() )
	{
		GetCharacterScreen()->SetCharacterScreenStep( ECharacterScreenStep::CHARACTERSCREENSTEP_NewCharacterTribe );

		//Hide New Character Button
		pWindow->GetElement<UI::Button>( BUTTON_NewCharacter )->Hide();
	}
	else
	{
		TITLEBOX( AY_OBFUSCATE( "You've reached the maximum number of Characters you can create" ) );
	}
}

void CCharacterScreenWindowHandler::OnButtonSelectClick( UIEventArgs eArgs )
{
	if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_None )
	{
		GetCharacterScreen()->SelectCharacter( true );
	}
	else if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterHair )
	{
		pcCharacterScreenNewHandler->ProcessCreateCharacter();
	}
}

void CCharacterScreenWindowHandler::OnButtonBackClick( UIEventArgs eArgs )
{
	if ( pWindow->GetElement<UI::Button>( BUTTON_Back )->IsOpen() )
	{
		if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterTribe )
		{
			GetCharacterScreen()->SetCharacterScreenStep( CHARACTERSCREENSTEP_None );

			//Show New Character Button
			pWindow->GetElement<UI::Button>( BUTTON_NewCharacter )->Show();
		}
		else if ( (GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterTempskronClass) || (GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterMorionClass) )
		{
			GetCharacterScreen()->SetCharacterScreenStep( CHARACTERSCREENSTEP_NewCharacterTribe );
		}
		else if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterHair )
		{
			GetCharacterScreen()->SetCharacterScreenStep( GetCharacterScreen()->GetOldStep() );
		}
	}
}

void CCharacterScreenWindowHandler::UpdateWindowPosition()
{
	int iX = RESOLUTION_WIDTH >> 1;
	int iY = RESOLUTION_HEIGHT >> 1;

	pWindow->SetBox( Rectangle2D( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT ) );

	auto pButtonNewCharacter = pWindow->GetElement<UI::Button>( BUTTON_NewCharacter );
	pButtonNewCharacter->SetPosition( iX - (pButtonNewCharacter->GetWidth() >> 1), (iY >> 2) + 40 );

	pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxBackgroundTop )->SetBox( Rectangle2D( 0, 0, RESOLUTION_WIDTH, 80 ) );
	pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxBackgroundBottom )->SetBox( Rectangle2D( 0, RESOLUTION_HEIGHT - 80, RESOLUTION_WIDTH, 80 ) );

	auto pBackgroundMainboxLeft = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxLeft );
	pBackgroundMainboxLeft->SetBox( Rectangle2D( 0, 86, 8, RESOLUTION_HEIGHT - 174 ) );
	
	auto pBackgroundMainboxRight = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxRight );
	pBackgroundMainboxRight->SetBox( Rectangle2D( RESOLUTION_WIDTH - 8, 86, 8, RESOLUTION_HEIGHT - 174 ) );
	
	auto pBackgroundMainboxTop = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxTop );
	pBackgroundMainboxTop->SetBox( Rectangle2D( 0, 80, RESOLUTION_WIDTH, 8 ) );
	
	auto pBackgroundMainboxBottom = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxBottom );
	pBackgroundMainboxBottom->SetBox( Rectangle2D( 0, RESOLUTION_HEIGHT - 87, RESOLUTION_WIDTH, 8 ) );

	auto pImageMainboxTopLeft = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxTopLeft );
	pImageMainboxTopLeft->SetPosition( 0, 79 );
	
	auto pImageMainboxTopRight = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxTopRight );
	pImageMainboxTopRight->SetPosition( RESOLUTION_WIDTH - pImageMainboxTopRight->GetWidth(), 79 );
	
	auto pImageMainboxBottomLeft = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxBottomLeft );
	pImageMainboxBottomLeft->SetPosition( 0, RESOLUTION_HEIGHT - pImageMainboxBottomLeft->GetHeight() - 80 );
	
	auto pImageMainboxBottomRight = pWindow->GetElement<UI::ImageBox>( IMAGE_MainboxBottomRight );
	pImageMainboxBottomRight->SetPosition( RESOLUTION_WIDTH - pImageMainboxBottomRight->GetWidth(), RESOLUTION_HEIGHT - pImageMainboxBottomRight->GetHeight() - 80 );

	auto pWindowTitle = pWindow->GetElement<UI::Window>( WINDOW_Titlebox );
	pWindowTitle->SetPosition( (RESOLUTION_WIDTH >> 1) - (pWindowTitle->GetWidth() >> 1), 60 );

	UpdateButtonsPosition();

	auto pWindowCharacterInfo = pWindow->GetElement<UI::Window>( WINDOW_CharacterInfo );
	pWindowCharacterInfo->SetPosition( (RESOLUTION_WIDTH >> 1) - (pWindowCharacterInfo->GetWidth() >> 1), RESOLUTION_HEIGHT - 260 );
}

CCharacterScreen * CCharacterScreenWindowHandler::GetCharacterScreen()
{
	return (CCharacterScreen*)GAMESCREEN->GetActiveScreen();
}

void CCharacterScreenWindowHandler::OnButtonDeleteClick( UIEventArgs eArgs )
{
	if ( GetCharacterScreen()->GetModel()->CanDeleteSelectedCharacter() )
	{
		MESSAGEBOX->SetBoxType( CMessageBox::BOXTYPEID_YesNo );
		MESSAGEBOX->SetTitle( "Are You Sure" );
		MESSAGEBOX->SetDescription( FormatString( "Do you want to delete this character?" ) );
		MESSAGEBOX->SetEvent( std::bind( &CCharacterScreenWindowHandler::OnDeleteButtonConfirm, this ) );
		MESSAGEBOX->SetAutoClose( 10 );
		MESSAGEBOX->Show();

		PLAYMINISOUND( 11 );
		SOUNDHANDLER->UpdateVolume();
	}
	else
	{
		TITLEBOX( "You must leave the clan before deleting this character" );
	}
}

void CCharacterScreenWindowHandler::OnButtonCreateClick( UIEventArgs eArgs )
{
}

void CCharacterScreenWindowHandler::OnButtonExitClick( UIEventArgs eArgs )
{
	MESSAGEBOX->ClearEvents();
	MESSAGEBOX->SetAutoClose( 0 );
	GetCharacterScreen()->UpdateScreen( SCREEN_Login );
}

void CCharacterScreenWindowHandler::UpdateButtonsMainbox( bool bShowCreateButton, bool bShowSelectButton, bool bShowBackButton, bool bShowExitButton )
{	
	auto pButtonCreate = pWindow->GetElement<UI::Button>( BUTTON_Create );
	auto pButtonSelect = pWindow->GetElement<UI::Button>( BUTTON_Select );
	auto pButtonBack = pWindow->GetElement<UI::Button>( BUTTON_Back );
	auto pButtonExit = pWindow->GetElement<UI::Button>( BUTTON_Exit );

	if ( bShowCreateButton )
		pButtonCreate->Show();
	else
		pButtonCreate->Hide();

	if ( bShowSelectButton )
		pButtonSelect->Show();
	else
		pButtonSelect->Hide();

	if ( bShowBackButton )
		pButtonBack->Show();
	else
		pButtonBack->Hide();

	if ( bShowExitButton )
		pButtonExit->Show();
	else
		pButtonExit->Hide();

	UpdateButtonsPosition();
}

void CCharacterScreenWindowHandler::UpdateButtonsPosition()
{
	int iWidth = 0;

	auto pButtonCreate = pWindow->GetElement<UI::Button>( BUTTON_Create );
	auto pButtonSelect = pWindow->GetElement<UI::Button>( BUTTON_Select );
	auto pButtonBack = pWindow->GetElement<UI::Button>( BUTTON_Back );
	auto pButtonExit = pWindow->GetElement<UI::Button>( BUTTON_Exit );
	auto pButtonDelete = pWindow->GetElement<UI::Button>( BUTTON_Delete );

	if ( pButtonCreate->IsOpen() )
	{
		iWidth += pButtonCreate->GetWidth();
	}

	if ( pButtonSelect->IsOpen() )
	{
		iWidth += pButtonSelect->GetWidth();
	}
	
	if ( pButtonBack->IsOpen() )
	{
		iWidth += pButtonBack->GetWidth();
	}

	if ( pButtonExit->IsOpen() )
	{
		iWidth += pButtonExit->GetWidth();
	}

	int iX = (RESOLUTION_WIDTH >> 1) - (iWidth >> 1);
	int iY = (RESOLUTION_HEIGHT - 60);

	if ( pButtonCreate->IsOpen() )
	{
		pButtonCreate->SetPosition( iX, iY );
		iX += pButtonCreate->GetWidth();
	}

	if ( pButtonSelect->IsOpen() )
	{
		pButtonSelect->SetPosition( iX, iY );
		iX += pButtonSelect->GetWidth();
	}
	
	if ( pButtonBack->IsOpen() )
	{
		pButtonBack->SetPosition( iX, iY );
		iX += pButtonBack->GetWidth();
	}

	if ( pButtonExit->IsOpen() )
	{
		pButtonExit->SetPosition( iX, iY );
	}

	pButtonDelete->SetPosition( 0, iY );
}

void CCharacterScreenWindowHandler::ShowTitle( int iTitleID )
{
	auto pWindowTitlebox = pWindow->GetElement<UI::Window>( WINDOW_Titlebox );
	
	for ( auto pElement : pWindowTitlebox->GetElements() )
	{
		if ( pElement->GetID() == iTitleID )
		{
			pElement->Show();
		}
		else
		{
			pElement->Hide();
		}
	}
}

void CCharacterScreenWindowHandler::OnDeleteButtonConfirm()
{
	if ( MESSAGEBOX->GetType() )
	{
		GetCharacterScreen()->GetModel()->DeleteSelectedCharacter();
	}
}

std::string CCharacterScreenWindowHandler::GetImagePath( const std::string & strImageName )
{
	return "game\\images\\chrselect\\" + strImageName;
}
