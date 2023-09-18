#include "stdafx.h"
#include "CCharacterScreenModel.h"

static const Point3D saFireParticles[] =
{
	Point3D( -117 << 8, 35 << 8, -98 << 8 ),
	Point3D( -117 << 8, 35 << 8, -30 << 8 ),
	Point3D( 117 << 8, 35 << 8, -98 << 8 ),
	Point3D( 117 << 8, 35 << 8, -30 << 8 )
};


CCharacterScreenModel::CCharacterScreenModel()
{
	sCameraPosition = CAMERA_DEFAULT_POSITION;
	sCameraAngle		= CAMERA_DEFAULT_ANGLE;

	sNextCameraPosition = sCameraPosition;

	sUserCharactersPosition = CHARACTER_USER_POSITION;
	sCreateCharactersPosition = CHARACTER_CREATE_POSITION;

	sLookAt = CAMERA_DEFAULT_LOOKAT;
}


CCharacterScreenModel::~CCharacterScreenModel()
{
	ClearUserCharacters();

	for ( auto pc : vImageClasses )
	{
		CLEARPTR( pc );
		UI::ImageLoader::DeleteImage( pc );
	}

	for ( auto pc : vImageClassesText )
	{
		CLEARPTR( pc );
		UI::ImageLoader::DeleteImage( pc );
	}

	DELET( pcStage );

	CLEARPTR( pcImageBox );
	UI::ImageLoader::DeleteImage( pcImageBox );

	CLEARPTR( pcImageTempskron );
	UI::ImageLoader::DeleteImage( pcImageTempskron );
	
	CLEARPTR( pcImageMorion );
	UI::ImageLoader::DeleteImage( pcImageMorion );
}

void CCharacterScreenModel::Init()
{
	pcStage = Stage::LoadStage( "game\\maps\\chrselect\\select.smd" );
	pcStage->bState = TRUE;

	for ( int i = 0; i < _countof( saFireParticles ); i++ )
	{
		CParticleScript * pc = PARTICLEFACTORY->LoadScript( "game\\scripts\\particles\\CharacterSelectFire.lua" );
		pc->SetOwner( new Engine::ObjectOwnerPosition( saFireParticles[i] ) );
		PARTICLEENGINE->AddScript( pc );
	}

	UpdateTextures();

	ClanWebStart();
	
	ClanMenuInit( 0 );

	for ( int i = 0; i < 11; i++ )
	{
		vImageClasses.push_back( UI::ImageLoader::LoadImage( GetCharacterScreen()->GetWindow()->GetImagePath( std::string( "T_cls-" + std::to_string( i + 1 ) + ".png" ) ).c_str() ) );

		vImageClassesText.push_back( UI::ImageLoader::LoadImage( GetCharacterScreen()->GetWindow()->GetImagePath( std::string( "T_chr-" + std::to_string( i + 1 ) + ".png" ) ).c_str() ) );
	}

	pcImageBox = UI::ImageLoader::LoadImage( GetCharacterScreen()->GetWindow()->GetImagePath( "box_main_info.png" ).c_str() );

	pcImageTempskron = UI::ImageLoader::LoadImage( GetCharacterScreen()->GetWindow()->GetImagePath( "mark_T.png" ).c_str() );
	
	pcImageMorion = UI::ImageLoader::LoadImage( GetCharacterScreen()->GetWindow()->GetImagePath( "mark_M.png" ).c_str() );
}

BOOL CCharacterScreenModel::Shutdown()
{
	MESSAGEBOX->ClearEvents();
	MESSAGEBOX->SetAutoClose( 0 );

	DELET( pcStage );

	return TRUE;
}

