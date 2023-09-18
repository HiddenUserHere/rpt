#include "stdafx.h"
#include "unitserver.h"
#include "unitdataserver.h"
#include "Priestess.h"

BOOL bFreeExpEvent = FALSE;
int iExpFreeExp = 50;

int iUpdateMaxPacket = 80;
int iUpdateMaxUnitMask = 2048;
int iUpdateFrameCounter = 160;

UnitServer::UnitServer()
{
	pcaUnitInGame	= new Unit*[MAX_UNITS];
	pcaUnit			= new Unit[MAX_UNITS];
	pcaUnitData		= new UnitData[MAX_UNITS];

	WRITEDWORD( 0x07AC3E78, pcaUnitData );

	for ( UINT i = 0; i < MAX_UNITS; i++ )
	{
		pcaUnit[i].uIndex = i;
		pcaUnit[i].pcUnitData = pcaUnitData + i;
	}

	TEST( "UnitData", sizeof( UnitData ), 0x4B3C );
}

UnitData * UnitServer::GetUnit( UINT uEnemyID )
{
	typedef UnitData*( __cdecl *t_FindUnitEnemybyID )( UINT uID );
	t_FindUnitEnemybyID FindUnitEnemybyID = ( t_FindUnitEnemybyID )0x0054CC60;
	return FindUnitEnemybyID( uEnemyID );
}

NAKED UnitData * UnitServer::UnitDataByIDMap( int iID, int iMapID )
{
	JMP( pfnUnitDataByIDMap );
}

Unit * UnitServer::UnitDataToUnit( UnitData * pcUnitData )
{
	if ( pcUnitData == NULL )
		return NULL;

	Unit * pcUnit = pcaUnit + (pcUnitData - pcaUnitData);
	if ( pcUnit->iID != pcUnitData->iID )
		pcUnit->Load();

	return pcUnit;
}

Unit * UnitServer::GetUnitByExclusiveUser( UnitData * pcUnitData )
{
	for ( UINT i = 0; i < uUnitsInGame; i++ )
	{
		if ( pcaUnitInGame[i]->pcUnitData && (pcaUnitInGame[i]->pcUnitData != pcUnitData) && (pcaUnitInGame[i]->pcUnitData->pcExclusiveUnitTarget == pcUnitData) )
		{
			if ( pcaUnitInGame[i]->pcUnitData->sCharacterData.sHP.sCur > 0 )
			{
				if ( (pcUnitData->pcExclusiveTarget == nullptr) || (pcUnitData->pcExclusiveTarget == pcaUnitInGame[i]->pcUnitData->pcExclusiveTarget) )
				{
					return pcaUnitInGame[i];
				}
			}
		}
	}

	return nullptr;
}

CharacterData * UnitServer::GetCharacterDataByEffect( DWORD dwEffect, int iClass )
{
	CharacterData * psCharInfo = *(CharacterData**)0x07AB3050;
	for ( int i = 0; i < (*(int*)0x07AC57E8); i++ )
	{
		if ( psCharInfo[i].iMonsterEffectID == dwEffect )
		{
			if ( iClass != 0 )
			{
				if ( psCharInfo[i].sGlow == (WORD)iClass )
				{
					return &psCharInfo[i];
				}
			}
			else
				return &psCharInfo[i];
		}
	}
	return NULL;
}

CharacterData * UnitServer::GetCharacterDataByEffectLevel( DWORD dwEffect, int iLevel )
{
	CharacterData * psCharInfo = *(CharacterData * *)0x07AB3050;
	for ( int i = 0; i < (*(int *)0x07AC57E8); i++ )
	{
		if ( (psCharInfo[i].iMonsterEffectID == dwEffect) && ((iLevel == 0) || (psCharInfo[i].iLevel == iLevel)) )
			return &psCharInfo[i];
	}
	return NULL;
}

CharacterData * UnitServer::GetCharacterDataByName( const char * pszName )
{
	CharacterData * psCharInfo = *(CharacterData**)0x07AB3050;
	for ( int i = 0; i < (*(int*)0x07AC57E8); i++ )
	{
		if ( STRINGCOMPARE( psCharInfo[i].szName, pszName ) )
		{
			return &psCharInfo[i];
		}
	}
	return NULL;
}

UnitInfo * UnitServer::GetUnitInfoByName( const char * pszName )
{
	UnitInfo * psaUnitInfo = (*(UnitInfo**)0x07AB22A4);

	for ( int i = 0; i < (*(int*)0x07AC57E8); i++ )
	{
		if ( STRINGCOMPARE( psaUnitInfo[i].szName, pszName ) )
			return &psaUnitInfo[i];
	}

	return NULL;
}

int UnitServer::GetArrayPosition( UnitData * pcUnitData )
{
	UINT uaUnits = *(DWORD*)0x7AC3E78;
	return ((UINT)pcUnitData - uaUnits) / 0x4B3C;
}

INT64 UnitServer::GetExpLevelDifference( int iLevel1, int iLevel2, INT64 iExp )
{
	int iLevel = abs( iLevel1 - iLevel2 );
/*
	if ( iLevel < 10 )
		iExp = iExp;
	else if ( iLevel < 15 )
		iExp = ( iExp * 75 ) / 100;
	else if ( iLevel < 20 )
		iExp = ( iExp * 50 ) / 100;
	else
		iExp = ( iExp * 25 ) / 100;
*/
	if ( iLevel < 10 )
		iExp = iExp;
	else if ( iLevel < 15 )
		iExp = (iExp * 50) / 100;
	else
		iExp = (iExp * 25) / 100;

	return iExp;
}

INT64 UnitServer::GetExp( UnitData * pcUnitData )
{
	if ( pcUnitData )
		return ((INT64)pcUnitData->sCharacterData.iCurrentExpHigh << 32) | ((INT64)pcUnitData->sCharacterData.iCurrentExpLow & 0xFFFFFFFF);
	return 0;
}

INT64 UnitServer::GetExp( const CharacterData * psCharacterData )
{
	if ( psCharacterData )
		return ((INT64)psCharacterData->iCurrentExpHigh << 32) | ((INT64)psCharacterData->iCurrentExpLow & 0xFFFFFFFF);
	return 0;
}

BOOL UnitServer::SetMotionFromCode( UnitData * pcUnitData, int iMotionCode )
{
	typedef int( __thiscall *t_SetMotionFromCodePT )( UnitData * pUnit, int iCode );
	t_SetMotionFromCodePT SetMotionFromCodePT = ( t_SetMotionFromCodePT )0x00416E30;
	
	if ( pcUnitData )
	{
		if ( iMotionCode == 0x120 )
			pcUnitData->sCharacterData.sHP.sCur = 0;
		
		return SetMotionFromCodePT( pcUnitData, iMotionCode );
	}
	return FALSE;
}

void UnitServer::SendBossMessage( BaseMap * pcBaseMap, UINT pUnit )
{
	CHATSERVER->SendChatAllUsersInStage( pcBaseMap->GetID(), CHATCOLOR_Global, "Boss> %s Spawned!", (char *)*(UINT*)(pUnit) );
}

BOOL UnitServer::OnUnitNpcClick( UserData * pcUserData, Unit * pcUnit )
{
	if ( pcUnit )
	{
		if ( pcUserData->iGameLevel == GAMELEVEL_Two )
			return FALSE;

		User * pcUser = USERDATATOUSER( pcUserData );

		if ( pcUnit->pcUnitData->sUnitInfo.iWeaponShopCounter || pcUnit->pcUnitData->sUnitInfo.iDefenseShopCounter || pcUnit->pcUnitData->sUnitInfo.iMiscShopCounter )
		{
			bool bSendShop = true;
			if ( pcUnit->pcUnitData->sUnitInfo.iWeaponShopCounter || pcUnit->pcUnitData->sUnitInfo.iDefenseShopCounter )
				bSendShop = !pcUser->IsAccountShareDenied( ACCOUNTSHARE_DenyBuySellItems );

			if ( bSendShop )
				ITEMSERVER->SendItemShopList( pcUser, pcUnit->pcUnitData );
			else
				CHATSERVER->SendChatMessageBox( pcUser, "You can't open Blacksmith, because you're logged in with the Share PW!" );
		}

		// Is only Game Master?
		if ( pcUnit->pcUnitData->sUnitInfo.bGMOnly )
		{
			CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Global, "> Only for GameMaster!" );
			return FALSE;
		}

		if ( QUESTSERVER->OnClickNPC( USERDATATOUSER( pcUserData ), pcUnit->pcUnitData ) == FALSE )
			QUESTSERVER->OnClickDynamicNPC( USERDATATOUSER( pcUserData ), pcUnit->pcUnitData );

		// Is Warehouse Pet?
		if ( pcUnit->pcUnitData->sUnitInfo.iWarehouse && pcUnit->pcUnitData->lExclusiveTargetID && pcUserData->iGameLevel <= GAMELEVEL_One &&
			 pcUserData->iID != pcUnit->pcUnitData->lExclusiveTargetID )
			 return FALSE;

		// Manufacture
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_Manufacture )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_OpenManufacture, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_Smelting )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_OpenSmelting, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_IhinEvent )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_NPCIhinRewardRequest, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_EasterNPC )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_NPCEasterEgg, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_CaravanNPC )
		{
			if ( !pcUser->IsAccountShareDenied( ACCOUNTSHARE_DenyCaravanPet ) )
				FOLLOWUNITHANDLER->SendOpenCaravan( USERDATATOUSER( pcUserData ), pcUnit );
			else
				CHATSERVER->SendChatMessageBox( pcUser, "You can't open Caravan, because you're logged in with the Share PW!" );
		}

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ResetItem )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_OpenResetItem, TRUE );
		if ( (pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_CoinShop) || (pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_TimeShop) )
		{
			PacketOpenCoinShopNPC sPacketCoinShopNPC;
			sPacketCoinShopNPC.iLength = sizeof( PacketOpenCoinShopNPC );
			sPacketCoinShopNPC.iHeader = PKTHDR_OpenCoinShop;
			sPacketCoinShopNPC.iID = (pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_CoinShop) ? CCoinShopHandler::COINSHOP_DEFAULT_ID : CCoinShopHandler::COINSHOP_TIMESHOP_ID;
			SENDPACKET( pcUser, &sPacketCoinShopNPC, TRUE );
		}
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_LarryQuestsNPC )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_NPCLarryOpen, TRUE );
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_MarinaQuestsNPC )
			SENDPACKETBLANK( USERDATATOUSER(pcUserData), PKTHDR_NPCMarinaOpen, TRUE );
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_RudolphXmasNPC )
			SENDPACKETBLANK( USERDATATOUSER( pcUserData ), PKTHDR_NPCRudolphXmas, TRUE );
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_CarnivalNPC )
			EVENTSERVER->SendNPCCarnival( pcUser );
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_FuryArenaNPCTeleportCT2 )
		{
			if ( SERVER_CODE == 1 )
			{
				PacketFuryArenaRequest sPacket;
				sPacket.iLength = sizeof( PacketFuryArenaRequest );
				sPacket.iHeader = PKTHDR_FuryArena;
				sPacket.eState = FURYARENASTATEID_OpenNPC;
				sPacket.iEventID = FURYARENAHANDLER->GetEventID();
				sPacket.dwTimeLeft = (DWORD)FURYARENAHANDLER->GetTimeLeft( GetServerTime() );
				SENDPACKET( USERDATATOUSER(pcUserData), &sPacket, TRUE );
			}
			else
			{
				CHATSERVER->SendChat( USERDATATOUSER( pcUserData ), CHATCOLOR_Global, "BossArena> Boss Arena is closed on this server!" );
			}
		}
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_FuryArenaNPCEnter )
		{
			if ( SERVER_CODE == 1 )
			{
				PacketFuryArenaRequest sPacket;
				sPacket.iLength = sizeof( PacketFuryArenaRequest );
				sPacket.iHeader = PKTHDR_FuryArena;
				sPacket.eState = FURYARENASTATEID_OpenNPCEntrance;
				sPacket.iEventID = FURYARENAHANDLER->GetEventID();
				sPacket.dwTimeLeft = (DWORD)FURYARENAHANDLER->GetTimeLeft( GetServerTime() );
				SENDPACKET( USERDATATOUSER(pcUserData), &sPacket, TRUE );
			}
			else
			{
				CHATSERVER->SendChat( USERDATATOUSER( pcUserData ), CHATCOLOR_Global, "BossArena> Boss Arena is closed on this server!" );
			}
		}
		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_SocketSystemNPCDrill || pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_SocketSystemNPCStone )
		{
			int iID = pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_SocketSystemNPCDrill ? 0x0E : 0x0F;
			PacketSimple s;
			s.iLength	= sizeof( PacketSimple );
			s.iHeader	= PKTHDR_OpenNPC;
			s.iUnk		= 0;
			s.iUnk2		= iID;
			s.iUnk3		= 0;
			s.iUnk4		= 0;
			SENDPACKET( USERDATATOUSER(pcUserData), &s, TRUE );
		}

		if ( (pcUser->GetGameLevel() >= GAMELEVEL_Two) || EVENT_ACTIONFIELD )
		{
			PacketActionFieldRequest sPacketActionField;
			sPacketActionField.iLength	= sizeof( PacketActionFieldRequest );
			sPacketActionField.iHeader	= PKTHDR_ActionFieldNPCEnter;
			sPacketActionField.iCount	= 0;

			if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldDG1 )
			{
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_DragonsDungeon1 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_DragonsDungeon2 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_DragonsDungeon3 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				SENDPACKET( pcUser, &sPacketActionField );
			}

			if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldFD1 )
			{
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_ForestDungeon1 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_ForestDungeon2 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_ForestDungeon3 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				SENDPACKET( pcUser, &sPacketActionField );
			}

			if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldFD2 )
			{
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_ForestDungeon2 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount]				= pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount]		= pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount]	= ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				SENDPACKET( pcUser, &sPacketActionField );
			}

			if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldFD3 )
			{
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_ForestDungeon3 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount]				= pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount]		= pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount]	= ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				SENDPACKET( pcUser, &sPacketActionField );
			}

			if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldEnter )
			{
				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_GhostCastle1 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				if ( auto pcActionField = ACTIONFIELDHANDLER->GetActionField( EActionFieldModeID::ACTIONFIELDID_GhostCastle2 ) )
				{
					sPacketActionField.iaModeID[sPacketActionField.iCount] = pcActionField->GetModeID();
					sPacketActionField.saRequiredLevel[sPacketActionField.iCount] = pcActionField->GetSettings().sRequiredLevel;
					sPacketActionField.iaInstanceCountToday[sPacketActionField.iCount] = ACTIONFIELDHANDLER->SQLGetInstanceTodayCount( pcUser, pcActionField->GetModeID() );
					sPacketActionField.iaInstanceLimitPerDay[sPacketActionField.iCount] = pcActionField->GetSettings().iInstanceDayLimit;
					STRINGCOPY( sPacketActionField.szaName[sPacketActionField.iCount], pcActionField->GetName() );

					sPacketActionField.iCount++;
				}

				SENDPACKET( pcUser, &sPacketActionField );
			}
		}

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_RankingLevel )
			RANKINGHANDLER->SendUserRankingLevel( pcUser );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_ActionFieldExchange )
			SENDPACKETBLANK( pcUser, PKTHDR_ActionFieldNPCExchange, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_NPCBeginnerItemTimer )
			SENDPACKETBLANK( pcUser, PKTHDR_NPCBeginnerItemTimer, TRUE );

		if ( STRINGCOMPAREI( pcUnit->GetName(), "The Chemist, Legasov" ) )
			SENDPACKETBLANK( pcUser, PKTHDR_ChemistHatItemTimer, TRUE );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_XmasTreeNPC )
			XMASTREEHANDLER->OpenTree( pcUser, pcUnit );

		if ( pcUnit->pcUnitData->sUnitInfo.iSmelting == NPCEVENTCODE_XmasTreeGiftNPC )
			XMASTREEHANDLER->OpenGiftNPC( pcUser, pcUnit );

		if ( pcUnit->pcUnitData->sUnitInfo.iEventGirl )
		{
			if ( !pcUser->IsAccountShareDenied( ACCOUNTSHARE_DenyUseResetStones ) )
			{
				PacketEventGirl s;
				s.iLength = sizeof( PacketEventGirl );
				s.iHeader = PTKHDR_OpenEventGirl;
				s.bFreeGold = FREE_EVENTGIRL;
				SENDPACKET( USERDATATOUSER( pcUserData ), &s );
			}
			else
				CHATSERVER->SendChatMessageBox( pcUser, "You can't open Event Girl, because you're logged in with the Share PW!" );
		}

		if ( pcUnit->pcUnitData->sUnitInfo.iClanController )
		{
			if ( !pcUser->IsAccountShareDenied( ACCOUNTSHARE_DenyClanMaster ) )
				SENDPACKETBLANK( pcUser, PKTHDR_OpenClanMaster, TRUE );
			else
				CHATSERVER->SendChatMessageBox( pcUser, "You can't open Clan Master, because you're logged in with the Share PW!" );
		}

		if ( pcUnit->pcUnitData->sUnitInfo.iItemBox )
		{
			if ( !pcUser->IsAccountShareDenied( ACCOUNTSHARE_DenyOpenItemBox ) )
				SENDPACKETBLANK( pcUser, PKTHDR_NPCItemBoxOpen, TRUE );
			else
				CHATSERVER->SendChatMessageBox( pcUser, "You can't open Item Distributor, because you're logged in with the Share PW!" );
		}

		SERVEREVENTHANDLER->OnNPCClick( pcUser, pcUnit );
	}
	return TRUE;
}

