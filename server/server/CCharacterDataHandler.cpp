#include "stdafx.h"
#include "CCharacterDataHandler.h"


CCharacterDataHandler::CCharacterDataHandler()
{
	pcSQLCharacterDataHandler			= new CSQLCharacterDataHandler();
	pcSQLCharacterItemHandler			= new CSQLCharacterItemHandler();
	pcSQLCharacterSaveHandler			= new CSQLCharacterSaveHandler();
	pcSQLCharacterDropItemSaveHandler	= new CSQLCharacterDropItemSaveHandler();
}


CCharacterDataHandler::~CCharacterDataHandler()
{
	DELET( pcSQLCharacterDropItemSaveHandler );
	DELET( pcSQLCharacterSaveHandler );
	DELET( pcSQLCharacterItemHandler );
	DELET( pcSQLCharacterDataHandler );
}

BOOL CCharacterDataHandler::LoadCharacterInventory( User * pcUser, PacketCharacterRecordData & sPacket )
{
/* 
	RecordItem sRecordItem;

	int iBufferSize = 0;
	BYTE * pbaItemDataRecord = (BYTE *)&sPacket.baData;

	DropItemData saDropItemData[MAX_RECORD_ITEMS];

	int iDropItemCount = 0;

	ZeroMemory( pcUser->pcUserData->iaServerPotion, sizeof( int ) * 3 * 4 );

	if ( sPacket.iItemCount > MAX_RECORD_ITEMS )
		sPacket.iItemCount = MAX_RECORD_ITEMS;
	
	if ( sPacket.iDropItemCount > _countof( sPacket.saDropItemData ) ) 
		sPacket.iDropItemCount = _countof( sPacket.saDropItemData );

	for ( UINT i = 0; i < sPacket.iItemCount; i++ )
	{
		if ( i >= (INVENTORYSERVER_MAX * 2) )
			break;

		ITEMSERVER->DecodeItemsDataPotion( pcUser->pcUserData, (BYTE *)pbaItemDataRecord, (BYTE *)&sRecordItem, 44, &sPacket );

		if ( sRecordItem.sItem.sItemID.ToItemID() && sRecordItem.sItem.iChk1 &&
			sRecordItem.sItem.iChk2 &&
			(sRecordItem.sItem.sItemID.ToItemBase() != ITEMBASE_Potion) )
		{
			int j = 0;

			for ( j = 0; j < sPacket.iDropItemCount; j++ )
			{
				if ( sPacket.saDropItemData[j].iItemID == sRecordItem.sItem.sItemID.ToItemID() &&
					sPacket.saDropItemData[j].iChk1 == sRecordItem.sItem.iChk1 &&
					sPacket.saDropItemData[j].iChk2 == sRecordItem.sItem.iChk2 )
				{

					if ( iDropItemCount < _countof( sPacket.saDropItemData ) )
					{
						CopyMemory( &saDropItemData[iDropItemCount++], &sPacket.saDropItemData[j], sizeof( DropItemData ) );
					}

					break;
				}
			}

			if ( j >= sPacket.iDropItemCount )
			{
				for ( j = 0; j < i; j++ )
				{
					if ( pcUser->pcUserData->sInventoryItems[j].iItemID &&
						pcUser->pcUserData->sInventoryItems[j].iItemID == sRecordItem.sItem.sItemID.ToItemID() &&
						pcUser->pcUserData->sInventoryItems[j].iChk1 == sRecordItem.sItem.iChk1 &&
						pcUser->pcUserData->sInventoryItems[j].iChk2 == sRecordItem.sItem.iChk2 )
					{
						PacketLogCheatEx sPacketLogCheatEx;

						sPacketLogCheatEx.iCheatID = CHEATLOGID_ServerInventoryCopiedItem;
						sPacketLogCheatEx.WxParam = 1;
						//sPacketLogCheatEx.LxParam = (int)"*INVENTORY";
						sPacketLogCheatEx.LParam = sRecordItem.sItem.sItemID.ToItemID();
						sPacketLogCheatEx.SParam = sRecordItem.sItem.iChk1;
						sPacketLogCheatEx.EParam = sRecordItem.sItem.iChk2;
						LOGSERVER->OnLogCheat( pcUser->pcUserData, &sPacketLogCheatEx );
						break;
					}
				}
				if ( j >= i )
				{
					pcUser->pcUserData->sInventoryItems[i].iItemID = sRecordItem.sItem.sItemID.ToItemID();
					pcUser->pcUserData->sInventoryItems[i].iChk1 = sRecordItem.sItem.iChk1;
					pcUser->pcUserData->sInventoryItems[i].iChk2 = sRecordItem.sItem.iChk2;
				}
			}
		}

		int iSize = ((int *)pbaItemDataRecord)[0];
		iBufferSize += iSize;
		pbaItemDataRecord += iSize;
		if ( iBufferSize >= (sizeof( RecordItem ) * MAX_RECORD_ITEMS) ) 
			break;
	}


	pcUser->pcUserData->iSaveItems = iDropItemCount;
	CopyMemory( pcUser->pcUserData->sDropItems, saDropItemData, sizeof( DropItemData ) * iDropItemCount );
	*/
	return 0;
}

BOOL CCharacterDataHandler::LoadCharacterInventoryNew( User * pcUser, PacketCharacterRecordData & sPacket )
{
	RecordItem sRecordItem;

	int iBufferSize = 0;
	BYTE * pbaItemDataRecord = (BYTE *)&sPacket.baData;

	ZeroMemory( pcUser->pcUserData->iaServerPotion, sizeof( int ) * 3 * 4 );

	if ( sPacket.iItemCount > MAX_RECORD_ITEMS )
		sPacket.iItemCount = MAX_RECORD_ITEMS;

	int iInventoryCount = 0;

	ZeroMemory( pcUser->pcUserData->sInventoryItems, sizeof( DropItemData ) * _countof( pcUser->pcUserData->sInventoryItems ) );

	for ( UINT i = 0; i < sPacket.iItemCount; i++ )
	{
		if ( i >= (INVENTORYSERVER_MAX * 2) )
			break;

		ITEMSERVER->DecodeItemsDataPotion( pcUser->pcUserData, (BYTE *)pbaItemDataRecord, (BYTE *)&sRecordItem, 44, &sPacket );

		if ( sRecordItem.sItem.sItemID.ToItemID() && sRecordItem.sItem.iChk1 &&
			sRecordItem.sItem.iChk2 &&
			(sRecordItem.sItem.sItemID.ToItemBase() != ITEMBASE_Potion) )
		{
			bool bFound = false;
			for ( int j = 0; j < iInventoryCount; j++ )
			{
				if ( pcUser->pcUserData->sInventoryItems[j].iItemID == sRecordItem.sItem.sItemID.ToItemID() &&
					pcUser->pcUserData->sInventoryItems[j].iChk1 == sRecordItem.sItem.iChk1 &&
					pcUser->pcUserData->sInventoryItems[j].iChk2 == sRecordItem.sItem.iChk2 )
				{
					bFound = true;
					break;
				}
			}

			if ( bFound == false )
			{
				pcUser->pcUserData->sInventoryItems[iInventoryCount].iItemID = sRecordItem.sItem.sItemID.ToItemID();
				pcUser->pcUserData->sInventoryItems[iInventoryCount].iChk1 = sRecordItem.sItem.iChk1;
				pcUser->pcUserData->sInventoryItems[iInventoryCount].iChk2 = sRecordItem.sItem.iChk2;

				iInventoryCount++;
			}
			else
			{
				PacketLogCheatEx sPacketLogCheatEx;

				sPacketLogCheatEx.iCheatID = CHEATLOGID_ServerInventoryCopiedItem;
				sPacketLogCheatEx.WxParam = 1;
				sPacketLogCheatEx.LParam = sRecordItem.sItem.sItemID.ToItemID();
				sPacketLogCheatEx.SParam = sRecordItem.sItem.iChk1;
				sPacketLogCheatEx.EParam = sRecordItem.sItem.iChk2;
				LOGSERVER->OnLogCheat( pcUser->pcUserData, &sPacketLogCheatEx );
				break;
			}
		}

		int iSize = ((int *)pbaItemDataRecord)[0];
		iBufferSize += iSize;
		pbaItemDataRecord += iSize;
		if ( iBufferSize >= (sizeof( RecordItem ) * MAX_RECORD_ITEMS) )
			break;
	}

	return 0;
}