void CCharacterScreenModel::Render3D()
{
	RENDERER->ClearLight();
	RENDERER->r = 0;
	RENDERER->g = 0;
	RENDERER->b = 0;
	RENDERER->a = 0;

	GRAPHICS->GetRenderer()->SetAmbientColor( Math::Color( RENDERER->r - 30, RENDERER->g - 30, RENDERER->b - 30 ) );

	GRAPHICENGINE->GetCamera()->SetPosition( Math::Vector3( sCameraPosition.iX / 256.0f, sCameraPosition.iY / 256.0f, sCameraPosition.iZ / 256.0f ), Math::Vector3( sLookAt.iX / 256.0f, (sLookAt.iY / 256.0f), sLookAt.iZ / 256.0f ) );

	SetCharacterCamera( sCameraPosition.iX, sCameraPosition.iY, sCameraPosition.iZ, sCameraAngle.iX, sCameraAngle.iY, sCameraAngle.iZ );

	SetCamera( sCameraPosition, sLookAt );

	for ( int i = 0; i < _countof( saFireParticles ); i++ )
	{
		SetDynamicLight( saFireParticles[i].iX, saFireParticles[i].iY, saFireParticles[i].iZ, 200, 75, 0, 255, 15 );
	}

	SetDynamicLights();

	EXERender::SetRenderWithShaders( TRUE );

	if ( EXERender::IsRenderingWithShaders() )
	{
		if ( GRAPHICENGINE->GetTerrainShader() )
		{
			GRAPHICS->GetRenderer()->PrepareLights( GRAPHICENGINE->GetTerrainShader() );
			GRAPHICS->GetRenderer()->PrepareShadowMap( GRAPHICENGINE->GetTerrainShader() );

			GRAPHICENGINE->GetTerrainShader()->SetMatrix( "Projection", GRAPHICENGINE->GetCamera()->Projection().Get() );
			GRAPHICENGINE->GetTerrainShader()->SetTechnique( "Terrain" );
			GRAPHICENGINE->GetTerrainShader()->SetFloat( "FogEnd", 1500.f );
			GRAPHICENGINE->GetTerrainShader()->SetInt( "NumActiveLights", 0 );

			GRAPHICS->GetRenderer()->SetFogEnd( 1500.f );
		}
	}

	pcStage->Render( sCameraPosition.iX, sCameraPosition.iY, sCameraPosition.iZ, sCameraAngle.iX, sCameraAngle.iY, sCameraAngle.iZ );

	EXERender::SetRenderWithShaders( FALSE );


	PARTICLEENGINE->Render3D( NULL, sCameraPosition, sCameraAngle );

	EFFEKSEERHANDLER->Render();

	if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterTempskronClass )
	{
		UI::ImageRender::Render( pcImageTempskron, (RESOLUTION_WIDTH >> 1) - (pcImageTempskron->GetWidth() >> 1), 150, pcImageTempskron->GetWidth(), pcImageTempskron->GetHeight(), -1 );
	}
	else if ( GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterMorionClass )
	{
		UI::ImageRender::Render( pcImageMorion, (RESOLUTION_WIDTH >> 1) - (pcImageMorion->GetWidth() >> 1), 150, pcImageMorion->GetWidth(), pcImageMorion->GetHeight(), -1 );
	}
	
	RenderUserCharacters();

	RenderNewCharacters();

	//CALLT( 0x0047F200, (DWORD)RENDERER );

	DX::cSelectGlow1.Draw();
}

void CCharacterScreenModel::AddCameraAxis( Point3D sPosition )
{
	sCameraPosition += sPosition;
}

void CCharacterScreenModel::AddNextCameraAxis( Point3D sPosition )
{
	SetNextCamera( sNextCameraPosition + sPosition );
}

void CCharacterScreenModel::Frame()
{
	FrameUserCharacters();

	FrameNewCharacters();
}

void CCharacterScreenModel::Update( float fTime )
{
	UpdateCamera( fTime );

	UpdateUserCharacters( fTime );

	UpdateNewCharacters( fTime );
}

void CCharacterScreenModel::ClearUserCharacters()
{
	for ( auto & pc : vUserCharacters )
	{
		pc->Free();
		DELET( pc );
	}

	vUserCharacters.clear();
}

void CCharacterScreenModel::AddUserCharacter( const char * pszName, const char * pszModelFile, const char * pszHeadFile, ECharacterClass iClass, int iLevel, int iClanID, EItemID iItemRightHandID, EItemID iItemLeftHandID )
{
	UnitData * pcUnitData = new UnitData;

	pcUnitData->Init();
	pcUnitData->pcStage = pcStage;

	LoadUnitData( pcUnitData, pszModelFile, pszHeadFile, &sUserCharactersPosition, &Point3D( CHARACTER_DEFAULT_ANGLE ) );
	pcUnitData->eUpdateMode = UPDATEMODE_ClientSelf;


	STRINGCOPY( pcUnitData->sCharacterData.szName, pszName );
	pcUnitData->sCharacterData.iClanID = iClanID;
	pcUnitData->sCharacterData.iClass = iClass;
	pcUnitData->sCharacterData.iLevel = iLevel;
	pcUnitData->sCharacterData.sWarpHomeTown = 0;
	pcUnitData->sCharacterData.iType = CHARACTERTYPE_Player;

	pcUnitData->SetPosition( sUserCharactersPosition.iX, pcStage->GetHighestPoint( sUserCharactersPosition.iX, sUserCharactersPosition.iZ ), sUserCharactersPosition.iZ, 0, 0, 0 );
	pcUnitData->sAngle = CHARACTER_DEFAULT_ANGLE;

	pcUnitData->Animate( ANIMATIONTYPE_Idle );

	pcUnitData->ChangeAnimationID( 10 );

	pcUnitData->iClanInfoIndex = -1;

	pcUnitData->bActive = TRUE;
	pcUnitData->bVisible = TRUE;

	pcUnitData->sRightHandTool.pcBlendingMaterial = nullptr;
	pcUnitData->sLeftHandTool.pcBlendingMaterial = nullptr;

	ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 1 );
	ITEMHANDLER->EquipItem( pcUnitData, iItemLeftHandID, 2 );

	if ( iClass == CHARACTERCLASS_Assassin )
	{
		ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 1 );
		ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 2 );
	}

	vUserCharacters.push_back( pcUnitData );

	GetCharacterClan( ACCOUNT_NAME, pcUnitData->sCharacterData.szName );

	UpdateTextures();

	UpdateUserCharactersPosition();
}

