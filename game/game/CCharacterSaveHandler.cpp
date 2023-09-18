#include "stdafx.h"
#include "CCharacterSaveHandler.h"

CCharacterSaveHandler::CCharacterSaveHandler()
{
}

CCharacterSaveHandler::~CCharacterSaveHandler()
{
	DELET( pcCharacterSaveData );
}

void CCharacterSaveHandler::HandlePacket( PacketCharacterRecordDataSplit * psPacket )
{
	if ( pcCharacterSaveData == nullptr )
	{
		pcCharacterSaveData = new CCharacterSaveData();
		pcCharacterSaveData->iPacketCount = 0;

		for ( int i = 0; i < _countof( pcCharacterSaveData->bPacketBlockReceived ); i++ )
			pcCharacterSaveData->bPacketBlockReceived[i] = false;
	}

	if ( HandlePacketBlock( psPacket ) )
	{
		HandleData( UNITDATA->sCharacterData, INVENTORYITEMS, INVENTORYITEMS + 100, ITEMHANDLER->GetMouseItem() );

		GAME_LOADINGTIME = 0;

		switch ( UNITDATA->sCharacterData.iClass )
		{
			case CHARACTERCLASS_Archer:
			case CHARACTERCLASS_Atalanta:
			case CHARACTERCLASS_Magician:
			case CHARACTERCLASS_Assassin:
			case CHARACTERCLASS_Shaman:
				if ( UNITDATA->pcBodyMeshContainer )
					SetBoneMeshes( UNITDATA->pcBodyMeshContainer );
				break;
			default:
				break;
		}

		//Not used because some players want to not change their skin of Tier
		//if ( (UNITDATA->sCharacterData.iRank >= CHARACTERRANK_Rank1) && (UNITDATA->sCharacterData.iRank <= CHARACTERRANK_Rank5) )
		//	CharacterGame::ChangeModelSkin( UNITDATA, FALSE, UNITDATA->sCharacterData.iRank, nullptr, CharacterGame::GetHeadModelSkin( UNITDATA ) );

		//Leader?
		if ( (READDWORD( 0x3B17830 ) == 100) || (READDWORD( 0x3B17830 ) == 101) || (READDWORD( 0x3B17830 ) == 104) )
		{
			UNITDATA->sCharacterData.iClanID = CLAN_ID_GAME;
			UNITDATA->iBellatraCrown = CLAN_BELLATRA_CROWN;
		}
		else
		{
			UNITDATA->sCharacterData.iClanID = 0;
			UNITDATA->iBellatraCrown = 0;
		}

		//????
		if ( READDWORD( 0x3B17830 ) != 0 )
			WRITEDWORD( 0x3A976C4, READDWORD( 0x3B17830 ) );

		CHARACTERGAME->SendCharacterData( UNITDATA );

		TIME_NEXTSENDCHARACTERDATA = TICKCOUNT + (5 * 60 * 1000);

		SetPlayerPattern( UNITDATA, UNITDATA->sCharacterData.Player.szBodyModel, UNITDATA->sCharacterData.Player.szHeadModel );

		UPDATE_CHARACTER_CHECKSUM;

		if ( UNITDATA->sCharacterData.sHP.sCur == 0 )
		{
			UNITDATA->bWalking = FALSE;
			
			int iX = 0;
			int iZ = 0;
			MapGame::TeleportStartField( &iX, &iZ );

			UNITDATA->SetPosition( iX, 0, iZ );

			*TRACECAMERA = UNITDATA->sPosition;
			*TRACETARGET = UNITDATA->sPosition;

			UNITDATA->Animate( ANIMATIONTYPE_Revive );

			SkillManager::StartSkillEffectOld( UNITDATA->sPosition.iX, UNITDATA->sPosition.iY, UNITDATA->sPosition.iZ, 500 );

			SetDynamicLight( UNITDATA->sPosition.iX, UNITDATA->sPosition.iY, UNITDATA->sPosition.iZ, 100, 100, 100, 0, 200 );

			PlayUnitSound( UNITDATA );

			BATTLETIME = 0;

			BATTLETIME_QUIT = 0;

			UNITDATA->iLoadedMapIndex = -1;

			int iY = 0;
			if ( STAGE1 )
			{
				iY = STAGE1->GetHighestPoint( UNITDATA->sPosition.iX, UNITDATA->sPosition.iY, UNITDATA->sPosition.iZ, UNITDATA->pcBodyMeshContainer->uMeshHeight );

				UNITDATA->iLoadedMapIndex = 0;
			}

			if ( (iY == (-32767)) && STAGE2 )
				UNITDATA->iLoadedMapIndex = 1;
		}

		if ( (UNITDATA->sCharacterData.sHP.sCur > UNITDATA->sCharacterData.sHP.sMax) || (UNITDATA->sCharacterData.sHP.sCur < 0) )
		{
			CGameProtect::ValidateCharacterCheck();

			UNITDATA->sCharacterData.sHP.sCur = UNITDATA->sCharacterData.sHP.sMax / 2;

			UPDATE_CHARACTER_CHECKSUM;
		}

		CGameProtect::ValidateCharacterCheck();

		XOREXP_LOW = TICKCOUNT + UNITDATA->sCharacterData.iCurrentExpLow;

		XOREXP_HIGH = TICKCOUNT | (TICKCOUNT * TICKCOUNT);

		XORHP_NUM = (TICKCOUNT * UNITDATA->sCharacterData.sHP.sCur) & 0xFFFF;

		UNITDATA->sCharacterData.iCurrentExpLow ^= XOREXP_LOW;
		UNITDATA->sCharacterData.iCurrentExpHigh ^= XOREXP_HIGH;
		UNITDATA->sCharacterData.sHP.sCur ^= XORHP_NUM;

		UPDATE_CHARACTER_CHECKSUM;

		ResetPotionSave();

		ITEMHANDLER->ResetInventoryItemChecksum();

		ValidateSkillInfo();
		
		//Send 484700E4 BUT i think its not needed anymore
		//...

		CharacterGame::ResetPotionGraph( 4 );

		CharacterGame::ResetInventoryGold();

		SkillManager::ChecksumSkills();

		DELET( pcCharacterSaveData );
	}
}