BOOL CCharacterDataHandler::LoadCharacterDataFile( std::string strFile, PacketCharacterRecordData & sPacket )
{
	BOOL bRet = FALSE;

	FILE * fp = NULL;
	fopen_s( &fp, strFile.c_str(), "rb" );
	if ( fp )
	{
		bRet = TRUE;

		fread( &sPacket, sizeof( PacketCharacterRecordData ), 1, fp );
		fclose( fp );
	}

	if ( bRet )
	{
		if ( (sPacket.iLength == 0) && (sPacket.iHeader == 0) )
			bRet = FALSE;
		else if ( (sPacket.iLength <= 0) || (sPacket.iLength > sizeof( PacketCharacterRecordData )) )
			bRet = FALSE;
	}

	return bRet;
}

BOOL CCharacterDataHandler::LoadCharacterDataSQL( std::string strCharacterName, PacketCharacterRecordData & sPacket )
{
	int iAccountID = -1;
	int iCharacterID = -1;
	SQLCharacter sSQLCharacter;
	if ( CHARACTERSERVER->SQLGetCharacter( strCharacterName.c_str(), &sSQLCharacter ) )
	{
		iCharacterID = sSQLCharacter.iID;
		iAccountID = UserServer::SQLGetAccountID( sSQLCharacter.szAccountName );
	}

	if ( iCharacterID == -1 )
		return FALSE;

	if ( GetSQLCharacterDataHandler()->LoadCharacterData( iCharacterID, sPacket.sCharacterData ) &&
		GetSQLCharacterSaveHandler()->LoadCharacterSave( iCharacterID, sPacket.sCharacterSaveData ) )
	{
		sPacket.iItemCount = 0;
		sPacket.iDataSize = 0;

		std::vector<SQLRecordItem> vSQLRecordItems;
		if ( GetSQLCharacterItemHandler()->LoadCharacterInventoryItems( iAccountID, iCharacterID, vSQLRecordItems, sPacket.iItemSubStart ) )
		{
			for ( auto & sSQLRecordItem : vSQLRecordItems )
			{
				//if ( ITEMSERVER->CheckValidItem( &sSQLRecordItem.sRecordItem.sItem ) == FALSE )
				//{
				//	if ( sSQLRecordItem.sRecordItem.sItem.sItemID.ToInt() == 33640714 )
				//	{
				//		std::string strFileItem = FormatString( "0x%08X.dat", sSQLRecordItem.sRecordItem.sItem.sItemID.ToItemID() );
				//		SaveBinaryData( strFileItem, (char*)&sSQLRecordItem.sRecordItem.sItem, sizeof(Item));
				//	}

				//	WRITEERR( "Invalid Item [%s][0x%08X]", sSQLRecordItem.sRecordItem.sItem.szItemName, sSQLRecordItem.sRecordItem.sItem.sItemID.ToItemID() );
				//}

				UINT iSize = ITEMSERVER->EncodeItemsData( &sSQLRecordItem.sRecordItem, &sPacket.baData[sPacket.iDataSize], sizeof( RecordItem ) );

				sPacket.iItemCount++;

				sPacket.iDataSize += iSize;

				if ( sPacket.iDataSize >= sizeof( sPacket.baData ) )
					break;
			}

			std::vector<DropItemData> vDropItems;

			if ( GetSQLCharacterDropItemSaveHandler()->LoadCharacterDropItem( iCharacterID, vDropItems ) )
			{
				UINT uMaxDropItems = vDropItems.size();
				if ( uMaxDropItems > _countof( sPacket.saDropItemData ) )
					uMaxDropItems = _countof( sPacket.saDropItemData );

				sPacket.iDropItemCount = uMaxDropItems;
				CopyMemory( sPacket.saDropItemData, vDropItems.data(), sizeof( DropItemData ) * uMaxDropItems );
			}
		}

		sPacket.iLength = sizeof( PacketCharacterRecordData ) - sizeof( sPacket.baData ) + sPacket.iDataSize;
		sPacket.iHeader = PKTHDR_SaveData;

		STRINGCOPY( sPacket.szHeader, "RC 1.70" );

		return TRUE;
	}

	return FALSE;
}

BOOL CCharacterDataHandler::LoadCharacterData( std::string strCharacterName, PacketCharacterRecordData & sPacket, bool bUseSQL )
{
	BOOL bRet = FALSE;

	if ( bUseSQL )
		bRet = LoadCharacterDataSQL( strCharacterName, sPacket );
	else
		bRet = LoadCharacterDataFile( "Data\\Character\\" + strCharacterName + ".chr", sPacket );

	return bRet;
}