void CCharacterScreenModel::ClearNewCharacters()
{
	for ( auto & pc : vCreateCharacters )
	{
		pc->Free();
		DELET( pc );
	}

	vCreateCharacters.clear();
}

void CCharacterScreenModel::AddNewCharacter( const char * pszName, const char * pszModelFile, const char * pszHeadFile, ECharacterClass iClass, EItemID iItemRightHandID, EItemID iItemLeftHandID )
{
	static int CHARACTER_ID_SELECT_WHEEL = 1500;

	UnitData * pcUnitData = new UnitData;

	pcUnitData->Init();
	pcUnitData->pcStage = pcStage;

	LoadUnitData( pcUnitData, pszModelFile, pszHeadFile, &sCreateCharactersPosition, &Point3D( CHARACTER_DEFAULT_ANGLE ) );
	pcUnitData->eUpdateMode = UPDATEMODE_ClientSelf;

	pcUnitData->iID = CHARACTER_ID_SELECT_WHEEL++;

	STRINGCOPY( pcUnitData->sCharacterData.szName, pszName );
	pcUnitData->sCharacterData.iClass = iClass;
	pcUnitData->sCharacterData.iLevel = 1;
	pcUnitData->sCharacterData.sWarpHomeTown = 0;
	pcUnitData->sCharacterData.iType = CHARACTERTYPE_Player;

	pcUnitData->SetPosition( sCreateCharactersPosition.iX, pcStage->GetHighestPoint( sCreateCharactersPosition.iX, sCreateCharactersPosition.iZ ), sCreateCharactersPosition.iZ, 0, 0, 0 );
	pcUnitData->sAngle = CHARACTER_DEFAULT_ANGLE;

	pcUnitData->Animate( ANIMATIONTYPE_Idle );

	pcUnitData->ChangeAnimationID( 10 );

	pcUnitData->iClanInfoIndex = -1;

	pcUnitData->bActive = TRUE;
	pcUnitData->bVisible = TRUE;

	pcUnitData->sRightHandTool.pcBlendingMaterial = nullptr;
	pcUnitData->sLeftHandTool.pcBlendingMaterial = nullptr;

	ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 1 );
	ITEMHANDLER->EquipItem( pcUnitData, iItemLeftHandID, 2 );

	if ( iClass == CHARACTERCLASS_Assassin )
	{
		ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 1 );
		ITEMHANDLER->EquipItem( pcUnitData, iItemRightHandID, 2 );
	}

	vCreateCharacters.push_back( pcUnitData );

	UpdateTextures();

	UpdateCreateCharactersPosition();
}

void CCharacterScreenModel::ResetCharacterSelect()
{
	pcCharacter = nullptr;

	fCharacterSelectTimeUpdate = 0.0f;

	GetCharacterScreen()->GetWindow()->UpdateDeleteButton( false );

	//Is Message Box open?
	if ( MESSAGEBOX->IsOpen() )
		MESSAGEBOX->SetAutoClose( 0 );

	SetCharacterLocked( false );
}

void CCharacterScreenModel::SetCharacter( UnitData * pcUnitData )
{
	if ( pcCharacter == nullptr )
		fCharacterSelectTimeUpdate = 0.0f;

	pcCharacter = pcUnitData;

	pcUnitData->iTransparencySelf = 0;

	GetCharacterScreen()->GetWindow()->UpdateDeleteButton( true );
}