NAKED BOOL CCharacterSaveHandler::ResetPotionSave()
{
	JMP( 0x00461220 );
}

NAKED BOOL CCharacterSaveHandler::ValidateSkillInfo()
{
	JMP( 0x00620C60 );
}

void CCharacterSaveHandler::ProcessPacket()
{
	ProcessData( UNITDATA->sCharacterData, INVENTORYITEMS, INVENTORYITEMS + 100, ITEMHANDLER->GetMouseItem() );

	ProcessPacket( &pcCharacterSaveData->sPacket );

	DELET( pcCharacterSaveData );
}

bool CCharacterSaveHandler::HandlePacketBlock( PacketCharacterRecordDataSplit * psPacket )
{
	BYTE * pbData = (BYTE *)&pcCharacterSaveData->sPacket;

	CopyMemory( pbData + (psPacket->iCurrentPacket * MAX_PKTSIZE_SAVEDATA), psPacket->szData, psPacket->iSize );

	if ( psPacket->iCurrentPacket < _countof( pcCharacterSaveData->bPacketBlockReceived ) )
		pcCharacterSaveData->bPacketBlockReceived[psPacket->iCurrentPacket] = true;

	int iBlockCount = 0;
	for ( iBlockCount = 0; iBlockCount < psPacket->iMaxPackets; iBlockCount++ )
	{
		if ( pcCharacterSaveData->bPacketBlockReceived[iBlockCount] == false )
			break;
	}

	if ( (iBlockCount == psPacket->iMaxPackets) && (pcCharacterSaveData->sPacket.iHeader == PKTHDR_SaveData) )
	{
		pcCharacterSaveData->sPacket.iHeader = 0;
		return true;
	}

	pcCharacterSaveData->iPacketCount = psPacket->iCurrentPacket;

	return false;
}