void CCharacterDataHandler::HandlePacket( User * pcUser, PacketSelectCharacter * psPacket )
{
	const auto start_time = std::chrono::system_clock::now();

	if ( pcUser->pcUserData->szAccountName[0] == 0 )
		return;

	if ( (pcUser->GetGameLevel() == GAMELEVEL_None) && pcUser->pcUserData->bBlock )
		return;

	psPacket->szName[_countof( psPacket->szName ) - 1] = 0;

	BOOL bDisconnect = FALSE;

	const char * pszIP = pcUser->pcUserData->pcSocketData ? pcUser->pcUserData->pcSocketData->szIP : "";

	for ( UINT i = 0; i < USERSERVER->uUsersInGame; i++ )
	{
		User * pc = USERSERVER->pcaUserInGame[i];
		if ( pc != pcUser )
		{
			if ( pcUser->pcUserData->dwHashAccountName == pc->pcUserData->dwHashAccountName )
			{
				if ( STRINGCOMPAREI( pcUser->pcUserData->szAccountName, pc->pcUserData->szAccountName ) )
					bDisconnect = TRUE;
			}
			else if ( STRINGCOMPAREI( psPacket->szName, pc->pcUserData->szCharacterName ) )
				bDisconnect = TRUE;

			if ( bDisconnect )
			{
				WNDPROC_SOCKETCLOSE( SOCKETSERVER->GetHWND(), pcUser->pcUserData->pcSocketData );
				WNDPROC_SOCKETCLOSE( SOCKETSERVER->GetHWND(), pc->pcUserData->pcSocketData );
				break;
			}
		}
	}

	if ( bDisconnect == FALSE )
	{
		CCharacterSaveData * pcSaveData = new CCharacterSaveData();
		if ( LoadCharacterData( psPacket->szName, pcSaveData->sPacket ) )
		{
			CharacterData & sCharacterData = pcSaveData->sPacket.sCharacterData;
			CharacterSave & sCharacterSaveData = pcSaveData->sPacket.sCharacterSaveData;

			pcUser->pcUserData->dwCharacterUpdateTime = TICKCOUNT;

			//Set EXP Verification
			LARGE_INTEGER liEXP;
			liEXP.LowPart = sCharacterData.iCurrentExpLow;
			liEXP.HighPart = sCharacterData.iCurrentExpHigh;
			if ( liEXP.QuadPart < 0 )
				liEXP.QuadPart = 0;
			sCharacterData.iCurrentExpLow = liEXP.LowPart;
			sCharacterData.iCurrentExpHigh = liEXP.HighPart;

			SQLCharacter sSQLCharacter;
			if ( CHARACTERSERVER->SQLGetCharacter( sCharacterData.szName, &sSQLCharacter ) )
			{
				if ( sSQLCharacter.iEXP > liEXP.QuadPart )
					liEXP.QuadPart = sSQLCharacter.iEXP;

				sCharacterData.iCurrentExpLow = liEXP.LowPart;
				sCharacterData.iCurrentExpHigh = liEXP.HighPart;
			}

			//Level
			sCharacterData.iLevel = CHARACTERSERVER->GetLevelFromEXP( liEXP.QuadPart );
			sCharacterData.iOwnerID = (int)CHARACTERSERVER->GetExpFromLevel( sCharacterData.iLevel + 1 );

			ValidateStatusPoints( pcUser, sCharacterData, sCharacterSaveData.iQuestFlag );

			sCharacterData.iClanID = 0;

			if ( (sCharacterData.iLevel < 20) && (sCharacterData.iRank != CHARACTERRANK_Rank1) )
				sCharacterData.iRank = CHARACTERRANK_Rank1;

			sCharacterSaveData.sPotionUpdate[1] = 1;

			sCharacterSaveData.iBlessCastleTax = BLESSCASTLESERVER->psBlessCastleStatusData->iTax;
			sCharacterSaveData.iBlessCastleClanID = BLESSCASTLESERVER->psBlessCastleStatusData->iClanID;

			//Map Check
			if ( sCharacterSaveData.iMapID == MAPID_Bellatra )
				sCharacterSaveData.iMapID = MAPID_NaviskoTown;
			else if ( sCharacterSaveData.iMapID == MAPID_QuestArena )
			{
				ECharacterRace iRace = GetCharacterRace( sCharacterData.iClass );;
				if ( iRace == CHARACTERRACE_Tempskron )
					sCharacterSaveData.iMapID = MAPID_RicartenTown;
				else if ( iRace == CHARACTERRACE_Morion )
					sCharacterSaveData.iMapID = MAPID_PhillaiTown;
			}
			else if ( sCharacterSaveData.iMapID == MAPID_T5QuestArena )
				sCharacterSaveData.iMapID = MAPID_Atlantis;
			else if ( sCharacterSaveData.iMapID == MAPID_GhostCastle )
				sCharacterSaveData.iMapID = MAPID_Atlantis;
			else if ( sCharacterSaveData.iMapID == MAPID_ForestDungeon )
				sCharacterSaveData.iMapID = MAPID_Atlantis;
			else if ( sCharacterSaveData.iMapID == MAPID_DragonsDungeon )
				sCharacterSaveData.iMapID = MAPID_RicartenTown;

			if ( pcUser->GetID() == 0 )
				sCharacterData.iID = USERSERVER->GetNewUserID();
			else
				sCharacterData.iID = pcUser->GetID();

			CHARACTERSERVER->Validate( &sCharacterData );

			if ( (sCharacterSaveData.dwHeader < 140) && (STRINGCOMPAREI( pcSaveData->sPacket.szHeader, "RC 1.50" ) == FALSE) )
				sCharacterSaveData.dwChecksum = CHARACTERSERVER->GetHashCharacterDataOld( &sCharacterData );

			sCharacterData.bResetStatistics = 0;

			CCharacterDataHandler::SetLoginKey( &sCharacterData );

			ProcessPacket( pcUser, &pcSaveData->sPacket );

			STRINGCOPY( pcUser->pcUserData->szCharacterName, psPacket->szName );

			pcUser->pcUserData->dwHashCharacterName = CHARACTERSERVER->GetHashString( pcUser->pcUserData->szCharacterName );

			if ( pcSaveData->sPacket.iDropItemCount > 0 )
				if ( pcSaveData->sPacket.iDropItemCount > _countof( pcSaveData->sPacket.saDropItemData ) )
					pcSaveData->sPacket.iDropItemCount = _countof( pcSaveData->sPacket.saDropItemData );

			pcUser->pcUserData->iSaveItems = pcSaveData->sPacket.iDropItemCount;
			CopyMemory( pcUser->pcUserData->sDropItems, pcSaveData->sPacket.saDropItemData, sizeof( DropItemData ) * pcSaveData->sPacket.iDropItemCount );

			if ( sCharacterSaveData.iLastGold > 0 )
				pcUser->pcUserData->iSaveGold = sCharacterSaveData.iLastGold - 1;

			if ( pcUser->pcUserData->sCharacterData.szName[0] != 0 )
				CopyMemory( &pcUser->pcUserData->sCharacterData, &sCharacterData, sizeof( CharacterData ) );

			CopyMemory( pcUser->pcUserData->baSkillPoint, sCharacterSaveData.sSkillInfo.baSkillPoints, sizeof( BYTE ) * _countof( pcUser->pcUserData->baSkillPoint ) );

			pcUser->pcUserData->sCharacterData.iLevel = sCharacterData.iLevel;
			pcUser->pcUserData->iLevelStart = sCharacterData.iLevel;

			pcUser->pcUserData->sCharacterData.iCurrentExpLow = sCharacterData.iCurrentExpLow;
			pcUser->pcUserData->sCharacterData.iCurrentExpHigh = sCharacterData.iCurrentExpHigh;

			STRINGCOPY( pcUser->pcUserData->sCharacterData.szName, sCharacterData.szName );

			pcUser->pcUserData->sCharacterData.iClass = sCharacterData.iClass;

			pcUser->pcUserData->sCharacterData.iGold = sCharacterData.iGold;

			liEXP.LowPart = sCharacterData.iCurrentExpLow;
			liEXP.HighPart = sCharacterData.iCurrentExpHigh;
			pcUser->pcUserData->liEXP.QuadPart = liEXP.QuadPart;

			if ( sCharacterSaveData.iLastGold > 0 )
				pcUser->pcUserData->iGoldStart = sCharacterSaveData.iLastGold;
			else
				pcUser->pcUserData->iGoldStart = sCharacterData.iGold;

			USERSERVER->SetServerUserGold( pcUser->pcUserData, pcUser->pcUserData->iGoldStart, WHEREID_LoadPlayer );

			pcUser->pcUserData->sCharacterData.iClanID = sCharacterData.iClanID;

			AccountServer::SQLCharacterLog( pcUser->pcUserData->szAccountName, pcUser->pcUserData->sCharacterData.iClass,
				pcUser->pcUserData->szCharacterName, pcUser->pcUserData->szLoginCode, pszIP, ((char *)0x006E2660),
				pcUser->GetCharacterLevel(), CHARACTERSERVER->GetExpFromCharacterData( pcUser->pcUserData->sCharacterData ), pcUser->pcUserData->sCharacterData.iGold, 0 );

			if ( sCharacterSaveData.szAccountName[0] != 0 )
			{
				if ( STRINGCOMPAREI( sCharacterSaveData.szAccountName, pcUser->pcUserData->szAccountName ) == FALSE )
				{
					PacketLogCheat s;
					s.iCheatID = CHEATLOGID_CharacterAccountMismatch;
					s.LParam = (DWORD)sCharacterSaveData.szAccountName;
					s.SParam = (DWORD)pcUser->pcUserData->szAccountName;
					LOGSERVER->OnLogCheat( pcUser->pcUserData, &s );
				}
			}

			if ( pcUser->pcUserData->psCharacterFile )
			{
				LoadCharacterInventoryNew( pcUser, pcSaveData->sPacket );

				//???????? Where this size come from?
				if ( pcSaveData->sPacket.iLength < 24576 )
					CopyMemory( pcUser->pcUserData->psCharacterFile, &pcSaveData->sPacket, pcSaveData->sPacket.iLength );

				ITEMSERVER->ComparePotions( pcUser->pcUserData, &pcSaveData->sPacket.sCharacterSaveData );

				ITEMSERVER->LoadForceOrb( pcUser->pcUserData, &pcSaveData->sPacket.sCharacterSaveData );
			}

			pcUser->pcUserData->sCharacterData.iRank = sCharacterData.iRank;

			if ( pcUser->pcUserData->iID == 0 )
				pcUser->pcUserData->iID = sCharacterData.iID;
		}
		else
		{
			psPacket->iHeader = PKTHDR_SaveDataFail;
			if ( pcUser->GetID() == 0 )
				pcUser->pcUserData->iID = USERSERVER->GetNewUserID();

			pcUser->pcUserData->sCharacterData.iID = pcUser->GetID();
			psPacket->dwReserved2 = pcUser->GetID();
			psPacket->dwReserved1 = GetLoginKey( &pcUser->pcUserData->sCharacterData, CHARACTERSERVER->GetHashString( pcUser->pcUserData->szCharacterName ) );

			SENDPACKET( pcUser, psPacket );

			pcUser->pcUserData->psCharacterFile->iLength = 0;
			pcUser->pcUserData->psCharacterFile->iHeader = 0;

			pcUser->pcUserData->iLevelStart = 1;

			ZeroMemory( &pcUser->pcUserData->baSkillPoint, sizeof( BYTE ) * _countof( pcUser->pcUserData->baSkillPoint ) );

			pcUser->pcUserData->sCharacterData.iRank = CHARACTERRANK_Rank1;

			SENDPACKETBLANK( pcUser, PKTHDR_Save );
		}
		
		if ( pcUser->pcUserData->psCharacterFile )
		{
			if ( ((PacketCharacterRecordData *)pcUser->pcUserData->psCharacterFile)->iLength > 0 )
			{
				if ( STRINGCOMPAREI( pcUser->pcUserData->szCharacterName, ((PacketCharacterRecordData *)pcUser->pcUserData->psCharacterFile)->sCharacterData.szName ) == FALSE )
				{
					PacketLogCheat sCheatLog;
					sCheatLog.iCheatID = CHEATLOGID_InitialSaveMemoryError;
					STRINGCOPY( sCheatLog.szBuffer1, pcUser->pcUserData->szCharacterName );
					STRINGCOPY( sCheatLog.szBuffer2, ((PacketCharacterRecordData *)pcUser->pcUserData->psCharacterFile)->sCharacterData.szName );
					LOGSERVER->OnLogCheat( pcUser->pcUserData, &sCheatLog );
				}
			}
		}

		AccountServer::SQLUserOnline( pcUser->pcUserData->szAccountName, CHARACTERSERVER->GetCharacterName( pcUser->pcUserData ), pszIP,
			pcUser->pcUserData->iTicket, FALSE, pcUser->pcUserData->sCharacterData.iClass, pcUser->pcUserData->sCharacterData.iLevel );

		DAMAGEHANDLER->SendDamageCodeToClient( pcUser->pcUserData );

		USERSERVER->OnLoadUser( pcUser );

		const auto end_time = std::chrono::system_clock::now();

		WRITEERR( "Time to load character : %I64d ms", chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() );


		DELET( pcSaveData );
	}
}