void UnitServer::OnSendExp( UserData * pcUserData, UnitData * pcUnitData )
{
	auto IsUserInRangeParty = []( User * pcUserParty, User * pcUserTarget )-> bool
	{
		bool bRet = false;

		int iX = (pcUserParty->pcUserData->sPosition.iX - pcUserTarget->pcUserData->sPosition.iX) >> 8;
		int iZ = (pcUserParty->pcUserData->sPosition.iZ - pcUserTarget->pcUserData->sPosition.iZ) >> 8;
		int iDist = iX * iX + iZ * iZ;
		if ( (iDist < PARTY_GETTING_DIST) && (pcUserTarget->GetMapID() == pcUserParty->GetMapID()) )
			bRet = true;

		return bRet;
	};

	User * pcUser = USERDATATOUSER( pcUserData );

	if ( pcUserData && pcUnitData && pcUser )
	{
		// First all, notify Quest Handler
		if ( FURYARENAHANDLER->OnUnitKilled( pcUserData, pcUnitData ) )
			return;
		if ( HALLOWEENHANDLER->OnUnitKilled( pcUnitData ) )
			return;
		if ( QUESTSERVER->OnUnitKilled( pcUser, UNITDATATOUNIT( pcUnitData ) ) )
			return;

		EVENTSERVER->OnUnitKilled( pcUnitData );

		ACTIONFIELDHANDLER->OnUnitKilled( pcUser, UNITDATATOUNIT( pcUnitData ) );

		INT64 iExp = GetExp( pcUnitData );

		if ( pcUserData->iMapID != MAPID_Bellatra )
			iExp += (iExp * (*(int*)0x0084601C)) / 100;

		//EXP Party
		if( (pcUser->bParty && pcUser->psParty && pcUser->psParty->pcLeader) && pcUser->psParty->iMembersCount > 1 )
		{
			PartyInfo * psParty = pcUser->psParty;

			int iPercent = 0;

			int iMembersCountAvg = 1;

			//Get Level Average
			int iLevelAvg = pcUserData->sCharacterData.iLevel;
			for ( size_t i = 0; i < MAX_PARTY_MEMBERS - 1; i++ )
			{
				User * p = psParty->pcaMembers[i];
				if ( p && p->pcUserData->pcSocketData && (p != pcUser) )
				{
					if ( IsUserInRangeParty( pcUser, p ) )
					{
						iLevelAvg += p->pcUserData->sCharacterData.iLevel;
						iMembersCountAvg++;
					}
				}
			}

			if ( psParty->pcLeader && (pcUser != psParty->pcLeader) )
			{
				if ( IsUserInRangeParty( pcUser, psParty->pcLeader ) )
				{
					iLevelAvg += psParty->pcLeader->pcUserData->sCharacterData.iLevel;
					iMembersCountAvg++;
				}
			}

			iLevelAvg /= iMembersCountAvg;

			//Percentage to EXP Mode, Normal Mode and Hunt Mode
			int iMembersPercentCount = iMembersCountAvg >= 2 ? (iMembersCountAvg - 2) : 0;
			if ( psParty->iMode == PARTYMODE_EXP )
			{
				iPercent = 180 + (80 * iMembersPercentCount);
				if ( iMembersCountAvg == 1 )
					iPercent = 100;
			}
			else if ( psParty->iMode == PARTYMODE_Normal )
			{
				iPercent = 140 + (40 * iMembersPercentCount);
				if ( iMembersCountAvg == 1 )
					iPercent = 100;
			}
			else if ( psParty->iMode == PARTYMODE_Hunt )
				iPercent = 80 + (20 * iMembersPercentCount);

			iExp = ((iExp * iPercent) / 100) / iMembersCountAvg;
			INT64 iExpParty = iExp;
			
			//Send EXP to Users
			for( size_t i = 0; i < MAX_PARTY_MEMBERS; i++ )
			{
				User * p = i == 0 ? psParty->pcLeader : psParty->pcaMembers[i-1];
				if( p )
				{
					if ( p->pcUserData->sCharacterData.iLevel > pcUnitData->sCharacterData.iLevel )
						iExpParty = GetExpLevelDifference( p->pcUserData->sCharacterData.iLevel, pcUnitData->sCharacterData.iLevel, iExp );

					int iX = (p->pcUserData->sPosition.iX - pcUserData->sPosition.iX) >> 8;
					int iZ = (p->pcUserData->sPosition.iZ - pcUserData->sPosition.iZ) >> 8;
					int iDist = iX*iX + iZ*iZ;

					INT64 iGain = 0;

					//EXP Pot
					if( p->iEXPPotion > 0 )
						iGain = (iExpParty * p->iEXPPotion) / 100;

					if ( p->bPvPMode )
					{
						iGain += ((iExpParty + iGain) * 3) / 100;

						CHATSERVER->SendDebugChat( p->pcUserData, CHATCOLOR_Error, "> PvP Mode EXP +%d%%", 3 );
					}

					//Is near distance?
					if( iDist < PARTY_GETTING_DIST && p->GetMapID() == pcUserData->iMapID )
					{
						int iLevel = p->pcUserData->sCharacterData.iLevel;
						if( iLevel >= 0 && iLevel <= FREE_EXP_EVENT_LEVEL && bFreeExpEvent )
						{
							iGain += ((iExpParty + iGain) * iExpFreeExp) / 100;
							CHATSERVER->SendChatEx( p, CHATCOLOR_Notice, "> Free EXP +%d%%", iExpFreeExp );
						}

						CHARACTERSERVER->GiveEXP( p, iExpParty + iGain );

						if ( p->iEXPPotion > 0 )
							CHATSERVER->SendDebugChat( p->pcUserData, CHATCOLOR_Error, "> ExpUp +%d%%", p->iEXPPotion );

						CHATSERVER->SendChatEx( p, CHATCOLOR_Notice, "> Gained %s exp [%d%% / %d] in party!", FormatNumber( iExpParty + iGain ), iPercent, iMembersCountAvg );
					}
				}
			}

			//Raid & Party Kill
			if ( psParty->iRaidCount > 0 )
			{
				for ( int i = 0; i < MAX_RAID; i++ )
				{
					PartyInfo * psP = i == 0 ? psParty->pcRaidLeader : psParty->pcaRaid[i - 1];
					for ( size_t j = 0; j < MAX_PARTY_MEMBERS; j++ )
					{
						User * p = j == 0 ? psP->pcLeader : psP->pcaMembers[j - 1];
						if ( p )
						{
							int iX = (p->pcUserData->sPosition.iX - pcUserData->sPosition.iX) >> 8;
							int iZ = (p->pcUserData->sPosition.iZ - pcUserData->sPosition.iZ) >> 8;
							int iDist = iX*iX + iZ*iZ;

							//Is near distance?
							if ( iDist < PARTY_GETTING_DIST && p->GetMapID() == pcUserData->iMapID )
							{
								if ( pcUnitData->pcMap && pcUnitData->pcMap->pcBaseMap )
									ProcessPacketKillUnitID( p, pcUnitData->iID, pcUnitData->pcMap->pcBaseMap->iMapID );
							}
						}
					}
				}
			}
			else
			{
				for ( size_t i = 0; i < MAX_PARTY_MEMBERS; i++ )
				{
					User * p = i == 0 ? psParty->pcLeader : psParty->pcaMembers[i - 1];
					if ( p )
					{
						int iX = (p->pcUserData->sPosition.iX - pcUserData->sPosition.iX) >> 8;
						int iZ = (p->pcUserData->sPosition.iZ - pcUserData->sPosition.iZ) >> 8;
						int iDist = iX*iX + iZ*iZ;

						//Is near distance?
						if ( iDist < PARTY_GETTING_DIST && p->GetMapID() == pcUserData->iMapID )
						{
							if ( pcUnitData->pcMap && pcUnitData->pcMap->pcBaseMap )
								ProcessPacketKillUnitID( p, pcUnitData->iID, pcUnitData->pcMap->pcBaseMap->iMapID );
						}
					}
				}
			}
		}
		else
		{
			if ( pcUserData->sCharacterData.iLevel > pcUnitData->sCharacterData.iLevel )
				iExp = GetExpLevelDifference( pcUserData->sCharacterData.iLevel, pcUnitData->sCharacterData.iLevel, iExp );

			if ( pcUser->iEXPPotion > 0 )
			{
				INT64 iGain = (iExp * pcUser->iEXPPotion) / 100;
				iExp += iGain;
			}

			if ( pcUser->bPvPMode )
			{
				iExp += (iExp * 3) / 100;

				CHATSERVER->SendDebugChat( pcUser->pcUserData, CHATCOLOR_Error, "> PvP Mode EXP +%d%%", 3 );
			}

			//Quest Increase EXP
			iExp += QUESTSERVER->GetQuestEXPOnKill( pcUser, pcUnitData, iExp );

			int iLevel = pcUserData->sCharacterData.iLevel;
			if ( iLevel >= 0 && iLevel <= FREE_EXP_EVENT_LEVEL && bFreeExpEvent )
			{
				iExp += (iExp * iExpFreeExp) / 100;
				CHATSERVER->SendChatEx( USERDATATOUSER( pcUserData ), CHATCOLOR_Notice, "> Free EXP +%d%%", iExpFreeExp );
			}
			CHARACTERSERVER->GiveEXP( USERDATATOUSER( pcUserData ), iExp );

			if ( pcUser->iEXPPotion > 0 )
				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> ExpUp +%d%%", pcUser->iEXPPotion );

			if ( pcUnitData->pcMap && pcUnitData->pcMap->pcBaseMap )
				ProcessPacketKillUnitID( pcUser, pcUnitData->iID, pcUnitData->pcMap->pcBaseMap->iMapID );
			
			CHATSERVER->SendChatEx( USERDATATOUSER( pcUserData ), CHATCOLOR_Notice, "> Gained %s Exp", FormatNumber( iExp ) );
		}
	}
}

void UnitServer::OnSetDrop( UserData * pcUserData, UnitData * pcUnitData )
{
	User * pcUser = USERDATATOUSER( pcUserData );

	if ( ACTIONFIELDHANDLER->IsOnInstance( UNITDATATOUNIT( pcUnitData ) ) )
		return;

	if( pcUserData && pcUnitData && pcUser )
	{
		//Premium?
		{
			ItemPremium * psPremium = ITEMSERVER->GetPremiumUse( pcUserData, ITEMTIMERTYPE_ThirdEye );
			if ( psPremium && psPremium->dwTimeLeft )
			{
				int iDropCount = pcUnitData->sUnitInfo.iNumDrops;
				pcUnitData->sUnitInfo.iNumDrops++;
				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> ThirdEye +1 Drop [ %d -> %d ]", iDropCount, pcUnitData->sUnitInfo.iNumDrops );
			}
		}
		{
			ItemPremium * psPremium = ITEMSERVER->GetPremiumUse( pcUserData, ITEMTIMERTYPE_ChemistHat );
			if ( psPremium && psPremium->dwTimeLeft )
			{
				int iDropCount = pcUnitData->sUnitInfo.iNumDrops;
				pcUnitData->sUnitInfo.iNumDrops++;
				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> Chemist Hat +1 Drop [ %d -> %d ]", iDropCount, pcUnitData->sUnitInfo.iNumDrops );
			}
		}

		{
			if ( pcUser->bPvPMode && RandomI( 0, 99 ) < 3 )
			{
				int iDropCount = pcUnitData->sUnitInfo.iNumDrops;
				pcUnitData->sUnitInfo.iNumDrops++;
				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> Pk Mode +1 Drop [ %d -> %d ]", iDropCount, pcUnitData->sUnitInfo.iNumDrops );
			}
		}

		if ( EVENT_PVPMODE )
		{
			if ( (RATE_DROPPVPMODE > 0) && (RATE_DROPPVPMODE > (rand() % 100)) )
			{
				int iDropCount = pcUnitData->sUnitInfo.iNumDrops;
				pcUnitData->sUnitInfo.iNumDrops++;
				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> PvP Server +1 Drop [ %d -> %d ]", iDropCount, pcUnitData->sUnitInfo.iNumDrops );
			}
		}

		//Is in Party?
		if( (pcUser->bParty && pcUser->psParty) && pcUser->psParty->iMembersCount > 1 && pcUser->psParty->iMode != PARTYMODE_Normal )
		{
			int iMembers = 1;
			for( int i = 0; i < MAX_PARTY_MEMBERS; i++ )
			{
				User * p = i == 0 ? pcUser->psParty->pcLeader : pcUser->psParty->pcaMembers[i-1];
				if( p && p != pcUser && p->pcUserData->pcSocketData )
				{
					int iX = (p->pcUserData->sPosition.iX - pcUserData->sPosition.iX) >> 8;
					int iZ = (p->pcUserData->sPosition.iZ - pcUserData->sPosition.iZ) >> 8;

					int iDistance = iX*iX + iZ*iZ;

					if( iDistance < PARTY_GETTING_DIST )
						iMembers++;
				}
			}

			//Party Drop
			if( iMembers > 1 )
			{
				int iRand = 0;

				if( iMembers == 2 )
					iRand = pcUser->psParty->iMode == PARTYMODE_EXP ? 10 : 10;
				else if( iMembers == 3 )
					iRand = pcUser->psParty->iMode == PARTYMODE_EXP ? 20 : 20;
				else if( iMembers == 4 )
					iRand = pcUser->psParty->iMode == PARTYMODE_EXP ? 40 : 30;
				else if( iMembers == 5 )
					iRand = pcUser->psParty->iMode == PARTYMODE_EXP ? 60 : 40;
				else if( iMembers == 6 )
					iRand = pcUser->psParty->iMode == PARTYMODE_EXP ? 80 : 50;

				bool bExtra = RandomI( 0, 100 ) < iRand ? true : false;

				int iDropCount = pcUnitData->sUnitInfo.iNumDrops;
				if( pcUser->psParty->iMode == PARTYMODE_EXP )
					pcUnitData->sUnitInfo.iNumDrops -= bExtra ? 2 : 1;
				else
					pcUnitData->sUnitInfo.iNumDrops += bExtra ? 2 : 1;

				CHATSERVER->SendDebugChat( pcUserData, CHATCOLOR_Error, "> Party Drop [ %d -> %d ]", iDropCount, pcUnitData->sUnitInfo.iNumDrops );
			}
		}

		//Quest Reduce Drop
		if ( pcUnitData->sUnitInfo.iNumDrops > 1 )
			pcUnitData->sUnitInfo.iNumDrops -= QUESTSERVER->GetReduceQuestDrop( pcUser, pcUnitData );

		pcUnitData->sUnitInfo.iNumDrops += (*(int*)0x0084602C);
	}
}

void UnitServer::OnSetTargetPosition( UserData * pcUserData, UnitData * pcUnitData )
{
	if ( pcUserData && pcUnitData )
	{
		if ( FURYARENAHANDLER->InsideArena( &pcUnitData->sPosition ) )
		{
			FURYARENAHANDLER->IsBoss( pcUnitData );
			return;
		}
		
		Unit * pcUnit = UNITDATATOUNIT( pcUnitData );
		
		if ( pcUnitData->iID != pcUnit->iID )
		{
			pcUnit->iID						= pcUnitData->iID;
			pcUnit->dwTimePositionUpdate	= TICKCOUNT;
		}

		int iX = (pcUnit->sLastRunningPosition.iX - pcUnitData->sPosition.iX) >> 8;
		int iZ = (pcUnit->sLastRunningPosition.iZ - pcUnitData->sPosition.iZ) >> 8;
		int iDistance = (iX*iX + iZ*iZ);

		int iMotionCode = pcUnitData->psModelAnimation ? (*(UINT*)pcUnitData->psModelAnimation) : 0;

		if ( (abs( iX ) > 4 || abs( iZ ) > 4) || (iMotionCode != 0x40 && iMotionCode != 0x50 && iMotionCode != 0x60) )
		{
			pcUnit->dwTimePositionUpdate = TICKCOUNT;
		}

		pcUnit->sLastRunningPosition.iX = pcUnitData->sPosition.iX;
		pcUnit->sLastRunningPosition.iZ = pcUnitData->sPosition.iZ;


		typedef void( __thiscall *tfnSetNextTarget ) (UnitData * pcUnitData, int iX, int iY, int iZ);
		tfnSetNextTarget fnSetNextTarget = (tfnSetNextTarget)0x00416A90;
		fnSetNextTarget( pcUnitData, pcUserData->sPosition.iX, pcUserData->sPosition.iY, pcUserData->sPosition.iZ );

		short sCharacterHP = pcUserData->sCharacterData.sHP.sCur;
		if ( pcUnitData->iSpecialType == FALSE && pcUnitData->iDamageLastAttacker && (pcUnitData->dwDamageLastTime + 2000) > TICKCOUNT )
		{
			if ( sCharacterHP > 0 && pcUnitData->sCharacterData.iMonsterType != 0x0A0 && (pcUnit->dwTimePositionUpdate + 10000) < TICKCOUNT && pcUnitData->sCharacterData.sHP.sCur != 0 && (iMotionCode == 0x50 || iMotionCode == 0x60 || iMotionCode == 0x40) )
			{

				int iY = abs( ((int)CALLT_WITH_ARG2( 0x004694D0, (DWORD)pcUnitData->pcStage, pcUserData->sPosition.iX, pcUserData->sPosition.iZ ) >> 8) - (pcUserData->sPosition.iY >> 8));

				if ( iY <= 120 )
				{
					pcUnitData->sPosition.iX = pcUserData->sPosition.iX;
					pcUnitData->sPosition.iY = pcUserData->sPosition.iY;
					pcUnitData->sPosition.iZ = pcUserData->sPosition.iZ;

					fnSetNextTarget( pcUnitData, 0, 0, 0 );
					pcUnitData->SetAngleByPosition( pcUserData->sPosition.iX >> 8, pcUserData->sPosition.iZ >> 8 );
					pcUnitData->Animate( ANIMATIONTYPE_Idle );
				}
				else
				{
					pcUnitData->sPosition.iX = pcUnitData->sSpawnPosition.iX;
					pcUnitData->sPosition.iZ = pcUnitData->sSpawnPosition.iZ;
					pcUnitData->sPosition.iY = CALLT_WITH_ARG2( 0x004694D0, (DWORD)pcUnitData->pcStage, pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ );

					fnSetNextTarget( pcUnitData, 0, 0, 0 );
					pcUnitData->SetAngleByPosition( pcUserData->sPosition.iX >> 8, pcUserData->sPosition.iZ >> 8 );
					pcUnitData->Animate( ANIMATIONTYPE_Idle );
				}

				pcUnit->dwTimePositionUpdate = TICKCOUNT;
			}
		}
	}
}