void CCharacterScreenModel::ResetCreateCharacter()
{
	if ( pcCreateCharacter )
	{
		pcCreateCharacter->sPosition = sCharacterCreateOldPosition;
		pcCreateCharacter->sAngle = CHARACTER_DEFAULT_ANGLE;

		pcCreateCharacter->iMoveCount = 0;

		pcCreateCharacter->bRunMode = FALSE;
		pcCreateCharacter->bWalking = FALSE;

		pcCreateCharacter->Animate( ANIMATIONTYPE_Idle );

		pcCreateCharacter->ChangeAnimationID( 10 );
	}

	pcCreateCharacter = nullptr;
}

void CCharacterScreenModel::SetCreateCharacter( UnitData * pcUnitData )
{
	pcCreateCharacter = pcUnitData;

	sCharacterCreateOldPosition = pcUnitData->sPosition;

	MoveCharacter( pcUnitData, CHARACTER_CREATE_MOVE_POSITION );
}

void CCharacterScreenModel::SelectNewCharacter()
{
}

void CCharacterScreenModel::SetHairCharacterCreate( const std::string strHairModelFile )
{
	if ( UnitData * pcUnitData = GetCreateCharacter() )
	{
		pcUnitData->bNoMove = TRUE;
		STRINGCOPY( pcUnitData->sCharacterData.Player.szHeadModel, strHairModelFile.c_str() );
		SETPLAYERPATTERN( pcUnitData, pcUnitData->sCharacterData.Player.szBodyModel, pcUnitData->sCharacterData.Player.szHeadModel );
	}
}

bool CCharacterScreenModel::CanDeleteSelectedCharacter()
{
	if ( HasSelectedCharacter() )
		return GetCharacter()->iClanInfoIndex == -1;

	return true;
}

void CCharacterScreenModel::DeleteSelectedCharacter()
{
	if ( HasSelectedCharacter() )
	{
		for ( auto it = vUserCharacters.begin(); it != vUserCharacters.end(); )
		{
			auto pc = (*it);
			if ( HasSelectedCharacter() && (pc == GetCharacter()) )
			{
				ProcessPacket( &PacketDeleteCharacter( ACCOUNT_NAME, GetCharacter()->sCharacterData.szName ) );

				ResetCharacterSelect();

				pc->Free();

				DELET( pc );

				it = vUserCharacters.erase( it );
			}
			else
				it++;
		}
	}

	UpdateUserCharactersPosition();
}

void CCharacterScreenModel::MoveCharacter( UnitData * pcUnitData, Point3D sPosition )
{
	pcUnitData->sCharacterData.sSP.sCur = 1000;
	pcUnitData->sCharacterData.sSP.sMax = 1000;
	pcUnitData->sCharacterData.sWeight.sMax = 30000;
	pcUnitData->sCharacterData.sWeight.sMax = 30000;

	int iDistanceA = 0, iDistanceB = 0, iDistanceC = 0;
	pcUnitData->sPosition.GetPTDistanceXZ( &sPosition, iDistanceA, iDistanceB, iDistanceC );

	pcUnitData->SetTarget( sPosition.iX >> 8, sPosition.iZ >> 8 );
	pcUnitData->bRunMode = TRUE;
	pcUnitData->sCharacterData.iMovementSpeed = 1;
	pcUnitData->iMoveCount = (int)sqrt( iDistanceA );

	//pcUnitData->sMoveToPosition = sPosition;
}

bool CCharacterScreenModel::CanSelectUserCharacters()
{
	bool bRet = false;

	if ( (GetStep() == CHARACTERSCREENSTEP_None) && (GetNewStep() == CHARACTERSCREENSTEP_None) )
		bRet = true;

	return bRet;
}

bool CCharacterScreenModel::CanSelectCreateCharacter()
{
	return ((GetStep() == CHARACTERSCREENSTEP_NewCharacterMorionClass) && (GetNewStep() == CHARACTERSCREENSTEP_None)) ||
			((GetStep() == CHARACTERSCREENSTEP_NewCharacterTempskronClass) && (GetNewStep() == CHARACTERSCREENSTEP_None));
}

UnitData * CCharacterScreenModel::GetCharacterMouseOver( std::vector<UnitData *> & vCharacters, Point2D * psPoint )
{
	UnitData * pcRet = nullptr;
	for ( auto & pc : vCharacters )
	{
		if ( pc->bVisible )
		{
			RECT * psRect2D = &pc->rRenderRectangle;
			if ( (psRect2D->left < psPoint->iX) && (psRect2D->right > psPoint->iX) )
			{
				if ( (psRect2D->top < psPoint->iY) && (psRect2D->bottom > psPoint->iY) )
				{
					pcRet = pc;
					break;
				}
			}
		}
	}

	return pcRet;
}