bool CCharacterSaveHandler::ProcessPacket( PacketCharacterRecordData * psPacket )
{
	PacketCharacterRecordDataSplit * psPacketSplit = new PacketCharacterRecordDataSplit;

	BYTE * pbData = (BYTE *)psPacket;

	ZeroMemory( psPacketSplit, sizeof( PacketCharacterRecordDataSplit ) );

	int iPacketCount = 0;

	int iTotalLength = 0;

	int iPartialSize = psPacket->iLength / _countof( psPacketSplit->szData );
	if ( (psPacket->iLength % _countof( psPacketSplit->szData )) != 0 )
	{
		if ( psPacket->iLength > _countof( psPacketSplit->szData ) )
		{
			iPartialSize++;
		}
	}

	while ( true )
	{
		int iLength = psPacket->iLength - iTotalLength;
		if ( iLength > _countof( psPacketSplit->szData ) )
		{
			iLength = _countof( psPacketSplit->szData );
		}

		psPacketSplit->iHeader = PKTHDR_SaveData;
		psPacketSplit->iLength = iLength + 0x14;

		psPacketSplit->iCurrentPacket = iPacketCount;
		psPacketSplit->iMaxPackets = iPartialSize;

		psPacketSplit->iSize = iLength;
		CopyMemory( &psPacketSplit->szData, &pbData[iTotalLength], iLength );

		SENDPACKETL( psPacketSplit, TRUE );

		iPacketCount++;

		iTotalLength += iLength;

		if ( iTotalLength >= psPacket->iLength )
		{
			break;
		}
	}

	DELET( psPacketSplit );

	return true;
}