BOOL UnitServer::IsMultiplyHPMonter( UnitData * pcUnitData )
{
	BOOL bMultiply = FALSE;

	// Monsters from effect ID
	switch ( pcUnitData->sCharacterData.iMonsterEffectID )
	{
		case MONSTEREFFECTID_DEATHKNIGHT:
		case MONSTEREFFECTID_KELVEZU:
		case MONSTEREFFECTID_MOKOVA:
		case MONSTEREFFECTID_TULLA:
		case MONSTEREFFECTID_DarkGuardian:
		case MONSTEREFFECTID_GORGON:
		case MONSTEREFFECTID_GOBLINSHAMAN:
		case MONSTEREFFECTID_Yagditha:
		case MONSTEREFFECTID_Draxos:
		case MONSTEREFFECTID_DEVILSHY:
		case MONSTEREFFECTID_Igniculus:
		case MONSTEREFFECTID_CHAOSCARA:
		case MONSTEREFFECTID_Pelgia:
		case MONSTEREFFECTID_Greedy:
		case MONSTEREFFECTID_Flamo:
		case MONSTEREFFECTID_Hagios:
			bMultiply = TRUE;
			break;
		default:
			break;
	}

	//By Name
	if ( STRINGCOMPARE( "King Fury", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Red Fury Spirit", pcUnitData->sCharacterData.szName ) ||
		 STRINGCOMPARE( "Green Fury Spirit", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Blue Fury Spirit", pcUnitData->sCharacterData.szName ) ||
		 STRINGCOMPARE( "King Babel", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Lord Santa Skeleton", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE("Lizard Commander", pcUnitData->sCharacterData.szName) ||
		 STRINGCOMPARE( "King Valdo", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Argonian", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Ancient", pcUnitData->sCharacterData.szName ) ||
		 STRINGCOMPARE( "Flamo", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "King Pumpkin Ghost", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Volcanos", pcUnitData->sCharacterData.szName ) ||
		 STRINGCOMPARE( "Mini Volcanos", pcUnitData->sCharacterData.szName ) || STRINGCOMPARE( "Mini Ancient", pcUnitData->sCharacterData.szName ) )
		bMultiply = TRUE;

	pcUnitData->iHPBlocks		= 0;
	pcUnitData->sHPBlocks.sCur	= (short)pcUnitData->iHPBlocks;
	pcUnitData->sHPBlocks.sMax	= (short)pcUnitData->iHPBlocks;
	pcUnitData->sVirtualHP.sCur	= (short)pcUnitData->iHPBlocks;
	pcUnitData->sVirtualHP.sMax	= (short)pcUnitData->iHPBlocks;

	// Multiply HP?
	if ( bMultiply )
	{
		pcUnitData->iHPBlocks					= pcUnitData->sUnitInfo.iMaxPotions;
		pcUnitData->sHPBlocks.sCur				= (short)pcUnitData->iHPBlocks;
		pcUnitData->sHPBlocks.sMax				= (short)pcUnitData->iHPBlocks;
		pcUnitData->sVirtualHP.sCur				= (short)pcUnitData->iHPBlocks;
		pcUnitData->sVirtualHP.sMax				= (short)pcUnitData->iHPBlocks;
	}
	else
	{
		// Set potion percent, maybe pot anim
		if ( pcUnitData->sUnitInfo.iMaxPotions )
		{
			if ( (rand() % 100) < pcUnitData->sUnitInfo.iPerPotions )
			{
				pcUnitData->sUnitInfo.iMaxPotions = (rand() % pcUnitData->sUnitInfo.iMaxPotions) + 1;
			}
			else
			{
				pcUnitData->sUnitInfo.iMaxPotions = 0;
			}
		}
	}
	return bMultiply;
}

NAKED BOOL UnitServer::GetTop10Damage( UnitData * pcUnitData, AttackDamageData * psaDamageData )
{
	JMP( pfnGetTop10DamageUnitData );
}

BOOL UnitServer::HandleItemDrop( UserData * pcUserData, UnitData * pcUnitData, Map::Loot::LootItem * psItemLoot )
{
	if ( (pcUnitData->sUnitInfo.iDropPercentNum == 0) || (pcUnitData->sUnitInfo.iNumDrops == 0) )
		return FALSE;

	BOOL bRet = FALSE;

	std::random_device rd;
	std::uniform_int_distribution<int> dist( 0, pcUnitData->sUnitInfo.iDropPercentNum );
	int iRandom = dist( rd );

	int iRandomCount = 0;
	for ( int i = 0; i < pcUnitData->sUnitInfo.iDropCount; i++ )
	{
		DropSettingsData * ps = pcUnitData->sUnitInfo.saDropData + i;
		if ( (iRandom >= iRandomCount) && (iRandom < (iRandomCount + ps->iPercent)) )
		{
			if ( ps->iItemID == ITEMID_None )
			{
				bRet = FALSE;
				break;
			}

			if ( ps->iItemID == ITEMID_Gold )
			{
				int iGold = (ps->sGoldMax + 1) - ps->sGoldMin;
				if ( iGold == 0 )
					iGold = ps->sGoldMax;
				else
					iGold = ps->sGoldMin + (rand() % iGold);

				psItemLoot->sItem.sItemID.SetItemID( ITEMID_Gold );
				
				STRINGFORMAT( psItemLoot->sItem.szItemName, "%d gp", iGold );
				psItemLoot->sItem.iGold = iGold;
				
				ITEMSERVER->ValidateItem( &psItemLoot->sItem );

				//Total Gold Dropped
				WRITEDWORD( 0x07AC9E1C, READDWORD( 0x07AC9E1C ) + iGold );
				
				bRet = TRUE;
				break;
			}

			DefinitionItem * psDefinitionItem = ITEMSERVER->FindItemDefByCode( ps->iItemID );
			if ( psDefinitionItem )
			{
				if ( psDefinitionItem->sItem.iItemUniqueID == 0 )
				{
					//Limit to create items per day...
					if ( psDefinitionItem->sGenDay[0] > 0 )
					{
						if ( psDefinitionItem->sGenDay[0] <= psDefinitionItem->sGenDay[1] )
						{
							bRet = FALSE;
							break;
						}
						else
							psDefinitionItem->sGenDay[1]++;
					}

					ITEMSERVER->CreateItem( &psItemLoot->sItem, psDefinitionItem );

					ITEMSERVER->SaveItemData( &psItemLoot->sItem );
					WRITEDWORD( 0x07AC9E18, READDWORD( 0x07AC9E18 ) + 1 );

					SQLSaveItemRare( USERDATATOUSER( pcUserData ), UNITDATATOUNIT( pcUnitData ), &psItemLoot->sItem );

					bRet = TRUE;
					break;
				}
			}
		}

		iRandomCount += ps->iPercent;
	}

	return bRet;
}

BOOL UnitServer::HandleKill( UnitData * pcUnitData, UserData * pcUserData )
{
	//Quest
	if ( pcUnitData->sUnitInfo.iQuestItemID )
	{
		BOOL bGetItem = TRUE;
		if ( pcUnitData->sUnitInfo.iQuestItemID == ITEMID_RoyalAmulet )
		{
			int iRandom = rand() % 100;
			if ( (pcUserData->sCharacterData.iRank < CHARACTERRANK_Rank4) )
			{
				if ( iRandom > 20 )
					bGetItem = FALSE;
			}
		}
		
		if ( bGetItem )
			ITEMSERVER->CreateItem( pcUserData, pcUnitData->sUnitInfo.iQuestItemID, 1 );
	}

	//Extra Drops
	for( int i = 0; i < pcUnitData->sUnitInfo.iDropPlusCount; i++ )
	{
		if( RandomI( 0, 10000 ) < pcUnitData->sUnitInfo.saDropDataCustom[i].iPercent )
		{
			Map::Loot::LootItem sLootItem;
			EItemID eItemID = (EItemID)pcUnitData->sUnitInfo.saDropDataCustom[i].iItemID;
			DefinitionItem * pItem = ITEMSERVER->FindItemDefByCode( eItemID );

			if( pItem && (pItem->sItem.iItemUniqueID == FALSE))
			{
				ITEMSERVER->CreateItem( &sLootItem.sItem, pItem );

				if ( (sLootItem.sItem.sItemID.ToItemType() == ITEMTYPE_Quest) || (sLootItem.sItem.sItemID.ToItemType() == ITEMTYPE_MorionT2Quest) )
				{
					ITEMSERVER->SendItemData( pcUserData, &sLootItem.sItem );
					break;
				}

				if( pcUnitData->dwExclusiveNum != 0x100 || (pItem->sItem.sItemID.ToItemType() != ITEMTYPE_GoldAndExp && pItem->sItem.sItemID.ToItemBase() != ITEMBASE_Potion) )
				{
					Map * pcMap = (Map*)pcUnitData->pcMap;
					if( pcMap )
					{
						Map::Loot * psItemD = pcMap->AddItem( &sLootItem, pcUnitData->sPosition.iX >> 8, pcUnitData->sPosition.iY >> 8, pcUnitData->sPosition.iZ >> 8, TRUE );
						if( psItemD )
						{
							if ( pcUnitData->sUnitInfo.bPublicDrop )
								MAPSERVER->SendItemStageNearUsers( pcMap, psItemD );
							else
								MAPSERVER->SendItemStageUser( pcMap, pcUserData, psItemD );

							psItemD->dwDropTime += 5000;
							break;
						}
					}
				}
			}
		}
	}

	//Set the quantity of drops
	OnSetDrop( pcUserData, pcUnitData );

	//Normal Drops
	int iDropIndex = 1;

	for( int i = 0; i < pcUnitData->sUnitInfo.iNumDrops; i++ )
	{
		Map::Loot::LootItem sItemLoot;
		if( HandleItemDrop( pcUserData, pcUnitData, &sItemLoot ) )
		{
			if ( (sItemLoot.sItem.sItemID.ToItemType() == ITEMTYPE_Quest) || (sItemLoot.sItem.sItemID.ToItemType() == ITEMTYPE_MorionT2Quest) )
			{
				ITEMSERVER->SendItemData( pcUserData, &sItemLoot.sItem );
				break;
			}

			if( pcUnitData->dwExclusiveNum != 0x100 || (sItemLoot.sItem.sItemID.ToItemType() != ITEMTYPE_GoldAndExp && sItemLoot.sItem.sItemID.ToItemBase() != ITEMBASE_Potion) )
			{
				sItemLoot.bUse = TRUE;

				Map * pcMap = (Map*)pcUnitData->pcMap;
				if( pcMap )
				{
					Map::Loot * psItemD = pcMap->AddItem( &sItemLoot, pcUnitData->sPosition.iX >> 8, pcUnitData->sPosition.iY >> 8, pcUnitData->sPosition.iZ >> 8, TRUE );
					if( psItemD )
					{
						psItemD->dwDropTime += 5000;

						if ( pcUnitData->sUnitInfo.bPublicDrop )
						{
							if ( ROLLDICEHANDLER->OnExchangeItemDrop( pcUnitData, pcMap, psItemD, iDropIndex++ ) == FALSE )
								MAPSERVER->SendItemStageNearUsers( pcMap, psItemD );
						}
						else
							MAPSERVER->SendItemStageUser( pcMap, pcUserData, psItemD );
					}
				}
			}
		}
	}

	//Has a Soul Crystal?
	if ( pcUserData->dwSoulCrystalTimeOut > TICKCOUNT )
	{
		if ( auto psCharacterData = UNITINFODATA->GetCharacterDataByMonsterName( pcUnitData->GetName() ) )
		{
			if ( psCharacterData->sGlow == FALSE )
			{
				if ( auto psItemDefinition = ITEMSERVER->FindItemDefByCode( ITEMID_SoulSummonCrystal ) )
				{
					ITEMSERVER->OnUseCrystalDie( pcUserData, 0x7FFFABBB );

					Item sItem;
					ITEMSERVER->CreateItem( &sItem, psItemDefinition );

					STRINGCOPY( sItem.szItemName, psCharacterData->szName );
					ITEMSERVER->ValidateItem( &sItem );

					ITEMSERVER->SendItemData( pcUserData, &sItem );

					pcUserData->dwSoulCrystalTimeOut = 0;
				}
			}
		}
	}

	//Send EXP from Monster
	OnSendExp( pcUserData, pcUnitData );

	//Update Monsters killed by User
	pcUserData->uUnitKillCounter++;

	return TRUE;
}

void UnitServer::MainUnitData( UnitData * pcUnitData )
{
	CALLT( 0x00429930, (DWORD)pcUnitData );
}

NAKED void UnitServer::SkillUnitDamage( UnitData * pcUnitData, UserData * pcUserData )
{
	JMP( pfnUnitDamageSkill );
}

NAKED void UnitServer::UnitSwapper( UnitData * pcUnitData, CharacterData * psCharacterData, UnitInfo * psUnitInfo )
{
	JMP( pfnUnitSwapper );
}

BOOL UnitServer::OnUnitChanger( UnitData * pcUnitData )
{
	CharacterData * psaCharacterData	= *(CharacterData**)0x07AB3050;
	UnitInfo * psaUnitInfo			= *(UnitInfo**)0x07AB22A4;

	int iCounterCharacterData			= (*(int*)0x07AC57E8);

	BOOL bRet							= FALSE;

	//Special Summon Effect?
	if ( pcUnitData->psModelAnimation->iType == ANIMATIONTYPE_Special && pcUnitData->iFrameTimer > 60 )
	{
		EMonsterEffectID eEffect			= (EMonsterEffectID)pcUnitData->sCharacterData.iMonsterEffectID;

		//Mini Hulk?
		if ( eEffect == MONSTEREFFECTID_HUNGKY )
		{
			//Find Hulk
			for ( int i = 0; i < iCounterCharacterData; i++ )
			{
				CharacterData * ps	= psaCharacterData + i;
				UnitInfo * psU = psaUnitInfo + i;

				//Hulk and not Crystal or Bellatra Monster?
				if ( ps->iMonsterEffectID == MONSTEREFFECTID_HULK && ps->szName[0] != ' ' )
				{
					if ( pcUnitData->iHits <= 80 )
					{
						if ( ps->iLevel < 80 )
						{
							//Change
							UnitSwapper( pcUnitData, ps, psU );
							bRet = TRUE;
							break;
						}
					}
					else if ( ps->iLevel >= 80 )
					{
						//Change
						UnitSwapper( pcUnitData, ps, psU );
						bRet = TRUE;
						break;
					}
				}
			}
		}

		//Mini Dead Hopy?
		if ( eEffect == MONSTEREFFECTID_DEADHOPT )
		{
			//Find Dead King Hopy
			for ( int i = 0; i < iCounterCharacterData; i++ )
			{
				CharacterData * ps	= psaCharacterData + i;
				UnitInfo * psU = psaUnitInfo + i;

				//Dead King Hopy and not Crystal or Bellatra Monster?
				if ( ps->iMonsterEffectID == MONSTEREFFECTID_DEADKINGHOPY && ps->szName[0] != ' ' )
				{
					if ( ps->iLevel >= 108 )
					{
						//Change
						UnitSwapper( pcUnitData, ps, psU );
						bRet = TRUE;
						break;
					}
				}
			}
		}
	}

	return bRet;
}

BOOL UnitServer::UpdateUnitData( UnitData * pcUnitData )
{
	//Cannot move? no Update...
	if ( pcUnitData->bNoMove )
		return TRUE;

	MainServerUnitData( pcUnitData );

	//Animated?
	if ( pcUnitData->psModelAnimation )
	{
		//Die?
		if ( pcUnitData->psModelAnimation->iType == ANIMATIONTYPE_Die )
		{
			//Handle Servers Update
			XMASHANDLER->OnSayTime( pcUnitData );
			EVENTSERVER->OnSayTime( pcUnitData );
			QUESTSERVER->OnSayTime( pcUnitData );

			//Bless Castle
			BLESSCASTLESERVER->OnUnitKilled( pcUnitData );

			//Killed by an User?
			if ( pcUnitData->pcKillerData && pcUnitData->iFrameTimer > 128 && !pcUnitData->pcOwner )
			{
				UserData * ud				= pcUnitData->pcKillerData;

				QUESTARENAHANDLER->OnUnitKilled( pcUnitData->pcKillerData, pcUnitData, TRUE );

				pcUnitData->pcKillerData	= NULL;

				//Have Last User Attack and not the same User?
				if ( pcUnitData->pcLastAttacker && pcUnitData->pcLastAttacker != ud )
				{
					//Unit received Damage more than 40% from Last User?
					if ( pcUnitData->iDamageLastAttacker >= ((pcUnitData->sCharacterData.sHP.sMax * 40) / 100) )
					{
						//Update User and discard the Killer User
						ud = pcUnitData->pcLastAttacker;
					}
				}

				//Quest Unit?
				if ( pcUnitData->sUnitInfo.iQuestItemID )
				{
					if ( pcUnitData->sUnitInfo.eSpecialType )
						MapServer::HandleUnitQuestMap( pcUnitData->pcMap, pcUnitData );
				}

				//User Alive?
				if ( ud->pcSocketData && ud->iID )
				{
					SERVEREVENTHANDLER->OnUnitKilled( USERDATATOUSER( ud ), UNITDATATOUNIT( pcUnitData ) );

					//Handle Kill
					HandleKill( pcUnitData, ud );

					//Increment Counter Unit Kills
					ud->uUnitKillCounter++;
				}

				//Update Top10 Damage on Bosses
				if ( pcUnitData->psaDamageUsersData )
				{
					if ( pcUnitData->pcKillerData )
					{
						QUESTARENAHANDLER->OnUnitKilled( pcUnitData->pcKillerData, pcUnitData, TRUE );
					}

					//Not Valhalla?
					if ( pcUnitData->sCharacterData.iMonsterEffectID != MONSTEREFFECTID_CASTLE_TOWER_B )
					{
						//Get Top10
						UnitServer::GetTop10Damage( pcUnitData, pcUnitData->psaDamageUsersData );
					}
				}

				//Bellatra User and Unit?
				if ( ud->dwExclusiveNum == 0x100 && pcUnitData->dwExclusiveNum == 0x100 )
				{
					//Don't Have Killer of Bellatra?
					if ( !pcUnitData->pcBellatraKiller )
					{
						//Score
						LARGE_INTEGER lsScore;
						lsScore.QuadPart	= UNITSERVER->GetExp( pcUnitData ) / EVENT_BELLATRA_DIVSCORE;

						//Room Kill
						BELLATRASERVER->UnitKilled( pcUnitData );

						if ( BELLATRASERVER->IsKilledUnitSameRoom( ud, pcUnitData ) )
						{
							//Boss?
							if ( pcUnitData->sCharacterData.sGlow == TRUE )
							{
								//Notify Kill
								CHATSERVER->SendChatEx( USERDATATOUSER( ud ), EChatColor::CHATCOLOR_Global, "Bellatra> %s killed!", pcUnitData->sCharacterData.szName );
								CHATSERVER->SendChatEx( USERDATATOUSER( ud ), EChatColor::CHATCOLOR_Global, "Bellatra> %s points", FormatNumber( lsScore.LowPart ) );
							}

							//Add Score
							ud->iBellatraScore += lsScore.LowPart;
							ud->iBellatraKills++;

							//Handle Score Team if is in Room
							if ( ud->sBellatraRoomID >= 0 && ud->sBellatraRoomID < 4 )
								BELLATRASERVER->AddRoomScore( (int)ud->sBellatraRoomID, lsScore.LowPart );

							//Update User Score
							BellatraServer::UpdateBellatraUserScore( ud );

							//User Personal Kills
							{
								PacketSimple s;
								s.iLength = sizeof( PacketSimple );
								s.iHeader = PKTHDR_KillsInSod;
								s.iUnk = ud->iBellatraKills;
								SENDPACKET( USERDATATOUSER( ud ), &s, TRUE );
							}

							//Handle Item Room
							BellatraServer::HandleItemRoom( pcUnitData, ud );
						}
					}
				}
			}

			//Unit disappearing?
			if ( pcUnitData->iFrameTimer > 200 )
			{
				//Clear
				DELET( pcUnitData->psaDamageUsersData );

				QUESTARENAHANDLER->OnUnitKilled( pcUnitData );

				BELLATRASERVERHANDLER->OnUnitKilledPost( UNITDATATOUNIT( pcUnitData ) );

				//Is a Pet and User Owner Pet is Online?
				if ( pcUnitData->pcOwner && pcUnitData->pcOwner->pcSocketData )
					OnBeforeCrystalKill( pcUnitData );

				Map * pcMap = (Map *)pcUnitData->pcMap;

				//Clear
				DELET( pcUnitData->psaDamageUsersData );

				//Close Unit
				pcUnitData->Free();

				//Decrease Monster Counter on Map and delete from Spawn Flag
				pcMap->iNumAliveUnitData--;
				MAPSERVER->DelMonsterSpawn( pcMap, pcUnitData );

				//Done
				return TRUE;
			}
		}

		//Unit Swapper
		OnUnitChanger( pcUnitData );
	}

	//Unit Hitted?
	if ( pcUnitData->bMonsterAttacking )
	{
		pcUnitData->bMonsterAttacking = FALSE;
		//Reset
		if ( pcUnitData->pcFocusTarget )
			pcUnitData->pcFocusTarget->iUnitHitCounter = 0;
	}

	//If Negative, set zero
	if ( pcUnitData->sCharacterData.sHP.sCur < 0 )
		pcUnitData->sCharacterData.sHP.sCur = 0;

	//Min > Max, set Max
	if ( pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax )
		pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;

	//Reload Timer to Appear
	pcUnitData->iDeathCounter = 256;

	if ( FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
		pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;

	//Update Unit Status and Scout Enemies?
	if ( READBYTE( 0x07AC9D68 ) != 0 )
	{
		//Update Animation Data
		Unit * pcUnit = UNITDATATOUNIT( pcUnitData );
		pcUnitData->HandleUnitDataBuffer( (char*)pcUnit->baUnitBufferNew, 16, 4 );
		OnHandleUnitDataBufferNew( pcUnit, (PacketPlayData*)pcUnit->baUnitBufferNew );

		int iX								= pcUnitData->sPosition.iX;
		int iY								= pcUnitData->sPosition.iY;
		int iZ								= pcUnitData->sPosition.iZ;

		int iDistanceMin					= 0x00320000;

		Map * psStageData	= (Map *)pcUnitData->pcMap;

		//On Stage and have Owner User?
		if( psStageData && pcUnitData->pcOwner )
		{
			UnitData * pcTarget = NULL;

			BOOL bCanAttack		= FALSE;

			//Set Target for Pet
			for ( int i = 0; i < psStageData->iNumUsingUnitData; i++ )
			{
				UnitData * ps	= (UnitData*)psStageData->pcaUnitData[i];

				//Alive and not the same Unit?
				if ( ps && ps != pcUnitData && ps->sCharacterData.sHP.sCur > 0 )
				{
					//Both is Pet?
					if ( pcUnitData->pcOwner && ps->pcOwner )
						continue;

					//Boss?
					if ( (ps->sCharacterData.sGlow == TRUE) && pcUnitData->psModelAnimation && ps->psModelAnimation &&
						(pcUnitData->psModelAnimation->iType != ANIMATIONTYPE_Die) && (ps->psModelAnimation->iType != ANIMATIONTYPE_Die) &&
						 (FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) == false) && (pcUnitData->pcOwner && (pcUnitData->sCharacterData.sGlow != TRUE)) )
					{
						//Relative Position
						int iX = (pcUnitData->sPosition.iX - ps->sPosition.iX) >> 8;
						int iY = (pcUnitData->sPosition.iY - ps->sPosition.iY) >> 8;
						int iZ = (pcUnitData->sPosition.iZ - ps->sPosition.iZ) >> 8;

						//Calculate Distance
						int iDistance = (iX*iX) + (iY*iY) + (iZ*iZ);

						//Unit Near from Pet?
						if ( iDistance < 0x4B000 && abs( iX ) < (64 * 64) && abs( iZ ) < (64 * 64) )
						{
							int iEffectID = ps->sCharacterData.iMonsterEffectID;

							if ( iEffectID != MONSTEREFFECTID_CASTLE_TOWER_B &&
								iEffectID != MONSTEREFFECTID_CASTLE_DOOR &&
								iEffectID != MONSTEREFFECTID_CASTLE_CRYSTAL_R &&
								iEffectID != MONSTEREFFECTID_CASTLE_CRYSTAL_G &&
								iEffectID != MONSTEREFFECTID_CASTLE_CRYSTAL_B &&
								iEffectID != MONSTEREFFECTID_CASTLE_CRYSTAL_N &&
								iEffectID != MONSTEREFFECTID_CASTLE_SOLDER_A &&
								iEffectID != MONSTEREFFECTID_CASTLE_SOLDER_B &&
								iEffectID != MONSTEREFFECTID_CASTLE_SOLDER_C )
							{
								//Kill
								pcUnitData->sCharacterData.sHP.sCur = 0;
								pcUnitData->Animate( ANIMATIONTYPE_Die );
								return TRUE;
							}
						}
					}

					if ( pcUnitData->iPetID && !pcUnitData->bAggressive && pcUnitData->pcOwner )
					{
						BOOL bContinue = FALSE;

						//User Owner Pet not targetting same Unit? go to the next Target
						if ( pcUnitData->pcOwner->pcLastAttack != ps )
							bContinue = TRUE;

						if ( pcUnitData->pcFocusingTarget )
						{
							if ( (pcUnitData->pcFocusingTarget == ps) && (ps->iID == pcUnitData->iFocusingLastID) )
								bContinue = FALSE;
						}
						
						if ( bContinue )
							continue;
					}
					
					//Can Attack Unit
					bCanAttack		= TRUE;

					//Unit is the same Clan Pet? then cannot Attack
					if ( pcUnitData->sCharacterData.iClanID && pcUnitData->sCharacterData.iClanID == ps->sCharacterData.iClanID )
						bCanAttack	= FALSE;

					//Check Unit Clan SW Master Type
					if ( pcUnitData->iPetID && pcUnitData->bBlessCastleCrown && ps->bBlessCastleCrown )
					{
						//Pet? then cannot Attack
						if ( ps->iPetID )
							bCanAttack = FALSE;
						
						//Unit Pet is not of User? then cannot Attack
						if ( ps->lExclusiveTargetID && ps->lExclusiveTargetID != pcUnitData->pcOwner->iID )
							bCanAttack = FALSE;
					}

					//SW Pet? then cannot Attack
					if ( pcUnitData->bBlessCastleCrown && ps->bBlessCastleCrown )
						bCanAttack = FALSE;

					//Unit Exclusive? then cannot Attack
					if ( pcUnitData == ps->pcExclusiveUnitTarget || pcUnitData->pcExclusiveUnitTarget == ps )
						bCanAttack = FALSE;

					//Unit is Follow Type?
					if ( FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
						bCanAttack = FALSE;

					if ( bCanAttack && (pcUnitData->pcOwner->iMapID == MAPID_Bellatra) )
						bCanAttack = ROOMINSTANCEHANDLER->CanSee( pcUnitData->pcOwner->iID, ps->iID );

					//Can Attack?
					if ( bCanAttack )
					{
						//Priority to this Target
						if ( pcUnitData->pcFocusingTarget == ps )
						{
							if ( ps->iID == pcUnitData->iFocusingLastID )
							{
								pcTarget = pcUnitData->pcFocusingTarget;
								break;
							}
							else
							{
								pcUnitData->pcFocusingTarget	= NULL;
								pcUnitData->iFocusingLastID		= -1;
							}
						}


						//Relative Position
						int iX			= (pcUnitData->sPosition.iX - ps->sPosition.iX) >> 8;
						int iY			= (pcUnitData->sPosition.iY - ps->sPosition.iY) >> 8;
						int iZ			= (pcUnitData->sPosition.iZ - ps->sPosition.iZ) >> 8;

						//Calculate Distance
						int iDistance	= (iX*iX) + (iY*iY) + (iZ*iZ);

						//Unit Near from Pet?
						if ( iDistance < DISTANCE_UNIT_FAR && abs( iX ) < (64 * 64) && abs( iZ ) < (64 * 64) )
						{
							//Unit Near from Minimal Distance and Not bove or above from Pet?
							if ( iDistance < iDistanceMin && abs( iY ) < 140 )
							{
								//Update Minimal Distance
								iDistanceMin = iDistance;

								//Pet can view Unit?
								if( iDistance < pcUnitData->sCharacterData.iSight )
								{
									//Set Pet Target
									pcTarget = ps;
								}
							}
						}
					}
				}
			}	

			//Update Target
			pcUnitData->pcAttackTarget = pcTarget;

			//Have Unit Target?
			if ( pcTarget )
			{
				//Update Position to Target Position
				pcUnitData->NextTarget( pcTarget->sPosition.iX, pcTarget->sPosition.iY, pcTarget->sPosition.iZ );
			}

			//Time Alive is Over?
			if ( pcUnitData->dwLastUpdateTime && pcUnitData->dwLastUpdateTime < TICKCOUNT && pcUnitData->sCharacterData.sHP.sCur > 0 )
			{
				if ( !FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
				{
					//Kill Unit
					pcUnitData->sCharacterData.sHP.sCur = 0;
					pcUnitData->Animate( ANIMATIONTYPE_Die );
				}
			}
		}
		else
		{
			BOOL bNextTarget = FALSE;

			//Quest Arena Check
			if ( QUESTARENAHANDLER->IsUpdatedUnit( pcUnitData ) )
			{
				UnitData * pcTarget = pcUnitData->pcAttackTarget;

				//Update Position to Target Position
				pcUnitData->NextTarget( pcTarget->sPosition.iX, pcTarget->sPosition.iY, pcTarget->sPosition.iZ );

				bNextTarget = TRUE;
			}

			//Unit Follow Type?
			if ( FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
				bNextTarget = TRUE;

			//Unit received Compulsion Skill?
			if ( pcUnitData->dwHookedTargetEndTime && (pcUnitData->sCharacterData.sGlow == 0) )
			{
				UserData * ud = pcUnitData->pcHookedTarget;

				//Compulsion Alive?
				if ( pcUnitData->dwHookedTargetEndTime > TICKCOUNT && ud && ud->pcSocketData )
				{
					//Set Focus on User
					pcUnitData->pcFocusTarget = ud;

					//Update Position to Target Position
					pcUnitData->NextTarget( ud->sPosition.iX, ud->sPosition.iY, ud->sPosition.iZ );

					//Find Next Target
					bNextTarget = TRUE;
				}
				else
				{
					//Clear Compulsion Skill
					pcUnitData->dwHookedTargetEndTime	= 0;
					pcUnitData->pcHookedTarget			= NULL;
				}
			}

			//Next Target?
			if ( !bNextTarget )
			{
				//Not Target?
				if ( !pcUnitData->pcAttackTarget )
				{
					//Find Users Near
					UserData * pcTarget = NULL;

					for ( int i = 0; i < PLAYERS_MAX; i++ )
					{
						UserData * ud = USERSDATA + i;
						User * u = USERDATATOUSER( ud );

						if ( ud->pcSocketData && ud->iID && ud->sCharacterData.sHP.sCur && ud->sCharacterData.iType != 1 && ud->bCloak == FALSE && ROOMINSTANCEHANDLER->CanSee( pcUnitData->iID, ud->iID ) )
						{
							//Not Exlusive User? Next Target...
							if ( pcUnitData->lExclusiveTargetID && pcUnitData->lExclusiveTargetID != ud->iID )
								continue;

							//Unit is Pet Clan of User? Next Target...
							if ( pcUnitData->iClanID && pcUnitData->iClanID == ud->iClanID )
								continue;

							//Relative Position
							int iX = (pcUnitData->sPosition.iX - ud->sPosition.iX) >> 8;
							int iY = (pcUnitData->sPosition.iY - ud->sPosition.iY) >> 8;
							int iZ = (pcUnitData->sPosition.iZ - ud->sPosition.iZ) >> 8;

							//Calculate Distance
							int iDistance = (iX*iX) + (iY*iY) + (iZ*iZ);

							//User Near from Unit?
							if ( iDistance < DISTANCE_UNIT_FAR && abs( iX ) < (64 * 64) && abs( iZ ) < (64 * 64) )
							{
								//Area Skill?
								if ( pcUnitData->iAreaDamageArea && abs( iY ) < 140 )
								{
									//Inside Area Skill?
									if ( pcUnitData->sUnitInfo.iSkillPierceRange && pcUnitData->sUnitInfo.iSkillBoxRight && pcUnitData->sUnitInfo.iSkillBoxLeft )
									{
										//Inside X?
										if ( ud->sPosition.iX > pcUnitData->sUnitInfo.iSkillBoxLeft && ud->sPosition.iX < pcUnitData->sUnitInfo.iSkillBoxRight )
										{
											//Inside Z?
											if ( ud->sPosition.iZ > pcUnitData->sUnitInfo.iSkillBoxTop && ud->sPosition.iZ < pcUnitData->sUnitInfo.iSkillBoxBottom )
											{
												//Send Damage to User
												SkillUnitDamage( pcUnitData, ud );
											}
										}
									}
									else if ( iDistance < pcUnitData->iAreaDamageArea )
									{
										//Send Damage to User
										SkillUnitDamage( pcUnitData, ud );
									}
								}

								//Distance minor than Minimal Distance?
								if ( iDistance < iDistanceMin && abs( iY ) < 140 )
								{
									//New Distance relative to View Sight of Unit
									int iDistanceN = pcUnitData->sCharacterData.iSight;

									//Vanish Effect
									DWORD dwTimeEnd = ud->dwVanishEndTime;
									
									if ( dwTimeEnd == 0 )
										dwTimeEnd = u->dwBlindTime;

									//Vanish?
									if ( dwTimeEnd && dwTimeEnd > TICKCOUNT )
									{
										//Not Boss?
										if ( pcUnitData->sCharacterData.sGlow != 1 )
										{
											//Vanish Distance can affect Unit
											iDistanceN = (pcUnitData->sUnitInfo.iSight * (int)ud->saVanishParam[1]) / 100;
											iDistanceN += pcUnitData->sUnitInfo.iSight;

											//Compute Sight Distance
											if ( iDistanceN > 0 )
												iDistanceN *= iDistanceN;
											else
												iDistanceN = 0;
										}
									}
									else
									{
										//Clear Vanish
										ud->dwVanishEndTime = 0;
										u->dwBlindTime = 0;
									}

									//Not Hidden and on distance?
									if ( iDistance < iDistanceN && !ud->bCloak )
									{
										//Both in Bellatra?
										if ( pcUnitData->dwExclusiveNum == 0x100 )
										{
											if ( ud->dwExclusiveNum == 0x100 )
											{
												//Target User
												pcTarget = ud;
											}
										}
										else
											pcTarget = ud;

										//Update Minimal Distance
										iDistanceMin = iDistance;
									}
								}
							}
						}
					}

					//Freeze Stun? No Target
					if ( pcUnitData->iStunTimeLeft )
						pcTarget = NULL;

					//Range Skill? Clear it...
					if ( pcUnitData->iAreaDamageArea )
						pcUnitData->iAreaDamageArea = 0;

					//Not Time Locked Target?
					if ( pcUnitData->dwIgnoreOthersTime == 0 )
					{
						//Set New Target
						pcUnitData->pcFocusTarget = pcTarget;

						//Have Target?
						if ( pcTarget )
						{
							Stage * pcStage = pcUnitData->pcStage;
							if ( pcStage )
							{
								Unit * pcUnit = UNITDATATOUNIT( pcUnitData );

								//Update Position to Target Position
								pcUnitData->NextTarget( pcTarget->sPosition.iX, pcTarget->sPosition.iY, pcTarget->sPosition.iZ );

								if ( IsSafeStuckUnit( pcUnitData ) )
								{
									if ( pcUnitData->sPosition.WithinPTDistance( &pcUnitData->sSpawnPosition, 448 ) == FALSE )
									{
										pcUnitData->NextTarget( pcUnitData->sSpawnPosition.iX, pcUnitData->sSpawnPosition.iY, pcUnitData->sSpawnPosition.iZ );

										if ( pcUnitData->sCharacterData.sHP.sCur < pcUnitData->sCharacterData.sHP.sMax )
										{
											pcUnitData->sCharacterData.sHP.sCur += 1000;

											if ( pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax )
												pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;

											if ( pcUnitData->iHPBlocks > 0 )
												pcUnitData->sHPBlocks.sCur = (short)pcUnitData->iHPBlocks;

											pcUnitData->pcFocusTarget = NULL;

											pcUnitData->dwIgnoreOthersTime = TICKCOUNT + 5000;
										}
									}
								}
							}
						}
					}
					else
					{
						if ( pcUnitData->dwIgnoreOthersTime < TICKCOUNT )
							pcUnitData->dwIgnoreOthersTime = 0;
					}
				}
				else
				{
					UnitData * pcTarget = pcUnitData->pcAttackTarget;

					//Update Position to Target Position
					pcUnitData->NextTarget( pcTarget->sPosition.iX, pcTarget->sPosition.iY, pcTarget->sPosition.iZ );
				}
			}
		}


		if ( (pcUnitData->sCharacterData.sHP.sCur == 0) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Die) )
		{
			pcUnitData->Animate( ANIMATIONTYPE_Die );
		}

		//Not Alive?
		if ( (pcUnitData->eUpdateMode == UPDATEMODE_ServerMonster) && pcUnitData->dwLastActiveTime && !FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
		{
			DWORD dwTime = pcUnitData->dwLastActiveTime + (5 * 60 * 1000);
			
			if ( !pcUnitData->dwLastUpdateTime && (dwTime < TICKCOUNT) )
			{
				//Close Unit
				pcUnitData->Free();

				//Decrease Monster Counter on Map and delete from Spawn Flag
				psStageData->iNumAliveUnitData--;
				MAPSERVER->DelMonsterSpawn( psStageData, pcUnitData );
			}
		}

		//Pet?
		if ( pcUnitData->pcOwner )
		{
			if ( !FOLLOWUNITHANDLER->IsFollowUnitType( UNITDATATOUNIT( pcUnitData ) ) )
			{
				UserData * ud = pcUnitData->pcOwner;
				Map * pcMap = pcUnitData->pcMap;

				//Online?
				if ( pcMap && pcMap->pcBaseMap && ud->pcSocketData )
				{
					if ( ud->iMapID != pcMap->pcBaseMap->iMapID )
					{
						//Is a Pet and User Owner Pet is Online?
						if ( pcUnitData->pcOwner && pcUnitData->pcOwner->pcSocketData )
							OnBeforeCrystalKill( pcUnitData );

						//Clear
						DELET( pcUnitData->psaDamageUsersData );

						//Close Unit
						pcUnitData->Free();

						//Decrease Monster Counter on Map and delete from Spawn Flag
						psStageData->iNumAliveUnitData--;
						MAPSERVER->DelMonsterSpawn( psStageData, pcUnitData );
					}
				}
			}
		}
	}

	return TRUE;
}

void UnitServer::UpdateCharacterData(User* pcUser, UnitData* pcUnitData)
{
	if (pcUnitData && pcUser)
	{
		PacketUnitInfo sPacket = { 0 };
		sPacket.iHeader = PKTHDR_CharacterData;
		sPacket.iLength = sizeof(PacketUnitInfo);
		sPacket.iID = pcUnitData->iID;
		sPacket.sPosition = pcUnitData->sPosition;
		CopyMemory(&sPacket.sCharacterData, &pcUnitData->sCharacterData, sizeof(CharacterData));
		SENDPACKET(pcUser, &sPacket, TRUE);
	}
}


BOOL UnitServer::IsSafeStuckUnit( UnitData * pcUnitData )
{
	Point3D sPosition;

	if ( MAPSERVER->SetBossPosition( pcUnitData->pcMap, &pcUnitData->sCharacterData, &sPosition ) )
		return TRUE;

	return FALSE;
}

AttackDamageData * UnitServer::GetUserInDamageList( UnitData * pcUnitData, User * pcUser )
{
	AttackDamageData * psRet = NULL;

	if ( pcUnitData->psaDamageUsersData )
	{
		for ( int i = 0; i < ATTACK_DAMAGE_MAX; i++ )
		{
			AttackDamageData * p = pcUnitData->psaDamageUsersData + i;
			
			if ( p && p->iID == pcUser->GetID() )
			{
				psRet = p;
				break;
			}
		}
	}

	return psRet;
}

int UnitServer::GetTotalHP( UnitData * pcUnitData )
{
	if ( pcUnitData )
	{
		int iHP = pcUnitData->iHPBlocks;
		return ((int)pcUnitData->sCharacterData.sHP.sMax * iHP) + (int)pcUnitData->sCharacterData.sHP.sMax;
	}

	return 0;
}

void UnitServer::ProcessPacketKillUnitID( User * pcUser, int iID, int iMapID )
{
	PacketExperienceOld sPacketOld;
	sPacketOld.iLength = sizeof( PacketExperienceOld );
	sPacketOld.iHeader = PKTHDR_SetExpOld;
	sPacketOld.iMonsterID = iID;
	sPacketOld.WParam = 0;
	sPacketOld.SParam = 0;
	sPacketOld.EParam = 0;
	sPacketOld.iMapID = iMapID;
	SENDPACKET( pcUser, &sPacketOld );
}

void UnitServer::OnHandleUnitDataBufferNew( Unit * pcUnit, PacketPlayData * psPacket )
{
	int iPercent = pcUnit->pcUnitData->iHPBlocks != 0 ? pcUnit->pcUnitData->sHPBlocks.sCur : 0;
	int iTotal = pcUnit->pcUnitData->iHPBlocks;
	psPacket->sHPLong.iMin = ((int)pcUnit->pcUnitData->sCharacterData.sHP.sMax * iPercent) + pcUnit->pcUnitData->sCharacterData.sHP.sCur;
	psPacket->sHPLong.iMax = ((int)pcUnit->pcUnitData->sCharacterData.sHP.sMax * iTotal) + pcUnit->pcUnitData->sCharacterData.sHP.sMax;
	pcUnit->sHPLong = psPacket->sHPLong;

	psPacket->bCaravan = pcUnit->bFollowUnitType && ((pcUnit->iCharacterXmasTreeID != -1) && (pcUnit->iCharacterXmasTreeID != 0));

	PacketBuffData * psPacketBuff = (PacketBuffData*)&((BYTE*)psPacket)[psPacket->iLength];

	if ( (pcUnit->pcUnitData->bDistortion || pcUnit->pcUnitData->iIceOverlay || pcUnit->iInertiaCount) && pcUnit->pcUnitData->iFreezeTimeLeft )
	{
		psPacketBuff->iLength = sizeof( PacketBuffData );
		psPacketBuff->iHeader = PKTHDR_IceDistortionBuff;

		if ( pcUnit->pcUnitData->bDistortion )
			psPacketBuff->Ice.iDistortion = pcUnit->pcUnitData->bDistortion;
		else
			psPacketBuff->Ice.iDistortion = 0;

		if ( pcUnit->iInertiaCount )
			psPacketBuff->Ice.iCurseLazy = pcUnit->iInertiaCount;
		else
			psPacketBuff->Ice.iCurseLazy = 0;

		psPacketBuff->Ice.iIceOverlay = pcUnit->pcUnitData->iIceOverlay;

		psPacketBuff->Ice.iStunCount = pcUnit->pcUnitData->iFreezeTimeLeft;

		psPacket->bEventInfo[3]++;

		psPacket->iLength += sizeof( PacketBuffData );
		psPacketBuff++;
	}
}

BOOL UnitServer::HandleUnitDamageTop( UserData * pcUserData, UnitData * pcUnitData, int iDamage )
{
	int iIndex = -1;

	//Valhalla
	if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_CASTLE_TOWER_B )
	{
		if ( BLESSCASTLESERVER->HasCrystalsTowersAlive() )
			return FALSE;
	}

	if ( pcUnitData->psaSiegeWarDataList )
	{
		if ( pcUserData->iClanID && (pcUserData->iMapID == MAPID_BlessCastle) )
		{
			pcUserData->sNumDeath += iDamage;

			for ( int i = 0; i < MAX_DAMAGEDATA; i++ )
			{
				if ( pcUnitData->psaSiegeWarDataList[i].iID )
				{
					int iClanID = BLESSCASTLESERVER->GetClanCodeByID( pcUnitData->psaSiegeWarDataList[i].iID );
					if ( iClanID == pcUserData->iClanID )
					{
						pcUnitData->psaSiegeWarDataList[i].iDamage += iDamage;
						pcUnitData->psaSiegeWarDataList[i].iCounter++;
						return TRUE;
					}
				}

				if ( (iIndex < 0) && (pcUnitData->psaSiegeWarDataList[i].iID == 0) )
					iIndex = i;
			}

			if ( iIndex >= 0 )
			{
				pcUnitData->psaSiegeWarDataList[iIndex].iID = pcUserData->sCharacterData.iClanID;
				pcUnitData->psaSiegeWarDataList[iIndex].iDamage = iDamage;
				pcUnitData->psaSiegeWarDataList[iIndex].iCounter++;

				return TRUE;
			}
		}

		return TRUE;
	}

	if ( pcUnitData->psaDamageUsersData == NULL )
		return FALSE;

	if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_CASTLE_TOWER_B )
	{
		if ( pcUserData->iClanID && (pcUserData->iMapID == MAPID_BlessCastle) )
		{
			pcUserData->sNumDeath += iDamage;

			for ( int i = 0; i < MAX_DAMAGEDATA; i++ )
			{
				if ( pcUnitData->psaDamageUsersData[i].iID )
				{
					int iClanID = BLESSCASTLESERVER->GetClanCodeByID( pcUnitData->psaDamageUsersData[i].iID );
					if ( iClanID == pcUserData->iClanID )
					{
						pcUnitData->psaDamageUsersData[i].iDamage += iDamage;
						pcUnitData->psaDamageUsersData[i].iCounter++;
						return TRUE;
					}
				}

				if ( (iIndex < 0) && (pcUnitData->psaDamageUsersData[i].iID == 0) )
					iIndex = i;
			}

			if ( iIndex >= 0 )
			{
				pcUnitData->psaDamageUsersData[iIndex].iID = pcUserData->sCharacterData.iClanID;
				pcUnitData->psaDamageUsersData[iIndex].iDamage = iDamage;
				pcUnitData->psaDamageUsersData[iIndex].iCounter++;

				return TRUE;
			}
		}
	}
	else
	{
		for ( int i = 0; i < MAX_DAMAGEDATA; i++ )
		{
			if ( pcUnitData->psaDamageUsersData[i].iID == pcUserData->iID )
			{
				pcUnitData->psaDamageUsersData[i].iDamage += iDamage;
				pcUnitData->psaDamageUsersData[i].iCounter++;
				return TRUE;
			}

			if ( (iIndex < 0) && (pcUnitData->psaDamageUsersData[i].iID == 0) )
				iIndex = i;
		}

		if ( iIndex >= 0 )
		{
			pcUnitData->psaDamageUsersData[iIndex].iID = pcUserData->iID;
			pcUnitData->psaDamageUsersData[iIndex].iDamage = iDamage;
			pcUnitData->psaDamageUsersData[iIndex].iCounter++;

			return TRUE;
		}
	}

	return FALSE;
}

int UnitServer::GetUnitDataHPRegen( UnitData * pcUnitData )
{
	if ( (pcUnitData->pcMap == nullptr) || (pcUnitData->pcMap->pcBaseMap == nullptr) )
		return 0;

	int iHPRegen = 0;

	//If Monster hp is too low (< (maxHP / 5)), Regen more (index 1) else regen less (index 0)
	int iaBabelHPRegenTable[10][2] = 
	{
		{	1800	,	2300	},
		{	1900	,	2400	},
		{	2000	,	2550	},
		{	2150	,	2650	},
		{	2300	,	2800	},
		{	2400	,	2900	},
		{	2500	,	3000	},
		{	2600	,	3100	},
		{	2700	,	3200	},
		{	2800	,	3300	}
	};

	std::vector<User *> vUsersGM;

	int iUsersCount = 0;

	if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL )
	{
		int iRegenRange = 260 * 260;

		for ( UINT i = 0; i < USERSERVER->uUsersInGame; i++ )
		{
			User * pcUser = USERSERVER->pcaUserInGame[i];
			if ( pcUser->GetMapID() == pcUnitData->pcMap->pcBaseMap->iMapID )
			{
				int iX = (pcUnitData->sPosition.iX - pcUser->GetPosition().iX) >> 8;
				int iY = (pcUnitData->sPosition.iY - pcUser->GetPosition().iY) >> 8;
				int iZ = (pcUnitData->sPosition.iZ - pcUser->GetPosition().iZ) >> 8;

				int iDistance = (iX * iX) + (iZ * iZ);
				if ( (abs( iY ) < 60) && (iDistance < iRegenRange) && (abs( iX ) < DISTANCE_UNIT_VIEWLIMIT) && (abs( iZ ) < DISTANCE_UNIT_VIEWLIMIT) )
				{
					iUsersCount++;

					if ( pcUser->GetGameLevel() >= GAMELEVEL_Four )
					{
						vUsersGM.push_back( pcUser );
					}
				}
			}
		}

		int iTableIndex = 0;
		if ( iUsersCount > 6 )
		{
			iTableIndex = iUsersCount - 6;
		}

		if ( iTableIndex > 9 )
			iTableIndex = 9;

		if ( pcUnitData->sCharacterData.sHP.sCur < (pcUnitData->sCharacterData.sHP.sMax / 5) )
		{
			iHPRegen = iaBabelHPRegenTable[iTableIndex][1];
		}
		else
		{
			iHPRegen = iaBabelHPRegenTable[iTableIndex][0];
		}
	}

	if ( vUsersGM.size() > 0 )
	{
		for ( auto pcUser : vUsersGM )
			CHATSERVER->SendDebugChat( pcUser->pcUserData, CHATCOLOR_Error, "> Monster HP Regen: UsersCount[%d] HPRegen[%d]", iUsersCount, iHPRegen );

		vUsersGM.clear();
	}

	return iHPRegen;
}

int UnitServer::GetUnitDataSpawnFlagCount( UnitData * pcUnitData )
{
	if ( pcUnitData->pcMap == nullptr )
		return -1;

	int iIndex = pcUnitData->iSpawnPointIndex;

	if ( (iIndex >= 0) && (iIndex < _countof( pcUnitData->pcMap->iaSpawnMonsterActive )) )
		return pcUnitData->pcMap->iaSpawnMonsterActive[iIndex];

	return -1;
}

NAKED void UnitServer::GetMoveLocation( int iX, int iY, int iZ, int iAngleX, int iAngleY, int iAngleZ )
{
	JMP( 0x0046E760 );
}

NAKED UnitData * UnitServer::CreateMonsterFromSkill( int iSkillID, int iX, int iY, int iZ, UserData * pcUserData, UnitData * pcUnitDataTarget )
{
	JMP( 0x0055E5A0 );
}

void UnitServer::UpdateUnit( Unit * pcUnit )
{
	UnitData * pcUnitData = pcUnit->pcUnitData;

	WRITEDWORD( 0x07AC5658, pcUnitData );

	if ( (pcUnitData->iAliveCounter != 0) ||
		(pcUnitData->GetAnimation() == ANIMATIONTYPE_Die) ||
		(pcUnitData->sUnitInfo.eSpecialType != SPECIALUNITTYPE_None) )
	{
		UpdateUnitData( pcUnit->pcUnitData );

		pcUnitData->iAliveCounter--;
	}
	else
	{
		if ( pcUnitData->sCharacterData.iType == CHARACTERTYPE_Monster )
		{
			pcUnitData->iDeathCounter--;

			if ( pcUnitData->iDeathCounter <= 0 )
			{
				pcUnitData->Free();

				pcUnitData->pcMap->iNumAliveUnitData--;
				MAPSERVER->DelMonsterSpawn( pcUnitData->pcMap, pcUnitData );
			}
		}
	}
}


void UnitServer::Update()
{
	//Only GameServers
	if ( LOGIN_SERVER && (GAME_SERVER == FALSE) )
		return;

	static UINT iUnitWheel = 0;
	static UINT iActiWheel = 0;

	//Check if action wheel must be set
	if ( iActiWheel == 0 )
	{
		if ( (iUnitWheel % 64) == 0 )
			iActiWheel = 4;
	}
	else
		iActiWheel--;

	//Write global unit vars
	WRITEDWORD( 0x008B8D18, GetTickCount() );
	WRITEDWORD( 0x07AC9D68, iActiWheel );

	uUnitsInGame = 0;
	for ( UINT i = 0; i < MAX_UNITS; i++ )
	{
		Unit * pcUnit = pcaUnit + i;
		UnitData * pcUnitData = pcUnit->pcUnitData;

		if ( pcUnit->iID != pcUnitData->iID )
			pcUnit->Load();

		if ( pcUnitData->bActive )
		{
			pcaUnitInGame[uUnitsInGame] = pcUnit;
			uUnitsInGame++;

			//Frame Check
			if ( (i % 4) == (iUnitWheel % 4) )
			{
				//Update Unit
				UpdateUnit( pcUnit );

				//Status Update?
				if ( iActiWheel != 0 )
					pcUnit->uLastUpdate++;
			}
		}
	}

	iUnitWheel++;
}

UnitServer::EUnitFollowType UnitServer::GetUnitFollowType( UnitData * pcUnitData )
{ //return UNITFOLLOWTYPE_DontFollow = jump all, UNITFOLLOWTYPE_Die = die, UNITFOLLOWTYPE_Follow = follow
	Unit * pcUnit = UNITDATATOUNIT( pcUnitData );

	if ( pcUnitData->pcOwner == NULL )
		return UNITFOLLOWTYPE_DontFollow;

	User * pcUser = USERDATATOUSER( pcUnitData->pcOwner );
	if ( pcUser == NULL )
		return UNITFOLLOWTYPE_DontFollow;

	if ( pcUnitData->iPetID == 0 )
	{
		if ( pcUnitData->lExclusiveTargetID )
		{
			Map * pcMap = MAPSDATA + MAPID_T5QuestArena;

			if ( pcMap )
			{
				if ( (pcUnitData->pcMap == pcMap) && (pcUnitData->lExclusiveTargetID == pcUnitData->pcOwner->iID) )
					return UNITFOLLOWTYPE_DontFollow;
			}
		}

		//Caravan?
		if ( FOLLOWUNITHANDLER->IsFollowUnitType( pcUnit ) )
		{
			if ( pcUnit->bCaravanFollow )
			{
				if ( FOLLOWUNITHANDLER->CanWalkOnMap( pcUnitData->pcOwner->iMapID ) == false )
				{
					if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Idle )
						pcUnitData->Animate( ANIMATIONTYPE_Idle );

					return UNITFOLLOWTYPE_DontFollow;
				}

				if ( (pcUser->bInGrandFury) || (FURYARENAHANDLER->InsideArena( &pcUser->pcUserData->sPosition )) )
				{
					if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Idle )
						pcUnitData->Animate( ANIMATIONTYPE_Idle );

					return UNITFOLLOWTYPE_DontFollow;
				}

				return UNITFOLLOWTYPE_Follow;
			}
			else
			{
				if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Idle )
					pcUnitData->Animate( ANIMATIONTYPE_Idle );

				return UNITFOLLOWTYPE_DontFollow;
			}
		}

		//Follow
		if ( pcUnitData->sCharacterData.iMonsterType == MONSTERTYPE_Friendly )
			return UNITFOLLOWTYPE_Follow;

		//Follow
		if ( pcUnitData->sCharacterData.iType == CHARACTERTYPE_NPC )
			return UNITFOLLOWTYPE_Follow;

		return UNITFOLLOWTYPE_DontFollow;
	}

	if ( pcUnitData->pcOwner->pcSocketData == NULL )
		return UNITFOLLOWTYPE_DontFollow;

	if ( pcUnitData->sCharacterData.sHP.sCur <= 0 )
		return UNITFOLLOWTYPE_DontFollow;

	if ( pcUnitData->pcOwner->pcPetData != pcUnitData )
	{
		//Die
		if ( pcUnitData->iPetID != 1 )
			return UNITFOLLOWTYPE_Die;
	}

	//Follow
	return UNITFOLLOWTYPE_Follow;
}

BOOL UnitServer::MainServerUnitData( UnitData * pcUnitData )
{
	if ( (pcUnitData->pcBodyMeshContainer == nullptr) || pcUnitData->bNoMove || (pcUnitData->psModelAnimation == nullptr) )
	{
		return FALSE;
	}

	if ( pcUnitData->pcBody->iCount <= 0 )
	{
		pcUnitData->Free();

		return FALSE;
	}

	switch ( pcUnitData->eUpdateMode )
	{
		case UPDATEMODE_ServerMonster:
			MainUnitMonsterData( pcUnitData );
			break;		
		case UPDATEMODE_ServerNPC:
			MainUnitNPCData( pcUnitData );
			break;
	}

	if ( pcUnitData->iTransparencySelf < 0 )
	{
		pcUnitData->iTransparencySelf += 4;
		if ( pcUnitData->iTransparencySelf > 0 )
			pcUnitData->iTransparencySelf = 0;
	}
	if ( pcUnitData->iTransparencySelf > 0 )
	{
		pcUnitData->iTransparencySelf -= 4;
		if ( pcUnitData->iTransparencySelf < 0 )
			pcUnitData->iTransparencySelf = 0;
	}

	pcUnitData->iLoopTimer++;
	pcUnitData->iFrameTimer++;

	return TRUE;
}

BOOL UnitServer::MainUnitMonsterData( UnitData * pcUnitData )
{
	//Frames
	int iMovementSpeed = 0;
	int iFrameSpeed = 0;

	if ( pcUnitData->iFreezeTimeLeft )
	{
		if ( pcUnitData->iUpdateMoveReduce )
		{
			iMovementSpeed = (pcUnitData->iUpdateMoveReduce * 4) >> 8;
			iFrameSpeed = (pcUnitData->iUpdateMoveReduce * 320) >> 8;
		}
		else
		{
			iMovementSpeed = 4 >> 1;
			iFrameSpeed = 320 >> 1;
		}
	}
	else
	{
		iMovementSpeed = 4;
		iFrameSpeed = 320;
	}

	//Special (No Movement?)
	if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Special )
	{
		pcUnitData->iFrame += iFrameSpeed;

		if ( pcUnitData->psModelAnimation )
		{
			if ( pcUnitData->iFrame > (pcUnitData->psModelAnimation->iEndFrame * 160) )
			{
				pcUnitData->Free();
				return TRUE;
			}
		}

		//Save Frames
		SaveFrames( pcUnitData );
		pcUnitData->iFrameTimer += 3;

		return TRUE;
	}

	//No Move? No speed...
	if ( pcUnitData->bNoMove )
		iMovementSpeed = 0;

	//Babel? Set Delay to spawn...
	if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL )
	{
		if ( pcUnitData->sCharacterData.sHP.sCur > 0 )
			WRITEDWORD( 0x07AC9D54, TICKCOUNT + 5000 );
	}

	//Have Animation and speed?
	if ( iMovementSpeed && pcUnitData->GetAnimation() < ANIMATIONTYPE_Attack )
	{
		//Running or Walking?
		if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking) || (pcUnitData->GetAnimation() == ANIMATIONTYPE_Running) )
		{
			//Save Old Position...
			Point3D sOldPosition = pcUnitData->sPosition;

			//Running
			BOOL bRunning = (pcUnitData->GetAnimation() == ANIMATIONTYPE_Running) ? TRUE : FALSE;

			//Movement Based on IQ
			if ( pcUnitData->saMoveStepCount[0] )
			{
				int iSpeed = iMovementSpeed;

				if ( pcUnitData->saMoveStepCount[1] )
					iSpeed *= pcUnitData->saMoveStepCount[1];

				//Move...
				for ( int i = 0; i < pcUnitData->saMoveStepCount[0]; i++ )
				{
					pcUnitData->Move( iSpeed << 8 );
					pcUnitData->Move( (iSpeed >> 1) << 8 );

					if ( bRunning )
						pcUnitData->Move( iSpeed << 8 );
				}
			}
			else
			{
				//Follow Target
				BOOL bFollowTarget = FALSE;

				//Set Monster Speed
				switch ( pcUnitData->sCharacterData.iMonsterEffectID )
				{
					case MONSTEREFFECTID_DEATHKNIGHT:
						if ( pcUnitData->sCharacterData.sGlow == 1 )
							iMovementSpeed *= 2;
						break;

					case MONSTEREFFECTID_GREVEN:
						if ( bRunning )
						{
							if ( pcUnitData->pcFocusTarget && pcUnitData->iTargetTraceMode )
								bFollowTarget = TRUE;

							iMovementSpeed *= 5;
						}
						break;

					case MONSTEREFFECTID_WOLVERLIN:
						if ( bRunning )
						{
							if ( pcUnitData->pcFocusTarget && pcUnitData->iTargetTraceMode )
								bFollowTarget = TRUE;

							iMovementSpeed *= 2;
						}
						break;

					default:
						break;
				}

				//Follow Target
				if ( pcUnitData->pcFocusTarget && bFollowTarget )
					pcUnitData->SetAngleByPosition( pcUnitData->pcFocusTarget->sPosition.iX >> 8, pcUnitData->pcFocusTarget->sPosition.iZ >> 8 );

				//Move
				pcUnitData->Move( (iMovementSpeed >> 1) << 8 );
				pcUnitData->Move( iMovementSpeed << 8 );
			}

			if ( (pcUnitData->GetAnimation() != ANIMATIONTYPE_Walking) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Running) )
				pcUnitData->Animate( ANIMATIONTYPE_Walking );
		}
		else
		{
			if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Idle )
				pcUnitData->Animate( ANIMATIONTYPE_Idle );
		}
	}

	//Update Frame
	pcUnitData->iFrame += iFrameSpeed;

	BOOL bSpawnBegin = FALSE;

	if ( pcUnitData->iFrame > (pcUnitData->psModelAnimation->iEndFrame * 160) )
	{
		//Die?
		if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Die )
		{
			//End Frame
			pcUnitData->iFrame = pcUnitData->psModelAnimation->iEndFrame * 160;
		}
		else
		{
			//Use Potion?
			if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Potion) && (pcUnitData->iPotionStatus == POTIONSTATUS_HP) )
			{
				//Use virtual potion..
				pcUnitData->sCharacterData.sHP.sCur += (pcUnitData->sCharacterData.sHP.sMax >> 1);
				if ( pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax )
					pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;
			}

			//???
			if ( pcUnitData->sUnitInfo.iSkillType && (pcUnitData->sCharacterData.iMonsterType != MONSTERTYPE_Friendly) )
			{
				if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Attack )
					pcUnitData->pcFocusTarget = NULL;

				//Have Target Monster and use skill? poison it
				if ( pcUnitData->pcTarget && (pcUnitData->GetAnimation() == ANIMATIONTYPE_Skill) )
				{
					pcUnitData->pcTarget->sSuccubus.sMin = RandomI( 10, 20 );
					pcUnitData->pcTarget->sSuccubus.sMax = 16 * 20;
					pcUnitData->pcTarget = NULL;
				}
			}

			if ( (pcUnitData->pcFocusTarget || pcUnitData->iTargetMoveCount) && (pcUnitData->GetAnimation() < ANIMATIONTYPE_Attack) )
				pcUnitData->iMoveBeginCounter++;

			//Repeat Animation?
			if ( pcUnitData->psModelAnimation->bLoop )
			{
				//Repeat
				pcUnitData->iFrame = (pcUnitData->psModelAnimation->iBeginFrame * 160) + (pcUnitData->iFrame - (pcUnitData->psModelAnimation->iEndFrame * 160));
			}
			else
			{
				if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Flinch) && !pcUnitData->iStunTimeLeft && !pcUnitData->iStopMotionCounter &&
					pcUnitData->Animate( ANIMATIONTYPE_Walking ) )
				{
					pcUnitData->iTargetMoveCount = 2;
				}
				else
					pcUnitData->Animate( ANIMATIONTYPE_Idle );

				pcUnitData->iMoveCounter = 1;
			}

			//When Spawn
			if ( pcUnitData->iMoveBeginCounter > 4 )
			{
				int iX = (pcUnitData->sPosition.iX - pcUnitData->sMoveBeginPosition.iX) >> 8;
				int iZ = (pcUnitData->sPosition.iZ - pcUnitData->sMoveBeginPosition.iZ) >> 8;

				int iDistance = (iX * iX) + (iZ * iZ);

				pcUnitData->sMoveBeginPosition.iX = pcUnitData->sPosition.iX;
				pcUnitData->sMoveBeginPosition.iZ = pcUnitData->sPosition.iZ;
				pcUnitData->iMoveBeginCounter = 0;

				if ( iDistance < (32 * 32) )
				{
					pcUnitData->SetTarget( pcUnitData->sSpawnPosition.iX >> 8, pcUnitData->sSpawnPosition.iZ >> 8 );
					pcUnitData->iTargetTraceMode = 0;

					int iAngle = (rand() & PTANGLE_90) - PTANGLE_45;
					pcUnitData->sAngle.iY = (pcUnitData->sAngle.iY + iAngle) & PTANGLE_Mask;

					pcUnitData->iTargetMoveCount = 10;

					pcUnitData->bCheckCollision = FALSE;

					pcUnitData->sMoveToPosition = Point3D( 0, 0, 0 );

					if ( pcUnitData->Animate( ANIMATIONTYPE_Idle ) == FALSE )
						pcUnitData->Animate( ANIMATIONTYPE_Walking );

					bSpawnBegin = TRUE;
				}
			}

			//Not Spawn Time?
			if ( bSpawnBegin == FALSE )
			{
				if ( pcUnitData->iTargetMoveCount > 0 )
				{
					pcUnitData->iTargetMoveCount--;
					if ( pcUnitData->iTargetMoveCount <= 0 )
						pcUnitData->Animate( ANIMATIONTYPE_Idle );
				}

				if ( pcUnitData->iLureDistance && ((pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking) || (pcUnitData->GetAnimation() == ANIMATIONTYPE_Running)) )
				{
					int iX = (pcUnitData->sPosition.iX - pcUnitData->sSpawnPosition.iX) >> 8;
					int iZ = (pcUnitData->sPosition.iZ - pcUnitData->sSpawnPosition.iZ) >> 8;
					int iDistance = (iX * iX) + (iZ * iZ);
					if ( iDistance > pcUnitData->iLureDistance )
					{
						pcUnitData->SetTarget( pcUnitData->sSpawnPosition.iX >> 8, pcUnitData->sSpawnPosition.iZ >> 8 );
						pcUnitData->sMoveToPosition.iX = 0;
						pcUnitData->sMoveToPosition.iZ = 0;
						pcUnitData->iTargetMoveCount = 6;
						pcUnitData->iTargetTraceMode = 0;
					}
				}

				if ( !pcUnitData->iTargetMoveCount && pcUnitData->sMoveToPosition.iX && pcUnitData->sMoveToPosition.iZ )
				{
					pcUnitData->SetTarget( pcUnitData->sMoveToPosition.iX >> 8, pcUnitData->sMoveToPosition.iZ >> 8 );
					pcUnitData->iTargetTraceMode = TRUE;

					//Movement based on IQ
					if ( pcUnitData->iMoveCounter <= 0 )
					{
						if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Idle )
						{
							pcUnitData->iMoveCounter = pcUnitData->sUnitInfo.iIntelligence - 5;
							if ( pcUnitData->iMoveCounter < 0 )
								pcUnitData->iMoveCounter = 0;

							int iX = (pcUnitData->sPosition.iX - pcUnitData->sMoveToPosition.iX) >> 8;
							int iZ = (pcUnitData->sPosition.iZ - pcUnitData->sMoveToPosition.iZ) >> 8;
							int iDistance = (iX * iX) + (iZ * iZ);

							if ( (pcUnitData->sUnitInfo.iIntelligence >= 6) && (iDistance < (64 * 8 * 64 * 8)) )
							{
								if ( pcUnitData->Animate( ANIMATIONTYPE_Running ) )
									pcUnitData->iMoveCounter <<= 2;
								else
									pcUnitData->Animate( ANIMATIONTYPE_Walking );
							}
							else
								pcUnitData->Animate( ANIMATIONTYPE_Walking );
						}
						else
							pcUnitData->Animate( ANIMATIONTYPE_Idle );
					}
					else
					{
						pcUnitData->SetAngleByPosition( pcUnitData->sMoveToPosition.iX >> 8, pcUnitData->sMoveToPosition.iZ >> 8 );
						pcUnitData->iMoveCounter--;
					}

					pcUnitData->sMoveToPosition = Point3D( 0, 0, 0 );
				}

				if ( pcUnitData->sUnitInfo.iMaxPotions )
				{
					if ( pcUnitData->sCharacterData.sHP.sMax > 0 )
					{
						int iHP = (pcUnitData->sCharacterData.sHP.sCur * 100) / pcUnitData->sCharacterData.sHP.sMax;
						int iHPMin = (pcUnitData->sCharacterData.sHP.sMax * 10) / 100;

						if ( iHP < iHPMin )
						{
							if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_NPC_MAGICMASTER )
							{
								PacketSimple sPacket;
								sPacket.iLength = sizeof( PacketSimple );
								sPacket.iHeader = PKTHDR_SkillCastData;
								sPacket.iUnk = 0x71;
								sPacket.iUnk2 = pcUnitData->iID;

								if ( pcUnitData->pcExclusiveTarget )
									SENDPACKET( USERDATATOUSER( pcUnitData->pcExclusiveTarget ), &sPacket );

								//Use virtual potion..
								pcUnitData->sCharacterData.sHP.sCur += (pcUnitData->sCharacterData.sHP.sMax >> 1);
								if ( pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax )
									pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;
							}
							else
							{
								pcUnitData->Animate( ANIMATIONTYPE_Potion );
								pcUnitData->iPotionStatus = POTIONSTATUS_HP;
							}

							pcUnitData->sUnitInfo.iMaxPotions--;
						}
					}
				}

				if ( pcUnitData->eNextAnimationType )
				{
					if ( pcUnitData->Animate( pcUnitData->eNextAnimationType ) == FALSE )
						pcUnitData->Animate( ANIMATIONTYPE_Idle );

					pcUnitData->eNextAnimationType = ANIMATIONTYPE_None;
				}

				if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_DEADHOPT )
				{
					if ( pcUnitData->sCharacterData.sHP.sMax > 0 )
					{
						int iHP = (pcUnitData->sCharacterData.sHP.sCur * 100) / pcUnitData->sCharacterData.sHP.sMax;
						if ( (iHP < 20) || (pcUnitData->iHits > 80) )
						{
							pcUnitData->Animate( ANIMATIONTYPE_Special );
							pcUnitData->pcFocusTarget = NULL;
						}
					}
				}
			}
		}
	}

	if ( pcUnitData->iSpecialType == 2 )
		pcUnitData->sAngle = Point3D( 0, 0, 0 );

	//Save Frames
	SaveFrames( pcUnitData );

	EUnitFollowType eFollowType = GetUnitFollowType( pcUnitData );

	if ( eFollowType != EUnitFollowType::UNITFOLLOWTYPE_DontFollow )
	{
		if ( eFollowType == EUnitFollowType::UNITFOLLOWTYPE_Die )
		{
			pcUnitData->sCharacterData.sHP.sCur = 0;
			pcUnitData->Animate( ANIMATIONTYPE_Die );
		}
		else if ( eFollowType == EUnitFollowType::UNITFOLLOWTYPE_Follow )
		{
			int iX = (pcUnitData->pcOwner->sPosition.iX - pcUnitData->sPosition.iX) >> 8;
			int iZ = (pcUnitData->pcOwner->sPosition.iZ - pcUnitData->sPosition.iZ) >> 8;
			int iDistance = (iX * iX) + (iZ * iZ);

			if ( iDistance >= (500 * 500) )
			{
				if ( pcUnitData->pcStage )
				{
					int iY = pcUnitData->pcStage->GetHighestPoint( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ );
					if ( iY < 0 )
					{
						pcUnitData->sCharacterData.sHP.sCur = 0;
						pcUnitData->Animate( ANIMATIONTYPE_Die );
					}
				}

				if ( pcUnitData->sCharacterData.sHP.sCur > 0 )
				{
					pcUnitData->sPosition = pcUnitData->pcOwner->sPosition;
					pcUnitData->sMoveToPosition.iX = 0;
					pcUnitData->sMoveToPosition.iZ = 0;

					pcUnitData->iTargetMoveCount = 0;
					pcUnitData->iTargetTraceMode = 0;

					pcUnitData->pcAttackTarget = nullptr;
				}
			}
			else if ( (iDistance >= (300 * 300)) || ((pcUnitData->pcAttackTarget == nullptr) && (iDistance >= (150 * 150))) )
			{
				pcUnitData->SetTarget( pcUnitData->pcOwner->sPosition.iX >> 8, pcUnitData->pcOwner->sPosition.iZ >> 8 );

				pcUnitData->iTargetMoveCount = 2;
				pcUnitData->iTargetTraceMode = 0;

				pcUnitData->pcAttackTarget = nullptr;

				if ( (pcUnitData->GetAnimation() != ANIMATIONTYPE_Die) && ((pcUnitData->GetAnimation() >= ANIMATIONTYPE_Attack) || (pcUnitData->GetAnimation() == ANIMATIONTYPE_Idle)) )
				{
					if ( pcUnitData->Animate( ANIMATIONTYPE_Running ) == FALSE )
						pcUnitData->Animate( ANIMATIONTYPE_Walking );
				}
			}
		}
	}

	if ( pcUnitData->lExclusiveTargetID )
	{
		switch ( pcUnitData->sCharacterData.iMonsterEffectID )
		{
			case MONSTEREFFECTID_NPC_SKILLMASTER:
			case MONSTEREFFECTID_NPC_MAGICMASTER:
				if ( pcUnitData->sCharacterData.sHP.sCur <= (pcUnitData->sCharacterData.sHP.sMax >> 4) )
				{
					if ( pcUnitData->sCharacterData.sHP.sCur > 0 )
					{
						pcUnitData->sCharacterData.sHP.sCur = (pcUnitData->sCharacterData.sHP.sMax >> 4);
					}

					pcUnitData->pcFocusTarget = nullptr;
					pcUnitData->pcAttackTarget = nullptr;

					if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Die )
					{
						pcUnitData->Animate( ANIMATIONTYPE_Idle );
					}

					pcUnitData->eUpdateMode = UPDATEMODE_ServerNPC;

					pcUnitData->sSpawnPosition = pcUnitData->sPosition;

					pcUnitData->iLureDistance = 256 * 256;

					if ( pcUnitData->pcExclusiveTarget && (pcUnitData->pcExclusiveTarget->iID == pcUnitData->lExclusiveTargetID) )
					{
						ProcessQuestPacket( pcUnitData->pcExclusiveTarget, pcUnitData, QUESTCODESKILLUNIT_Tier4, pcUnitData->iID );
					}

					if ( Unit * pcUnit = GetUnitByExclusiveUser( pcUnitData ) )
					{
						pcUnit->pcUnitData->Animate( ANIMATIONTYPE_Die );
						pcUnit->pcUnitData->sCharacterData.sHP.sCur = 0;
					}
				}
				else
				{
					if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_NPC_MAGICMASTER )
					{
						if ( ((rand() % 100) < 3) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Skill) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Potion) )
						{
							if ( pcUnitData->pcExclusiveTarget )
							{
								int iX = (pcUnitData->sPosition.iX - pcUnitData->pcExclusiveTarget->sPosition.iX) >> 8;
								int iZ = (pcUnitData->sPosition.iZ - pcUnitData->pcExclusiveTarget->sPosition.iZ) >> 8;
								int iDistance = (iX * iX) + (iZ * iZ);

								if ( iDistance < (300 * 300) )
								{
									//Quest Arena
									pcUnitData->sPosition.iX = (22091 << 8) + (((rand() % 600) - 300) << 8);
									pcUnitData->sPosition.iZ = (9673 << 8) + (((rand() % 600) - 300) << 8);
								}
							}
						}
					}
				}
				break;

			case MONSTEREFFECTID_FURY:
				if ( (pcUnitData->sCharacterData.sHP.sCur <= 0) && (pcUnitData->sUnitInfo.iMaxPotions == 0) && (pcUnitData->sCharacterData.sHP.sMax > 1) )
				{
					if ( pcUnitData->pcExclusiveTarget && (pcUnitData->pcExclusiveTarget->iID == pcUnitData->lExclusiveTargetID) )
					{
						ProcessQuestPacket( pcUnitData->pcExclusiveTarget, pcUnitData, QUESTCODESKILLUNIT_FuryQuest, pcUnitData->iID );
						pcUnitData->sCharacterData.sHP.sMax = 1;
					}
				}
				break;

			default:
				break;
		}
	}

	if ( pcUnitData->pcAttackTarget )
	{
		if ( pcUnitData->sUnitInfo.iSkillType && (pcUnitData->sCharacterData.iMonsterType != MONSTERTYPE_Friendly) && (pcUnitData->pcAttackTarget->sCharacterData.iMonsterType != MONSTERTYPE_Friendly) )
		{
			if ( pcUnitData->GetAnimation() < ANIMATIONTYPE_Attack )
			{
				pcUnitData->iTargetMoveCount = 0;
				pcUnitData->iMoveCounter = 0;
				pcUnitData->pcAttackTarget = nullptr;
			}
		}
		else if ( pcUnitData->pcAttackTarget->bActive && (pcUnitData->pcAttackTarget->iStunTimeLeft == 0) && (pcUnitData->pcAttackTarget->iStopMotionCounter == 0) )
		{
			int iDistance = pcUnitData->sCharacterData.iAttackRange + 32;
			if ( iDistance < 80 )
				iDistance = 80;

			if ( (pcUnitData->GetAnimation() != ANIMATIONTYPE_Flinch) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Die) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Potion) )
			{
				if ( (pcUnitData->GetAnimation() != ANIMATIONTYPE_Attack) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Skill) )
				{
					int iX = (pcUnitData->sPosition.iX - pcUnitData->pcAttackTarget->sPosition.iX) >> 8;
					int iZ = (pcUnitData->sPosition.iZ - pcUnitData->pcAttackTarget->sPosition.iZ) >> 8;
					int iDistanceTarget = (iX * iX) + (iZ * iZ);

					iDistance >>= 8;
					iDistance *= iDistance;

					if ( (iDistanceTarget < iDistance) && (abs( pcUnitData->sPosition.iY - pcUnitData->pcAttackTarget->sPosition.iY ) < (64 << 8)) )
					{
						pcUnitData->sMoveBeginPosition.iX = 0;
						pcUnitData->sMoveBeginPosition.iZ = 0;
						pcUnitData->iMoveBeginCounter = 0;

						pcUnitData->dwLastActiveTime = TICKCOUNT;

						pcUnitData->Animate( ANIMATIONTYPE_Attack );

						if ( (pcUnitData->sUnitInfo.iSkillChance > 0) && (pcUnitData->GetAnimation() == ANIMATIONTYPE_Attack) )
						{
							if ( RandomI( 0, 99 ) < pcUnitData->sUnitInfo.iSkillChance )
							{
								pcUnitData->Animate( ANIMATIONTYPE_Skill );
							}
						}

						pcUnitData->sAngle.iY = GetRadian2D( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ, pcUnitData->pcAttackTarget->sPosition.iX, pcUnitData->pcAttackTarget->sPosition.iZ );
						pcUnitData->sAttackPosition = pcUnitData->pcAttackTarget->sPosition;
					}
					else
					{
						if ( iDistanceTarget > DISTANCE_UNIT_TOO_FAR )
						{
							pcUnitData->pcAttackTarget = nullptr;
						}
					}
				}
			}

			if ( pcUnitData->sCharacterData.iMonsterType != MONSTERTYPE_Friendly )
			{
				if ( (pcUnitData->iFrameTimer == 80) && (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL) )
				{
					pcUnitData->iAreaDamageArea = pcUnitData->sUnitInfo.iSkillArea * pcUnitData->sUnitInfo.iSkillArea;

					pcUnitData->sCharacterData.sHP.sCur += GetUnitDataHPRegen( pcUnitData );

					if ( (pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax) || (pcUnitData->sCharacterData.sHP.sCur < 0) )
						pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;
				}
			}

			if ( (pcUnitData->iFrameTimer == 40) && pcUnitData->pcAttackTarget )
			{
				if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Attack) || (pcUnitData->GetAnimation() == ANIMATIONTYPE_Skill) )
				{
					pcUnitData->bMonsterAttacking = TRUE;
					pcUnitData->iHits++;

					int iDefendChance = DefendChance( pcUnitData->pcAttackTarget->sCharacterData.iDefenseRating, pcUnitData->sCharacterData.iAttackRating );
					if ( ((rand() % 100) < iDefendChance) && (pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur > 0) )
					{
						if ( pcUnitData->pcAttackTarget->pcFocusTarget == nullptr )
						{
							pcUnitData->pcAttackTarget->pcAttackTarget = pcUnitData;
						}
						else
						{
							int iDistanceTarget = GetDistanceDouble( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ, pcUnitData->pcAttackTarget->sPosition.iX, pcUnitData->pcAttackTarget->sPosition.iZ );
							if ( GetDistanceDouble( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ, pcUnitData->pcAttackTarget->pcFocusTarget->sPosition.iX, pcUnitData->pcAttackTarget->pcFocusTarget->sPosition.iZ ) > iDistanceTarget )
							{
								pcUnitData->pcAttackTarget->pcAttackTarget = pcUnitData;
								pcUnitData->pcAttackTarget->pcFocusTarget = nullptr;
							}
						}

						int iRandomDamage = 0;
						if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Skill) && (pcUnitData->sCharacterData.iMinDamage > 0) )
						{
							iRandomDamage = RandomI( pcUnitData->sUnitInfo.sSkillPower.sMin, pcUnitData->sUnitInfo.sSkillPower.sMax );

							switch ( pcUnitData->sCharacterData.iMonsterEffectID )
							{
								case MONSTEREFFECTID_GUARDIAN_SAINT:
									DoDamageAreaSkillOnUnits( pcUnitData );
									iRandomDamage = 0;
									break;
							}
						}
						else
						{
							iRandomDamage = RandomI( pcUnitData->sCharacterData.iMinDamage, pcUnitData->sCharacterData.iMaxDamage );
						}

						if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL) ||
							(pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_CHAOSCARA) ||
							(pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_DEATHKNIGHT) ||
							(pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_MOKOVA) ||
							(pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_KELVEZU) ||
							(pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_MOKOVA) )
						{
							iRandomDamage *= 8;
							if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_KELVEZU )
							{
								pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur = 0;
								iRandomDamage = 1;
							}
						}


						if ( pcUnitData->pcAttackTarget )
							iRandomDamage -= (iRandomDamage * pcUnitData->pcAttackTarget->sCharacterData.iAbsorbRating) / 100;
						else
							iRandomDamage = 0;

						if ( iRandomDamage > 0 )
						{
							pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur -= iRandomDamage;

							if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_WEB )

								if ( pcUnitData->pcAttackTarget->sCharacterData.iMonsterEffectID != MONSTEREFFECTID_KELVEZU )
									pcUnitData->pcAttackTarget->iFreezeTimeLeft = -36;
						}


						if ( pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur <= 0 )
						{
							pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur = 0;
							pcUnitData->pcAttackTarget->Animate( ANIMATIONTYPE_Die );

							if ( pcUnitData->pcOwner && pcUnitData->pcOwner->iID && (pcUnitData->pcOwner->iID == pcUnitData->sCharacterData.iOwnerID) )
							{
								pcUnitData->pcAttackTarget->pcKillerData = pcUnitData->pcOwner;

								pcUnitData->pcAttackTarget->pcBellatraKiller = this;
							}
							else
							{
								if ( pcUnitData->pcOwner )
								{
									pcUnitData->sCharacterData.sHP.sCur = 0;
									pcUnitData->Animate( ANIMATIONTYPE_Die );
								}
								else
								{
									pcUnitData->pcAttackTarget = 0;
								}
							}
						}
						else
						{
							if ( pcUnitData->pcAttackTarget->GetAnimation() < ANIMATIONTYPE_Attack )
							{
								pcUnitData->pcAttackTarget->Animate( ANIMATIONTYPE_Flinch );
							}
						}
					}
				}
			}
		}
		else
		{
			pcUnitData->NextTarget( 0, 0, 0 );
		}

		if ( pcUnitData->pcAttackTarget && ((pcUnitData->pcAttackTarget->bActive == FALSE) || (pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur <= 0)) )
		{
			pcUnitData->pcAttackTarget = nullptr;
			if ( pcUnitData->GetAnimation() < ANIMATIONTYPE_Attack )
			{
				pcUnitData->iTargetMoveCount = 1;
				pcUnitData->iMoveCounter = 1;
			}
		}

		if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_DEATHKNIGHT) && pcUnitData->pcAttackTarget )
		{
			if ( GetDistanceDouble( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ, pcUnitData->pcAttackTarget->sPosition.iX, pcUnitData->pcAttackTarget->sPosition.iZ ) > (120 * 120) )
			{
				pcUnitData->pcAttackTarget = 0;
				pcUnitData->NextTarget( 0, 0, 0 );
			}
		}
	}

	BOOL bCanAttack = TRUE;

	switch ( pcUnitData->sCharacterData.iMonsterEffectID )
	{
		case MONSTEREFFECTID_VAMPRICMACHINE:
			if ( GetUnitDataSpawnFlagCount( pcUnitData ) > 3 )
				bCanAttack = FALSE;
			break;

		case MONSTEREFFECTID_MYSTIC:
			if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Die) && (pcUnitData->iFrameTimer < 16) )
			{
				pcUnitData->iAreaDamageArea = pcUnitData->sUnitInfo.iSkillArea * pcUnitData->sUnitInfo.iSkillArea;
				pcUnitData->bMonsterAttacking = TRUE;
				pcUnitData->iFrameTimer = 16;
			}
			if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Die )
			{
				if ( pcUnitData->iLoopTimer > ( 16 * 60 * 2 ) )
				{
					pcUnitData->sCharacterData.sHP.sCur = 0;
					pcUnitData->Animate( ANIMATIONTYPE_Die );
					pcUnitData->iAreaDamageArea = pcUnitData->sUnitInfo.iSkillArea * pcUnitData->sUnitInfo.iSkillArea;
					pcUnitData->bMonsterAttacking = TRUE;
					pcUnitData->iFrameTimer = 16;
				}
			}
			if ( pcUnitData->pcAttackTarget && (pcUnitData->GetAnimation() == ANIMATIONTYPE_Die) )
			{
				if ( (pcUnitData->iFrameTimer == 16) && (pcUnitData->pcAttackTarget->GetAnimation() != ANIMATIONTYPE_Die) && (pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur > 0) )
				{
					pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur -= RandomI( pcUnitData->sUnitInfo.sSkillPower.sMin, pcUnitData->sUnitInfo.sSkillPower.sMax ) * 2;
					if ( pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur <= 0 )
					{
						pcUnitData->pcAttackTarget->sCharacterData.sHP.sCur = 0;
						pcUnitData->pcAttackTarget->Animate( ANIMATIONTYPE_Die );
					}
				}
				pcUnitData->pcAttackTarget = nullptr;
			}
			break;

		case MONSTEREFFECTID_CHAOSCARA:
			if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Skill && (pcUnitData->psModelAnimation->uaValue[0] != 'J') )
			{
				pcUnitData->sUnitInfo.iSkillBoxLeft = 0;
				pcUnitData->sUnitInfo.iSkillBoxRight = 0;
				pcUnitData->sUnitInfo.iSkillBoxTop = 0;
				pcUnitData->sUnitInfo.iSkillBoxBottom = 0;
			}
			break;
	}

	if ( (pcUnitData->pcAttackTarget == nullptr) && pcUnitData->pcFocusTarget && bCanAttack )
	{
		if ( pcUnitData->pcFocusTarget->pcSocketData && (pcUnitData->iStunTimeLeft == 0) && (pcUnitData->iStopMotionCounter == 0) )
		{
			int iDistance = pcUnitData->sCharacterData.iAttackRange;

			if ( (pcUnitData->GetAnimation() != ANIMATIONTYPE_Flinch) &&
				(pcUnitData->GetAnimation() != ANIMATIONTYPE_Die) &&
				(pcUnitData->GetAnimation() != ANIMATIONTYPE_Potion) && iDistance >= 0 )
			{
				if ( pcUnitData->AttackFromPosition(
					pcUnitData->pcFocusTarget->sPosition.iX,
					pcUnitData->pcFocusTarget->sPosition.iY,
					pcUnitData->pcFocusTarget->sPosition.iZ,
					iDistance ) == TRUE )
				{

					pcUnitData->sMoveBeginPosition.iX = 0;
					pcUnitData->sMoveBeginPosition.iZ = 0;
					pcUnitData->iMoveBeginCounter = 0;

					pcUnitData->dwLastActiveTime = TICKCOUNT;

					if ( pcUnitData->sUnitInfo.iSkillChance > 0 && pcUnitData->GetAnimation() == ANIMATIONTYPE_Attack )
					{
						if ( (rand() % 100) < pcUnitData->sUnitInfo.iSkillChance )
						{
							if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_NPC_MAGICMASTER) && (rand() % 100) < 20 )
							{
								pcUnitData->Animate( ANIMATIONTYPE_Potion );
								pcUnitData->iPotionStatus = POTIONSTATUS_MP;
							}
							else if ( pcUnitData->Animate( ANIMATIONTYPE_Skill ) > 0 )
							{
								if ( pcUnitData->sUnitInfo.iSkillType )
								{
									switch ( pcUnitData->sUnitInfo.iSkillType )
									{
										case 102:
										{
											UnitData * pcUnitDataAround = GetUnitDataAround( pcUnitData, 200 );
											if ( pcUnitDataAround )
											{
												pcUnitData->pcAttackTarget = pcUnitDataAround;
												pcUnitData->pcFocusTarget = nullptr;
												pcUnitData->sAngle.iY = GetRadian2D( pcUnitData->sPosition.iX, pcUnitData->sPosition.iZ, pcUnitData->pcAttackTarget->sPosition.iX, pcUnitData->pcAttackTarget->sPosition.iZ );
											}
											else
											{
												if ( pcUnitData->Animate( ANIMATIONTYPE_Attack ) == FALSE )
													pcUnitData->Animate( ANIMATIONTYPE_Idle );
											}
										}
										break;

										case 103:
											pcUnitData->pcFocusTarget->dwCurseQuestTimeOut = TICKCOUNT + 10000;
											pcUnitData->pcFocusTarget->dwCurseDamage = RandomI( 10, 15 );
											break;

										case 101:
											pcUnitData->pcFocusTarget->dwCurseDefenceTime = TICKCOUNT + 10000;
											pcUnitData->pcFocusTarget->dwCurseDefenceParam = RandomI( 2, 6 );

											if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_MYSTIC )
												pcUnitData->pcFocusTarget->dwCurseDefenceParam = RandomI( 10, 16 );

											break;

									}
								}


								if ( pcUnitData->sUnitInfo.iSkillArea > 0 )
								{
									pcUnitData->iAreaDamageArea = 0;

									if ( pcUnitData->sUnitInfo.iSkillPierceRange )
									{

										GetMoveLocation( 0, 0, pcUnitData->sUnitInfo.iSkillPierceRange * 256, 0, pcUnitData->sAngle.iY, 0 );
										int iX = pcUnitData->sPosition.iX + MOVELOCATION_X;
										int iZ = pcUnitData->sPosition.iZ + MOVELOCATION_Z;

										int iDistance = pcUnitData->sUnitInfo.iSkillArea << 8;

										pcUnitData->sUnitInfo.iSkillBoxLeft = iX - iDistance;
										pcUnitData->sUnitInfo.iSkillBoxRight = iX + iDistance;
										pcUnitData->sUnitInfo.iSkillBoxTop = iZ - iDistance;
										pcUnitData->sUnitInfo.iSkillBoxBottom = iZ + iDistance;
									}
								}

								switch ( pcUnitData->sCharacterData.iMonsterEffectID )
								{
									case MONSTEREFFECTID_RATOO:
										DoRemoveStunAreaSkillOnUnits( pcUnitData, 200 );
										break;
									case MONSTEREFFECTID_NPC_SKILLMASTER:
									{
										int iRandom = rand() % 100;
										if ( (iRandom < 3) && (GetUnitByExclusiveUser( pcUnitData ) == nullptr) && pcUnitData->pcExclusiveTarget )
										{
											if ( (iRandom & 1) == 0 )
											{
												//Metal Golem
												if ( UnitData * pcUnitDataSkillMonster = CreateMonsterFromSkill( 0xF, pcUnitData->sPosition.iX,
													pcUnitData->sPosition.iY,
													pcUnitData->sPosition.iZ, pcUnitData->pcExclusiveTarget, pcUnitData ) )
												{
													pcUnitDataSkillMonster->sCharacterData.sSize = 256 + 5 * 15;
													pcUnitDataSkillMonster->saMoveStepCount[0] = 1;
													pcUnitDataSkillMonster->saMoveStepCount[1] = 2;
												}
											}
											else
											{
												//Recall Wolverine
												if ( UnitData * pcUnitDataSkillMonster = CreateMonsterFromSkill( 0x4D, pcUnitData->sPosition.iX,
													pcUnitData->sPosition.iY,
													pcUnitData->sPosition.iZ, pcUnitData->pcExclusiveTarget, pcUnitData ) )
												{
													pcUnitDataSkillMonster->sCharacterData.sSize = 128 + 5 * 15;
													pcUnitDataSkillMonster->saMoveStepCount[0] = 1;
													pcUnitDataSkillMonster->saMoveStepCount[1] = 2;
												}
											}
										}
									}
									break;
									case MONSTEREFFECTID_NPC_MAGICMASTER:
									{
										int iRandom = rand() % 100;
										if ( (iRandom < 3) && (GetUnitByExclusiveUser( pcUnitData ) == nullptr) && pcUnitData->pcExclusiveTarget )
										{
											//Fire Elemental
											if ( UnitData * pcUnitDataSkillMonster = CreateMonsterFromSkill( 0x8D, pcUnitData->sPosition.iX,
												pcUnitData->sPosition.iY,
												pcUnitData->sPosition.iZ, pcUnitData->pcExclusiveTarget, pcUnitData ) )
											{
												pcUnitDataSkillMonster->sCharacterData.sSize = 256 + 5 * 15;
												pcUnitDataSkillMonster->saMoveStepCount[0] = 1;
												pcUnitDataSkillMonster->saMoveStepCount[1] = 2;
											}
										}
										break;
									}
								}
							}
						}
					}
				}
			}

			int iEventFrame = 0;
			if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL )
			{
				if ( (pcUnitData->iFrameTimer == 40) || (pcUnitData->iFrameTimer == 80) )
					iEventFrame = 1;
			}

			int iDamageSend = 0;
			if ( ((iEventFrame == 0) && (pcUnitData->iFrameTimer == 40)) || iEventFrame )
			{
				if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Attack) &&
					(CPriestess::OnHolyReflection( pcUnitData->pcFocusTarget, pcUnitData ) == TRUE) )
				{
					if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_VAMPRICMACHINE )
					{
						iDamageSend = 0;
						MapServer::SpawnSubMonster( pcUnitData, MONSTEREFFECTID_VAMPRICBEE, pcUnitData->sPosition.iX, pcUnitData->sPosition.iY, pcUnitData->sPosition.iZ, 4 );
					}
					else
					{
						if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL) && ((rand() % 100) < 60) )
						{
							CDamageHandler::SkillRangeMonsterSet( pcUnitData, pcUnitData->pcFocusTarget, 100, 128 );

							int iX = (pcUnitData->pcFocusTarget->sPosition.iX - pcUnitData->sPosition.iX) >> 8;
							int iZ = (pcUnitData->pcFocusTarget->sPosition.iZ - pcUnitData->sPosition.iZ) >> 8;
							int iDistance = iX * iX + iZ * iZ;
							if ( iDistance > (200 * 200) )
							{
								int iDamageMin = pcUnitData->sCharacterData.iMinDamage;
								int iDamageMax = pcUnitData->sCharacterData.iMaxDamage;

								pcUnitData->sCharacterData.iMinDamage = (pcUnitData->sCharacterData.iMinDamage * 70) / 100;
								pcUnitData->sCharacterData.iMaxDamage = (pcUnitData->sCharacterData.iMaxDamage * 70) / 100;

								iDamageSend = CDamageHandler::SendDamageBySocket( pcUnitData, nullptr, pcUnitData->pcFocusTarget->pcSocketData, 0 );

								pcUnitData->sCharacterData.iMinDamage = iDamageMin;
								pcUnitData->sCharacterData.iMaxDamage = iDamageMax;
							}
							else
							{
								iDamageSend = CDamageHandler::SendDamageBySocket( pcUnitData, nullptr, pcUnitData->pcFocusTarget->pcSocketData, 0 );
							}

						}
						else
						{
							iDamageSend = CDamageHandler::SendDamageBySocket( pcUnitData, nullptr, pcUnitData->pcFocusTarget->pcSocketData, 0 );
						}
					}

					if ( iDamageSend )
					{
						pcUnitData->pcFocusTarget->iSendAttackCount++;
						pcUnitData->pcFocusTarget->iSendAttackDamage += iDamageSend;

						//Hammer Event Monster
						//TODO: Check if this is correct, since we don't use old models load anymore
						//if ( (pcUnitData->sCharacterData.sGlow == 200) &&
						//	(pcUnitData->pcFocusTarget->sCharacterData.iShadowSize <= 0x1000) || (pcUnitData->pcFocusTarget->sCharacterData.iShadowSize > 0x1002) )
						//{

						//	if ( pcUnitData->lHammerUserID != pcUnitData->pcFocusTarget->iID )
						//	{

						//		pcUnitData->iHammerAttackCount = 0;
						//		pcUnitData->lHammerUserID = pcUnitData->pcFocusTarget->iID;
						//	}

						//	pcUnitData->iHammerAttackCount++;

						//	if ( pcUnitData->iHammerAttackCount >= 10 )
						//	{
						//		PacketSimple sPacketComicSkin;
						//		sPacketComicSkin.iLength = sizeof( PacketSimple );
						//		sPacketComicSkin.iHeader = PKTHDR_SkinComic;
						//		sPacketComicSkin.iUnk2 = (rand() % 2) + 1;
						//		sPacketComicSkin.iUnk = 60 * 10;
						//		sPacketComicSkin.iUnk3 = 0;
						//		sPacketComicSkin.iUnk4 = 0;
						//		SENDPACKET( USERDATATOUSER( pcUnitData->pcFocusTarget, &sPacketComicSkin );

						//		if ( pcUnitData->sUnitInfo.bUseHammerEventModel && pcUnitData->sCharacterData.Monster.szHeadModel[1] )
						//		{
						//			pcUnitData->Free();

						//			STRINGCOPY( pcUnitData->sCharacterData.Monster.szBodyModel, pcUnitData->sCharacterData.Monster.szHeadModel + 1 );
						//			//SetLoadPattern( pcUnitData->sCharacterData.Monster.szBodyModel );
						//			pcUnitData->sCharacterData.cNewLoad++;
						//			pcUnitData->Animate( ANIMATIONTYPE_Idle );

						//		}

						//		pcUnitData->sCharacterData.sGlow = 0;
						//		pcUnitData->sUnitInfo.bUseHammerEventModel = FALSE;
						//	}
						//}
					}

					pcUnitData->bMonsterAttacking = TRUE;
					pcUnitData->iHits++;
				}
			}

			iEventFrame = 32;

			if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_BABEL) || (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_DEATHKNIGHT) )
			{
				iEventFrame = 120;
				if ( pcUnitData->iFrameTimer == iEventFrame )
				{
					pcUnitData->sCharacterData.sHP.sCur += GetUnitDataHPRegen( pcUnitData );

					if ( (pcUnitData->sCharacterData.sHP.sCur > pcUnitData->sCharacterData.sHP.sMax) || (pcUnitData->sCharacterData.sHP.sCur < 0) )
						pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;

				}
			}

			if ( pcUnitData->iFrameTimer == iEventFrame )
			{
				if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Skill )
				{
					if ( pcUnitData->sCharacterData.iMonsterEffectID != MONSTEREFFECTID_MYSTIC )
					{
						pcUnitData->iAreaDamageArea = pcUnitData->sUnitInfo.iSkillArea * pcUnitData->sUnitInfo.iSkillArea;
						if ( (pcUnitData->iAreaDamageArea == 0) && pcUnitData->pcFocusTarget )
						{
							SkillUnitDamage( pcUnitData, pcUnitData->pcFocusTarget );
						}
						pcUnitData->bMonsterAttacking = TRUE;
					}
				}
			}
		}
		else
		{
			pcUnitData->NextTarget( 0, 0, 0 );
		}
	}

	if ( pcUnitData->iFreezeTimeLeft > 0 )
		pcUnitData->iFreezeTimeLeft--;

	if ( pcUnitData->iFreezeTimeLeft < 0 )
		pcUnitData->iFreezeTimeLeft++;

	if ( pcUnitData->iStunTimeLeft > 0 )
		pcUnitData->iStunTimeLeft--;

	if ( pcUnitData->iStopMotionCounter > 0 )
		pcUnitData->iStopMotionCounter--;

	if ( pcUnitData->sHolyMind.sMax > 0 )
		pcUnitData->sHolyMind.sMax--;

	if ( pcUnitData->sSuccubus.sMax > 0 )
		pcUnitData->sSuccubus.sMax--;

	if ( pcUnitData->sPoison.sMax > 0 )
	{
		pcUnitData->sPoison.sMax--;
		if ( (pcUnitData->iLoopTimer & 0xF) == 0 )
		{
			if ( (pcUnitData->sCharacterData.sHP.sCur > 0) && (pcUnitData->GetAnimation() != ANIMATIONTYPE_Die) )
			{
				pcUnitData->sCharacterData.sHP.sCur -= pcUnitData->sPoison.sMin;
				if ( pcUnitData->sCharacterData.sHP.sCur < 1 )
				{
					pcUnitData->sCharacterData.sHP.sCur = 1;

					pcUnitData->sPoison.sMax = 0;
				}
			}
		}
	}

	if ( pcUnitData->sHolyIncantation.sMax > 0 )
	{
		pcUnitData->sHolyIncantation.sMax--;
		if ( (pcUnitData->sHolyIncantation.sMax == 0) && (pcUnitData->sCharacterData.iMonsterType == MONSTERTYPE_Friendly) )
		{
			pcUnitData->sCharacterData.iMonsterType = (EMonsterType)pcUnitData->sHolyIncantation.sMin;	//smCHAR_MONSTER_USER
			pcUnitData->sCharacterData.cNewLoad++;
			pcUnitData->pcOwner = nullptr;
		}
	}

	if ( pcUnitData->iHPBlocks )
	{
		if ( pcUnitData->sHPBlocks.sCur > 0 )
		{
			short sHPMax = pcUnitData->sCharacterData.sHP.sMax;

			int iPercentHP = pcUnitData->sCharacterData.sHP.sMax <= 0 ? 0 : (pcUnitData->sCharacterData.sHP.sCur * 100) / pcUnitData->sCharacterData.sHP.sMax;
			if ( iPercentHP <= 10 )
			{
				pcUnitData->sHPBlocks.sCur--;
				pcUnitData->sCharacterData.sHP.sCur = pcUnitData->sCharacterData.sHP.sMax;

				if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_KELVEZU) && ((pcUnitData->sHPBlocks.sCur & 0x1) == 0) )
				{
					MapServer::SpawnSubMonster( pcUnitData, MONSTEREFFECTID_CHAOSCARA, pcUnitData->sPosition.iX + ((rand() % (200 * fONE)) - 100 * fONE), 0, pcUnitData->sPosition.iZ + ((rand() % (200 * fONE)) - 100 * fONE), 8);
					MapServer::SpawnSubMonster( pcUnitData, MONSTEREFFECTID_CHAOSCARA, pcUnitData->sPosition.iX + ((rand() % (200 * fONE)) - 100 * fONE), 0, pcUnitData->sPosition.iZ + ((rand() % (200 * fONE)) - 100 * fONE), 8 );
				}
				else if ( (pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_MOKOVA) && ((pcUnitData->sHPBlocks.sCur & 0x1) == 0) )
				{
					MapServer::SpawnSubMonster( pcUnitData, MONSTEREFFECTID_DARKGUARD, pcUnitData->sPosition.iX + ((rand() % (200 * fONE)) - 100 * fONE), 0, pcUnitData->sPosition.iZ + ((rand() % (200 * fONE)) - 100 * fONE), 8 );
					MapServer::SpawnSubMonster( pcUnitData, MONSTEREFFECTID_DARKGUARD, pcUnitData->sPosition.iX + ((rand() % (200 * fONE)) - 100 * fONE), 0, pcUnitData->sPosition.iZ + ((rand() % (200 * fONE)) - 100 * fONE), 8 );
				}

			}
		}

		int iPercentHP = pcUnitData->sCharacterData.sHP.sMax <= 0 ? 0 : (pcUnitData->sCharacterData.sHP.sCur * 100) / pcUnitData->sCharacterData.sHP.sMax;
		pcUnitData->sVirtualHP.sCur = pcUnitData->sHPBlocks.sCur * 100;
		pcUnitData->sVirtualHP.sCur += iPercentHP;
		pcUnitData->sVirtualHP.sMax = (pcUnitData->sHPBlocks.sMax + 1) * 100;

		if ( (pcUnitData->sVirtualHP.sCur <= 0) && (pcUnitData->sCharacterData.sHP.sCur > 0) )
		{
			pcUnitData->sVirtualHP.sCur = 1;
		}

		if ( pcUnitData->sVirtualHP.sCur > pcUnitData->sVirtualHP.sMax )
			pcUnitData->sVirtualHP.sCur = pcUnitData->sVirtualHP.sMax;
	}

	pcUnitData->iFrameTimer += 3;

	return 0;
}

