#include "stdafx.h"
#include "CCharacterScreen.h"

PacketCharacterSelectUser CCharacterScreen::sPacketCharactersUser;

CCharacterScreen::CCharacterScreen()
{
	pcCharacterScreenModel = new CCharacterScreenModel();
	pcWindow = new CCharacterScreenWindowHandler();
}


CCharacterScreen::~CCharacterScreen()
{
//	EFFEKSEERHANDLER->Kill( PARTICLEID_CHARACTERSELECT_FIRE );

	DELET( pcCharacterScreenModel );
}

const X3D::Vector3 saParticlesPosition[] =
{
	{-115.395f, 35.426f, -31.902f},
	{-115.832f, 35.426f, -99.014f},
	{115.598f, 35.426f, -99.014f},
	{116.035f, 35.426f, -31.902f}
};

BOOL CCharacterScreen::Init()
{
	iCurrentScreen = SCREEN_Character;

	RENDERER->iMultPerspectiveHeight = 684;

	Sky::GetInstance()->SetSky( 128 );

	//Add Fires
	{	
		for ( int i = 0; i < _countof( saParticlesPosition ); i++ )
		{
			if ( auto pcScript = EFFEKSEERHANDLER->LoadScript( FormatString( "game\\scripts\\particles\\FireCharacterSelect%d.efk", i + 1 ) ) )
			{
				pcScript->SetID( PARTICLEID_CHARACTERSELECT_FIRE + i );

				auto pcOwner = new Engine::ObjectOwnerPosition( Point3D( (int)(saParticlesPosition[i].x * 256.0f), (int)(saParticlesPosition[i].y * 256.0f), (int)(saParticlesPosition[i].z * 256.0f) ) );
				pcScript->SetOwner( pcOwner );

				EFFEKSEERHANDLER->AddScript( pcScript );
			}
		}
	}


	pcCharacterScreenModel->Init();
	pcWindow->Init();

	for ( int i = 0; i < sPacketCharactersUser.iCount; i++ )
	{
		pcCharacterScreenModel->AddUserCharacter( sPacketCharactersUser.saData[i].szName, sPacketCharactersUser.saData[i].szBodyModel,
			sPacketCharactersUser.saData[i].szHeadModel, sPacketCharactersUser.saData[i].iClass, sPacketCharactersUser.saData[i].iLevel, 0,
			(EItemID)sPacketCharactersUser.saData[i].iWeaponID, (EItemID)sPacketCharactersUser.saData[i].iShieldID );
	}
	
	CSound::PlayMusic( 14 );

	return TRUE;
}

void CCharacterScreen::Shutdown()
{
	for ( int i = 0; i < _countof( saParticlesPosition ); i++ )
		EFFEKSEERHANDLER->Kill( PARTICLEID_CHARACTERSELECT_FIRE + i );

	pcCharacterScreenModel->Shutdown();

	//Clan
	CALL( 0x0065B690 );
}

void CCharacterScreen::Render3D()
{
	pcCharacterScreenModel->Render3D();
	pcWindow->Render();

	Effekseer::Matrix44 mV;
	CopyMemory( &mV, &GRAPHICS->GetRenderer()->GetCamera()->View(), sizeof( Effekseer::Matrix44 ) );
	Effekseer::Matrix44 mP;
	CopyMemory( &mP, &GRAPHICS->GetRenderer()->GetCamera()->Projection(), sizeof( Effekseer::Matrix44 ) );

	EFFEKSEERHANDLER->GetRenderer()->SetProjectionMatrix( (Effekseer::Matrix44 &)GRAPHICENGINE->GetCamera()->Projection() );

	EFFEKSEERHANDLER->GetRenderer()->SetCameraMatrix( (Effekseer::Matrix44 &)GRAPHICENGINE->GetCamera()->View() );
}

void CCharacterScreen::Render2D()
{
}

EScreen CCharacterScreen::Frame()
{
	//Keep
	pcCharacterScreenModel->Frame();
	pcCharacterScreenModel->Update( 1000.0f / 60.0f );

	//New, Keep
	if ( bLoadingTime )
	{
		if ( (dwLoadingTime + 100) < TICKCOUNT )
			iCurrentScreen = SCREEN_World;
	}
	

	return iCurrentScreen;
}

void CCharacterScreen::OnMouseMove( CMouse * pcMouse )
{
	pcWindow->OnMouseMove( pcMouse );
}

BOOL CCharacterScreen::OnMouseClick( CMouse * pcMouse )
{
	//Old
	//return FALSE;

	BOOL bRet = pcWindow->OnMouseClick( pcMouse );
	if ( bRet == FALSE )
	{
		if ( (pcMouse->GetEvent() == EMouseEvent::ClickDownL) || (pcMouse->GetEvent() == EMouseEvent::DoubleClickL) )
		{
			if ( pcCharacterScreenModel->CanSelectUserCharacters() )
			{
				if ( pcCharacterScreenModel->GetCharacterLocked() == false )
				{
					UnitData * pc = pcCharacterScreenModel->GetUserCharacterMouseOver( pcMouse->GetPosition() );
					if ( pc )
					{
						pcCharacterScreenModel->SetCharacter( pc );

						SelectCharacter( pcMouse->GetEvent() == EMouseEvent::DoubleClickL );

						bRet = TRUE;
					}
					else
					{
						pcCharacterScreenModel->ResetCharacterSelect();
					}
				}
			}
			else if ( pcCharacterScreenModel->CanSelectCreateCharacter() )
			{
				UnitData * pc = pcCharacterScreenModel->GetCreateCharacterMouseOver( pcMouse->GetPosition() );
				if ( pc )
				{
					pcCharacterScreenModel->SetCreateCharacter( pc );

					SetCharacterScreenStep( CHARACTERSCREENSTEP_NewCharacterHair );

					bRet = TRUE;
				}
			}
		}
	}

	return bRet;
}