UnitData * CCharacterScreenModel::GetUserCharacterMouseOver( Point2D * psPoint )
{
	return GetCharacterMouseOver( vUserCharacters, psPoint );
}

UnitData * CCharacterScreenModel::GetCreateCharacterMouseOver( Point2D * psPoint )
{
	return GetCharacterMouseOver( vCreateCharacters, psPoint );
}

bool CCharacterScreenModel::CanBackLoginScreen()
{
	if ( (GetStep() == CHARACTERSCREENSTEP_None) && (GetNewStep() == CHARACTERSCREENSTEP_None) )
		return true;

	return false;
}

void CCharacterScreenModel::SetStep( ECharacterScreenStep iStep )
{
	this->iStepOld = this->iStep;
	this->iStep = iStep;

	switch ( iStep )
	{
		case CHARACTERSCREENSTEP_None:
		{
			SetNextCamera( CAMERA_DEFAULT_POSITION );
			break;
		}
		case CHARACTERSCREENSTEP_NewCharacterTribe:
		{
			SetCharacterLocked( false );
			ResetCharacterSelect();
			SetNextCamera( CAMERA_RACE_AXIS );
			break;
		}

		case CHARACTERSCREENSTEP_NewCharacterTempskronClass:
		case CHARACTERSCREENSTEP_NewCharacterMorionClass:
		{
			ResetCreateCharacter();		
			SetNextCamera( CAMERA_CLASS_AXIS );
			break;
		}

		default:
		{
			break;
		}
	}
}

bool CCharacterScreenModel::HasUserCharacter()
{
	return vUserCharacters.size() > 0;
}

void CCharacterScreenModel::HandlePacket( PacketCreateCharacter * psPacket )
{
	if ( psPacket->iCode )
	{
		if ( GetCreateCharacter() && (GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterHair) )
		{
			GetCharacterScreen()->CreateNewCharacter( GetCreateCharacter() );

			GetCharacterScreen()->GetWindow()->SetTemporaryBlockActions( 30000.0f );
		}
	}
	else
	{
		TITLEBOX( "Character Name already exist" );

		CSound::PlaySoundID( ACTIONSOUND_Back );
		SOUNDHANDLER->UpdateVolume();

		GetCharacterScreen()->GetWindow()->RemoveTemporaryBlockActions();
	}
}

void CCharacterScreenModel::CreateNewCharacter( const std::string strCharacterName )
{
	if ( strCharacterName.length() == 0 )
	{
		TITLEBOX( "Please enter a Character Name" );
	}
	else
	{
		if ( GetCreateCharacter() )
		{
			STRINGCOPY( GetCreateCharacter()->sCharacterData.szName, strCharacterName.c_str() );

			ProcessPacket( &PacketCreateCharacter( ACCOUNT_NAME, strCharacterName.c_str() ) );

			GetCharacterScreen()->GetWindow()->SetTemporaryBlockActions( 10000.0f );
		}
	}
}

void CCharacterScreenModel::UpdateCamera( float fTime )
{
	if ( sNextCameraPosition != sCameraPosition )
	{
		if ( fCameraTimeChangeDuration > 0.0f )
		{
			float fCameraTime = fCameraTimeUpdate / fCameraTimeChangeDuration;
			float f = X3D::easeInQuadf( 0.0f, 1.0f, fCameraTime );

			Point3D sPosition	= sNextCameraPosition - sCameraPosition;
			sPosition.iX		= (int)((float)sPosition.iX * f);
			sPosition.iY		= (int)((float)sPosition.iY * f);
			sPosition.iZ		= (int)((float)sPosition.iZ * f);

			sCameraPosition += sPosition;

			fCameraTimeUpdate += fTime;

			if ( fCameraTimeUpdate >= fCameraTimeChangeDuration )
			{
				sCameraPosition = sNextCameraPosition;

				fCameraTimeUpdate = 0.0f;
			}
		}
	}
}