BOOL UnitServer::MainUnitNPCData( UnitData * pcUnitData )
{
	if ( pcUnitData->psBody->iaTalkUnknown1[10] )
	{
		if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Special )
		{

			pcUnitData->iFrame += 320;
			if ( pcUnitData->iFrame > pcUnitData->psModelAnimation->iEndFrame * 160 )
			{
				pcUnitData->Free();
				return TRUE;
			}
			
			SaveFramesNPC( pcUnitData );
			pcUnitData->iFrameTimer += 3;
			return TRUE;
		}

		int iTalkCount = 0;
		if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking )
		{
			int iX = pcUnitData->sPosition.iX;
			int iZ = pcUnitData->sPosition.iZ;
			pcUnitData->Move( 4 << 8 );

			if ( (iX == pcUnitData->sPosition.iX) && (iZ == pcUnitData->sPosition.iZ) )
			{
				iTalkCount = rand() % 100;
				iTalkCount = pcUnitData->psBody->iaTalkUnknown2[iTalkCount];

				if ( (iTalkCount == pcUnitData->iAnimationID) && (pcUnitData->psBody->saModelAnimation[iTalkCount].iType == ANIMATIONTYPE_Walking) )
				{
					iX = (pcUnitData->sPosition.iX - pcUnitData->sSpawnPosition.iX) >> 8;
					iZ = (pcUnitData->sPosition.iZ - pcUnitData->sSpawnPosition.iZ) >> 8;
					int iDistance = iX * iX + iZ * iZ;
					if ( iDistance > pcUnitData->iLureDistance )
						pcUnitData->SetTarget( pcUnitData->sSpawnPosition.iX >> 8, pcUnitData->sSpawnPosition.iZ >> 8 );
				}
				else
				{
					pcUnitData->iFrame = pcUnitData->psModelAnimation->iEndFrame * 160;
					pcUnitData->psModelAnimation->iType = ANIMATIONTYPE_Idle;
				}
			}
		}

		pcUnitData->iFrame += 230;
		if ( pcUnitData->iFrame > (pcUnitData->psModelAnimation->iEndFrame * 160) )
		{
			if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking )
				pcUnitData->iFrame = (pcUnitData->psModelAnimation->iBeginFrame * 160) + (pcUnitData->iFrame - pcUnitData->psModelAnimation->iEndFrame * 160);
			else if ( pcUnitData->sCharacterData.iMonsterEffectID != MONSTEREFFECTID_FURY )
			{
				if ( iTalkCount == 0 )
					iTalkCount = rand() % 100;

				pcUnitData->ChangeAnimationID( pcUnitData->psBody->iaTalkUnknown2[iTalkCount] );
				if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking )
				{
					int iX = (pcUnitData->sPosition.iX - pcUnitData->sSpawnPosition.iX) >> 8;
					int iZ = (pcUnitData->sPosition.iZ - pcUnitData->sSpawnPosition.iZ) >> 8;
					int iDistance = iX * iX + iZ * iZ;
					if ( iDistance > pcUnitData->iLureDistance )
						pcUnitData->SetTarget( pcUnitData->sSpawnPosition.iX >> 8, pcUnitData->sSpawnPosition.iZ >> 8 );
				}
				else
					pcUnitData->sAngle.iY = rand() % 4095;
			}
			else
				pcUnitData->Animate( ANIMATIONTYPE_Idle );
		}
	}
	else
	{
		if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Special )
		{
			pcUnitData->iFrame += 320;

			if ( pcUnitData->iFrame > (pcUnitData->psModelAnimation->iEndFrame * 160) )
			{
				pcUnitData->Free();
				return TRUE;
			}

			SaveFramesNPC( pcUnitData );
			pcUnitData->iFrameTimer += 3;
			
			return TRUE;
		}


		if ( pcUnitData->GetAnimation() && (pcUnitData->GetAnimation() < ANIMATIONTYPE_Attack) )
		{
			if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking )
			{
				int iX = pcUnitData->sPosition.iX;
				int iZ = pcUnitData->sPosition.iZ;

				pcUnitData->Move( 4 << 8 );

				if ( (iX == pcUnitData->sPosition.iX) && iZ == (pcUnitData->sPosition.iZ) )
				{
					pcUnitData->sAngle.iY = rand() % 4095;
				}

				if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Walking ) 
					pcUnitData->Animate( ANIMATIONTYPE_Walking );
			}
			else
			{
				if ( pcUnitData->GetAnimation() != ANIMATIONTYPE_Idle )
					pcUnitData->Animate( ANIMATIONTYPE_Idle );
			}
		}

		pcUnitData->iFrame += 320;

		if ( pcUnitData->iFrame > (pcUnitData->psModelAnimation->iEndFrame * 160) )
		{

			if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Die )
			{
				pcUnitData->iFrame = pcUnitData->psModelAnimation->iEndFrame * 160;
			}
			else
			{

				if ( pcUnitData->psModelAnimation->bLoop == TRUE )
				{
					pcUnitData->iFrame = (pcUnitData->psModelAnimation->iBeginFrame * 160) + (pcUnitData->iFrame - pcUnitData->psModelAnimation->iEndFrame * 160);
				}
				else 
					pcUnitData->Animate( ANIMATIONTYPE_Idle );

				if ( (pcUnitData->GetAnimation() == ANIMATIONTYPE_Idle) && (pcUnitData->sCharacterData.iMonsterEffectID != MONSTEREFFECTID_FURY) )
				{
					int iRandom = rand() % 4095;
					if ( ((iRandom >> 2) & 3) == 0 )
					{
						if ( pcUnitData->Animate( ANIMATIONTYPE_True ) == FALSE )
						{
							if ( pcUnitData->iMovementSpeed )
								pcUnitData->sAngle.iY = iRandom & 4095;

							if ( pcUnitData->iLureDistance )
								pcUnitData->Animate( ANIMATIONTYPE_Walking );
							else
								pcUnitData->Animate( ANIMATIONTYPE_Idle );
						}
					}
					else
					{
						pcUnitData->Animate( ANIMATIONTYPE_Idle );
					}

					if ( pcUnitData->GetAnimation() == ANIMATIONTYPE_Walking )
					{
						int iX = (pcUnitData->sPosition.iX - pcUnitData->sSpawnPosition.iX) >> 8;
						int iZ = (pcUnitData->sPosition.iZ - pcUnitData->sSpawnPosition.iZ) >> 8;
						int iDistance = iX * iX + iZ * iZ;
						if ( iDistance > pcUnitData->iLureDistance )
							pcUnitData->SetTarget( pcUnitData->sSpawnPosition.iX >> 8, pcUnitData->sSpawnPosition.iZ >> 8 );
					}
				}
				else 
					pcUnitData->Animate( ANIMATIONTYPE_Idle );
			}
		}
	}

	SaveFramesNPC( pcUnitData );
	pcUnitData->iFrameTimer += 3;
	return TRUE;
}

