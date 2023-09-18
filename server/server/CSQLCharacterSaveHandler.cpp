#include "stdafx.h"
#include "CSQLCharacterSaveHandler.h"

#define INSERT_CharacterSave_QUERY R"Query(INSERT INTO [dbo].[CharacterSave]
           ([Header],
           [MapID],
           [CameraMode],
           [LastPositionX],
           [LastPositionZ],
           [LastGold],
           [Checksum],
           [SkillPoint1],
           [SkillPoint2],
           [SkillPoint3],
           [SkillPoint4],
           [SkillPoint5],
           [SkillPoint6],
           [SkillPoint7],
           [SkillPoint8],
           [SkillPoint9],
           [SkillPoint10],
           [SkillPoint11],
           [SkillPoint12],
           [SkillPoint13],
           [SkillPoint14],
           [SkillPoint15],
           [SkillPoint16],
           [SkillPoint17],
           [SkillPoint18],
           [SkillPoint19],
           [SkillPoint20],
           [SkillMastery1],
           [SkillMastery2],
           [SkillMastery3],
           [SkillMastery4],
           [SkillMastery5],
           [SkillMastery6],
           [SkillMastery7],
           [SkillMastery8],
           [SkillMastery9],
           [SkillMastery10],
           [SkillMastery11],
           [SkillMastery12],
           [SkillMastery13],
           [SkillMastery14],
           [SkillMastery15],
           [SkillMastery16],
           [SkillMastery17],
           [SkillMastery18],
           [SkillMastery19],
           [SkillMastery20],
           [ShortKey1],
           [ShortKey2],
           [ShortKey3],
           [ShortKey4],
           [ShortKey5],
           [ShortKey6],
           [ShortKey7],
           [ShortKey8],
           [ShortKey9],
           [ShortKey10],
           [ShortKey11],
           [ShortKey12],
           [ShortKey13],
           [ShortKey14],
           [ShortKey15],
           [ShortKey16],
           [ShortKey17],
           [ShortKey18],
           [ShortKey19],
           [ShortKey20],
           [SelectSkill1],
           [SelectSkill2],
           [SaveTime],
		   [ShowInitialPet],
		   [ElementPet],
           [PotionUpdate1],
           [PotionUpdate2],
           [PotionCount11],
           [PotionCount12],
           [PotionCount13],
           [PotionCount14],
           [PotionCount21],
           [PotionCount22],
           [PotionCount23],
           [PotionCount24],
           [PotionCount31],
           [PotionCount32],
           [PotionCount33],
           [PotionCount34],
           [ForceOrbCode],
           [ForceOrbTime],
           [QuestFlag],
           [ShortKeyDefaultSkill],
           [BlessCastleTax],
           [BlessCastleClanID],
		   [ElementaryQuestFlag],
           [QuestID1],
           [QuestID2],
           [QuestBuffer],
           [LastQuestID1],
           [LastQuestID2],
           [LastQuestID3],
           [LastQuestID4],
           [LastQuestID5],
           [LastQuestID6],
           [LastQuestID7],
           [LastQuestID8],
           [LastQuestID9],
           [LastQuestID10],
           [LastQuestID11],
           [LastQuestID12],
           [LastQuestID13],
           [LastQuestID14],
           [LastQuestID15],
           [LastQuestID16],
           [LastQuestID17],
           [LastQuestID18],
           [LastQuestID19],
           [LastQuestID20],
           [LastQuestID21],
           [LastQuestID22],
           [LastQuestID23],
           [LastQuestID24],
           [LastQuestID25],
           [LastQuestID26],
           [LastQuestID27],
           [LastQuestID28],
           [LastQuestID29],
           [LastQuestID30],
           [LastQuestID31],
           [LastQuestID32],
		   [CharacterID])
     VALUES
           (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
)Query"