bool CCharacterSaveHandler::HandleData( CharacterData & sCharacterData, ItemData * pcaItemDataLeft, ItemData * pcaItemDataRight, ItemData * pcItemDataMouse )
{
	CGameProtect::ValidateCharacterCheck();

	CopyMemory( &sCharacterData, &pcCharacterSaveData->sPacket.sCharacterData, sizeof( CharacterData ) );
	sCharacterData.cNewLoad = 0;

	ResetSavePotions( &pcCharacterSaveData->sPacket.sCharacterSaveData );

	CGameProtect::ValidateCharacterCheck();

	ZeroMemory( pcaItemDataLeft, sizeof( ItemData ) * INVENTORYSERVER_MAX );
	ZeroMemory( pcaItemDataRight, sizeof( ItemData ) * INVENTORYSERVER_MAX );

	BYTE * pbaItemRecord = (BYTE *)&pcCharacterSaveData->sPacket.baData;

	bool bWeightError = false;
	if ( (sCharacterData.sWeight.sCur < 0) || (sCharacterData.sWeight.sCur > ( sCharacterData.sWeight.sMax + 2000 )) )
	{
		SendHackUserPacket( CHEATLOGID_WeightPotionPositionError );

		bWeightError = true;
	}

	BOOL bItemSafe = TRUE;

	int iItemPositionErrorCount = 0;
	int iServerPotionError = 0;
	int iGamePotionError = 0;

	int iSameItemCount = 0;

	std::vector<ItemData *> vItemDataToCheck;


	RecordItem sRecordItem;
	for ( UINT i = 0; i < pcCharacterSaveData->sPacket.iItemCount; i++ )
	{
		ITEMHANDLER->DecodeItemsData( pbaItemRecord, &sRecordItem, 0x7FFFF );

		bItemSafe = TRUE;

		EItemType eItemType = sRecordItem.sItem.sItemID.ToItemType();

		if ( (sRecordItem.iItemPosition != ITEMSLOT_LeftHand) &&
			(sRecordItem.iItemPosition != ITEMSLOT_LeftHand) &&
			(sRecordItem.iItemPosition != ITEMSLOT_Armor) )
		{
			if ( sRecordItem.sItem.sItemID.ToItemBase() == ITEMBASE_Weapon )
			{
				iItemPositionErrorCount++;
			}

			if ( (sRecordItem.sItem.sItemID.ToItemType() == ITEMTYPE_Armor) ||
				(sRecordItem.sItem.sItemID.ToItemType() == ITEMTYPE_Shield) )
			{
				iItemPositionErrorCount++;
			}
		}

		if ( (sRecordItem.iItemPosition != 2) && (ITEMHANDLER->IsValidItem( &sRecordItem.sItem ) == FALSE) )
		{
			bItemSafe = FALSE;

			SendHackUserPacket( CHEATLOGID_ItemError );
		}

		if ( (sRecordItem.sItem.iChk1 == 0) || (sRecordItem.sItem.iChk2 == 0) )
		{
			if ( sRecordItem.sItem.szItemName[0] != 0 )
			{
				bItemSafe = FALSE;

				SendHackUserPacket( CHEATLOGID_ItemError );
			}
		}

		if ( sRecordItem.sItem.sItemID.ToItemBase() == ITEMBASE_Potion )
		{
			if ( sRecordItem.sItem.iPotionCount <= 0 )
			{
				bItemSafe = FALSE;
			}
			else
			{
				int iPotionCount = AddPotionRecord( sRecordItem.sItem.sItemID.ToItemID(), sRecordItem.sItem.iPotionCount );
				if ( iPotionCount < 0 )
				{
					sRecordItem.sItem.iPotionCount += iPotionCount;
					iServerPotionError -= iPotionCount;

					if ( sRecordItem.sItem.iPotionCount <= 0 )
					{
						bItemSafe = FALSE;
					}
				}
			}
		}

		if ( UI::ItemInfoBox::IsExpiredItemTime( &sRecordItem.sItem ) )
		{
			ChatGame::AddChatBoxTextEx( CHATCOLOR_Error, "> [%s] has expired.", sRecordItem.sItem.szItemName );

			continue;
		}

		if ( bItemSafe )
		{
			bool bFoundSameItem = false;
			if ( (sRecordItem.sItem.sItemID.ToItemBase() != ITEMBASE_Potion) && (sRecordItem.sItem.szItemName[0] != 0) )
			{
				for ( auto pcItemDataToCheck : vItemDataToCheck )
				{
					if ( ITEMHANDLER->IsSameItem( &sRecordItem.sItem, &pcItemDataToCheck->sItem ) )
					{
						iSameItemCount++;

						bFoundSameItem = true;
						break;
					}
				}
			}

			if ( sRecordItem.sItem.iChecksum3 == 0 )
				sRecordItem.sItem.iChecksum3 = sRecordItem.sItem.iChk2 + sRecordItem.sItem.sItemID.ToInt();

			if ( bFoundSameItem == false )
			{
				ItemData * pcItemData = (int)i < pcCharacterSaveData->sPacket.iItemSubStart ? &pcaItemDataLeft[sRecordItem.iItemCount] : &pcaItemDataRight[sRecordItem.iItemCount];

				if ( sRecordItem.iItemPosition == (-1) )
				{
					if ( pcItemDataMouse )
						pcItemData = pcItemDataMouse;
					else
						pcItemData = &pcaItemDataRight[INVENTORYSERVER_MAX - 1];

					sRecordItem.iItemPosition = 0;
				}

				pcItemData->sPosition.iX = sRecordItem.sItemPosition.iX;
				pcItemData->sPosition.iY = sRecordItem.sItemPosition.iY;
				pcItemData->iItemSlot = sRecordItem.iItemPosition;
				pcItemData->bValid = TRUE;
				CopyMemory( &pcItemData->sItem, &sRecordItem.sItem, sizeof( Item ) );

				if ( sRecordItem.sItem.sItemID.ToItemBase() != ITEMBASE_Potion )
				{
					vItemDataToCheck.push_back( pcItemData );
				}
			}
		}

		pbaItemRecord += ((int *)pbaItemRecord)[0];
	}

	HandleSaveData( pcCharacterSaveData->sPacket.sCharacterSaveData );

	if ( pcCharacterSaveData->sPacket.sCharacterSaveData.iLastGold > 0 )
	{
		CGameProtect::ValidateCharacterCheck();

		sCharacterData.iGold = pcCharacterSaveData->sPacket.sCharacterSaveData.iLastGold - 1;

		UPDATE_CHARACTER_CHECKSUM;
	}

	if ( iSameItemCount > 0 )
	{
		SendItemCopyHackUserPacket( iSameItemCount );
	}

	if ( iItemPositionErrorCount > 0 )
	{
		iItemPositionErrorCount += 10000;
		SendHackUserPacket( CHEATLOGID_WeightPotionPositionError, iItemPositionErrorCount );

		SendHackUserPacket( CHEATLOGID_CopiedItemError );
	}

	if ( iServerPotionError > 0 )
	{
		iServerPotionError += 10000;
		SendHackUserPacket( CHEATLOGID_WeightPotionPositionError, iServerPotionError );

		SendHackUserPacket( CHEATLOGID_CopiedItemError );
	}

	//Load Inventory Item Info
	CGameProtect::ValidateCharacterCheck();
	CALLT( 0x004C41A0, 0x035EBB20 );
	CGameProtect::ValidateCharacterCheck();

	HandleRecordSkill( &pcCharacterSaveData->sPacket.sCharacterSaveData.sSkillInfo, pcCharacterSaveData->sPacket.sCharacterSaveData.iQuestFlag );

	SKILLSCHARACTER[0].iShortKey = pcCharacterSaveData->sPacket.sCharacterSaveData.iShortKeyDefaultSkill & 0xF;
	SKILLSCHARACTER[0].iMouseIndexPosition = (pcCharacterSaveData->sPacket.sCharacterSaveData.iShortKeyDefaultSkill >> 4);

	if ( pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[0] && pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[0] )
	{
		SetForceOrb( ITEMTYPE_ForceOrb + pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[0], pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[1] );

		UNITDATA->eForceOrbItemID = (EItemID)(ITEMTYPE_ForceOrb + pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[0]);
		UNITDATA->dwForceOrbTimeOut = TICKCOUNT + (pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[1] * 1000);

		SKILLMANAGERHANDLER->SetForceOrbParticle( UNITDATA, pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[1] * 70, (pcCharacterSaveData->sPacket.sCharacterSaveData.wForceOrbUsing[0] >> 8) - 1 );
	}

	return false;
}

