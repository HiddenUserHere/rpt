#include "stdafx.h"
#include "CMountHandler.h"

MountCharacterAnimation saMountCharacterAnimation[] =
{
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      128},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         129},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      130},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         131},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      132},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         133},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      132},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         133},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      138},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         139},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      140},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         141},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      142},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         143},
	{EMountType::MOUNTTYPE_Dragon,				CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Running,      146},
	{EMountType::MOUNTTYPE_Dragon,				CHARACTERCLASS_Fighter,         ANIMATIONTYPE_Idle,         147},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      128},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         129},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      130},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         131},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      132},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         133},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      134},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         135},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      138},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         139},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      140},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         141},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      142},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         143},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Running,      146},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Mechanician,     ANIMATIONTYPE_Idle,         147},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      85},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         86},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      87},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         88},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      89},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         90},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      91},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         92},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      95},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         96},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      97},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         98},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      99},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         100},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Archer,          ANIMATIONTYPE_Running,      103},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Archer,          ANIMATIONTYPE_Idle,         104},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      78},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         79},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      80},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         81},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      82},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         83},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      84},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         85},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      88},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         89},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      90},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         91},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      92},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         93},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Running,      96},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Pikeman,         ANIMATIONTYPE_Idle,         97},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      85},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         86},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      87},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         88},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      89},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         90},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      91},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         92},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      95},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         96},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      97},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         98},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      99},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         100},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Running,      103},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Atalanta,        ANIMATIONTYPE_Idle,         104},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      128},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         129},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      130},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         131},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      132},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         133},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      134},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         135},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      138},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         139},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      140},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         141},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      142},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         143},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Knight,          ANIMATIONTYPE_Running,      146},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Knight,          ANIMATIONTYPE_Idle,         147},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      67},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         68},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      69},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         70},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      71},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         72},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      73},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         74},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      77},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         78},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      79},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         80},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      81},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         82},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Magician,        ANIMATIONTYPE_Running,      85},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Magician,        ANIMATIONTYPE_Idle,         86},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      66},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         67},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      68},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         69},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      70},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         71},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      72},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         73},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      76},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         77},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      78},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         79},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      80},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         81},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Running,      84},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Priestess,       ANIMATIONTYPE_Idle,         85},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      91},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         92},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      93},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         94},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      95},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         96},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      97},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         98},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      101},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         102},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      103},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         104},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      105},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         106},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Running,      109},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Assassin,        ANIMATIONTYPE_Idle,         110},

	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      85},
	{EMountType::MOUNTTYPE_Hopy,                CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         86},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      87},
	{EMountType::MOUNTTYPE_Horse,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         88},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      89},
	{EMountType::MOUNTTYPE_Wolf,                CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         90},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      91},
	{EMountType::MOUNTTYPE_Pingu,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         92},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      95},
	{EMountType::MOUNTTYPE_Snowdeer,            CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         96},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      97},
	{EMountType::MOUNTTYPE_Rabie,               CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         98},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      99},
	{EMountType::MOUNTTYPE_Raptor,              CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         100},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Running,      103},
	{EMountType::MOUNTTYPE_Dragon,              CHARACTERCLASS_Shaman,          ANIMATIONTYPE_Idle,         104},
};



CMountHandler::CMountHandler()
{
	pcWindow = new CMountWindowHandler();
}

CMountHandler::~CMountHandler()
{
	for ( auto pc : vInstances )
		DELET( pc );

	vInstances.clear();

	DELET( pcWindow );
}

void CMountHandler::SetCharacterAnimation( ECharacterClass eCharacterClass, EMountType eMountType, EAnimationType eAnimationType, int iCharacterAnimationID )
{
	for ( auto & sMountAnimation : saMountCharacterAnimation )
	{
		if ( (sMountAnimation.iMountType == eMountType) && (sMountAnimation.eCharacterClass == eCharacterClass) && (sMountAnimation.eMountAnimationType == eAnimationType) )
		{
			sMountAnimation.iCharacterAnimationID = iCharacterAnimationID;
			break;
		}
	}
}

void CMountHandler::Init()
{
	pcWindow->Init();
}