void CCharacterDataHandler::HandlePacket( User * pcUser, PacketCharacterRecordDataSplit * psPacket )
{
	UserData * pcUserData = pcUser->pcUserData;

	if ( pcUserData->dwDisconnectTime != 0 )
		return;

	if ( pcUserData->szAccountName[0] == 0 )
		return;

	if ( pcUserData->pcCharacterSaveData == nullptr )
	{
		pcUserData->pcCharacterSaveData = new CCharacterSaveData();

		pcUserData->pcCharacterSaveData->iPacketCount = 0;
	}

	PacketLogCheat sCheatLog;
	PacketSaveSuccess sPacketSaveSuccess;

	EResultSaveData eResult = ReceiveCharacterSaveData( pcUser, psPacket );
	switch ( eResult )
	{
		case EResultSaveData::RESULTSAVEDATA_InvalidatedCharacter:
			sCheatLog.iCheatID = CHEATLOGID_CharInfoSaveError;
			sCheatLog.SParam = 0;
			sCheatLog.LParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::REUSLTSAVEDATA_LevelGreaterThanServer:
			sCheatLog.iCheatID = CHEATLOGID_EditLevelError;
			sCheatLog.SParam = pcUserData->sCharacterData.iLevel;
			sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iLevel;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidRecordItem:
			sCheatLog.iCheatID = CHEATLOGID_SavedItemError;
			sCheatLog.SParam = pcUserData->pcCharacterSaveData->sPacket.iItemCount;
			sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.iDataSize;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_NegativeEXP:
			sCheatLog.iCheatID = 6600;
			sCheatLog.SParam = 0;
			sCheatLog.LParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidatedStatusPoint:
			sCheatLog.iCheatID = CHEATLOGID_CharacterStateError1;
			sCheatLog.SParam = 0;
			sCheatLog.LParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_EXPHack:
			sCheatLog.iCheatID = 6620;
			sCheatLog.SParam = 0;
			sCheatLog.LParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidatedSkillPoint:
			sCheatLog.iCheatID = CHEATLOGID_CharacterSkillPointError;
			sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iLevel;
			sCheatLog.SParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidatedWeight:
			sCheatLog.iCheatID = CHEATLOGID_CharacterWeightError;
			sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.sWeight.sCur;
			sCheatLog.SParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.sWeight.sMax;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidatedClass:
			sCheatLog.iCheatID = CHEATLOGID_JobCodeError;
			sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iLevel;
			sCheatLog.SParam = pcUserData->sCharacterData.iClass;
			sCheatLog.EParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iClass;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			break;

		case EResultSaveData::RESULTSAVEDATA_InvalidCharacterNameSave:
			sCheatLog.iCheatID = CHEATLOGID_WrongSavingCharacterName;
			sCheatLog.LParam = 0;
			sCheatLog.SParam = 0;
			sCheatLog.EParam = 0;
			LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );

			pcUserData->dwDisconnectTime = 1;
			break;

		case EResultSaveData::RESULTSAVEDATA_Success:
			pcUserData->iTradePotion = 0;

			pcUserData->sCharacterData.iLevel = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iLevel;
			pcUserData->sCharacterData.iCurrentExpLow = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iCurrentExpLow;
			pcUserData->sCharacterData.iCurrentExpHigh = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iCurrentExpHigh;
			pcUserData->sCharacterData.iGold = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iGold;

			if ( (pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iDefenseRating >= 12000) ||
				(pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iAbsorbRating >= 1000) ||
				(pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iMaxDamage >= 4000) )
			{
				sCheatLog.iCheatID = CHEATLOGID_HackWarning;
				sCheatLog.LParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iDefenseRating;
				sCheatLog.SParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iAbsorbRating;
				sCheatLog.EParam = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iMaxDamage;

				sCheatLog.LxParam = 12000;
				sCheatLog.SxParam = 1000;
				sCheatLog.ExParam = 4000;

				LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			}

			if ( pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iClanID )
				pcUserData->sCharacterData.iClanID = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iClanID;

			if ( pcUserData->sCharacterData.iRank != pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iRank )
			{
				pcUserData->sCharacterData.iRank = pcUserData->pcCharacterSaveData->sPacket.sCharacterData.iRank;
			}

			if ( pcUserData->psCharacterFile && (pcUserData->psCharacterFile->iLength > 0) )
			{
				if ( STRINGCOMPAREI( pcUserData->szCharacterName, pcUserData->psCharacterFile->sCharacterData.szName ) == FALSE )
				{
					sCheatLog.iCheatID = CHEATLOGID_MemorySaveNameError5;
					STRINGCOPY( sCheatLog.szBuffer1, pcUserData->szCharacterName );
					STRINGCOPY( sCheatLog.szBuffer2, pcUserData->psCharacterFile->sCharacterData.szName );
					LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
				}
			}

			DELET( pcUserData->pcCharacterSaveData );

			pcUserData->iSaveItems = 0;
			pcUserData->iSaveGold = -1;

			sPacketSaveSuccess.iLength = sizeof( sPacketSaveSuccess );
			sPacketSaveSuccess.iHeader = PKTHDR_SaveSuccess;
			sPacketSaveSuccess.iUnknown = 0;
			sPacketSaveSuccess.bSaved = TRUE;
			sPacketSaveSuccess.iBlessCastleTax = BLESSCASTLESERVER->psBlessCastleStatusData->iTax;
			sPacketSaveSuccess.iBlessCastleClanID = BLESSCASTLESERVER->psBlessCastleStatusData->iClanID;
			SENDPACKET( pcUser, &sPacketSaveSuccess );

			if ( (pcUserData->dwDisconnectTime > 0) && (pcUserData->dwDisconnectTime < TICKCOUNT) && (pcUser->GetGameLevel() < GAMELEVEL_Two) )
			{
				WNDPROC_SOCKETCLOSE( SOCKETSERVER->GetHWND(), pcUserData->pcSocketData );
			}

			SendBlessCastleSkill( pcUserData );

			break;

		default:
			if ( (int)eResult < 0 )
			{
				sPacketSaveSuccess.iLength = sizeof( sPacketSaveSuccess );
				sPacketSaveSuccess.iHeader = PKTHDR_SaveSuccess;
				sPacketSaveSuccess.iUnknown = 0;
				sPacketSaveSuccess.bSaved = FALSE;
				sPacketSaveSuccess.iBlessCastleTax = BLESSCASTLESERVER->psBlessCastleStatusData->iTax;
				sPacketSaveSuccess.iBlessCastleClanID = BLESSCASTLESERVER->psBlessCastleStatusData->iClanID;
				SENDPACKET( pcUser, &sPacketSaveSuccess );

				sCheatLog.iCheatID = CHEATLOGID_SavingUnkownReturn;
				sCheatLog.LParam = eResult;
				sCheatLog.SParam = 0;
				sCheatLog.EParam = 0;
				LOGSERVER->OnLogCheat( pcUserData, &sCheatLog );
			}
			break;
	}
}