void CCharacterScreenModel::SetCamera( Point3D sPosition, Point3D sLookAt )
{
	struct eCAMERA_TRACE
	{
		int x, y, z;
		int tx, ty, tz;

		EXEMatrixE	eRotMatrix;
		int AngX, AngY;
	};

	eCAMERA_TRACE eTrace;

	CALL_WITH_ARG7( 0x0048B540, (DWORD)&eTrace, sPosition.iX, sPosition.iY, sPosition.iZ, sLookAt.iX, sLookAt.iY, sLookAt.iZ );
	CALLT_WITH_ARG4( 0x0047F1A0, (DWORD)RENDERER, sPosition.iX, sPosition.iY, sPosition.iZ, (DWORD)&eTrace.eRotMatrix );


	RENDERER->SetCameraPosition( sPosition.iX, sPosition.iY, sPosition.iZ, 0, 0, 0 );

	sCameraAngle.iX = eTrace.AngX;
	sCameraAngle.iY = eTrace.AngY;

	(*(int*)0x00A17520) = sPosition.iX;
	(*(int*)0x00A17510) = sPosition.iY;
	(*(int*)0x00A1619C) = sPosition.iZ;

	(*(int*)0x00824FA8) = eTrace.AngX;
	(*(int*)0x00A17630) = eTrace.AngY;
}

bool CCharacterScreenModel::CanRenderUserCharacters()
{
	bool bRender = false;

	if ( (GetStep() == CHARACTERSCREENSTEP_None) || (GetNewStep() == CHARACTERSCREENSTEP_None) ||
		(GetStep() == CHARACTERSCREENSTEP_NewCharacterHair) || (GetNewStep() == CHARACTERSCREENSTEP_NewCharacterHair) )
		bRender = true;

	return bRender;
}

void CCharacterScreenModel::UpdateUserCharactersPosition()
{
	UpdateCharactersPosition( vUserCharacters, sUserCharactersPosition, true );
}

void CCharacterScreenModel::UpdateCreateCharactersPosition()
{
	UpdateCharactersPosition( vCreateCharacters, sCreateCharactersPosition, false );
}

void CCharacterScreenModel::UpdateCharactersPosition( std::vector<UnitData *> & vCharacters, Point3D sUpdatePosition, bool bPodiumStyle )
{
	Point3D sPosition = Point3D( 0, 0, 0 );

	if ( bPodiumStyle )
	{
		int iIndex = 0;

		int iX = 0;
		int iZ = 0;

		for ( auto & pc : vCharacters )
		{
			if ( pc->bVisible )
			{
				pc->sPosition = sUpdatePosition;

				if ( iIndex > 0 )
				{
					if ( ((iIndex - 1) % 2) == 0 )
					{
						iX++;

						pc->sPosition.iX = sUpdatePosition.iX - (iX * (36 << 8));
					}
					else
					{
						pc->sPosition.iX = sUpdatePosition.iX + (iX * (36 << 8));
					}
				}

				pc->sPosition.iZ += iZ;
				
				if ( (iIndex % 2) == 0 )
				{
					iZ += 14 << 8;
				}
				
				iIndex++;
			}
		}
	}
	else
	{
		int iIndex = 1;
		int iFirstX = sUpdatePosition.iX - ((vCharacters.size() / 2) * (36 << 8));

		int iCountPositive = vCharacters.size() / 2;
		if ( vCharacters.size() % 2 )
			iCountPositive++;

		for ( auto & pc : vCharacters )
		{
			if ( pc->bVisible )
			{
				pc->sPosition = sUpdatePosition;

				//Left
				pc->sPosition.iX = iFirstX;


				pc->sPosition.iZ += sPosition.iZ;

				if ( iIndex < iCountPositive )
				{
					sPosition.iZ -= 8 << 8; //0.5meter
				}
				else
				{
					sPosition.iZ += 8 << 8; //0.5meter				
				}
			}

			iFirstX += 36 << 8;

			iIndex++;
		}
	}
}