bool CMountHandler::AddMount( UnitData * pcUnitData, EMountType eMountType )
{
	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
	{
		if ( pcInstance->GetMountType() == eMountType )
			return false;
	}

	RemoveMount( pcUnitData );

	const auto & sMount = GetMount( eMountType );

	auto pcMount = new CMountInstance();
	pcMount->SetID( iWheelID++ );
	pcMount->SetOwnerID( pcUnitData->iID );
	pcMount->SetMountType( sMount.iMountType );
	pcMount->SetUnitData( GetNewUnitData( sMount.szMountPath, pcUnitData->sPosition ) );
	pcMount->GetUnitData()->iID = pcUnitData->iID;

	vInstances.push_back( pcMount );

	//UpdateAnimation( pcUnitData );

	pcUnitData->iModelHeight += sMount.iHeight;

	OnSpawnMount( pcUnitData, pcMount->GetUnitData(), sMount.iMountType );

	if ( auto pcScript = EFFEKSEERHANDLER->LoadScript( "game\\scripts\\particles\\MountSpawn.efk" ) )
	{
		pcScript->SetID( PARTICLEID_MAGNETIC_DISCHARGE + 2 );

		auto pcOwner = new Engine::ObjectOwnerUnit( pcUnitData );
		pcOwner->AddAnimationTypeLostCondition( ANIMATIONTYPE_Die );
		pcScript->SetOwner( pcOwner );

		EFFEKSEERHANDLER->AddScript( pcScript );
	}
	

	if ( pcUnitData == UNITDATA )
	{
		UNITDATA->Animate( ANIMATIONTYPE_Idle );

		//UNITDATA->bNoDeath = 5 * 70;
	}
	else
	{
		//pcUnitData->Animate( ANIMATIONTYPE_Idle );

		//pcUnitData->bNoDeath = 5 * 70;
	}

	ITEMHANDLER->UpdateItemCharacterStatus();

	return true;
}

bool CMountHandler::RemoveMount( UnitData * pcUnitData, UINT uCooldownTimeMS, bool bReUseAnimation )
{
	bool bRet = false;

	for ( auto it = vInstances.begin(); it != vInstances.end(); )
	{
		auto pcInstance = (*it);
		if ( pcInstance->GetOwnerID() == pcUnitData->iID )
		{
			const auto & sMount = GetMount( pcInstance->GetMountType() );

			pcUnitData->iModelHeight -= sMount.iHeight;

			DELET( pcInstance );
			it = vInstances.erase( it );

			bRet = true;

			if ( auto pcScript = EFFEKSEERHANDLER->LoadScript( "game\\scripts\\particles\\MountSpawn.efk" ) )
			{
				pcScript->SetID( PARTICLEID_MAGNETIC_DISCHARGE + 2 );

				auto pcOwner = new Engine::ObjectOwnerUnit( pcUnitData );
				pcOwner->AddAnimationTypeLostCondition( ANIMATIONTYPE_Die );
				pcScript->SetOwner( pcOwner );

				EFFEKSEERHANDLER->AddScript( pcScript );
			}

			ITEMHANDLER->UpdateItemCharacterStatus();
		}
		else
			it++;
	}

	if ( bRet && bReUseAnimation )
		pcUnitData->Animate( pcUnitData->GetAnimation() );

	GetWindow()->SetCooldownMount( uCooldownTimeMS );

	return bRet;
}