void UnitServer::ProcessQuestPacket( UserData * pcUserData, UnitData * pcUnitData, QuestCodeSkillUnit iQuestCode, int iQuestParam )
{
	if ( User * pcUser = USERDATATOUSER( pcUserData ) )
	{
		PacketSimple sPacket;
		sPacket.iLength = sizeof( PacketSimple );
		sPacket.iHeader = PKTHDR_QuestSkillStepUpdate;
		sPacket.iUnk2 = pcUnitData->sUnitInfo.iQuestTypeID;
		sPacket.iUnk = pcUnitData->sUnitInfo.iQuestSubID;
		sPacket.iUnk3 = 0;
		sPacket.iUnk4 = 0;

		switch ( iQuestCode )
		{
			case QUESTCODESKILLUNIT_NPCMorion:
			case QUESTCODESKILLUNIT_NPCTempriskron:
			{
				if ( iQuestParam == 1 )
				{
					sPacket.iUnk3 = TRUE;
					ITEMSERVER->CreateItem( pcUserData, ITEMID_PotionAgingQuestBee, 1 );
				}
			}
			break;

			case QUESTCODESKILLUNIT_Tier4:
			case QUESTCODESKILLUNIT_FuryQuest:
				sPacket.iUnk2 = iQuestCode;
				sPacket.iUnk = iQuestParam;
				break;

			default:
				break;
		}

		SENDPACKET( pcUser, &sPacket );
	}
}