CCharacterDataHandler::EResultSaveData CCharacterDataHandler::ReceiveCharacterSaveData( User * pcUser, PacketCharacterRecordDataSplit * psPacket )
{
	UserData * pcUserData = pcUser->pcUserData;

	CCharacterSaveData * pcCharacterSaveData = pcUser->pcUserData->pcCharacterSaveData;

	BYTE * pbData = (BYTE *)&pcCharacterSaveData->sPacket;

	PacketCharacterRecordData & sPacketRecordData = pcCharacterSaveData->sPacket;

	if ( (psPacket->iLength > sizeof( PacketCharacterRecordDataSplit )) || (psPacket->iSize > _countof( psPacket->szData )) )
	{
		return EResultSaveData::RESULTSAVEDATA_WrongPacketCount;
	}
	UINT uCurrentBufferPosition = psPacket->iCurrentPacket * _countof( psPacket->szData );
	UINT uTotalSize = uCurrentBufferPosition + psPacket->iSize;
	if ( uTotalSize > sizeof( pcCharacterSaveData->sPacket ) )
	{
		return EResultSaveData::RESULTSAVEDATA_WrongPacketCount2;
	}

	CopyMemory( &pbData[uCurrentBufferPosition], psPacket->szData, psPacket->iSize );

	if ( psPacket->iCurrentPacket > 0 )
	{
		if ( pcCharacterSaveData->iPacketCount != (psPacket->iCurrentPacket - 1) )
		{
			return EResultSaveData::RESULTSAVEDATA_WrongPacketCount3;
		}
	}
	else
	{
		if ( pcCharacterSaveData->iPacketCount != psPacket->iCurrentPacket )
		{
			return EResultSaveData::RESULTSAVEDATA_WrongPacketCount4;
		}
	}

	if ( (psPacket->iCurrentPacket >= (psPacket->iMaxPackets-1)) && (sPacketRecordData.iHeader == PKTHDR_SaveData) )
	{
		if ( sPacketRecordData.iLength == 0 )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_WrongPacketCount5;
		}

		sPacketRecordData.sCharacterSaveData.iSaveTime = READDWORD( 0x07AC9D6C );

		if ( CHARACTERSERVER->IsValidCharacterData( &sPacketRecordData.sCharacterData ) == FALSE )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidatedCharacter;
		}

		if ( (pcUser->GetGameLevel() < GAMELEVEL_Four) && (STRINGCOMPARE( sPacketRecordData.sCharacterData.szName, pcUserData->szCharacterName ) == FALSE) )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidCharacterNameSave;
		}

		if ( sPacketRecordData.sCharacterData.iLevel < pcUserData->sCharacterData.iLevel )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::REUSLTSAVEDATA_LevelGreaterThanServer;
		}

		if ( IsValidRecordItemData( sPacketRecordData ) == FALSE )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidRecordItem;
		}

		INT64 iEXP = CHARACTERSERVER->GetExpFromCharacterData( sPacketRecordData.sCharacterData );
		if ( iEXP < 0 )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_NegativeEXP;
		}

		INT64 iInitialEXP = iEXP - pcUserData->liEXP.QuadPart;

		int iInitialGold = sPacketRecordData.sCharacterData.iGold - pcUserData->iGoldStart;

		if ( ValidateStatusPoints( pcUser, sPacketRecordData.sCharacterData, sPacketRecordData.sCharacterSaveData.iQuestFlag ) == FALSE )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidatedStatusPoint;
		}

		if ( iEXP > pcUserData->liEXP.QuadPart )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_EXPHack;
		}

		CHARACTERSERVER->SetExpOnCharacterData( sPacketRecordData.sCharacterData, pcUserData->liEXP.QuadPart );

		if ( ValidateSkillPoints( sPacketRecordData.sCharacterData.iLevel, sPacketRecordData.sCharacterSaveData.sSkillInfo, sPacketRecordData.sCharacterSaveData.iQuestFlag ) == FALSE )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidatedSkillPoint;
		}

		CopyMemory( pcUserData->baSkillPoint, sPacketRecordData.sCharacterSaveData.sSkillInfo.baSkillPoints, sizeof( BYTE ) * _countof( pcUserData->baSkillPoint ) );

		if ( (sPacketRecordData.sCharacterData.sWeight.sCur < 0) || (sPacketRecordData.sCharacterData.sWeight.sCur > ( sPacketRecordData.sCharacterData.sWeight.sMax + 2000 )) )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidatedWeight;
		}

		if ( pcUserData->sCharacterData.iClass == ECharacterClass::CHARACTERCLASS_None )
			pcUserData->sCharacterData.iClass = sPacketRecordData.sCharacterData.iClass;

		if ( (pcUser->GetGameLevel() < GAMELEVEL_Four) && (pcUserData->sCharacterData.iClass != sPacketRecordData.sCharacterData.iClass) )
		{
			sPacketRecordData.iHeader = 0;
			return EResultSaveData::RESULTSAVEDATA_InvalidatedClass;
		}

		if ( (pcUser->GetGameLevel() < GAMELEVEL_Four) && (MAPLEVEL( sPacketRecordData.sCharacterSaveData.iMapID ) > sPacketRecordData.sCharacterData.iLevel) )
		{
			sPacketRecordData.sCharacterData.sHP.sCur = 0;
			pcUserData->dwDisconnectTime = TICKCOUNT + 5000;

			PacketLogCheat sPacketCheatLog;
			sPacketCheatLog.iCheatID = CHEATLOGID_RestrictedAreaTrespassed;
			sPacketCheatLog.SParam = pcUserData->sCharacterData.iLevel;
			sPacketCheatLog.LParam = sPacketRecordData.sCharacterSaveData.iMapID;
			LOGSERVER->OnLogCheat( pcUserData, &sPacketCheatLog );
		}

		if ( sPacketRecordData.sCharacterData.sHP.sCur == 0 )
		{
			ECharacterRace eRace = GetCharacterRace( sPacketRecordData.sCharacterData.iClass );

			if ( eRace == ECharacterRace::CHARACTERRACE_Tempskron )
			{
				sPacketRecordData.sCharacterSaveData.iMapID = MAPID_RicartenTown;
				sPacketRecordData.sCharacterSaveData.iLastPositionX = 746752;
				sPacketRecordData.sCharacterSaveData.iLastPositionZ = -4464384;
			}
			else
			{
				sPacketRecordData.sCharacterSaveData.iMapID = MAPID_PhillaiTown;
				sPacketRecordData.sCharacterSaveData.iLastPositionX = 505344;
				sPacketRecordData.sCharacterSaveData.iLastPositionZ = 18948864;
			}
		}

		STRINGCOPY( sPacketRecordData.sCharacterSaveData.szAccountName, pcUserData->szAccountName );

		SaveServerPotion( pcUser, sPacketRecordData );

		SaveForceOrb( pcUser, sPacketRecordData );

		ProcessSaveData( pcUser, sPacketRecordData );

		sPacketRecordData.iHeader = 0;

		return EResultSaveData::RESULTSAVEDATA_Success;
	}

	pcCharacterSaveData->iPacketCount = psPacket->iCurrentPacket;

	return EResultSaveData::RESULTSAVEDATA_Pending;
}

void CCharacterDataHandler::ProcessPacket( User * pcUser, PacketCharacterRecordData * psPacket )
{
	PacketCharacterRecordDataSplit * psPacketSplit = new PacketCharacterRecordDataSplit;
	psPacketSplit->iHeader			= PKTHDR_SaveData;
	psPacketSplit->iCurrentPacket	= 0;
	psPacketSplit->iSize			= 0;

	psPacketSplit->iMaxPackets = psPacket->iLength / MAX_PKTSIZE_SAVEDATA;
	if ( psPacket->iLength % MAX_PKTSIZE_SAVEDATA )
		if ( psPacket->iLength > MAX_PKTSIZE_SAVEDATA )
			psPacketSplit->iMaxPackets++;

	int iCurrentLength = 0;
	while ( iCurrentLength < psPacket->iLength )
	{
		psPacketSplit->iSize = psPacket->iLength - iCurrentLength;
		if ( psPacketSplit->iSize > MAX_PKTSIZE_SAVEDATA )
			psPacketSplit->iSize = MAX_PKTSIZE_SAVEDATA;

		psPacketSplit->iLength = psPacketSplit->iSize + sizeof( Packet ) + 12;

		CopyMemory( &psPacketSplit->szData, (BYTE*)psPacket + iCurrentLength, psPacketSplit->iSize );

		SENDPACKET( pcUser, psPacketSplit );

		psPacketSplit->iCurrentPacket++;

		iCurrentLength += psPacketSplit->iSize;
	}

	DELET( psPacketSplit );
}

NAKED BOOL CCharacterDataHandler::SetLoginKey( CharacterData * psCharacterData )
{
	JMP( 0x0044E990 );
}

DWORD CCharacterDataHandler::GetLoginKey( CharacterData * psCharacterData, DWORD dwHashName )
{
	return CALL_WITH_ARG2( 0x0044E950, (DWORD)psCharacterData, dwHashName );
}

std::string CCharacterDataHandler::GetCharacterDataSaveFolderFile( const std::string strCharacterName, ECharacterDataSaveFolder eFolder )
{
	std::string strFolder = GetCharacterDataSaveFolder( eFolder );

	return strFolder + strCharacterName + ".chr";
}

std::string CCharacterDataHandler::GetCharacterDataSaveFolder( ECharacterDataSaveFolder eFolder )
{
	std::string strFolder = "Data\\Character\\";

	switch ( eFolder )
	{
		case ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_Backup:
			strFolder = "Data\\CharacterBackup\\";
			break;
		case ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_BattleRoyale:
			strFolder = "Data\\CharacterBroyale\\";
			break;
		case ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_Warehouse:
			strFolder = "Data\\Warehouse\\";
			break;
		default:
			break;
	}

	return strFolder;
}