BOOL CCharacterScreen::OnKeyPress( CKeyboard * pcKeyboard )
{
	return pcWindow->OnKeyPress( pcKeyboard );
}

BOOL CCharacterScreen::OnKeyChar( CKeyboard * pcKeyboard )
{
	return pcWindow->OnKeyChar( pcKeyboard );
}

void CCharacterScreen::OnResolutionChanged()
{
	pcWindow->OnResolutionChanged();
}

void CCharacterScreen::UpdateScreen( EScreen iScreen )
{
	iCurrentScreen = iScreen;
	if ( iCurrentScreen == SCREEN_Login )
		Disconnect();
}

void CCharacterScreen::HandlePacket( PacketCharacterSelectUser * psPacket )
{
	CopyMemory( &sPacketCharactersUser, psPacket, sizeof( PacketCharacterSelectUser ) );
}

CharacterSelectPacketData & CCharacterScreen::GetCharacterData( const char * pszName )
{
	CharacterSelectPacketData & s = CCharacterScreen::sPacketCharactersUser.saData[0];

	for ( int i = 0; i < sPacketCharactersUser.iCount; i++ )
	{
		if ( STRINGCOMPARE( sPacketCharactersUser.saData[i].szName, pszName ) )
		{
			s = sPacketCharactersUser.saData[i];

			break;
		}
	}

	return s;
}

void CCharacterScreen::SelectCharacter( bool bLock )
{
	if ( pcCharacterScreenModel->HasSelectedCharacter() == false )
	{
		TITLEBOX( "Please select a character" );
		return;
	}

	if ( bLock )
	{
		pcCharacterScreenModel->SetCharacterLocked( true );

		MESSAGEBOX->SetBoxType( CMessageBox::BOXTYPEID_YesNo );
		MESSAGEBOX->SetTitle( "Character" );
		MESSAGEBOX->SetDescription( "Do you want to select this character?" );
		MESSAGEBOX->SetEvent( std::bind( &CCharacterScreen::OnSelectedCharacterBox, this ) );
		MESSAGEBOX->SetAutoClose( 10 );
		MESSAGEBOX->Show();

		PLAYMINISOUND( 11 );
	}
	else
		pcCharacterScreenModel->SetCharacterLocked( false );
}

void CCharacterScreen::CreateNewCharacter( UnitData * pcUnitData )
{
	pcWindow->SetTemporaryBlockActions( 30000.0f );

	STRINGCOPYLEN( CHARACTER_SELECT_NAME, 32, pcUnitData->sCharacterData.szName );
	STRINGCOPYLEN( CHARACTER_SELECT_BODYMODEL, 64, pcWindow->GetCharacterInitialBodyPath( pcUnitData->sCharacterData.iClass ).c_str() );
	STRINGCOPYLEN( CHARACTER_SELECT_HEADMODEL, 64, pcUnitData->sCharacterData.Player.szHeadModel );
	
	NEW_CHARACTER_DATA->iClass = pcUnitData->sCharacterData.iClass;
	
	const auto sCharacterStatusBase = GetCharacterStatusBaseData( pcUnitData->sCharacterData.iClass );
	NEW_CHARACTER_DATA->iStrength = sCharacterStatusBase.iStrength;
	NEW_CHARACTER_DATA->iSpirit = sCharacterStatusBase.iSpirit;
	NEW_CHARACTER_DATA->iTalent = sCharacterStatusBase.iTalent;
	NEW_CHARACTER_DATA->iAgility = sCharacterStatusBase.iAgility;
	NEW_CHARACTER_DATA->iHealth = sCharacterStatusBase.iHealth;

	CSound::PlaySoundID( ACTIONSOUND_SelectedChar );
	SOUNDHANDLER->UpdateVolume();

	dwLoadingTime = TICKCOUNT;
	bLoadingTime = TRUE;
}

void CCharacterScreen::HandlePacket( PacketCreateCharacter * psPacket )
{
	pcCharacterScreenModel->HandlePacket( psPacket );
}


void CCharacterScreen::OnSelectedCharacterBox()
{
	if ( MESSAGEBOX->GetType() )
		SetCharacterUser( GetCharacterData( pcCharacterScreenModel->GetCharacter()->GetName() ) );
	else
	{
		pcCharacterScreenModel->SetCharacterLocked( false );

		CSound::PlaySoundID( ACTIONSOUND_Back );
		SOUNDHANDLER->UpdateVolume();
	}
}

void CCharacterScreen::SetCharacterUser( CharacterSelectPacketData & sData )
{
	pcWindow->SetTemporaryBlockActions( 30000.0f );

	STRINGCOPYLEN( CHARACTER_SELECT_NAME, 32, sData.szName );
	STRINGCOPYLEN( CHARACTER_SELECT_BODYMODEL, 64, sData.szBodyModel );
	STRINGCOPYLEN( CHARACTER_SELECT_HEADMODEL, 64, sData.szHeadModel );
	CHARACTER_SELECT_CLASS = sData.iClass;
	CHARACTER_SELECT_MAPID = sData.iMapID;
	CHARACTER_SELECT_X = sData.iX;
	CHARACTER_SELECT_Z = sData.iZ;

	CSound::PlaySoundID( ACTIONSOUND_SelectedChar );
	SOUNDHANDLER->UpdateVolume();

	dwLoadingTime = TICKCOUNT;
	bLoadingTime = TRUE;
}

void CCharacterScreen::Disconnect()
{
	CALL( 0x0061E9B0 );
}