void UnitServer::SQLSaveItemRare( User * pcUser, Unit * pcUnit, Item * psItem )
{
	bool bSave = false;
	if ( (psItem->sItemID.ToItemBase() == ITEMBASE_Weapon) || (psItem->sItemID.ToItemBase() == ITEMBASE_Defense) || (psItem->sItemID.ToItemBase() == ITEMBASE_Accessory) )
	{
		switch ( psItem->sItemID.ToItemType() )
		{
			case ITEMTYPE_Axe:
			case ITEMTYPE_Claw:
			case ITEMTYPE_Hammer:
			case ITEMTYPE_Wand:
			case ITEMTYPE_Scythe:
			case ITEMTYPE_Bow:
			case ITEMTYPE_Sword:
			case ITEMTYPE_Javelin:
			case ITEMTYPE_Phantom:
			case ITEMTYPE_Dagger:

			case ITEMTYPE_Armor:
			case ITEMTYPE_Boots:
			case ITEMTYPE_Gauntlets:
			case ITEMTYPE_Shield:
			case ITEMTYPE_Robe:
			case ITEMTYPE_Bracelets:
			case ITEMTYPE_Orb:
				if ( (psItem->sItemID.ToItemID() == ITEMID_MokovaBoots) || (psItem->sItemID.ToItemID() == ITEMID_DraxosBoots) )
					bSave = true;

				if ( (psItem->iLevel == 106) || (psItem->iLevel == 110) || (psItem->iLevel == 114) || (psItem->iLevel == 118) || (psItem->iLevel == 138) || (psItem->iLevel == 134) )
					bSave = true;
				break;

			case ITEMTYPE_Amulet:
				if ( (psItem->sItemID.ToItemID() == ITEMID_KelvezuAmulet) )
					bSave = true;
				break;

			case ITEMTYPE_Sheltom:
				if ( (psItem->sItemID.ToItemID() == ITEMID_DarkGuardianSheltom) || (psItem->sItemID.ToItemID() == ITEMID_Oredo) || 
					(psItem->sItemID.ToItemID() == ITEMID_Sol) || (psItem->sItemID.ToItemID() == ITEMID_Sapphire) )
					bSave = true;
				break;

			case ITEMTYPE_Crafting:
				if ( (psItem->sItemID.ToItemID() == ITEMID_LightBlueRune) || (psItem->sItemID.ToItemID() == ITEMID_LightPurpleRune) || (psItem->sItemID.ToItemID() == ITEMID_DarkRedRune) || (psItem->sItemID.ToItemID() == ITEMID_LightOrangeRune) ||
					 (psItem->sItemID.ToItemID() == ITEMID_LightSilverRune) || (psItem->sItemID.ToItemID() == ITEMID_ThroneRedRune) || (psItem->sItemID.ToItemID() == ITEMID_ThroneYellowRune) || (psItem->sItemID.ToItemID() == ITEMID_ThroneGreenRune) )
					bSave = true;
				break;

			case ITEMTYPE_Ring:
				if ((psItem->sItemID.ToItemID() == ITEMID_ThroneRing ))
					bSave = true;
				break;

			case ITEMTYPE_Ring2:
				if ( (psItem->sItemID.ToItemID() == ITEMID_ValentoRing) || (psItem->sItemID.ToItemID() == ITEMID_DevilShyRing ) )
					bSave = true;
				break;

			case ITEMTYPE_Earring:
				if ( (psItem->sItemID.ToItemID() == ITEMID_GorgonyteEarring) )
					bSave = true;
				break;

			default:
				break;
		}
	}

	if ( bSave )
	{
		SQLConnection * pcDB = SQLCONNECTION( DATABASEID_LogDB );
		if ( pcDB->Open() )
		{
			if ( pcDB->Prepare( "INSERT INTO RareItemsLog([CharacterID],[MapID],[MonsterName],[IsBossMonster],[ItemID],[ItemName],[ItemLevel],[ItemCode1],[ItemCode2],[ServerID],[Date]) VALUES (?,?,?,?,?,?,?,?,?,?,GETDATE())" ) )
			{
				auto pszMonsterName = pcUnit->GetName();
				BOOL bIsBossMonster = (pcUnit->pcUnitData->sCharacterData.sGlow == TRUE);
				int iItemID = psItem->sItemID.ToInt();

				int iServerID = SERVER_CODE;

				pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &pcUser->iCharacterID );
				pcDB->BindParameterInput( 2, PARAMTYPE_Integer, &pcUser->pcUserData->iMapID );
				pcDB->BindParameterInput( 3, PARAMTYPE_String, (void*)pszMonsterName, STRLEN( pszMonsterName ) );
				pcDB->BindParameterInput( 4, PARAMTYPE_Integer, &bIsBossMonster );
				pcDB->BindParameterInput( 5, PARAMTYPE_Integer, &iItemID );
				pcDB->BindParameterInput( 6, PARAMTYPE_String, psItem->szItemName, STRLEN( psItem->szItemName ) );
				pcDB->BindParameterInput( 7, PARAMTYPE_Integer, &psItem->iLevel );
				pcDB->BindParameterInput( 8, PARAMTYPE_Integer, &psItem->iChk1 );
				pcDB->BindParameterInput( 9, PARAMTYPE_Integer, &psItem->iChk2 );
				pcDB->BindParameterInput( 10, PARAMTYPE_Integer, &iServerID );
				pcDB->Execute();
			}
			pcDB->Close();
		}
	}

}