bool CCharacterSaveHandler::ProcessData( CharacterData & sCharacterData, ItemData * pcaItemDataLeft, ItemData * pcaItemDataRight, ItemData * pcItemDataMouse )
{
	if ( pcCharacterSaveData == nullptr )
	{
		pcCharacterSaveData = new CCharacterSaveData();
		pcCharacterSaveData->iPacketCount = 0;

		for ( int i = 0; i < _countof( pcCharacterSaveData->bPacketBlockReceived ); i++ )
			pcCharacterSaveData->bPacketBlockReceived[i] = false;
	}

	STRINGCOPY( pcCharacterSaveData->sPacket.szHeader, "RC 1.70" );
	CopyMemory( &pcCharacterSaveData->sPacket.sCharacterData, &sCharacterData, sizeof( CharacterData ) );

	ZeroMemory( &pcCharacterSaveData->sPacket.sCharacterSaveData, sizeof( CharacterSave ) );

	//Not used anyway
	pcCharacterSaveData->sPacket.iDropItemCount = 0;

	ProcessSaveData( pcCharacterSaveData->sPacket.sCharacterSaveData );

	pcCharacterSaveData->sPacket.sCharacterSaveData.dwChecksum = CharacterGame::GetCharacterDataChecksum( &sCharacterData );

	pcCharacterSaveData->sPacket.iItemCount = 0;
	pcCharacterSaveData->sPacket.iDataSize = 0;

	RecordItem sRecordItem;

	BYTE * pbaItemRecord = pcCharacterSaveData->sPacket.baData;

	for ( int i = 0; i < INVENTORYSERVER_MAX; i++ )
	{
		if ( pcaItemDataLeft[i].bValid )
		{
			sRecordItem.iItemCount = i;
			sRecordItem.sItemPosition.iX = pcaItemDataLeft[i].sPosition.iX;
			sRecordItem.sItemPosition.iY = pcaItemDataLeft[i].sPosition.iY;
			sRecordItem.iItemPosition = pcaItemDataLeft[i].iItemSlot;
			CopyMemory( &sRecordItem.sItem, &pcaItemDataLeft[i].sItem, sizeof( Item ) );

			int iSize = ITEMHANDLER->EncodeItemsData( &sRecordItem, pbaItemRecord, sizeof( RecordItem ) );

			pbaItemRecord += iSize;

			pcCharacterSaveData->sPacket.iDataSize += iSize;

			pcCharacterSaveData->sPacket.iItemCount++;
		}
	}

	pcCharacterSaveData->sPacket.iItemSubStart = pcCharacterSaveData->sPacket.iItemCount;


	for ( int i = 0; i < INVENTORYSERVER_MAX; i++ )
	{
		if ( pcaItemDataRight[i].bValid )
		{
			sRecordItem.iItemCount = i;
			sRecordItem.sItemPosition.iX = pcaItemDataRight[i].sPosition.iX;
			sRecordItem.sItemPosition.iY = pcaItemDataRight[i].sPosition.iY;
			sRecordItem.iItemPosition = pcaItemDataRight[i].iItemSlot;
			CopyMemory( &sRecordItem.sItem, &pcaItemDataRight[i].sItem, sizeof( Item ) );

			int iSize = ITEMHANDLER->EncodeItemsData( &sRecordItem, pbaItemRecord, sizeof( RecordItem ) );

			pbaItemRecord += iSize;

			pcCharacterSaveData->sPacket.iDataSize += iSize;

			pcCharacterSaveData->sPacket.iItemCount++;
		}
	}

	if ( pcItemDataMouse && pcItemDataMouse->bValid )
	{
		sRecordItem.iItemCount = 0;
		sRecordItem.sItemPosition.iX = 0;
		sRecordItem.sItemPosition.iY = 0;
		sRecordItem.iItemPosition = -1;
		CopyMemory( &sRecordItem.sItem, &pcItemDataMouse->sItem, sizeof( Item ) );

		int iSize = ITEMHANDLER->EncodeItemsData( &sRecordItem, pbaItemRecord, sizeof( RecordItem ) );

		pbaItemRecord += iSize;

		pcCharacterSaveData->sPacket.iDataSize += iSize;

		pcCharacterSaveData->sPacket.iItemCount++;
	}

	pcCharacterSaveData->sPacket.iLength = sizeof( PacketCharacterRecordData ) - sizeof( pcCharacterSaveData->sPacket.baData ) + pcCharacterSaveData->sPacket.iDataSize;
	pcCharacterSaveData->sPacket.iHeader = PKTHDR_SaveData;

	return true;
}