void CCharacterScreenModel::RenderUserCharacters()
{
	if ( CanRenderUserCharacters() )
	{
		auto pcUnitHover = GetCharacterMouseOver( vUserCharacters, MOUSEHANDLER->GetPosition() );
		if( CanSelectUserCharacters() == false )
			pcUnitHover = nullptr;

		auto pcUnitDataInfo = pcCharacter ? pcCharacter : pcUnitHover;
		if ( pcUnitDataInfo )
		{
			const ClanGameData * psCharacterSelectClanInfo = ((const ClanGameData *)0x03ADF350);
			std::string strClanName = pcUnitDataInfo->iClanInfoIndex >= 0 ? psCharacterSelectClanInfo[pcUnitDataInfo->iClanInfoIndex].szName : "";

			EXESurfaceOld * psClanSurfaceOld = pcUnitDataInfo->iClanInfoIndex >= 0 ? psCharacterSelectClanInfo[pcUnitDataInfo->iClanInfoIndex].psClanIcon16 : nullptr;

			GetCharacterScreen()->GetWindow()->UpdateCharacterInfo( CharacterScreenCharacterData( pcUnitDataInfo->sCharacterData.szName, pcUnitDataInfo->sCharacterData.iLevel,
				strClanName, pcUnitDataInfo->sCharacterData.iClass, psClanSurfaceOld ) );
		}
		else
		{
			GetCharacterScreen()->GetWindow()->HideCharacterInfo();
		}

		for ( auto & pc : vUserCharacters )
		{
			if ( pc->bVisible )
			{
				bool bColor = false;

				if ( pcCharacter )
				{
					if ( pcCharacter == pc )
					{
						RENDERER->r += bCharacterLocked ? 150 : 50;
						RENDERER->g += bCharacterLocked ? 150 : 50;
						RENDERER->b += bCharacterLocked ? 150 : 50;
						bColor = true;
					}
				}

				//Glow if its on hover
				bool bGlowCharacter = false;
				if ( (pcUnitHover == pc) || (pcCharacter && (pcCharacter == pc)) )
				{
					bGlowCharacter = true;

					if ( (pcUnitHover == pc) )
						if ( bCharacterLocked && (pcCharacter != pc) )
							bGlowCharacter = false;
				}

				if ( bGlowCharacter )
				{
					if ( DX::cSelectGlow1.Begin( D3DCOLOR_XRGB( 255, 255, 255 ), TRUE ) )
					{
						pc->Render();
						DX::cSelectGlow1.End( TRUE );

						if ( DX::cSelectGlow1.Begin( D3DCOLOR_XRGB( 255, 255, 255 ), FALSE ) )
						{
							pc->Render();
							DX::cSelectGlow1.End( FALSE );
						}
					}
				}

				pc->Render();

				if ( bColor )
				{
					RENDERER->r -= bCharacterLocked ? 150 : 50;
					RENDERER->g -= bCharacterLocked ? 150 : 50;
					RENDERER->b -= bCharacterLocked ? 150 : 50;
				}
			}
		}
	}
}

void CCharacterScreenModel::RenderNewCharacters()
{
	if ( true )
	{
		bool bRenderTitle = (GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterTempskronClass) || (GetCharacterScreen()->GetStep() == CHARACTERSCREENSTEP_NewCharacterMorionClass);

		auto pcUnitHover = GetCharacterMouseOver( vCreateCharacters, MOUSEHANDLER->GetPosition() );

		for ( auto & pc : vCreateCharacters )
		{
			if ( pc->bVisible )
			{
				bool bColor = false;

				RENDERER->r += bCharacterLocked ? 150 : 50;
				RENDERER->g += bCharacterLocked ? 150 : 50;
				RENDERER->b += bCharacterLocked ? 150 : 50;
				bColor = true;

				//Glow if its on hover
				bool bGlowCharacter = false;
				if ( pcUnitHover == pc )
				{
					bGlowCharacter = true;
				}

				if ( CanSelectCreateCharacter() == false )
					bGlowCharacter = false;

				if ( bGlowCharacter )
				{
					if ( DX::cSelectGlow1.Begin( D3DCOLOR_XRGB( 255, 255, 255 ), TRUE ) )
					{
						pc->Render();
						DX::cSelectGlow1.End( TRUE );

						if ( DX::cSelectGlow1.Begin( D3DCOLOR_XRGB( 255, 255, 255 ), FALSE ) )
						{
							pc->Render();
							DX::cSelectGlow1.End( FALSE );
						}
					}
				}

				pc->Render();
				
				if ( bRenderTitle )
				{
					const auto pcImage = vImageClasses[pc->sCharacterData.iClass - 1];

					int iAlpha = bGlowCharacter ? 255 : 200;

					int iXCenter = ((pc->rRenderRectangle.right - pc->rRenderRectangle.left) >> 1) - (pcImage->GetWidth() >> 1);
					UI::ImageRender::Render( pcImage, pc->rRenderRectangle.left + iXCenter, pc->rRenderRectangle.top - 70, pcImage->GetWidth(), pcImage->GetHeight(), DX::Color( iAlpha, 255, 255, 255 ).Get() );

					if ( bGlowCharacter )
					{
						int iX = (RESOLUTION_WIDTH >> 1) - (pcImageBox->GetWidth() >> 1);
						int iY = (RESOLUTION_HEIGHT >> 2) - 50;

						auto pcImageClassText = vImageClassesText[pc->sCharacterData.iClass - 1];

						UI::ImageRender::Render( pcImageBox, iX, iY, pcImageBox->GetWidth(), pcImageBox->GetHeight(), -1 );

						UI::ImageRender::Render( pcImageClassText, iX + 17, iY + 14, pcImageClassText->GetWidth(), pcImageClassText->GetHeight(), -1 );
					}
				}

				if ( bColor )
				{
					RENDERER->r -= bCharacterLocked ? 150 : 50;
					RENDERER->g -= bCharacterLocked ? 150 : 50;
					RENDERER->b -= bCharacterLocked ? 150 : 50;
				}
			}
		}
	}
}