void CCharacterDataHandler::DoImport()
{
	//Get All files inside the folder
	std::string strFolder = GetCharacterDataSaveFolder( ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_Normal );

	PacketCharacterRecordData * psPacket = new PacketCharacterRecordData;

	auto vFiles = SYSTEM->ListFilesInDirectory( strFolder, "chr" );

	UINT iCount = vFiles.size();

	UINT iCurrent = 1;

	UINT iTotalCharacterDataNotFounds = 0;
	UINT iTotalCharacterDataIDNotFounds = 0;
	UINT iTotalCharacterDataImported = 0;

	WRITEERR( "Importing Character Data Total: %d", iCount );

	for ( auto & strFile : vFiles )
	{
		ZeroMemory( psPacket, sizeof( PacketCharacterRecordData ) );

		std::string strCharacterName = strFile.substr( 0, strFile.length() - 4 );

		//Get all after the last Reverse Slash
		strCharacterName = strCharacterName.substr( strCharacterName.find_last_of( "\\" ) + 1 );

		//Load the file
		if ( LoadCharacterData( strCharacterName, *psPacket, false ) )
		{
			SQLCharacter sSQLCharacter;
			if ( CHARACTERSERVER->SQLGetCharacter( strCharacterName.c_str(), &sSQLCharacter ) )
			{
				//Save the file
				ProcessSQLSave( sSQLCharacter.iAccountID, sSQLCharacter.iID, *psPacket );

				WRITEERR( "%d/%d [%d%%] - Imported Character Data: %s", iCurrent, iCount, (iCurrent * 100) / iCount, strCharacterName.c_str() );

				iTotalCharacterDataImported++;
			}
			else
			{
				WRITEERR( "%d/%d [%d%%] - Character Data ID not found: %s", iCurrent, iCount, (iCurrent * 100) / iCount, strCharacterName.c_str() );

				iTotalCharacterDataIDNotFounds++;
			}
		}
		else
		{
			WRITEERR( "%d/%d [%d%%] - Character Data not found: %s", iCurrent, iCount, (iCurrent * 100) / iCount, strCharacterName.c_str() );

			iTotalCharacterDataNotFounds++;
		}

		iCurrent++;
	}

	DELET( psPacket );

	WRITEERR( "Importing Character Data Complete! Total: Amount[%d] Imported[%d] NotFoundID[%d] NotFoundData[%d]", iCount, iTotalCharacterDataImported, iTotalCharacterDataIDNotFounds, iTotalCharacterDataNotFounds );
}

BOOL CCharacterDataHandler::ValidateStatusPoints( User * pcUser, CharacterData & sCharacterData, UINT iQuestFlag )
{
	BOOL bRet = TRUE;

	int iBaseStatusCount = abs( sCharacterData.iStrength ) +
		abs( sCharacterData.iSpirit ) +
		abs( sCharacterData.iTalent ) +
		abs( sCharacterData.iAgility ) +
		abs( sCharacterData.iHealth ) +
		abs( sCharacterData.iStatPoints );

	int iAddStatusPoints = ((sCharacterData.iLevel - 1) * 5) + 99;
	iAddStatusPoints += GetStatusPointQuest( pcUser, sCharacterData.iLevel, iQuestFlag );

	sCharacterData.iStatPoints += (iAddStatusPoints - iBaseStatusCount);

	if ( sCharacterData.iStatPoints < 0 )
	{
		if ( sCharacterData.iStatPoints <= (-10) )
		{
			sCharacterData.iStrength = 1;
			sCharacterData.iSpirit = 8;
			sCharacterData.iTalent = 1;
			sCharacterData.iAgility = 1;
			sCharacterData.iHealth = 30;

			bRet = FALSE;
		}

		sCharacterData.iStatPoints = 0;
	}

	return bRet;
}

BOOL CCharacterDataHandler::ValidateSkillPoints( int iCharacterLevel, const RecordSkill & sRecordSkill, UINT iQuestFlag, int * piOut )
{
	int iNormalSkillPoints = 0;
	int iEliteSkillPoints = 0;

	for ( int i = 1; i < 13; i++ )
	{
		if ( sRecordSkill.baSkillPoints[i-1] > 10 )
			return FALSE;

		iNormalSkillPoints += abs( sRecordSkill.baSkillPoints[i-1] );
	}

	for ( int i = 13; i < 17; i++ )
	{
		//KPT WTF
		int iIndex = i & 31;

		if ( sRecordSkill.baSkillPoints[iIndex-1] > 10 )
			return FALSE;

		iEliteSkillPoints += abs( sRecordSkill.baSkillPoints[iIndex-1] );
	}

	if ( piOut )
	{
		*piOut = iNormalSkillPoints + (iEliteSkillPoints << 16);
	}

	if ( iCharacterLevel >= 10 )
	{
		int iSkillPoints = ((iCharacterLevel - 8) / 2) - iNormalSkillPoints;
		iSkillPoints += GetSkillPointQuest( iCharacterLevel, iQuestFlag );
		if ( iSkillPoints < 0 )
			return FALSE;
	}
	else
	{
		if ( iNormalSkillPoints > 0 )
			return FALSE;
	}

	if ( iCharacterLevel >= 60 )
	{
		int iExtraSkillPoints = ((iCharacterLevel - 58) / 2) - iEliteSkillPoints;
		if ( iExtraSkillPoints < 0 )
			return FALSE;
	}
	else
	{
		if ( iEliteSkillPoints > 0 )
			return FALSE;
	}

	return TRUE;
}

BOOL CCharacterDataHandler::ProcessSaveData( User * pcUser, PacketCharacterRecordData & sPacket, bool bUseSQL )
{
	if ( bUseSQL )
		return ProcessSaveDataToSQL( pcUser, sPacket );

	return ProcessSaveDataToFile( pcUser, sPacket );
}


BOOL CCharacterDataHandler::ProcessSaveDataToFile( User * pcUser, PacketCharacterRecordData & sPacket )
{
	UINT iLength = sPacket.iLength;
	if ( iLength < 0x4000 )
		iLength = 0x4000;

	if ( pcUser->pcUserData->psCharacterFile && (iLength <= 0x6000) )
	{
		CopyMemory( pcUser->pcUserData->psCharacterFile, &sPacket, iLength );
	}
	
	//else
	{

		std::string strFile = GetCharacterDataSaveFolderFile( pcUser->pcUserData->szCharacterName, ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_Normal );
		std::string strFileBackup = GetCharacterDataSaveFolderFile( pcUser->pcUserData->szCharacterName, ECharacterDataSaveFolder::CHARACTERDATASAVEFOLDER_Backup );

		CopyFile( strFile.c_str(), strFileBackup.c_str(), FALSE );

		SaveDropItems( pcUser->pcUserData, sPacket );

		InventoryServerCheckNew( pcUser, sPacket );

		FILE * pFile = nullptr;
		if ( fopen_s( &pFile, strFile.c_str(), "wb" ) == 0 )
		{
			fwrite( &sPacket, iLength, 1, pFile );
			fclose( pFile );
		}

		if ( pcUser->pcUserData->psCharacterFile )
		{
			pcUser->pcUserData->psCharacterFile->iLength = 0;
		}
	}

	return TRUE;
}

BOOL CCharacterDataHandler::ProcessSaveDataToSQL( User * pcUser, PacketCharacterRecordData & sPacket )
{
	int iCharacterID = -1;
	int iAccountID = -1;
	SQLCharacter sSQLCharacter;
	if ( CHARACTERSERVER->SQLGetCharacter( CHARACTERSERVER->GetCharacterName( pcUser ), &sSQLCharacter ) )
	{
		iCharacterID = sSQLCharacter.iID;

		iAccountID = sSQLCharacter.iAccountID;
	}

	SaveDropItems( pcUser->pcUserData, sPacket );

	InventoryServerCheckNew( pcUser, sPacket );

	return ProcessSQLSave( iAccountID, iCharacterID, sPacket );
}

BOOL CCharacterDataHandler::ProcessSQLSave( const int iAccountID, const int iCharacterID, PacketCharacterRecordData & sPacket )
{
	GetSQLCharacterDataHandler()->SaveCharacterData( iCharacterID, sPacket.sCharacterData );
	GetSQLCharacterSaveHandler()->SaveCharacterSave( iCharacterID, sPacket.sCharacterSaveData );

	BYTE * pbaItemRecordData = sPacket.baData;

	std::vector<RecordItem> vRecordItems;

	RecordItem sRecordItem;

	if ( sPacket.iItemCount > MAX_RECORD_ITEMS )
		sPacket.iItemCount = MAX_RECORD_ITEMS;

	for ( UINT i = 0; i < sPacket.iItemCount; i++ )
	{
		ITEMSERVER->DecodeItemsData( pbaItemRecordData, &sRecordItem, 0x7FFFF );

		//if ( sRecordItem.sItem.sItemID.ToInt() == 33640714 )
		//{
		//	std::string strFileItem = FormatString( "0x%08X.dat", sRecordItem.sItem.sItemID.ToItemID() );
		//	SaveBinaryData( strFileItem, (char*)&sRecordItem.sItem, sizeof(Item));
		//}


		vRecordItems.push_back( sRecordItem );

		pbaItemRecordData += ((int *)pbaItemRecordData)[0];
	}

	GetSQLCharacterItemHandler()->SaveCharacterInventoryItems( iAccountID, iCharacterID, vRecordItems, sPacket.iItemSubStart );

	std::vector<DropItemData> vDropItems;

	if ( sPacket.iDropItemCount > _countof( sPacket.saDropItemData ) )
		sPacket.iDropItemCount = _countof( sPacket.saDropItemData );

	vDropItems.resize( sPacket.iDropItemCount );

	CopyMemory( vDropItems.data(), sPacket.saDropItemData, sizeof( DropItemData ) * sPacket.iDropItemCount );

	GetSQLCharacterDropItemSaveHandler()->SaveCharacterDropItem( iCharacterID, vDropItems );

	return TRUE;
}