bool CCharacterSaveHandler::ProcessSaveData( CharacterSave & sCharacterSave )
{
	sCharacterSave.dwHeader = 170;

	sCharacterSave.iCameraMode = CHARACTER_CAMERAMODE;

	sCharacterSave.iMapID = MAP_ID;

	sCharacterSave.iLastPositionX = UNITDATA->sPosition.iX;
	sCharacterSave.iLastPositionZ = UNITDATA->sPosition.iZ;

	if ( STRLEN( ACCOUNT_NAME ) < ACCOUNT_NAME_MAX_LENGTH )
	{
		STRINGCOPY( sCharacterSave.szAccountName, ACCOUNT_NAME );
	}

	ProcessRecordSkill( sCharacterSave.sSkillInfo );

	sCharacterSave.iShortKeyDefaultSkill = SKILLSCHARACTER[0].iShortKey | (SKILLSCHARACTER[0].iMouseIndexPosition << 4);

	ZeroMemory( &sCharacterSave.sQuestInfo, sizeof( QuestCharacterSave ) );
	sCharacterSave.sQuestInfo.waQuestID[0] = (WORD)GetQuestCodeBuffer( sCharacterSave.sQuestInfo.dwaData );

	CopyMemory( &sCharacterSave.sLastQuestInfo, LAST_QUESTSAVE_INFO_OLD, sizeof( LastQuestSave ) );

	sCharacterSave.bShowInitialPet = (short)SHOW_INITIAL_PET;

	sCharacterSave.sElementPet = (short)INITIAL_PET_ELEMENT;

	sCharacterSave.iQuestFlag = QUEST_GLOBAL_FLAG;

	sCharacterSave.iElementaryQuestFlag = QUEST_GLOBAL_ELEMENTARY_FLAG;

	return false;
}