void CMountHandler::Render( UnitData * pcUnitData )
{
	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
	{
		if ( auto pcUnitDataMount = pcInstance->GetUnitData() )
		{
			UpdateAnimation( pcUnitData, pcUnitData->GetAnimation() );

			//Update Position
			Point3D sPosition = pcUnitData->sPosition;
			Point3D sAngle = pcUnitData->sAngle;

			sAngle.iY = (-sAngle.iY + PTANGLE_180) & PTANGLE_Mask;

			pcUnitDataMount->pcStage = nullptr;

//			pcUnitDataMount->Animate( pcUnitData->GetAnimation() );

		//	pcUnitDataMount->SetPosition( sPosition.iX, sPosition.iY, sPosition.iZ, sAngle.iX, sAngle.iY, sAngle.iZ );

			//if ( auto [pcMeshOwner, pcMeshMount] = GetMeshes( pcUnitData, pcUnitDataMount ); (pcMeshOwner && pcMeshMount) )
			{
				Point3D sCameraPosition;
				sCameraPosition.iX = UNITTARGETPOSITION_X;
				sCameraPosition.iY = UNITTARGETPOSITION_Y;
				sCameraPosition.iZ = UNITTARGETPOSITION_Z;

				Point3D sCameraAngle;
				sCameraAngle.iX = UNITTARGETANGLE_X;
				sCameraAngle.iY = UNITTARGETANGLE_Y;
				sCameraAngle.iZ = UNITTARGETANGLE_Z;

				pcUnitDataMount->dwLastActiveTime = TICKCOUNT;

				//Update Frame of Animation
				pcUnitDataMount->iFrame = pcUnitDataMount->iStartFrame + (pcUnitData->iFrame - pcUnitData->iStartFrame);

				//Reset Animation
				if ( pcUnitDataMount->iFrame > (pcUnitDataMount->iEndFrame - 1) )
					pcUnitDataMount->iFrame = pcUnitDataMount->iEndFrame - 1;

				if ( pcUnitDataMount->iFrame < pcUnitDataMount->iStartFrame )
					pcUnitDataMount->iFrame = pcUnitDataMount->iStartFrame;

				//Render Body of Unit
				if ( pcUnitDataMount->pcBodyMeshContainer )
				{
					pcUnitDataMount->pcBodyMeshContainer->SetPosition( &sPosition, &Point3D( sAngle.iX, sAngle.iY, sAngle.iZ ) );

					if ( (pcUnitDataMount->pcBodyMeshContainer->pcBoneContainer) && (int)pcUnitDataMount->pcBodyMeshContainer->pcBoneContainer->uMaxFrame > pcUnitDataMount->iFrame )
						pcUnitDataMount->pcBodyMeshContainer->uFrame = (UINT)pcUnitDataMount->iFrame;

					if ( pcUnitDataMount->pcBodyMeshContainer->pcBoneContainer )
						pcUnitDataMount->pcBodyMeshContainer->pcBoneContainer->iLastFrame = -1;
					else
						pcUnitDataMount->pcBodyMeshContainer->iLastFrame = -1;

					//pcUnitDataMount->pcBodyMeshContainer->pcNewModel->GetMaterialCollection()->SetBlendingMaterial( GRAPHICENGINE->GetRainbowMaterial() );

					pcUnitDataMount->pcBodyMeshContainer->pcNewModel->Render( (IO::SMD::FrameInfo *)&pcUnitData->iStartFrame );
				}
			}
		}
	}
}

void CMountHandler::Update( UnitData * pcUnitData )
{

}

void CMountHandler::Update( float fTime )
{
	static float fKeyTimePressed = 0.0f;

	if ( Game::GetGameMode() == GAMEMODE_InGame )
	{
		//if ( KEYBOARDHANDLER->IsKeyDown( VK_SPACE ) )
		//{
		//	fKeyTimePressed += fTime;

		//	if ( fKeyTimePressed >= 2000.0f )
		//	{
		//		if ( UNITDATA->GetAnimation() != ANIMATIONTYPE_Die )
		//			Mount();

		//		fKeyTimePressed = 0.0f;
		//	}
		//}
		//else
		//	fKeyTimePressed = 0.0f;

		if ( IsRiding( UNITDATA ) )
		{
			if ( CanMount( UNITDATA ) == false )
			{
				RemoveMount( UNITDATA );

				TITLEBOX( AY_OBFUSCATE( "You can't mount at this place!" ) );
			}
		}
	}
}

void CMountHandler::UpdateAnimation( UnitData * pcUnitData, EAnimationType eAnimationType )
{
	auto UpdateMountAnimation = [eAnimationType]( UnitData * pcUnitDataMount ) -> void
	{
		EAnimationType eAnimationMountType = pcUnitDataMount->GetAnimation();
		if ( eAnimationMountType != eAnimationType )
		{
			pcUnitDataMount->Animate( eAnimationType );
		}
	};

	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
	{
		if ( auto pcUnitDataMount = pcInstance->GetUnitData(); pcUnitDataMount && (pcUnitDataMount != pcUnitData) )
		{
			UpdateMountAnimation( pcUnitDataMount );
		}
	}
}

