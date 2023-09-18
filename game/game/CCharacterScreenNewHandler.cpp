#include "stdafx.h"
#include "CCharacterScreenNewHandler.h"

const std::vector<CharacterSelectNewCharacterData> CCharacterScreenNewHandler::vNewCharacterData =
{
	{
		CHARACTERCLASS_Fighter, CHARACTERRACE_Tempskron, 
		{
			{"B015.ini", "B001.ini", "WA114", "DS114", {"tmh-b01.inf", "tmh-b02.inf", "tmh-b03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Mechanician, CHARACTERRACE_Tempskron,
		{
			{"A014.ini", "A001.ini", "WC115", "DS114", {"tmh-a01.inf", "tmh-a02.inf", "tmh-a03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Archer, CHARACTERRACE_Tempskron,
		{
			{"D015.ini", "D001.ini", "WS115", "", {"tfh-D01.inf", "tfh-D02.inf", "tfh-D03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Pikeman, CHARACTERRACE_Tempskron,
		{
			{"C015.ini", "C001.ini", "WP115", "", {"tmh-C01.inf", "tmh-C02.inf", "tmh-C03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Atalanta, CHARACTERRACE_Morion,
		{
			{"MB015.ini", "MB001.ini", "WT115", "DS115", {"Mfh-B01.inf", "Mfh-B02.inf", "Mfh-B03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Knight, CHARACTERRACE_Morion,
		{
			{"MA014.ini", "MA001.ini", "WS217", "DS115", {"Mmh-A01.inf", "Mmh-A02.inf", "Mmh-A03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Magician, CHARACTERRACE_Morion,
		{
			{"MD015.ini", "MD001.ini", "WM116", "", {"Mmh-D01.inf", "Mmh-D02.inf", "Mmh-D03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Priestess, CHARACTERRACE_Morion,
		{
			{"MC015.ini", "MC001.ini", "WM111", "", {"Mfh-C01.inf", "Mfh-C02.inf", "Mfh-C03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Assassin, CHARACTERRACE_Tempskron,
		{
			{"E015.ini", "E001.ini", "WD112", "", {"tfh-E01.inf", "tfh-E02.inf", "tfh-E03.inf"}},
		}
	},
	{
		CHARACTERCLASS_Shaman, CHARACTERRACE_Morion,
		{
			{"ME015.ini", "ME001.ini", "WN112", "", {"Mmh-E01.inf", "Mmh-E02.inf", "Mmh-E03.inf"}},
		}
	},
	/*{
		CHARACTERCLASS_Brawler, CHARACTERRACE_Tempskron,
		{
			{"F015.ini", "WB112", "", {"tfh-F01.inf", "tfh-F02.inf", "tfh-F03.inf"}},
		}
	},*/

};


CCharacterScreenNewHandler::CCharacterScreenNewHandler()
{
	pWindow = std::make_shared<UI::Window>( Rectangle2D( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT ) );
}

CCharacterScreenNewHandler::~CCharacterScreenNewHandler()
{
}

void CCharacterScreenNewHandler::Init()
{
	UI::Window_ptr pWindowMainbox = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 209, 336 ) );
	pWindowMainbox->SetID( WINDOW_Mainbox );
	pWindowMainbox->SetImage( UI::ImageLoader::LoadImage( GetImagePath( "box_char.png" ).c_str() ) );
	pWindow->AddElement( pWindowMainbox );

	UI::UnitMesh_ptr pUnitMainbox = std::make_shared<UI::UnitMesh>( Rectangle2D( 4, 40, 201, 264 ) );
	pUnitMainbox->SetID( UNITMESH_Mainbox );
	pWindowMainbox->AddElement( pUnitMainbox );

	UI::Group_ptr pGroupInput = std::make_shared<UI::Group>();
	pWindow->AddElement( pGroupInput );

	UI::InputField_ptr pInputName = std::make_shared<UI::InputField>( Rectangle2D( 60, 309, 145, 23 ) );
	pInputName->SetID( INPUT_Name );
	pInputName->SetAllowedCharacters( ALLOWED_CHARS_NICK );
	pInputName->SetFont( "Arial", 16, 0, FALSE, FALSE, -1 );
	pInputName->SetMaxLength( 15 );
	pInputName->SetTextAlign( ALIGN_Left, ALIGN_Middle );
	pInputName->SetGroup( pGroupInput );
	pInputName->SetSelect( TRUE );
	pInputName->SetSelectedAlways( TRUE );
	pWindowMainbox->AddElement( pInputName );

	UI::Group_ptr pGroupHair = std::make_shared<UI::Group>();
	pWindow->AddElement( pGroupHair );

	UI::Window_ptr pWindowHair1 = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 128, 128 ) );
	pWindowHair1->SetID( WINDOW_Hair1 );
	pWindow->AddElement( pWindowHair1 );

	UI::Button_ptr pButtonHair1 = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 128 ) );
	pButtonHair1->SetID( BUTTON_Hair1 );
	pButtonHair1->SetActiveImage( UI::ImageLoader::LoadActiveImage( GetImagePath( "headbox.png" ).c_str(), GetImagePath( "headbox_.png" ).c_str() ) );
	pButtonHair1->SetGroup( pGroupHair );
	pButtonHair1->SetSelect( TRUE );
	pButtonHair1->SetSelected( TRUE );
	pButtonHair1->SetEvent(UI::Event::Build( std::bind( &CCharacterScreenNewHandler::OnButtonSelectHairClick, this, 0, std::placeholders::_1 ) ) );
	pWindowHair1->AddElement( pButtonHair1 );

	UI::UnitMesh_ptr pMeshHair1 = std::make_shared<UI::UnitMesh>( Rectangle2D( 11, 6, 117, 117 ) );
	pMeshHair1->SetID( UNITMESH_Hair1 );
	pWindowHair1->AddElement( pMeshHair1 );
	
	UI::Window_ptr pWindowHair2 = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 128, 128 ) );
	pWindowHair2->SetID( WINDOW_Hair2 );
	pWindow->AddElement( pWindowHair2 );

	UI::Button_ptr pButtonHair2 = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 128 ) );
	pButtonHair2->SetID( BUTTON_Hair2 );
	pButtonHair2->SetActiveImage( UI::ImageLoader::LoadActiveImage( GetImagePath( "headbox.png" ).c_str(), GetImagePath( "headbox_.png " ).c_str() ) );
	pButtonHair2->SetGroup( pGroupHair );
	pButtonHair2->SetSelect( TRUE );
	pButtonHair2->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenNewHandler::OnButtonSelectHairClick, this, 1, std::placeholders::_1 ) ) );
	pWindowHair2->AddElement( pButtonHair2 );

	UI::UnitMesh_ptr pMeshHair2 = std::make_shared<UI::UnitMesh>( Rectangle2D( 11, 6, 117, 117 ) );
	pMeshHair2->SetID( UNITMESH_Hair2 );
	pWindowHair2->AddElement( pMeshHair2 );

	UI::Window_ptr pWindowHair3 = std::make_shared<UI::Window>( Rectangle2D( 0, 0, 128, 128 ) );
	pWindowHair3->SetID( WINDOW_Hair3 );
	pWindow->AddElement( pWindowHair3 );

	UI::Button_ptr pButtonHair3 = std::make_shared<UI::Button>( Rectangle2D( 0, 0, 128, 128 ) );
	pButtonHair3->SetID( BUTTON_Hair3 );
	pButtonHair3->SetActiveImage( UI::ImageLoader::LoadActiveImage( GetImagePath( "headbox.png" ).c_str(), GetImagePath( "headbox_.png " ).c_str() ) );
	pButtonHair3->SetGroup( pGroupHair );
	pButtonHair3->SetSelect( TRUE );
	pButtonHair3->SetEvent( UI::Event::Build( std::bind( &CCharacterScreenNewHandler::OnButtonSelectHairClick, this, 2, std::placeholders::_1 ) ) );
	pWindowHair3->AddElement( pButtonHair3 );

	UI::UnitMesh_ptr pMeshHair3 = std::make_shared<UI::UnitMesh>( Rectangle2D( 11, 6, 117, 117 ) );
	pMeshHair3->SetID( UNITMESH_Hair3 );
	pWindowHair3->AddElement( pMeshHair3 );

	OnResolutionChanged();

	Close();
}

BOOL CCharacterScreenNewHandler::Shutdown()
{
	return 0;
}

void CCharacterScreenNewHandler::Open()
{
	pWindow->Show();
}

void CCharacterScreenNewHandler::Close()
{
	pWindow->Hide();
}

void CCharacterScreenNewHandler::Render()
{
	pWindow->Render( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

void CCharacterScreenNewHandler::Update( float fTime )
{
	pWindow->Update( fTime );
}

BOOL CCharacterScreenNewHandler::OnMouseClick( CMouse * pcMouse )
{
	return pWindow->OnMouseClick( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

void CCharacterScreenNewHandler::OnMouseMove( CMouse * pcMouse )
{
	pWindow->OnMouseMove( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

BOOL CCharacterScreenNewHandler::OnKeyChar( CKeyboard * pcKeyboard )
{
	return pWindow->OnKeyChar( pcKeyboard );
}

BOOL CCharacterScreenNewHandler::OnKeyPress( CKeyboard * pcKeyboard )
{
	if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterHair )
	{
		if ( MESSAGEBOX->IsOpen() == FALSE )
		{
			if ( (pcKeyboard->GetEvent() == KeyDown) && (pcKeyboard->GetKey() == VK_RETURN) )
			{
				ProcessCreateCharacter();
				return TRUE;
			}
		}
	}

	return pWindow->OnKeyPress( pcKeyboard );
}

void CCharacterScreenNewHandler::UpdateNewCharacters( ECharacterScreenStep iStep )
{
	switch ( iStep )
	{
		case CHARACTERSCREENSTEP_NewCharacterTribe:
			GetCharacterScreen()->GetModel()->ClearNewCharacters();
			
			ResetCharacterHair();

			bBlockNewCharactersCreate = false;
			break;

		case CHARACTERSCREENSTEP_NewCharacterTempskronClass:
			SetNewCharacters( CHARACTERRACE_Tempskron );

			Close();
			break;
			
		case CHARACTERSCREENSTEP_NewCharacterMorionClass:
			SetNewCharacters( CHARACTERRACE_Morion );

			Close();
			break;

		case CHARACTERSCREENSTEP_NewCharacterHair:
			Open();
			
			OnSelectCreateCharacter( GetCharacterScreen()->GetModel()->GetCreateCharacter() );
			break;
			
		default:
			break;
	}
}

void CCharacterScreenNewHandler::OnSelectCreateCharacter( UnitData * pcUnitData )
{
	auto pUnitMeshMainbox = pWindow->GetElement<UI::Window>( WINDOW_Mainbox )->GetElement<UI::UnitMesh>( UNITMESH_Mainbox );
	auto pUnitMeshHair1 = pWindow->GetElement<UI::Window>( WINDOW_Hair1 )->GetElement<UI::UnitMesh>( UNITMESH_Hair1 );
	auto pUnitMeshHair2 = pWindow->GetElement<UI::Window>( WINDOW_Hair2 )->GetElement<UI::UnitMesh>( UNITMESH_Hair2 );
	auto pUnitMeshHair3 = pWindow->GetElement<UI::Window>( WINDOW_Hair3 )->GetElement<UI::UnitMesh>( UNITMESH_Hair3 );
	auto pInputName = pWindow->GetElement<UI::Window>( WINDOW_Mainbox )->GetElement<UI::InputField>( INPUT_Name );

	RENDERER->SetUseNewRenderToLoad( true );

	pUnitMeshMainbox->FreeUnit();
	pUnitMeshHair1->FreeUnit();
	pUnitMeshHair2->FreeUnit();
	pUnitMeshHair3->FreeUnit();


	pUnitMeshMainbox->PrepareUnitData( pcUnitData );
	pUnitMeshHair1->PrepareUnitData( pcUnitData );
	pUnitMeshHair2->PrepareUnitData( pcUnitData );
	pUnitMeshHair3->PrepareUnitData( pcUnitData );

	const auto & sNewCharacterData = GetCharacterData( pcUnitData->sCharacterData.iClass );

	EItemID eWeaponID = ITEMID_None;
	if ( sNewCharacterData.vModelData[0].strWeaponCode.length() > 0 )
	{
		if ( auto pcItemData = ITEMHANDLER->FindItemTableByCode( sNewCharacterData.vModelData[0].strWeaponCode ) )
		{
			eWeaponID = pcItemData->sBaseItemID.ToItemID();
		}
	}

	EItemID eShieldID = ITEMID_None;
	if ( sNewCharacterData.vModelData[0].strShieldCode.length() > 0 )
	{
		if ( auto pcItemData = ITEMHANDLER->FindItemTableByCode( sNewCharacterData.vModelData[0].strShieldCode ) )
		{
			eShieldID = pcItemData->sBaseItemID.ToItemID();
		}
	}

	pUnitMeshMainbox->EquipShield( eShieldID );
	pUnitMeshMainbox->EquipWeapon( eWeaponID );

	RENDERER->ResetUseNewRenderToLoad();

	pUnitMeshHair1->SetHeadModel( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[0] ).c_str() );
	pUnitMeshHair2->SetHeadModel( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[1] ).c_str() );
	pUnitMeshHair3->SetHeadModel( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[2] ).c_str() );

	pInputName->ClearInput();

	auto pButtonHair1 = pWindow->GetElement<UI::Window>( WINDOW_Hair1 )->GetElement<UI::Button>( BUTTON_Hair1 );
	pButtonHair1->SetSelected( TRUE );
}

void CCharacterScreenNewHandler::ProcessCreateCharacter()
{
	auto pInputName = pWindow->GetElement<UI::Window>( WINDOW_Mainbox )->GetElement<UI::InputField>( INPUT_Name );

	if ( pInputName->GetText().length() == 0 )
	{
		TITLEBOX( "Please enter a Character Name" );
		CSound::PlaySoundID( ACTIONSOUND_Back );
		SOUNDHANDLER->UpdateVolume();
		return;
	}

	MESSAGEBOX->SetBoxType( CMessageBox::BOXTYPEID_YesNo );
	MESSAGEBOX->SetTitle( "New Character" );
	MESSAGEBOX->SetDescription( "Do you want to create a new character?" );
	MESSAGEBOX->SetEvent( std::bind( &CCharacterScreenNewHandler::OnCallbackProcessCreateCharacter, this ) );
	MESSAGEBOX->SetAutoClose( 10 );
	MESSAGEBOX->Show();

	PLAYMINISOUND( 11 );
}

void CCharacterScreenNewHandler::OnResolutionChanged()
{
	pWindow->SetBox( Rectangle2D( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT ) );
	
	auto pWindowMainBox = pWindow->GetElement<UI::Window>( WINDOW_Mainbox );
	pWindowMainBox->SetPosition( (pWindow->GetWidth() >> 1) - (pWindowMainBox->GetWidth() >> 1), (pWindow->GetHeight() >> 2) - (pWindowMainBox->GetHeight() >> 2) + 30 );
	
	auto pWindowHair1 = pWindow->GetElement<UI::Window>( WINDOW_Hair1 );
	auto pWindowHair2 = pWindow->GetElement<UI::Window>( WINDOW_Hair2 );
	auto pWindowHair3 = pWindow->GetElement<UI::Window>( WINDOW_Hair3 );

	int iHairWindowY = (pWindow->GetHeight() >> 1) - (((pWindowHair1->GetHeight() + 10) * 3) >> 1);
	
	//Set on Right Side
	pWindowHair1->SetPosition( pWindow->GetWidth() - pWindowHair1->GetWidth() - 5, iHairWindowY );
	iHairWindowY += pWindowHair1->GetHeight() + 10;
	pWindowHair2->SetPosition( pWindow->GetWidth() - pWindowHair2->GetWidth() - 5, iHairWindowY );
	iHairWindowY += pWindowHair2->GetHeight() + 10;
	pWindowHair3->SetPosition( pWindow->GetWidth() - pWindowHair3->GetWidth() - 5, iHairWindowY );
}

const std::string CCharacterScreenNewHandler::GetCharacterInitialModelPath( ECharacterClass eCharacterClass )
{
	for ( const auto & sNewCharacter : vNewCharacterData )
	{
		if ( sNewCharacter.iClass == eCharacterClass )
		{
			return GetCharacterModelPath( sNewCharacter.vModelData[0].strInitialBodyModelName );
		}
	}

	return GetCharacterModelPath( vNewCharacterData[0].vModelData[0].strInitialBodyModelName );
}

void CCharacterScreenNewHandler::OnButtonSelectHairClick( int iHairID, UIEventArgs eArgs )
{
	const auto & sNewCharacterData = GetCharacterData( GetCharacterScreen()->GetModel()->GetCreateCharacter()->sCharacterData.iClass );

	auto pUnitMainbox = pWindow->GetElement<UI::Window>( WINDOW_Mainbox )->GetElement<UI::UnitMesh>( UNITMESH_Mainbox );

	pUnitMainbox->SetHeadModel( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[iHairID] ).c_str() );

	GetCharacterScreen()->GetModel()->SetHairCharacterCreate( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[iHairID] ) );
}

void CCharacterScreenNewHandler::ResetCharacterHair()
{
	if ( GetCharacterScreen()->GetModel()->GetCreateCharacter() )
	{
		const auto & sNewCharacterData = GetCharacterData( GetCharacterScreen()->GetModel()->GetCreateCharacter()->sCharacterData.iClass );

		GetCharacterScreen()->GetModel()->SetHairCharacterCreate( GetCharacterModelPath( sNewCharacterData.vModelData[0].vHairModelNames[0] ) );
	}
}

void CCharacterScreenNewHandler::SetNewCharacters( ECharacterRace iRace )
{
	if ( bBlockNewCharactersCreate == true )
		return;

	for ( const auto & sNewCharacter : vNewCharacterData )
	{
		if ( sNewCharacter.iRace == iRace )
		{
			EItemID eWeaponID = ITEMID_None;		
			if ( sNewCharacter.vModelData[0].strWeaponCode.length() > 0 )
			{
				if ( auto pcItemData = ITEMHANDLER->FindItemTableByCode( sNewCharacter.vModelData[0].strWeaponCode ) )
				{
					eWeaponID = pcItemData->sBaseItemID.ToItemID();
				}
			}

			EItemID eShieldID = ITEMID_None;
			if ( sNewCharacter.vModelData[0].strShieldCode.length() > 0 )
			{
				if ( auto pcItemData = ITEMHANDLER->FindItemTableByCode( sNewCharacter.vModelData[0].strShieldCode ) )
				{
					eShieldID = pcItemData->sBaseItemID.ToItemID();
				}
			}

			GetCharacterScreen()->GetModel()->AddNewCharacter( GetCharacterClassByFlag( CharacterClassToClassFlag(sNewCharacter.iClass)), 
				GetCharacterModelPath( sNewCharacter.vModelData[0].strBodyModelName ).c_str(), GetCharacterModelPath( sNewCharacter.vModelData[0].vHairModelNames[0] ).c_str(),
				sNewCharacter.iClass,
				eWeaponID, eShieldID );
		}
	}

	bBlockNewCharactersCreate = true;
}

const std::string CCharacterScreenNewHandler::GetCharacterModelPath( const std::string strFileName )
{
	return std::string( "char\\tmABCD\\" + strFileName );
}

CCharacterScreen * CCharacterScreenNewHandler::GetCharacterScreen()
{
	return ((CCharacterScreen*)GAMESCREEN->GetActiveScreen());
}

const std::string CCharacterScreenNewHandler::GetImagePath( const std::string strFileName )
{
	return GetCharacterScreen()->GetWindow()->GetImagePath( strFileName );
}

void CCharacterScreenNewHandler::OnCallbackProcessCreateCharacter()
{
	if ( MESSAGEBOX->GetType() )
	{
		auto pInputName = pWindow->GetElement<UI::Window>( WINDOW_Mainbox )->GetElement<UI::InputField>( INPUT_Name );

		if ( pInputName->GetText().length() == 0 )
		{
			TITLEBOX( "Please enter a Character Name" );
			CSound::PlaySoundID( ACTIONSOUND_Back );
			SOUNDHANDLER->UpdateVolume();
			return;
		}
		
		GetCharacterScreen()->GetModel()->CreateNewCharacter( pInputName->GetText() );
	}
}

const CharacterSelectNewCharacterData & CCharacterScreenNewHandler::GetCharacterData( ECharacterClass iClass )
{
	for ( const auto & sNewCharacterData : vNewCharacterData )
	{
		if ( sNewCharacterData.iClass == iClass )
			return sNewCharacterData;
	}

	return vNewCharacterData[0];
}
