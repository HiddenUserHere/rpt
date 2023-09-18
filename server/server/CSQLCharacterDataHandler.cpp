#include "stdafx.h"
#include "CSQLCharacterDataHandler.h"

#define INSERT_CharacterData_QUERY R"Query(INSERT INTO [dbo].[CharacterData]
    ([CharacterName],
    [BodyModel],
    [HeadModel],
    [ClanID],
    [CharacterType],
    [ShadowSize],
    [Class],
    [Level],
    [Strength],
    [Spirit],
    [Talent],
    [Agility],
    [Health],
    [Accuracy],
    [AttackRating],
    [MinDamage],
    [MaxDamage],
    [AttackSpeed],
    [AttackRange],
    [Critical],
    [DefenseRating],
    [BlockRating],
    [AbsorbRating],
    [MovementSpeed],
    [Sight],
    [CurrentWeight],
    [MaxWeight],
    [OrganicResistance],
    [EarthResistance],
    [FireResistance],
    [IceResistance],
    [LightningResistance],
    [PoisonResistance],
    [WaterResistance],
    [WindResistance],
    [CurrentHP],
    [MaxHP],
    [CurrentMP],
    [MaxMP],
    [CurrentSTM],
    [MaxSTM],
    [HPRegen],
    [MPRegen],
    [STMRegen],
    [EXP],
    [Gold],
    [StatPoints],
    [Checksum],
    [CharacterRank],
    [ClassFlag],
    [Glow],
    [Size],
    [CustomHead],
    [LevelAgingArmor],
	[CharacterID])
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
)Query"

#define UPDATE_CharacterData_QUERY R"Query(UPDATE [dbo].[CharacterData]
SET [CharacterName] = ?,
	[BodyModel] = ?,
    [HeadModel] = ?,
    [ClanID] = ?,
    [CharacterType] = ?,
    [ShadowSize] = ?,
    [Class] = ?,
    [Level] = ?,
    [Strength] = ?,
    [Spirit] = ?,
    [Talent] = ?,
    [Agility] = ?,
    [Health] = ?,
    [Accuracy] = ?,
    [AttackRating] = ?,
    [MinDamage] = ?,
    [MaxDamage] = ?,
    [AttackSpeed] = ?,
    [AttackRange] = ?,
    [Critical] = ?,
    [DefenseRating] = ?,
    [BlockRating] = ?,
    [AbsorbRating] = ?,
    [MovementSpeed] = ?,
    [Sight] = ?,
    [CurrentWeight] = ?,
    [MaxWeight] = ?,
    [OrganicResistance] = ?,
    [EarthResistance] = ?,
    [FireResistance] = ?,
    [IceResistance] = ?,
    [LightningResistance] = ?,
    [PoisonResistance] = ?,
    [WaterResistance] = ?,
    [WindResistance] = ?,
    [CurrentHP] = ?,
    [MaxHP] = ?,
    [CurrentMP] = ?,
    [MaxMP] = ?,
    [CurrentSTM] = ?,
    [MaxSTM] = ?,
    [HPRegen] = ?,
    [MPRegen] = ?,
    [STMRegen] = ?,
    [EXP] = ?,
    [Gold] = ?,
    [StatPoints] = ?,
    [Checksum] = ?,
    [CharacterRank] = ?,
    [ClassFlag] = ?,
    [Glow] = ?,
    [Size] = ?,
    [CustomHead] = ?,
    [LevelAgingArmor] = ?
WHERE [CharacterID] = ?
)Query"

CSQLCharacterDataHandler::CSQLCharacterDataHandler()
{
}

CSQLCharacterDataHandler::~CSQLCharacterDataHandler()
{
}