#define UPDATE_CharacterSave_QUERY R"Query(UPDATE [dbo].[CharacterSave]
SET
   [Header] = ?,
   [MapID] = ?,
   [CameraMode] = ?,
   [LastPositionX] = ?,
   [LastPositionZ] = ?,
   [LastGold] = ?,
   [Checksum] = ?,
   [SkillPoint1] = ?,
   [SkillPoint2] = ?,
   [SkillPoint3] = ?,
   [SkillPoint4] = ?,
   [SkillPoint5] = ?,
   [SkillPoint6] = ?,
   [SkillPoint7] = ?,
   [SkillPoint8] = ?,
   [SkillPoint9] = ?,
   [SkillPoint10] = ?,
   [SkillPoint11] = ?,
   [SkillPoint12] = ?,
   [SkillPoint13] = ?,
   [SkillPoint14] = ?,
   [SkillPoint15] = ?,
   [SkillPoint16] = ?,
   [SkillPoint17] = ?,
   [SkillPoint18] = ?,
   [SkillPoint19] = ?,
   [SkillPoint20] = ?,
   [SkillMastery1] = ?,
   [SkillMastery2] = ?,
   [SkillMastery3] = ?,
   [SkillMastery4] = ?,
   [SkillMastery5] = ?,
   [SkillMastery6] = ?,
   [SkillMastery7] = ?,
   [SkillMastery8] = ?,
   [SkillMastery9] = ?,
   [SkillMastery10] = ?,
   [SkillMastery11] = ?,
   [SkillMastery12] = ?,
   [SkillMastery13] = ?,
   [SkillMastery14] = ?,
   [SkillMastery15] = ?,
   [SkillMastery16] = ?,
   [SkillMastery17] = ?,
   [SkillMastery18] = ?,
   [SkillMastery19] = ?,
   [SkillMastery20] = ?,
   [ShortKey1] = ?,
   [ShortKey2] = ?,
   [ShortKey3] = ?,
   [ShortKey4] = ?,
   [ShortKey5] = ?,
   [ShortKey6] = ?,
   [ShortKey7] = ?,
   [ShortKey8] = ?,
   [ShortKey9] = ?,
   [ShortKey10] = ?,
   [ShortKey11] = ?,
   [ShortKey12] = ?,
   [ShortKey13] = ?,
   [ShortKey14] = ?,
   [ShortKey15] = ?,
   [ShortKey16] = ?,
   [ShortKey17] = ?,
   [ShortKey18] = ?,
   [ShortKey19] = ?,
   [ShortKey20] = ?,
   [SelectSkill1] = ?,
   [SelectSkill2] = ?,
   [SaveTime] = ?,
   [ShowInitialPet] = ?,
   [ElementPet] = ?,
   [PotionUpdate1] = ?,
   [PotionUpdate2] = ?,
   [PotionCount11] = ?,
   [PotionCount12] = ?,
   [PotionCount13] = ?,
   [PotionCount14] = ?,
   [PotionCount21] = ?,
   [PotionCount22] = ?,
   [PotionCount23] = ?,
   [PotionCount24] = ?,
   [PotionCount31] = ?,
   [PotionCount32] = ?,
   [PotionCount33] = ?,
   [PotionCount34] = ?,
   [ForceOrbCode] = ?,
   [ForceOrbTime] = ?,
   [QuestFlag] = ?,
   [ShortKeyDefaultSkill] = ?,
   [BlessCastleTax] = ?,
   [BlessCastleClanID] = ?,
   [ElementaryQuestFlag] = ?,
   [QuestID1] = ?,
   [QuestID2] = ?,
   [QuestBuffer] = ?,
   [LastQuestID1] = ?,
   [LastQuestID2] = ?,
   [LastQuestID3] = ?,
   [LastQuestID4] = ?,
   [LastQuestID5] = ?,
   [LastQuestID6] = ?,
   [LastQuestID7] = ?,
   [LastQuestID8] = ?,
   [LastQuestID9] = ?,
   [LastQuestID10] = ?,
   [LastQuestID11] = ?,
   [LastQuestID12] = ?,
   [LastQuestID13] = ?,
   [LastQuestID14] = ?,
   [LastQuestID15] = ?,
   [LastQuestID16] = ?,
   [LastQuestID17] = ?,
   [LastQuestID18] = ?,
   [LastQuestID19] = ?,
   [LastQuestID20] = ?,
   [LastQuestID21] = ?,
   [LastQuestID22] = ?,
   [LastQuestID23] = ?,
   [LastQuestID24] = ?,
   [LastQuestID25] = ?,
   [LastQuestID26] = ?,
   [LastQuestID27] = ?,
   [LastQuestID28] = ?,
   [LastQuestID29] = ?,
   [LastQuestID30] = ?,
   [LastQuestID31] = ?,
   [LastQuestID32] = ?