int CMountHandler::GetAnimationID( UnitData * pcUnitData, EAnimationType eAnimationType )
{
	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
	{
		if ( auto pcUnitDataMount = pcInstance->GetUnitData(); pcUnitDataMount && (pcUnitDataMount != pcUnitData) )
		{
			const auto & sMountAnimation = GetCharacterAnimationID( pcInstance->GetMountType(), pcUnitData->sCharacterData.iClass, eAnimationType );

			return sMountAnimation.iCharacterAnimationID;
		}
	}
	return -1;
}

void CMountHandler::OnRenderCharacter( UnitData * pcUnitData )
{

}

bool CMountHandler::IsRiding( UnitData * pcUnitData )
{
	return GetInstanceByOwnerID( pcUnitData->iID ) != nullptr ? true : false;
}

EMountType CMountHandler::GetMountType( UnitData * pcUnitData )
{
	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
		return pcInstance->GetMountType();

	return EMountType::MOUNTTYPE_None;
}

int CMountHandler::GetMountModelHeight( UnitData * pcUnitData )
{
	if ( auto pcInstance = GetInstanceByOwnerID( pcUnitData->iID ) )
		return GetMount( pcInstance->GetMountType() ).iHeight;

	return 0;
}

void CMountHandler::ShowMessage( EMountMessage iMessage )
{
	static UINT uTimeMessage = 0;

	if ( uTimeMessage > TICKCOUNT )
		return;

	switch ( iMessage )
	{
		case MOUNTMESSAGE_ErrorEnterNPC:
			TITLEBOX( AY_OBFUSCATE( "You can't enter while on a Mount!" ) );
			CHATGAMEHANDLER->AddChatBoxText( AY_OBFUSCATE( "> You can't enter while on a Mount!" ), CHATCOLOR_Global );
			break;

		case MOUNTMESSAGE_ErrorWalk:
			TITLEBOX( AY_OBFUSCATE( "You can't walk while on a Mount!" ) );
			CHATGAMEHANDLER->AddChatBoxText( AY_OBFUSCATE( "> You can't walk while on a Mount!" ), CHATCOLOR_Error );
			break;

		case MOUNTMESSAGE_ErrorBC:
			TITLEBOX( AY_OBFUSCATE( "You can't ride while at Bless Castle!" ) );
			CHATGAMEHANDLER->AddChatBoxText( AY_OBFUSCATE( "> You can't ride while at Bless Castle!" ), CHATCOLOR_Error );
			break;

		default:
			break;
	}

	uTimeMessage = TICKCOUNT + 2000;
}

BOOL CMountHandler::OnKeyPress( CKeyboard * pcKeyboard )
{
	return FALSE;
}

BOOL CMountHandler::OnKeyChar( CKeyboard * pcKeyboard )
{
	if ( (Game::GetGameMode() == GAMEMODE_InGame) && (MAP_ID != MAPID_GhostCastle) )
	{
		if ( toupper( pcKeyboard->GetChar() ) == 'X' )
		{
			if ( GetWindow()->IsOpen() == FALSE )
				GetWindow()->Open();
			else
				GetWindow()->Close();
		}
	}

	return GetWindow()->OnKeyChar( pcKeyboard );
}

bool CMountHandler::CanMount( UnitData * pcUnitData )
{
	if ( (MAP_ID == MAPID_OldRuinen2) || (MAP_ID == MAPID_RoyalDesert) )
		return false;

	return true;
}

void CMountHandler::HandlePacket( PacketMountUserList * psPacket )
{
	if ( psPacket->IsFirst() )
	{
		GetWindow()->ClearMounts();

		vRarityData.clear();
	}

	if ( (psPacket->iCount > 0) && (psPacket->iCount <= MAX_MOUNTS_PER_DATA) )
	{
		for ( int i = 0; i < psPacket->iCount; i++ )
		{
			GetWindow()->AddMount( psPacket->saMounts[i].iID, psPacket->saMounts[i].szName, psPacket->saMounts[i].szImageFile, psPacket->saMounts[i].eMountType, psPacket->saMounts[i].eRareType, psPacket->saMounts[i].saItemList, psPacket->saMounts[i].iItemCount,
				psPacket->saMounts[i].bIsSelected );

			vRarityData.push_back( MountRarityData( psPacket->saMounts[i].eMountType, psPacket->saMounts[i].eRareType ) );
		}
	}
}