BOOL CCharacterDataHandler::IsValidRecordItemData( const PacketCharacterRecordData & sPacket )
{
	const BYTE * pbaItemRecordData = sPacket.baData;

	UINT uBufferPosition = 0;

	for ( UINT i = 0; i < sPacket.iItemCount; i++ )
	{
		UINT uSize = ((int *)pbaItemRecordData)[0];
		uBufferPosition += uSize;
		pbaItemRecordData += uSize;

		if ( uBufferPosition > (UINT)sPacket.iDataSize )
			return FALSE;
	}

	if ( uBufferPosition == (UINT)sPacket.iDataSize ) 
		return TRUE;

	return FALSE;
}

int CCharacterDataHandler::GetSkillPointQuest( int iLevel, UINT iQuestFlag )
{
	int iPoints = 0;

	//The Cave
	if ( (iQuestFlag & 2) && (iLevel >= 55) )
		iPoints++;

	//Help Michelle
	if ( (iQuestFlag & 4) && (iLevel >= 70) )
		iPoints++;

	//Closed Fury
	if ( (iQuestFlag & 8) && (iLevel >= 80) )
		iPoints += 2;

	return iPoints;
}

int CCharacterDataHandler::GetStatusPointQuest( User * pcUser, int iLevel, UINT iQuestFlag )
{
	int iPoints = 0;

	//For Her
	if ( (iQuestFlag & 1) && (iLevel >= 30) )
		iPoints += 5;

	//Help Michelle
	if ( (iQuestFlag & 4) && (iLevel >= 70) )
		iPoints += 5;

	//Closed Fury
	if ( (iQuestFlag & 8) && (iLevel >= 80) )
		iPoints += 5;

	//Kingdom Test
	if ( (iQuestFlag & 64) && (iLevel >= 80) )
		iPoints += (iLevel - 79) * 2;

	//Bitter Ordeal
	if ( (iQuestFlag & 128) && (iLevel >= 90) )
		iPoints += (iLevel - 89) * 3;

	//Custom Quests
	if ( (iQuestFlag & QUESTSAVEFLAG_TheLoyaltyTest) && (iLevel >= 120) )
		iPoints += (iLevel - 119) * 3;

	return iPoints;
}

BOOL CCharacterDataHandler::SaveForceOrb( User * pcUser, PacketCharacterRecordData & sPacket )
{
	int iTime = pcUser->pcUserData->dwForceOrb_SaveTime / 1000;
	if ( iTime > 0 )
	{
		sPacket.sCharacterSaveData.wForceOrbUsing[0] = (WORD)pcUser->pcUserData->dwForceOrb_SaveCode;
		sPacket.sCharacterSaveData.wForceOrbUsing[1] = iTime;
	}
	else
	{
		sPacket.sCharacterSaveData.wForceOrbUsing[0] = 0;
		sPacket.sCharacterSaveData.wForceOrbUsing[1] = 0;
	}

	return TRUE;
}

BOOL CCharacterDataHandler::SaveServerPotion( User * pcUser, PacketCharacterRecordData & sPacket )
{
	sPacket.sCharacterSaveData.sPotionUpdate[0] = 1;
	sPacket.sCharacterSaveData.sPotionUpdate[1] = 0;

	for ( int i = 0; i < MAX_POTION_SAVE_SLOTS; i++ )
	{
		for ( int j = 0; j < MAX_POTION_SAVE_COUNT; j++ )
		{
			sPacket.sCharacterSaveData.sPotionCount[i][j] = pcUser->pcUserData->iaServerPotion[i][j];
		}
	}

	return TRUE;
}

NAKED BOOL CCharacterDataHandler::SendBlessCastleSkill( UserData * pcUserData )
{
	JMP( 0x00557F10 );
}

bool CCharacterDataHandler::SaveBinaryData( const std::string & strFileName, const char * pbData, UINT iSize )
{
	FILE * pFile = nullptr;
	if ( fopen_s( &pFile, strFileName.c_str(), "wb" ) == 0 )
	{
		fwrite( pbData, iSize, 1, pFile );
		fclose( pFile );

		return true;
	}

	return false;
}

BOOL CCharacterDataHandler::InventoryServerCheck( User * pcUser, PacketCharacterRecordData & sPacket )
{
	/*
	//if ( pcUser->GetGameLevel() >= GAMELEVEL_Three )
	//	return FALSE;

	if ( pcUser->bInBattleRoyale )
		return FALSE;

	UserData * pcUserData = pcUser->pcUserData;

	PacketCharacterRecordData * psCharacter = &sPacket;

	RecordItem sRecordItem;

	DropItemData sDropData[64];

	ZeroMemory( sDropData, sizeof( DropItemData ) * 64 );

	BYTE * pbaItems = psCharacter->baData;

	int iBufferWheel = 0;

	int iCount = 0;

	int iDropItemCount = 0;

	for ( int i = 0; i < psCharacter->iItemCount; i++ )
	{
		//Inventory Limit...
		if ( i >= 200 )
			break;

		ITEMSERVER->DecodeItemsData( pbaItems, &sRecordItem, 44 );

		if ( sRecordItem.sItem.sItemID.ToInt() && sRecordItem.sItem.iChk1 && sRecordItem.sItem.iChk2 )
		{
			iCount = 0;

			if ( sRecordItem.sItem.sItemID.ToItemBase() == ITEMBASE_Potion )
			{
				if ( pcUserData->iTradePotion > 0 )
				{
					if ( ITEMSERVER->GetDropPotion( pcUserData, sRecordItem.sItem.sItemID.ToInt() ) )
						iCount = 1;
				}
				else
					iCount = (-1);
			}
			else
			{
				//Inventory 1 and 2, backup
				for ( int j = 0; j < 300; j++ )
				{
					DropItemData * ps = pcUserData->sInventoryItems + j;

					if ( ps->iItemID && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
					{
						ps->iItemID = ITEMID_None;

						iCount++;
					}
				}

				//Warehouse
				for ( int j = 0; j < WAREHOUSE_ITEM_MAX; j++ )
				{
					DropItemData * ps = pcUser->sWarehouseItems + j;

					if ( ps->iItemID && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
					{
						PacketLogCheat s;
						s.iCheatID = CHEATLOGID_ItemErrorInventoryRecord;
						s.WxParam = 1;
						s.LxParam = (int)"*RECORD ITEM WAREHOUSE";
						s.LParam = sRecordItem.sItem.sItemID.ToInt();
						s.SParam = sRecordItem.sItem.iChk1;
						s.EParam = sRecordItem.sItem.iChk2;

						LOGSERVER->OnLogCheat( pcUserData, &s );
						iCount++;

						ITEMSERVER->SaveItemData( &sRecordItem.sItem );
					}
				}
			}

			//Not Found...
			if ( iCount == 0 )
			{
				int iLoop = 0;

				for ( iLoop = 0; iLoop < psCharacter->iDropItemCount; iLoop++ )
				{
					DropItemData * ps = psCharacter->saDropItemData + iLoop;

					if ( (ps->iItemID == sRecordItem.sItem.sItemID.ToInt()) && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
						break;
				}


				if ( iDropItemCount < 64 )
				{
					int k = 0;

					for ( k = 0; k < iDropItemCount; k++ )
					{
						DropItemData * ps = sDropData + k;

						if ( (ps->iItemID == sRecordItem.sItem.sItemID.ToInt()) && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
							break;
					}

					if ( k >= iDropItemCount )
					{
						sDropData[iDropItemCount].iItemID = sRecordItem.sItem.sItemID.ToInt();
						sDropData[iDropItemCount].iChk1 = sRecordItem.sItem.iChk1;
						sDropData[iDropItemCount].iChk2 = sRecordItem.sItem.iChk2;
						iDropItemCount++;
					}
				}

				if ( iLoop >= psCharacter->iDropItemCount )
				{
					PacketLogCheat s;
					s.iCheatID = CHEATLOGID_ItemErrorInventoryRecord;
					s.WxParam = 1;
					s.LxParam = (int)"*RECORD ITEM";
					s.LParam = sRecordItem.sItem.sItemID.ToInt();
					s.SParam = sRecordItem.sItem.iChk1;
					s.EParam = sRecordItem.sItem.iChk2;

					LOGSERVER->OnLogCheat( pcUserData, &s );

					ITEMSERVER->SaveItemData( &sRecordItem.sItem );
				}
			}
			else
			{
				if ( iCount > 0 )
				{
					for ( int j = 0; j < psCharacter->iDropItemCount; j++ )
					{
						DropItemData * ps = psCharacter->saDropItemData + j;

						if ( (ps->iItemID == sRecordItem.sItem.sItemID.ToInt()) && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
						{
							if ( iDropItemCount < 64 )
							{
								sDropData[iDropItemCount].iItemID = sRecordItem.sItem.sItemID.ToInt();
								sDropData[iDropItemCount].iChk1 = sRecordItem.sItem.iChk1;
								sDropData[iDropItemCount].iChk2 = sRecordItem.sItem.iChk2;
								iDropItemCount++;
							}
							break;
						}
					}
				}
			}

			if ( iCount > 1 )
			{
				PacketLogCheat s;
				s.iCheatID = CHEATLOGID_ItemErrorInventoryRecord;
				s.WxParam = iCount;
				s.LxParam = (int)"*RECORD COPIED ITEM";
				s.LParam = sRecordItem.sItem.sItemID.ToInt();
				s.SParam = sRecordItem.sItem.iChk1;
				s.EParam = sRecordItem.sItem.iChk2;

				LOGSERVER->OnLogCheat( pcUserData, &s );

				ITEMSERVER->SaveItemData( &sRecordItem.sItem );
			}
		}

		int iSize = ((int *)pbaItems)[0];

		iBufferWheel += iSize;

		pbaItems += iSize;

		if ( iBufferWheel >= (sizeof( RecordItem ) * 200) )
			break;
	}

	if ( iDropItemCount > 0 )
	{
		CopyMemory( psCharacter->saDropItemData, sDropData, sizeof( DropItemData ) * iDropItemCount );
		psCharacter->iDropItemCount = iDropItemCount;
	}

	if ( psCharacter->sCharacterData.iGold > pcUserData->iInventoryGold )
	{
		PacketLogCheat s;
		s.iCheatID = CHEATLOGID_ServerMoneyErrorInventory;
		s.LParam = 1;
		s.SParam = pcUserData->iInventoryGold;
		s.EParam = psCharacter->sCharacterData.iGold;
		LOGSERVER->OnLogCheat( pcUserData, &s );

		psCharacter->sCharacterData.iGold = pcUserData->iInventoryGold;

		pcUserData->iSaveGold = (-1);

		if ( psCharacter->sCharacterData.iGold < 0 )
			psCharacter->sCharacterData.iGold = 0;
	}

	int iGold = psCharacter->sCharacterSaveData.iLastGold;

	if ( iGold && (iGold - 1) > pcUserData->iInventoryGold )
	{
		PacketLogCheat s;
		s.iCheatID = CHEATLOGID_ServerMoneyErrorInventory;
		s.LParam = 3;
		s.SParam = pcUserData->iInventoryGold;
		s.EParam = psCharacter->sCharacterSaveData.iLastGold - 1;
		LOGSERVER->OnLogCheat( pcUserData, &s );

		psCharacter->sCharacterSaveData.iLastGold = pcUserData->iInventoryGold + 1;

		if ( psCharacter->sCharacterSaveData.iLastGold < 0 )
			psCharacter->sCharacterSaveData.iLastGold = 0;
	}
	*/
	return TRUE;
}