WHERE [CharacterID] = ?)Query"

CSQLCharacterSaveHandler::CSQLCharacterSaveHandler()
{
}

CSQLCharacterSaveHandler::~CSQLCharacterSaveHandler()
{
}

bool CSQLCharacterSaveHandler::LoadCharacterSave( const int iCharacterID, CharacterSave & sCharacterSave )
{
	bool bResult = false;

	ZeroMemory( &sCharacterSave, sizeof( CharacterSave ) );

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT TOP 1 * FROM CharacterSave Where CharacterID=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );
			if ( pcDB->Execute() )
			{
				if ( pcDB->Fetch() )
				{
					sCharacterSave.dwHeader = pcDB->GetData<DWORD>( "Header" );

					sCharacterSave.iMapID = (EMapID)pcDB->GetData<int>( "MapID" );

					sCharacterSave.iCameraMode = pcDB->GetData<int>( "CameraMode" );

					sCharacterSave.iLastPositionX = pcDB->GetData<int>( "LastPositionX" );
					sCharacterSave.iLastPositionZ = pcDB->GetData<int>( "LastPositionZ" );

					sCharacterSave.iLastGold = pcDB->GetData<int>( "LastGold" );

					sCharacterSave.dwChecksum = pcDB->GetData<DWORD>( "Checksum" );

					for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.baSkillPoints ); i++ )
						sCharacterSave.sSkillInfo.baSkillPoints[i] = pcDB->GetData<BYTE>( "SkillPoint" + std::to_string( i + 1 ) );

					for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.waSkillMastery ); i++ )
						sCharacterSave.sSkillInfo.waSkillMastery[i] = pcDB->GetData<WORD>( "SkillMastery" + std::to_string( i + 1 ) );

					for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.baShortKey ); i++ )
						sCharacterSave.sSkillInfo.baShortKey[i] = pcDB->GetData<BYTE>( "ShortKey" + std::to_string( i + 1 ) );

					sCharacterSave.sSkillInfo.waSelectSkill[0] = pcDB->GetData<WORD>( "SelectSkill1" );
					sCharacterSave.sSkillInfo.waSelectSkill[1] = pcDB->GetData<WORD>( "SelectSkill2" );

					sCharacterSave.iSaveTime = pcDB->GetData<int>( "SaveTime" );

					sCharacterSave.bShowInitialPet = pcDB->GetData<short>( "ShowInitialPet" );
					sCharacterSave.sElementPet = pcDB->GetData<short>( "ElementPet" );

					sCharacterSave.sPotionUpdate[0] = pcDB->GetData<short>( "PotionUpdate1" );
					sCharacterSave.sPotionUpdate[1] = pcDB->GetData<short>( "PotionUpdate2" );

					sCharacterSave.sPotionCount[0][0] = pcDB->GetData<short>( "PotionCount11" );
					sCharacterSave.sPotionCount[0][1] = pcDB->GetData<short>( "PotionCount12" );
					sCharacterSave.sPotionCount[0][2] = pcDB->GetData<short>( "PotionCount13" );
					sCharacterSave.sPotionCount[0][3] = pcDB->GetData<short>( "PotionCount14" );
					sCharacterSave.sPotionCount[1][0] = pcDB->GetData<short>( "PotionCount21" );
					sCharacterSave.sPotionCount[1][1] = pcDB->GetData<short>( "PotionCount22" );
					sCharacterSave.sPotionCount[1][2] = pcDB->GetData<short>( "PotionCount23" );
					sCharacterSave.sPotionCount[1][3] = pcDB->GetData<short>( "PotionCount24" );
					sCharacterSave.sPotionCount[2][0] = pcDB->GetData<short>( "PotionCount31" );
					sCharacterSave.sPotionCount[2][1] = pcDB->GetData<short>( "PotionCount32" );
					sCharacterSave.sPotionCount[2][2] = pcDB->GetData<short>( "PotionCount33" );
					sCharacterSave.sPotionCount[2][3] = pcDB->GetData<short>( "PotionCount34" );

					sCharacterSave.wForceOrbUsing[0] = pcDB->GetData<WORD>( "ForceOrbCode" );
					sCharacterSave.wForceOrbUsing[1] = pcDB->GetData<WORD>( "ForceOrbTime" );

					sCharacterSave.iQuestFlag = pcDB->GetData<int>( "QuestFlag" );

					sCharacterSave.iShortKeyDefaultSkill = pcDB->GetData<int>( "ShortKeyDefaultSkill" );

					sCharacterSave.iBlessCastleTax = pcDB->GetData<int>( "BlessCastleTax" );
					sCharacterSave.iBlessCastleClanID = pcDB->GetData<int>( "BlessCastleClanID" );

					sCharacterSave.iElementaryQuestFlag = pcDB->GetData<int>( "ElementaryQuestFlag" );

					sCharacterSave.sQuestInfo.waQuestID[0] = pcDB->GetData<WORD>( "QuestID1" );
					sCharacterSave.sQuestInfo.waQuestID[1] = pcDB->GetData<WORD>( "QuestID2" );

					pcDB->GetData( "QuestBuffer", PARAMTYPE_Binary, sCharacterSave.sQuestInfo.szBuffer, sizeof( sCharacterSave.sQuestInfo.szBuffer ) );

					for ( int i = 0; i < _countof( sCharacterSave.sLastQuestInfo.waQuestID ); i++ )
					{
						sCharacterSave.sLastQuestInfo.waQuestID[i] = pcDB->GetData<WORD>( "LastQuestID" + std::to_string( i + 1 ) );

						if ( sCharacterSave.sLastQuestInfo.waQuestID[i] != 0 )
							sCharacterSave.sLastQuestInfo.iCount++;
					}

					bResult = true;
				}
			}
		}

		pcDB->Close();
	}

	return bResult;
}