void CMountHandler::HandlePacket( PacketNewMount * psPacket )
{
	GetWindow()->AddMount( psPacket->iID, psPacket->szName, psPacket->szImageFile, psPacket->eMountType, psPacket->eRareType, psPacket->saItemList, psPacket->iItemCount, psPacket->bIsSelected );

	SAVE;
	SAVEEX;
}

void CMountHandler::SaveMounts()
{
	PacketMountUserList sPacket;
	std::vector<PacketMountUserList> vPackets;
	sPacket.iCount = 0;
	sPacket.sPacketCount.sMax = sPacket.sPacketCount.sMin = 0;

	for ( auto psData : GetWindow()->GetMounts() )
	{
		sPacket.saMounts[sPacket.iCount].iID = psData->iMountID;
		STRINGCOPY( sPacket.saMounts[sPacket.iCount].szName, psData->strName.c_str() );
		sPacket.saMounts[sPacket.iCount].eMountType = psData->eMountType;
		sPacket.saMounts[sPacket.iCount].eRareType = psData->eRareType;
		sPacket.saMounts[sPacket.iCount].bIsSelected = psData->bIsSelected;

		for ( UINT i = 0; i < psData->vItems.size(); i++ )
			CopyMemory( &sPacket.saMounts[sPacket.iCount].saItemList[i], &psData->vItems[i], sizeof( Item ) );

		sPacket.saMounts[sPacket.iCount].iItemCount = psData->vItems.size();

		sPacket.iCount++;

		if ( sPacket.iCount == MAX_MOUNTS_PER_DATA )
		{
			sPacket.sPacketCount.sMin++;
			vPackets.push_back( sPacket );

			sPacket.iCount = 0;
		}
	}

	if ( sPacket.iCount > 0 )
	{
		sPacket.sPacketCount.sMin++;
		vPackets.push_back( sPacket );
	}

	for ( auto & sSendPacket : vPackets )
	{
		sSendPacket.sPacketCount.sMax = (short)vPackets.size();

		ProcessPacket( &sSendPacket );
	}

	vPackets.clear();
}

bool CMountHandler::HasMount( EMountType eMountType )
{
	for ( auto psMount : GetWindow()->GetMounts() )
	{
		if ( psMount->eMountType == eMountType )
			return true;
	}

	return false;
}

int CMountHandler::UpdateMovementSpeed()
{
	int iMovementSpeed = 0;

	if ( MOUNTHANDLER->IsRiding( UNITDATA ) )
	{
		if ( GetWindow()->GetMountType() != EMountType::MOUNTTYPE_None )
		{
			switch ( GetWindow()->GetMountRareType() )
			{
				case EMountRareType::MOUNTRARETYPE_Common:
					iMovementSpeed = 4;
					break;

				case EMountRareType::MOUNTRARETYPE_Rare:
					iMovementSpeed = 6;
					break;

				case EMountRareType::MOUNTRARETYPE_Epic:
					iMovementSpeed = 10;
					break;

				case EMountRareType::MOUNTRARETYPE_Legendary:
					iMovementSpeed = 18;
					break;

				default:
					break;
			}
		}
	}

	return iMovementSpeed;
}

int CMountHandler::UpdateEvade()
{
	int iEvade = 0;

	if ( MOUNTHANDLER->IsRiding( UNITDATA ) )
	{
		if ( GetWindow()->GetMountType() != EMountType::MOUNTTYPE_None )
		{
			switch ( GetMountRareByType( GetWindow()->GetMountType() ) )
			{
				case EMountRareType::MOUNTRARETYPE_Common:
					iEvade = 4;
					break;

				case EMountRareType::MOUNTRARETYPE_Rare:
					iEvade = 6;
					break;

				case EMountRareType::MOUNTRARETYPE_Epic:
					iEvade = 10;
					break;

				case EMountRareType::MOUNTRARETYPE_Legendary:
					iEvade = 18;
					break;

				default:
					break;
			}
		}
	}

	return iEvade;
}