void UnitServer::OnBeforeCrystalKill( UnitData * pcUnitData )
{
	if ( pcUnitData->pcOwner )
	{
		//Crystal Die
		ITEMSERVER->OnUseCrystalDie( pcUnitData->pcOwner, pcUnitData->iID );

		if ( pcUnitData->iPetID )
		{
			//Send Cancel Pet Skill
			PacketSimple s;

			ZeroMemory( &s, sizeof( PacketSimple ) );

			//Convert PetID
			int iPetID = 0;

			//Metal Golem
			if ( pcUnitData->iPetID == 15 )
				iPetID = 77;
			//Fire Elemental
			else if ( pcUnitData->iPetID == 77 )
				iPetID = 74;
			//Wolverine
			else if ( pcUnitData->iPetID == 141 )
				iPetID = 74;
			//Hestian
			else if ( pcUnitData->iPetID == 16 )
				iPetID = 74;

			s.iLength = sizeof( PacketSimple );
			s.iHeader = PKTHDR_CancelSkill;
			s.iUnk = pcUnitData->iID;
			s.iUnk2 = iPetID;
			SENDPACKET( USERDATATOUSER( pcUnitData->pcOwner ), &s, TRUE );
		}
	}
}

UnitData * UnitServer::DoDamageAreaSkillOnUnits( UnitData * pcUnitData )
{
	int iSkillAreaDouble = pcUnitData->sUnitInfo.iSkillArea * pcUnitData->sUnitInfo.iSkillArea;
	int iDamage = RandomI( pcUnitData->sUnitInfo.sSkillPower.sMin, pcUnitData->sUnitInfo.sSkillPower.sMax );

	int iHP = 0;

	if ( pcUnitData->pcMapSpawn )
	{
		for ( int i = 0; i < MAX_ALIVEMONSTERS; i++ )
		{
			if ( pcUnitData->pcMapSpawn->pcaUnitData[i] && (pcUnitData->pcMapSpawn->pcaUnitData[i] != pcUnitData) && (pcUnitData->pcMapSpawn->pcaUnitData[i]->sCharacterData.sHP.sCur > 0) )
			{
				int iX = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iX - pcUnitData->sPosition.iX) >> 8;
				int iY = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iY - pcUnitData->sPosition.iY) >> 8;
				int iZ = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iZ - pcUnitData->sPosition.iZ) >> 8;
				int iDistance = iX * iX + iZ * iZ;

				if ( abs( iY ) < 80 && iDistance < iSkillAreaDouble )
				{
					UnitData * pcAttackTarget = pcUnitData->pcMapSpawn->pcaUnitData[i];

					if ( pcUnitData->sCharacterData.iClanID != 0 && pcUnitData->sCharacterData.iClanID == pcAttackTarget->sCharacterData.iClanID )
					{
						iHP = 0;
					}
					else
					{
						iHP = iDamage;
						iHP -= (iDamage * pcAttackTarget->sCharacterData.iAbsorbRating) / 100;
					}

					if ( iHP > 0 )
					{
						pcAttackTarget->sCharacterData.sHP.sCur -= (short)iHP;
						if ( pcAttackTarget->sCharacterData.sHP.sCur <= 0 )
						{
							pcAttackTarget->sCharacterData.sHP.sCur = 0;
							pcAttackTarget->Animate( ANIMATIONTYPE_Die );

							if ( pcUnitData->pcOwner && pcUnitData->pcOwner->iID && (pcUnitData->pcOwner->iID == pcUnitData->sCharacterData.iOwnerID) )
							{
								pcAttackTarget->pcKillerData = pcUnitData->pcOwner;
								pcAttackTarget->sUnitInfo.iExp >>= 1;
								pcAttackTarget->pcKillerUnitData = pcUnitData;
							}
							else
							{
								if ( pcUnitData->pcOwner )
								{
									pcUnitData->sCharacterData.sHP.sCur = 0;
									pcUnitData->Animate( ANIMATIONTYPE_Die );
								}
								else
								{
									pcUnitData->pcAttackTarget = 0;
								}
							}
						}
						else
						{
							if ( pcAttackTarget->GetAnimation() != ANIMATIONTYPE_Die )
							{
								pcAttackTarget->Animate( ANIMATIONTYPE_Flinch );
							}
						}
					}
				}
			}
		}
	}

	return nullptr;
}