bool CSQLCharacterSaveHandler::SaveCharacterSave( const int iCharacterID, CharacterSave & sCharacterSave )
{
	bool bResult = false;

	bool bCharacterSaveExists = SQLCharacterSaveExists( iCharacterID );

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( bCharacterSaveExists ? UPDATE_CharacterSave_QUERY : INSERT_CharacterSave_QUERY ) )
		{
			pcDB->AddData( PARAMTYPE_Integer, "Header", sCharacterSave.dwHeader );

			pcDB->AddData( PARAMTYPE_Integer, "MapID", sCharacterSave.iMapID );

			pcDB->AddData( PARAMTYPE_Integer, "CameraMode", sCharacterSave.iCameraMode );

			pcDB->AddData( PARAMTYPE_Integer, "LastPositionX", sCharacterSave.iLastPositionX );
			pcDB->AddData( PARAMTYPE_Integer, "LastPositionZ", sCharacterSave.iLastPositionZ );

			pcDB->AddData( PARAMTYPE_Integer, "LastGold", sCharacterSave.iLastGold );

			pcDB->AddData( PARAMTYPE_Integer, "Checksum", sCharacterSave.dwChecksum );

			for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.baSkillPoints ); i++ )
				pcDB->AddData( PARAMTYPE_Byte, "SkillPoint" + std::to_string( i + 1 ), sCharacterSave.sSkillInfo.baSkillPoints[i] );

			for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.waSkillMastery ); i++ )
				pcDB->AddData( PARAMTYPE_Short, "SkillMastery" + std::to_string( i + 1 ), sCharacterSave.sSkillInfo.waSkillMastery[i] );

			for ( int i = 0; i < _countof( sCharacterSave.sSkillInfo.baShortKey ); i++ )
				pcDB->AddData( PARAMTYPE_Byte, "ShortKey" + std::to_string( i + 1 ), sCharacterSave.sSkillInfo.baShortKey[i] );

			pcDB->AddData( PARAMTYPE_Short, "SelectSkill1", sCharacterSave.sSkillInfo.waSelectSkill[0] );
			pcDB->AddData( PARAMTYPE_Short, "SelectSkill2", sCharacterSave.sSkillInfo.waSelectSkill[1] );

			pcDB->AddData( PARAMTYPE_Integer, "SaveTime", sCharacterSave.iSaveTime );

			pcDB->AddData( PARAMTYPE_Short, "ShowInitialPet", sCharacterSave.bShowInitialPet );
			pcDB->AddData( PARAMTYPE_Short, "ElementPet", sCharacterSave.sElementPet );

			pcDB->AddData( PARAMTYPE_Short, "PotionUpdate1", sCharacterSave.sPotionUpdate[0] );
			pcDB->AddData( PARAMTYPE_Short, "PotionUpdate2", sCharacterSave.sPotionUpdate[1] );

			pcDB->AddData( PARAMTYPE_Short, "PotionCount11", sCharacterSave.sPotionCount[0][0] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount12", sCharacterSave.sPotionCount[0][1] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount13", sCharacterSave.sPotionCount[0][2] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount14", sCharacterSave.sPotionCount[0][3] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount21", sCharacterSave.sPotionCount[1][0] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount22", sCharacterSave.sPotionCount[1][1] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount23", sCharacterSave.sPotionCount[1][2] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount24", sCharacterSave.sPotionCount[1][3] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount31", sCharacterSave.sPotionCount[2][0] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount32", sCharacterSave.sPotionCount[2][1] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount33", sCharacterSave.sPotionCount[2][2] );
			pcDB->AddData( PARAMTYPE_Short, "PotionCount34", sCharacterSave.sPotionCount[2][3] );

			pcDB->AddData( PARAMTYPE_Short, "ForceOrbCode", sCharacterSave.wForceOrbUsing[0] );
			pcDB->AddData( PARAMTYPE_Short, "ForceOrbTime", sCharacterSave.wForceOrbUsing[1] );

			pcDB->AddData( PARAMTYPE_Integer, "QuestFlag", sCharacterSave.iQuestFlag );

			pcDB->AddData( PARAMTYPE_Integer, "ShortKeyDefaultSkill", sCharacterSave.iShortKeyDefaultSkill );

			pcDB->AddData( PARAMTYPE_Integer, "BlessCastleTax", sCharacterSave.iBlessCastleTax );
			pcDB->AddData( PARAMTYPE_Integer, "BlessCastleClanID", sCharacterSave.iBlessCastleClanID );

			pcDB->AddData( PARAMTYPE_Integer, "ElementaryQuestFlag", sCharacterSave.iElementaryQuestFlag );

			pcDB->AddData( PARAMTYPE_Short, "QuestID1", sCharacterSave.sLastQuestInfo.waQuestID[0] );
			pcDB->AddData( PARAMTYPE_Short, "QuestID2", sCharacterSave.sLastQuestInfo.waQuestID[1] );

			pcDB->AddDataBinary( "QuestBuffer", (BYTE *)sCharacterSave.sQuestInfo.szBuffer, sizeof( sCharacterSave.sQuestInfo.szBuffer ) );

			if ( sCharacterSave.sLastQuestInfo.iCount > _countof( sCharacterSave.sLastQuestInfo.waQuestID ) )
				sCharacterSave.sLastQuestInfo.iCount = _countof( sCharacterSave.sLastQuestInfo.waQuestID );

			WORD wQuestZeroID = 0;
			for ( UINT i = 0; i < _countof( sCharacterSave.sLastQuestInfo.waQuestID ); i++ )
			{
				pcDB->AddData( PARAMTYPE_Short, "LastQuestID" + std::to_string( i + 1 ), i < sCharacterSave.sLastQuestInfo.iCount ? sCharacterSave.sLastQuestInfo.waQuestID[i] : wQuestZeroID );
			}

			pcDB->AddData( PARAMTYPE_Integer, "CharacterID", iCharacterID );

			pcDB->Execute();

			bResult = true;
		}

		pcDB->Close();
	}

	return bResult;
}

bool CSQLCharacterSaveHandler::SQLCharacterSaveExists( const int iCharacterID )
{
	int iID = -1;

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT ID FROM CharacterSave WHERE CharacterID=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );

			if ( pcDB->Execute() )
			{
				if ( pcDB->Fetch() )
				{
					iID = pcDB->GetData<int>( 1 );
				}
			}
		}

		pcDB->Close();
	}

	return iID >= 0;
}

bool CSQLCharacterSaveHandler::SQLCharacterNameUpdate( const int iCharacterID, const std::string & strCharacterNameNew )
{
	return true;
}

bool CSQLCharacterSaveHandler::SQLGetLastPosition( const int iCharacterID, int & iLastPositionX, int & iLastPositionZ )
{
	return false;
}