EMountRareType CMountHandler::GetMountRareByType( EMountType eType )
{
	for ( const auto & rarity : vRarityData )
	{
		if ( rarity.eMountType == eType )
			return rarity.eMountRareType;
	}

	return EMountRareType::MOUNTRARETYPE_None;
}

void CMountHandler::Mount()
{
	if ( MOUNTHANDLER->IsRiding( UNITDATA ) == FALSE )
	{
		if ( GetWindow()->GetMountType() != EMountType::MOUNTTYPE_None )
		{
			if ( CanMount( UNITDATA ) )
				AddMount( UNITDATA, GetWindow()->GetMountType() );
			else
				TITLEBOX( AY_OBFUSCATE( "You can't mount at this place!" ) );
		}
	}
	else
		RemoveMount( UNITDATA );
}

void CMountHandler::SetCooldownMount( UINT uTimeMS )
{
	GetWindow()->SetCooldownMount( uTimeMS );
}

UINT CMountHandler::GetCooldownMount()
{
	return GetWindow()->GetCooldownMount();
}

UnitData * CMountHandler::GetNewUnitData( const std::string strBodyFile, Point3D sPosition )
{
	RENDERER->SetUseNewRenderToLoad( true );

	UnitData * pcUnitData = new UnitData;

	pcUnitData->Init();

	pcUnitData->pcStage = nullptr;

	UI::UnitMesh::LoadUnitData( pcUnitData, strBodyFile.c_str(), "" );
	pcUnitData->eUpdateMode = UPDATEMODE_ClientSelf;

	STRINGCOPY( pcUnitData->sCharacterData.szName, "Test" );
	pcUnitData->sCharacterData.sWarpHomeTown = 0;

	pcUnitData->SetPosition( sPosition.iX, sPosition.iY, sPosition.iZ, 0, 2048, 0 );

	pcUnitData->Animate( ANIMATIONTYPE_Idle );


	pcUnitData->bActive = TRUE;
	pcUnitData->bVisible = TRUE;

	pcUnitData->Main();

	RENDERER->ResetUseNewRenderToLoad();

    return pcUnitData;
}

UINT CMountHandler::GetCooldownMountPercent()
{
	return GetWindow()->GetCooldownMountPercent();
}

CMountInstance * CMountHandler::GetInstanceByOwnerID( int iOwnerID )
{
	for ( auto pc : vInstances )
		if ( pc->GetOwnerID() == iOwnerID )
			return pc;

	return nullptr;
}

std::tuple<EXEMesh *, EXEMesh *> CMountHandler::GetMeshes( UnitData * pcUnitData, UnitData * pcUnitDataMount )
{
	EXEMesh * pcMountMesh = nullptr;
	EXEMesh * pcOwnerMesh = nullptr;

	if ( auto pcBoneMount = pcUnitDataMount->pcBoneContainer )
	{
		if ( pcUnitData )
		{
			if ( auto pcOwnerBone = pcUnitData->pcBoneContainer )
			{
				//Get Mount Bip
				pcMountMesh = pcBoneMount->GetMesh( "Bip01 Mount" );
				pcOwnerMesh = pcOwnerBone->GetMesh( "Bip01" );
			}
		}
	}

	return std::make_tuple( pcOwnerMesh, pcMountMesh );
}


const MountModelPath & CMountHandler::GetMount( EMountType eMountType )
{
	for ( const auto & sMount : saMountModels )
		if ( sMount.iMountType == eMountType )
			return sMount;

	return saMountModels[0];
}

const MountCharacterAnimation & CMountHandler::GetCharacterAnimationID( EMountType eMountType, ECharacterClass eCharacterClass, EAnimationType eAnimationType )
{
	const auto & sMount = GetMount( eMountType );

	for ( const auto & sMountAnimation : saMountCharacterAnimation )
		if ( (sMountAnimation.iMountType == sMount.eAnimationByMountType) && (sMountAnimation.eCharacterClass == eCharacterClass) && (sMountAnimation.eMountAnimationType == eAnimationType) )
			return sMountAnimation;

	return saMountCharacterAnimation[0];
}