bool CSQLCharacterDataHandler::LoadCharacterData( const int iCharacterID, CharacterData & sCharacterData )
{
	bool bResult = false;

	ZeroMemory( &sCharacterData, sizeof( CharacterData ) );

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT * FROM CharacterData WHERE [CharacterID]=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );

			if ( pcDB->Execute() )
			{
				if ( pcDB->Fetch() )
				{
					STRINGCOPY( sCharacterData.szName, pcDB->GetDataString( "CharacterName" ).c_str() );
					
					STRINGCOPY( sCharacterData.Player.szBodyModel, pcDB->GetDataString( "BodyModel" ).c_str() );
					STRINGCOPY( sCharacterData.Player.szHeadModel, pcDB->GetDataString( "HeadModel" ).c_str() );

					sCharacterData.iClanID = pcDB->GetData<int>( "ClanID" );

					sCharacterData.iType = (ECharacterType)pcDB->GetData<int>( "CharacterType" );
					
					sCharacterData.iShadowSize = pcDB->GetData<int>( "ShadowSize" );
					
					sCharacterData.iClass = (ECharacterClass)pcDB->GetData<int>( "Class" );
					sCharacterData.iLevel = pcDB->GetData<int>( "Level" );
					sCharacterData.iStrength = pcDB->GetData<int>( "Strength" );
					sCharacterData.iSpirit = pcDB->GetData<int>( "Spirit" );
					sCharacterData.iTalent = pcDB->GetData<int>( "Talent" );
					sCharacterData.iAgility = pcDB->GetData<int>( "Agility" );
					sCharacterData.iHealth = pcDB->GetData<int>( "Health" );
					
					sCharacterData.iAccuracy = pcDB->GetData<int>( "Accuracy" );
					
					sCharacterData.iAttackRating = pcDB->GetData<int>( "AttackRating" );
					
					sCharacterData.iMinDamage = pcDB->GetData<int>( "MinDamage" );
					sCharacterData.iMaxDamage = pcDB->GetData<int>( "MaxDamage" );
					
					sCharacterData.iAttackSpeed = pcDB->GetData<int>( "AttackSpeed" );
					sCharacterData.iAttackRange = pcDB->GetData<int>( "AttackRange" );
					sCharacterData.iCritical = pcDB->GetData<int>( "Critical" );
					sCharacterData.iDefenseRating = pcDB->GetData<int>( "DefenseRating" );
					sCharacterData.iBlockRating = pcDB->GetData<int>( "BlockRating" );
					sCharacterData.iAbsorbRating = pcDB->GetData<int>( "AbsorbRating" );
					sCharacterData.iMovementSpeed = pcDB->GetData<int>( "MovementSpeed" );
					sCharacterData.iSight = pcDB->GetData<int>( "Sight" );
					
					sCharacterData.sWeight.sCur = pcDB->GetData<short>( "CurrentWeight" );
					sCharacterData.sWeight.sMax = pcDB->GetData<short>( "MaxWeight" );
					
					sCharacterData.sElementalDef[ELEMENTID_Organic] = pcDB->GetData<short>( "OrganicResistance" );
					sCharacterData.sElementalDef[ELEMENTID_Earth] = pcDB->GetData<short>( "EarthResistance" );
					sCharacterData.sElementalDef[ELEMENTID_Fire] = pcDB->GetData<short>( "FireResistance" );
					sCharacterData.sElementalDef[ELEMENTID_Ice] = pcDB->GetData<short>( "IceResistance" );
					sCharacterData.sElementalDef[ELEMENTID_Lighting] = pcDB->GetData<short>( "LightningResistance" );
					sCharacterData.sElementalDef[ELEMENTID_Poison] = pcDB->GetData<short>( "PoisonResistance" );

					sCharacterData.sHP.sCur = pcDB->GetData<short>( "CurrentHP" );
					sCharacterData.sHP.sMax = pcDB->GetData<short>( "MaxHP" );

					sCharacterData.sMP.sCur = pcDB->GetData<short>( "CurrentMP" );
					sCharacterData.sMP.sMax = pcDB->GetData<short>( "MaxMP" );
					
					sCharacterData.sSP.sCur = pcDB->GetData<short>( "CurrentSTM" );
					sCharacterData.sSP.sMax = pcDB->GetData<short>( "MaxSTM" );

					sCharacterData.fHPRegen = pcDB->GetData<float>( "HPRegen" );
					sCharacterData.fMPRegen = pcDB->GetData<float>( "MPRegen" );
					sCharacterData.fSPRegen = pcDB->GetData<float>( "STMRegen" );

					LARGE_INTEGER liEXP;
					liEXP.QuadPart = pcDB->GetData<INT64>( "EXP" );
					sCharacterData.iCurrentExpLow = liEXP.LowPart;
					sCharacterData.iCurrentExpHigh = liEXP.HighPart;

					sCharacterData.iGold = pcDB->GetData<int>( "Gold" );

					sCharacterData.iStatPoints = pcDB->GetData<int>( "StatPoints" );

					sCharacterData.dwChecksum = pcDB->GetData<DWORD>( "Checksum" );

					sCharacterData.iRank = (ECharacterRank)pcDB->GetData<int>( "CharacterRank" );

					sCharacterData.iFlag = (EClassFlag)pcDB->GetData<int>( "ClassFlag" );

					sCharacterData.sGlow = pcDB->GetData<short>( "Glow" );
					sCharacterData.sSize = pcDB->GetData<short>( "Size" );

					sCharacterData.bCustomHead = pcDB->GetData<BOOL>( "CustomHead" );

					sCharacterData.sLevelAgeArmor = (BYTE)pcDB->GetData<int>( "LevelAgingArmor" );

					bResult = true;
				}
			}
		}

		pcDB->Close();
	}

	return bResult;
}