void CCharacterScreenModel::UpdateUserCharacters( float fTime )
{
	if ( CanRenderUserCharacters() )
	{
		bool bUpdateTime = false;

		//Reset Transparency
		if ( pcCharacter == nullptr )
		{
			for ( auto & pc : vUserCharacters )
			{
				if ( pc->iTransparencySelf < -30 )
				{
					float f = X3D::easeInQuadf( 0.0f, 500.0f, fCharacterSelectTimeUpdate );

					pc->iTransparencySelf = (int)round( (-80.0f *fCharacterSelectTimeUpdate) / 500.0f );

					bUpdateTime = true;
				}
			}
		}
		else
		{
			for ( auto & pc : vUserCharacters )
			{
				if ( pc != pcCharacter )
				{
					if ( pc->iTransparencySelf > (-80) )
					{
						float f = X3D::easeInQuadf( 0.0f, 500.0f, fCharacterSelectTimeUpdate );

						pc->iTransparencySelf = -(int)round( (80.0f * fCharacterSelectTimeUpdate) / 500.0f );

						bUpdateTime = true;
					}
				}
				else
				{
					if ( pc->iTransparencySelf < -30 )
					{
						float f = X3D::easeInQuadf( 0.0f, 500.0f, fCharacterSelectTimeUpdate );

						pc->iTransparencySelf = (int)round( (-80.0f *fCharacterSelectTimeUpdate) / 500.0f );

						bUpdateTime = true;
					}
				}
			}
		}

		if ( bUpdateTime )
			fCharacterSelectTimeUpdate += fTime;

		if ( fCharacterSelectTimeUpdate > 500.0f )
			fCharacterSelectTimeUpdate = 500.0f;
	}
}

void CCharacterScreenModel::FrameUserCharacters()
{
	if ( CanRenderUserCharacters() )
	{
		for ( auto & pc : vUserCharacters )
		{
			if ( pc->bVisible )
			{
				pc->Main();

				if ( MainClanData() )
				{
					//Clan Data
					const ClanGameData * psCharacterSelectClanInfo = ((const ClanGameData *)0x03ADF350);
					for ( UINT i = 0; i < MAX_USER_CHARACTERS; i++ )
					{
						if ( psCharacterSelectClanInfo[i].szCharacterName[0] != 0 )
						{
							if ( STRINGCOMPAREI( psCharacterSelectClanInfo[i].szCharacterName, pc->sCharacterData.szName ) )
							{
								pc->iClanInfoIndex = i;
								break;
							}
						}
					}

					bUpdatedClanData = true;
				}
			}
		}
	}
}

void CCharacterScreenModel::UpdateNewCharacters( float fTime )
{
}

void CCharacterScreenModel::FrameNewCharacters()
{
	for ( auto & pc : vCreateCharacters )
	{
		if ( pc->bVisible )
		{
			pc->Main();
		}
	}
}

void CCharacterScreenModel::UpdateTextures()
{
	//function EXETexture::ReadTextures()
	CALL( 0x0048A260 );
}

NAKED BOOL CCharacterScreenModel::MainClanData()
{
	JMP( 0x0065E180 );
}

NAKED void CCharacterScreenModel::ClanWebStart()
{
	JMP( 0x00663170 );
}

NAKED void CCharacterScreenModel::ClanMenuInit( int iNum )
{
	JMP( 0x0065E2E0 );
}

CCharacterScreen * CCharacterScreenModel::GetCharacterScreen()
{
	return ((CCharacterScreen*)GAMESCREEN->GetActiveScreen());
}

void CCharacterScreenModel::ProcessPacket( PacketDeleteCharacter * psPacket )
{
	psPacket->iLength = sizeof( PacketDeleteCharacter );
	psPacket->iHeader = PKTHDR_DeleteCharacter;
	SENDPACKETL( psPacket, TRUE );
}

void CCharacterScreenModel::ProcessPacket( PacketCreateCharacter * psPacket )
{
	psPacket->iLength = sizeof( PacketCreateCharacter );
	psPacket->iHeader = PKTHDR_CreateCharacter;
	SENDPACKETL( psPacket, TRUE );
}

NAKED BOOL CCharacterScreenModel::GetCharacterClan( const char * pszAccountName, const char * pszCharacterName )
{
	JMP( 0x0065B710 );
}