void CMountHandler::OnSpawnMount( UnitData * pcUnitData, UnitData * pcUnitDataMount, EMountType eMountType )
{
	struct MountParticleKeep
	{
		std::string strParticleName;
		std::string strBipedName;

		MountParticleKeep( const std::string _strParticleName, const std::string _strBipedName )
		{
			strParticleName = _strParticleName;
			strBipedName	= _strBipedName;
		};

		~MountParticleKeep() {};
	};

	std::vector<MountParticleKeep> vParticleMounts;

	switch ( eMountType )
	{
		case EMountType::MOUNTTYPE_Hopy:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountHoptKeep_1.efk", "" ) );
			break;
		case EMountType::MOUNTTYPE_Unicorn:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_1.efk", "Bip01 Particle 1" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_2.efk", "Bip01 Particle 2" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_2.efk", "Bip01 Particle 3" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_2.efk", "Bip01 Particle 4" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_2.efk", "Bip01 Particle 5" ) );
			break;
		case EMountType::MOUNTTYPE_Unicorn_2:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_3.efk", "Bip01 Particle 1" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_4.efk", "Bip01 Particle 2" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_4.efk", "Bip01 Particle 3" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_4.efk", "Bip01 Particle 4" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountUnicornKeep_4.efk", "Bip01 Particle 5" ) );
			break;
		case EMountType::MOUNTTYPE_Wolf_2:
		case EMountType::MOUNTTYPE_Wolf_6:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountWolfKeep_1.efk", "Bip01 Particle 1" ) );
			break;
		case EMountType::MOUNTTYPE_Wolf_4:
		case EMountType::MOUNTTYPE_Wolf_8:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountWolfKeep_2.efk", "Bip01 Particle 2" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountWolfKeep_2.efk", "Bip01 Particle 3" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountWolfKeep_2.efk", "Bip01 Particle 4" ) );
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountWolfKeep_2.efk", "Bip01 Particle 5" ) );
			break;
		case EMountType::MOUNTTYPE_Pingu_1:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountPinguKeep_1.efk", "Bip01 Particle 1" ) );
			break;
		case EMountType::MOUNTTYPE_Pingu_2:
			vParticleMounts.push_back( MountParticleKeep( "game\\scripts\\particles\\MountPinguKeep_2.efk", "Bip01 Particle 1" ) );
			break;
		default:
			break;
	}

	if ( vParticleMounts.size() > 0 )
	{
		for ( const auto & s : vParticleMounts )
		{
			if ( auto pcScript = EFFEKSEERHANDLER->LoadScript( s.strParticleName.c_str() ) )
			{
				pcScript->SetID( PARTICLEID_MOUNT_TYPE + (UINT)eMountType );

				auto pcOwner = new Engine::ObjectOwnerUnit( pcUnitDataMount );
				pcOwner->AddAnimationTypeLostCondition( ANIMATIONTYPE_Die );
				if ( s.strBipedName.length() > 0 )
					pcOwner->SetBiped( s.strBipedName.c_str() );
				pcScript->SetOwner( pcOwner );

				EFFEKSEERHANDLER->AddScript( pcScript );
			}
		}
	}

	vParticleMounts.clear();
}

void CMountHandler::ProcessPacket( PacketMountUserList * psPacket )
{
	psPacket->iLength = sizeof( Packet ) + sizeof( MinMax ) + sizeof( int ) + (sizeof( MountUserListData ) * psPacket->iCount);
	psPacket->iHeader = PKTHDR_MountsList;
	SENDPACKETL( psPacket, TRUE );
}


CMountInstance::CMountInstance()
{
}

CMountInstance::~CMountInstance()
{
	if ( EFFEKSEERHANDLER )
		EFFEKSEERHANDLER->Kill( PARTICLEID_MOUNT_TYPE + (UINT)GetMountType(), pcUnitData->iID );

	pcUnitData->Free();

	DELET( pcUnitData );
}