bool CCharacterSaveHandler::HandleSaveData( CharacterSave & sCharacterSave )
{
	CHARACTER_CAMERAMODE = sCharacterSave.iCameraMode;

	if ( sCharacterSave.sQuestInfo.waQuestID[0] )
	{
		LoadOldQuests( (int)sCharacterSave.sQuestInfo.waQuestID[0], sCharacterSave.sQuestInfo.dwaData );
	}

	CopyMemory( LAST_QUESTSAVE_INFO_OLD, &sCharacterSave.sLastQuestInfo, sizeof( LastQuestSave ) );

	UpdateQuestsBoard();

	SHOW_INITIAL_PET = (BOOL)sCharacterSave.bShowInitialPet;
	INITIAL_PET_ELEMENT = (int)sCharacterSave.sElementPet;

	QUEST_GLOBAL_FLAG = sCharacterSave.iQuestFlag;

	QUEST_GLOBAL_ELEMENTARY_FLAG = sCharacterSave.iElementaryQuestFlag;

	BLESSCASTLE_TAX_RATE = sCharacterSave.iBlessCastleTax;

	CBlessCastleHandler::SetBlessCastleMasterClanID( sCharacterSave.iBlessCastleClanID );

	return true;
}

NAKED BOOL CCharacterSaveHandler::ResetSavePotions( CharacterSave * psCharacterSave )
{
	JMP( 0x00464290 );
}

void CCharacterSaveHandler::SendHackUserPacket( int iCode, int iParam )
{
	PacketSimple sPacket;
	sPacket.iLength = sizeof( PacketSimple );
	sPacket.iHeader = PKTHDR_SetBlackList;
	sPacket.iUnk2 = iCode;
	sPacket.iUnk = iParam;
	sPacket.iUnk3 = 0;

	SENDPACKETL( &sPacket, TRUE );
}

void CCharacterSaveHandler::SendItemCopyHackUserPacket( int iItemCount )
{
	PacketSimple sPacket;
	sPacket.iLength = sizeof( PacketSimple );
	sPacket.iHeader = PKTHDR_SetItemCopyHack;
	sPacket.iUnk2 = iItemCount;
	sPacket.iUnk = 0;
	sPacket.iUnk3 = 0;

	SENDPACKETL( &sPacket, TRUE );
}

NAKED int CCharacterSaveHandler::AddPotionRecord( EItemID iItemID, int iPotionCount )
{
	JMP( 0x004642F0 );
}

NAKED BOOL CCharacterSaveHandler::LoadOldQuests( int iQuestID, DWORD * pdwBufferQuest )
{
	JMP( 0x004DAB60 );
}

NAKED BOOL CCharacterSaveHandler::UpdateQuestsBoard()
{
	JMP( 0x0041A280 );
}

NAKED int CCharacterSaveHandler::GetQuestCodeBuffer( DWORD * pdwBufferQuest )
{
	JMP( 0x004D77E0 );
}

bool CCharacterSaveHandler::ProcessRecordSkill( RecordSkill & sRecordSkill )
{
	for ( int i = 1; i <= _countof( sRecordSkill.waSkillMastery ); i++ )
	{
		sRecordSkill.baSkillPoints[i-1] = SKILLSCHARACTER[i].iLevel;

		if ( SKILLSCHARACTER[i].UseSkillCount < 10000 )
		{
			sRecordSkill.waSkillMastery[i-1] = SKILLSCHARACTER[i].UseSkillCount;
		}
		else
		{
			sRecordSkill.waSkillMastery[i-1] = 10000;
		}

		sRecordSkill.baShortKey[i - 1] = SKILLSCHARACTER[i].iShortKey | (SKILLSCHARACTER[i].iMouseIndexPosition << 4);
	}

	sRecordSkill.waSelectSkill[0] = 0;
	sRecordSkill.waSelectSkill[1] = 0;

	if ( SKILL_LEFTSIDE && SKILL_LEFTSIDE->iID != SKILLID_DefaultAttack )
	{
		sRecordSkill.waSelectSkill[0] = SKILL_LEFTSIDE->sSkillInfo.iSkillIndex + 1;
	}

	if ( SKILL_RIGHTSIDE && SKILL_RIGHTSIDE->iID != SKILLID_DefaultAttack )
	{
		sRecordSkill.waSelectSkill[1] = SKILL_RIGHTSIDE->sSkillInfo.iSkillIndex + 1;
	}

	return true;
}

NAKED BOOL CCharacterSaveHandler::HandleRecordSkill( RecordSkill * psRecordSkill, DWORD dwQuestFlag )
{
	JMP( 0x00462470 );
}

NAKED BOOL CCharacterSaveHandler::SetForceOrb( int iItemID, int iTime )
{
	JMP( 0x004B29E0 );
}