BOOL CCharacterDataHandler::InventoryServerCheckNew( User * pcUser, PacketCharacterRecordData & sPacket )
{
	//if ( pcUser->GetGameLevel() >= GAMELEVEL_Three )
	//	return FALSE;

	if ( pcUser->bInBattleRoyale )
		return FALSE;

	UserData * pcUserData = pcUser->pcUserData;

	PacketCharacterRecordData * psCharacter = &sPacket;

	RecordItem sRecordItem;

	BYTE * pbaItems = psCharacter->baData;

	int iBufferWheel = 0;

	for ( UINT i = 0; i < psCharacter->iItemCount; i++ )
	{
		//Inventory Limit...
		if ( i >= 200 )
			break;

		ITEMSERVER->DecodeItemsData( pbaItems, &sRecordItem, 44 );

		if ( sRecordItem.sItem.sItemID.ToInt() && sRecordItem.sItem.iChk1 && sRecordItem.sItem.iChk2 )
		{
			if ( sRecordItem.sItem.sItemID.ToItemBase() == ITEMBASE_Potion )
			{
				//if ( pcUserData->iTradePotion > 0 )
				//{
				//	if ( ITEMSERVER->GetDropPotion( pcUserData, sRecordItem.sItem.sItemID.ToInt() ) )
				//		iCount = 1;
				//}
				//else
				//	iCount = (-1);
			}
			else
			{
				bool bFound = false;
				//Inventory 1 and 2, backup
				for ( int j = 0; j < 300; j++ )
				{
					DropItemData * ps = pcUserData->sInventoryItems + j;

					if ( ps->iItemID && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
					{
						bFound = true;
						break;
					}
				}

				if ( bFound == false )
				{
					//Warehouse
					for ( int j = 0; j < WAREHOUSE_ITEM_MAX; j++ )
					{
						DropItemData * ps = pcUser->sWarehouseItems + j;

						if ( ps->iItemID && (ps->iChk1 == sRecordItem.sItem.iChk1) && (ps->iChk2 == sRecordItem.sItem.iChk2) )
						{
							bFound = true;
							break;
						}
					}
				}

				if ( bFound == false )
				{
					PacketLogCheat s;
					s.iCheatID = CHEATLOGID_ItemErrorInventoryRecord;
					s.WxParam = 1;
					s.LxParam = (int)"*RECORD ITEM";
					s.LParam = sRecordItem.sItem.sItemID.ToInt();
					s.SParam = sRecordItem.sItem.iChk1;
					s.EParam = sRecordItem.sItem.iChk2;

					LOGSERVER->OnLogCheat( pcUserData, &s );

					ITEMSERVER->SaveItemData( &sRecordItem.sItem );
					break;
				}
			}
		}

		int iSize = ((int *)pbaItems)[0];

		iBufferWheel += iSize;

		pbaItems += iSize;

		if ( iBufferWheel >= (sizeof( RecordItem ) * 200) )
			break;
	}

	if ( psCharacter->sCharacterData.iGold > pcUserData->iInventoryGold )
	{
		PacketLogCheat s;
		s.iCheatID = CHEATLOGID_ServerMoneyErrorInventory;
		s.LParam = 1;
		s.SParam = pcUserData->iInventoryGold;
		s.EParam = psCharacter->sCharacterData.iGold;
		LOGSERVER->OnLogCheat( pcUserData, &s );

		psCharacter->sCharacterData.iGold = pcUserData->iInventoryGold;

		pcUserData->iSaveGold = (-1);

		if ( psCharacter->sCharacterData.iGold < 0 )
			psCharacter->sCharacterData.iGold = 0;
	}

	int iGold = psCharacter->sCharacterSaveData.iLastGold;

	if ( iGold && (iGold - 1) > pcUserData->iInventoryGold )
	{
		PacketLogCheat s;
		s.iCheatID = CHEATLOGID_ServerMoneyErrorInventory;
		s.LParam = 3;
		s.SParam = pcUserData->iInventoryGold;
		s.EParam = psCharacter->sCharacterSaveData.iLastGold - 1;
		LOGSERVER->OnLogCheat( pcUserData, &s );

		psCharacter->sCharacterSaveData.iLastGold = pcUserData->iInventoryGold + 1;

		if ( psCharacter->sCharacterSaveData.iLastGold < 0 )
			psCharacter->sCharacterSaveData.iLastGold = 0;
	}

	return TRUE;
}

BOOL CCharacterDataHandler::SaveDropItems( UserData * pcUserData, PacketCharacterRecordData & sPacket )
{
	UINT iSaveItems = pcUserData->iSaveItems;
	if ( iSaveItems > _countof( pcUserData->sDropItems ) ) 
		iSaveItems = _countof( pcUserData->sDropItems );

	CopyMemory( sPacket.saDropItemData, pcUserData->sDropItems, sizeof( DropItemData ) * iSaveItems );
	sPacket.iDropItemCount = iSaveItems;

	if ( pcUserData->iSaveGold >= 0 && sPacket.sCharacterData.iGold > pcUserData->iSaveGold )
		sPacket.sCharacterSaveData.iLastGold = pcUserData->iSaveGold + 1;

	pcUserData->iSaveItems = 0;
	pcUserData->iSaveGold = -1;

	return TRUE;
}