bool CSQLCharacterDataHandler::SaveCharacterData( const int iCharacterID, const CharacterData & sCharacterData )
{
	bool bResult = false;

	bool bNewCharacter = SQLCharacterDataExists( iCharacterID ) == false;

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( bNewCharacter ? INSERT_CharacterData_QUERY : UPDATE_CharacterData_QUERY ) )
		{
			pcDB->AddDataString( "CharacterName", sCharacterData.szName );
			pcDB->AddDataString( "BodyModel", sCharacterData.Player.szBodyModel );
			pcDB->AddDataString( "HeadModel", sCharacterData.Player.szHeadModel );

			pcDB->AddData( PARAMTYPE_Integer, "ClanID", sCharacterData.iClanID );

			pcDB->AddData( PARAMTYPE_Integer, "CharacterType", sCharacterData.iType );

			pcDB->AddData( PARAMTYPE_Integer, "ShadowSize", sCharacterData.iShadowSize );

			pcDB->AddData( PARAMTYPE_Integer, "Class", sCharacterData.iClass );
			pcDB->AddData( PARAMTYPE_Integer, "Level", sCharacterData.iLevel );
			pcDB->AddData( PARAMTYPE_Integer, "Strength", sCharacterData.iStrength );
			pcDB->AddData( PARAMTYPE_Integer, "Spirit", sCharacterData.iSpirit );
			pcDB->AddData( PARAMTYPE_Integer, "Talent", sCharacterData.iTalent );
			pcDB->AddData( PARAMTYPE_Integer, "Agility", sCharacterData.iAgility );
			pcDB->AddData( PARAMTYPE_Integer, "Health", sCharacterData.iHealth );

			pcDB->AddData( PARAMTYPE_Integer, "Accuracy", sCharacterData.iAccuracy );

			pcDB->AddData( PARAMTYPE_Integer, "AttackRating", sCharacterData.iAttackRating );

			pcDB->AddData( PARAMTYPE_Integer, "MinDamage", sCharacterData.iMinDamage );
			pcDB->AddData( PARAMTYPE_Integer, "MaxDamage", sCharacterData.iMaxDamage );

			pcDB->AddData( PARAMTYPE_Integer, "AttackSpeed", sCharacterData.iAttackSpeed );
			pcDB->AddData( PARAMTYPE_Integer, "AttackRange", sCharacterData.iAttackRange );
			pcDB->AddData( PARAMTYPE_Integer, "Critical", sCharacterData.iCritical );
			pcDB->AddData( PARAMTYPE_Integer, "DefenseRating", sCharacterData.iDefenseRating );
			pcDB->AddData( PARAMTYPE_Integer, "BlockRating", sCharacterData.iBlockRating );
			pcDB->AddData( PARAMTYPE_Integer, "AbsorbRating", sCharacterData.iAbsorbRating );
			pcDB->AddData( PARAMTYPE_Integer, "MovementSpeed", sCharacterData.iMovementSpeed );
			pcDB->AddData( PARAMTYPE_Integer, "Sight", sCharacterData.iSight );

			pcDB->AddData( PARAMTYPE_Short, "CurrentWeight", sCharacterData.sWeight.sCur );
			pcDB->AddData( PARAMTYPE_Short, "MaxWeight", sCharacterData.sWeight.sMax );

			pcDB->AddData( PARAMTYPE_Short, "OrganicResistance", sCharacterData.sElementalDef[ELEMENTID_Organic] );
			pcDB->AddData( PARAMTYPE_Short, "EarthResistance", sCharacterData.sElementalDef[ELEMENTID_Earth] );
			pcDB->AddData( PARAMTYPE_Short, "FireResistance", sCharacterData.sElementalDef[ELEMENTID_Fire] );
			pcDB->AddData( PARAMTYPE_Short, "IceResistance", sCharacterData.sElementalDef[ELEMENTID_Ice] );
			pcDB->AddData( PARAMTYPE_Short, "LightningResistance", sCharacterData.sElementalDef[ELEMENTID_Lighting] );
			pcDB->AddData( PARAMTYPE_Short, "PoisonResistance", sCharacterData.sElementalDef[ELEMENTID_Poison] );
			pcDB->AddData( PARAMTYPE_Short, "WaterResistance", sCharacterData.sElementalDef[ELEMENTID_Water] );
			pcDB->AddData( PARAMTYPE_Short, "WindResistance", sCharacterData.sElementalDef[ELEMENTID_Wind] );

			pcDB->AddData( PARAMTYPE_Short, "CurrentHP", sCharacterData.sHP.sCur );
			pcDB->AddData( PARAMTYPE_Short, "MaxHP", sCharacterData.sHP.sMax );

			pcDB->AddData( PARAMTYPE_Short, "CurrentMP", sCharacterData.sMP.sCur );
			pcDB->AddData( PARAMTYPE_Short, "MaxMP", sCharacterData.sMP.sMax );

			pcDB->AddData( PARAMTYPE_Short, "CurrentSTM", sCharacterData.sSP.sCur );
			pcDB->AddData( PARAMTYPE_Short, "MaxSTM", sCharacterData.sSP.sMax );

			pcDB->AddData( PARAMTYPE_Float, "HPRegen", sCharacterData.fHPRegen );
			pcDB->AddData( PARAMTYPE_Float, "MPRegen", sCharacterData.fMPRegen );
			pcDB->AddData( PARAMTYPE_Float, "STMRegen", sCharacterData.fSPRegen );

			LARGE_INTEGER liEXP;
			liEXP.HighPart = sCharacterData.iCurrentExpHigh;
			liEXP.LowPart = sCharacterData.iCurrentExpLow;
			pcDB->AddData( PARAMTYPE_Int64, "EXP", liEXP.QuadPart );

			pcDB->AddData( PARAMTYPE_Integer, "Gold", sCharacterData.iGold );

			pcDB->AddData( PARAMTYPE_Integer, "StatPoints", sCharacterData.iStatPoints );

			pcDB->AddData( PARAMTYPE_Integer, "Checksum", sCharacterData.dwChecksum );

			pcDB->AddData( PARAMTYPE_Integer, "CharacterRank", sCharacterData.iRank );

			pcDB->AddData( PARAMTYPE_Integer, "ClassFlag", sCharacterData.iFlag );

			pcDB->AddData( PARAMTYPE_Short, "Glow", sCharacterData.sGlow );
			pcDB->AddData( PARAMTYPE_Short, "Size", sCharacterData.sSize );

			pcDB->AddData( PARAMTYPE_Integer, "CustomHead", sCharacterData.bCustomHead );

			pcDB->AddData( PARAMTYPE_Byte, "LevelAgingArmor", sCharacterData.sLevelAgeArmor );

			pcDB->AddData( PARAMTYPE_Integer, "CharacterID", iCharacterID );

			pcDB->Execute();

			bResult = true;
		}

		pcDB->Close();
	}

	return bResult;
}

bool CSQLCharacterDataHandler::SQLCharacterDataExists( const int iCharacterID )
{
	int iID = -1;

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT ID FROM CharacterData WHERE CharacterID=?" ) )
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

bool CSQLCharacterDataHandler::SQLCharacterNameUpdate( const int iCharacterID, const std::string & strCharacterNameNew )
{
	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "UPDATE CharacterData SET [CharacterName]=? WHERE ([CharacterName]=?)" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_String, strCharacterNameNew.c_str() );
			pcDB->BindParameterInput( 2, PARAMTYPE_Integer, &iCharacterID );

			pcDB->Execute();
		}

		pcDB->Close();
	}

	return true;
}