UnitData * UnitServer::DoRemoveStunAreaSkillOnUnits( UnitData * pcUnitData, int iRange )
{
	int iSkillAreaDouble = iRange * iRange;

	if ( pcUnitData->pcMapSpawn )
	{
		for ( int i = 0; i < MAX_ALIVEMONSTERS; i++ )
		{
			if ( pcUnitData->pcMapSpawn->pcaUnitData[i] && (pcUnitData->pcMapSpawn->pcaUnitData[i] != pcUnitData) && (pcUnitData->pcMapSpawn->pcaUnitData[i]->sCharacterData.sHP.sCur > 0) )
			{
				int iX = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iX - pcUnitData->sPosition.iX) >> 8;
				int iY = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iY - pcUnitData->sPosition.iY) >> 8;
				int iZ = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iZ - pcUnitData->sPosition.iZ) >> 8;
				int iDistance = iX * iX + iZ * iZ;

				if ( abs( iY ) < 80 && iDistance < iSkillAreaDouble )
				{
					UnitData * pcAttackTarget = pcUnitData->pcMapSpawn->pcaUnitData[i];
					if ( pcUnitData->sCharacterData.iMonsterEffectID == MONSTEREFFECTID_RATOO )
					{
						pcAttackTarget->iStunTimeLeft = 0;
					}
				}
			}
		}
	}
	return nullptr;
}

UnitData * UnitServer::GetUnitDataAround( UnitData * pcUnitData, int iDistance )
{
	if ( pcUnitData->pcMapSpawn )
	{
		for ( int i = 0; i < MAX_ALIVEMONSTERS; i++ )
		{
			if ( pcUnitData->pcMapSpawn->pcaUnitData[i] && (pcUnitData->pcMapSpawn->pcaUnitData[i] != pcUnitData) && 
				(pcUnitData->pcMapSpawn->pcaUnitData[i]->sSuccubus.sMax == 0) &&
				(pcUnitData->pcMapSpawn->pcaUnitData[i]->sCharacterData.sHP.sCur > 0) )
			{
				int iX = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iX - pcUnitData->sPosition.iX) >> 8;
				int iY = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iY - pcUnitData->sPosition.iY) >> 8;
				int iZ = (pcUnitData->pcMapSpawn->pcaUnitData[i]->sPosition.iZ - pcUnitData->sPosition.iZ) >> 8;

				int iDistance2 = iX * iX + iZ * iZ;

				if ( (abs( iY ) < 80) && (iDistance2 < (iDistance * iDistance)) )
				{
					return pcUnitData->pcMapSpawn->pcaUnitData[i];
				}
			}
		}
	}
	
	return nullptr;
}

void UnitServer::SaveFrames( UnitData * pcUnitData )
{
	pcUnitData->iMoveSequenceCounter = (pcUnitData->iMoveSequenceCounter + 1) % _countof( pcUnitData->saMoveSequence );

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[0] = pcUnitData->sAngle.iX;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[1] = pcUnitData->sAngle.iY;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[2] = pcUnitData->sAngle.iZ;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[3] = pcUnitData->iAnimationID;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].sPosition = pcUnitData->sPosition;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].iFrame = (pcUnitData->iFrameType << 24) | pcUnitData->iFrame;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].lTargetID = 0;

	if ( pcUnitData->pcAttackTarget )
		pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].lTargetID = pcUnitData->pcAttackTarget->iID;

	if ( pcUnitData->pcFocusTarget )
		pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].lTargetID = pcUnitData->pcFocusTarget->iID;

	CopyMemory( pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saEquippedItems, pcUnitData->saEquippedItems, sizeof( short ) * _countof( pcUnitData->saEquippedItems ) );
}

void UnitServer::SaveFramesNPC( UnitData * pcUnitData )
{
	pcUnitData->iMoveSequenceCounter = (pcUnitData->iMoveSequenceCounter + 1) % _countof( pcUnitData->saMoveSequence );

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[0] = pcUnitData->sAngle.iX;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[1] = pcUnitData->sAngle.iY;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[2] = pcUnitData->sAngle.iZ;
	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saAngle[3] = pcUnitData->iAnimationID;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].sPosition = pcUnitData->sPosition;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].iFrame = (pcUnitData->iFrameType << 24) | pcUnitData->iFrame;

	pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].lTargetID = 0;

	if ( pcUnitData->pcTarget )
		pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].lTargetID = pcUnitData->pcTarget->iID;

	CopyMemory( pcUnitData->saMoveSequence[pcUnitData->iMoveSequenceCounter].saEquippedItems, pcUnitData->saEquippedItems, sizeof( short ) * _countof( pcUnitData->saEquippedItems ) );
}

int UnitServer::DefendChance( int iDefenseRating, int iAttackRating )
{
	//Chance to block = ((def - atr) / def) * 100
	if ( iDefenseRating <= iAttackRating )
		return 10;

	double def = iDefenseRating;
	double def_rtg = iDefenseRating - iAttackRating;
	double mul = 100.0f;
	double ret = (def_rtg / def) * mul;

	if ( ret > 90.0f )
		return 90;
	if ( ret < 10.0f )
		return 10;

	return (int)ret;
}

int UnitServer::GetDistanceDouble( int iX1, int iZ1, int iX2, int iZ2 )
{
	int iX = (iX1 - iX2) >> 8;
	int iZ = (iZ1 - iZ2) >> 8;

	return (iX * iX) + (iZ * iZ);
}
