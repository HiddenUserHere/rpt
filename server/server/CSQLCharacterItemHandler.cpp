#include "stdafx.h"
#include "CSQLCharacterItemHandler.h"

#define INSERT_CharacterItem_QUERY R"Query(INSERT INTO [dbo].[CharacterItem]
    ([AccountID],
	[CharacterID],
    [CharacterItemDataID],
    [ItemID],
    [Code1],
    [Code2],
	[ItemCountIndex],
	[ItemPosition],
	[ItemPositionX],
	[ItemPositionY],
    [Size],
    [Version],
    [ItemTime],
    [CurrentIntegrity],
    [MaxIntegrity],
    [ItemName],
	[ExtraItemName],
    [Weight],
    [SalePrice],
    [Index],
    [PotionCount],
    [OrganicElement],
    [EarthElement],
    [FireElement],
    [IceElement],
    [LightingElement],
    [PoisonElement],
    [WaterElement],
    [WindElement],
    [Sight],
	[Checksum3],
    [MinDamage],
    [MaxDamage],
    [AttackRange],
    [AttackSpeed],
    [AttackRating],
    [Critical],
    [AbsorbRating],
    [DefenseRating],
    [BlockRating],
    [MovementSpeed],
    [PotionStorage],
    [MagicMastery],
    [MPRegen],
    [HPRegen],
    [SPRegen],
    [AddHP],
    [AddMP],
    [AddSP],
    [Level],
    [Strength],
    [Spirit],
    [Talent],
    [Agility],
    [Health],
    [HPRecoveryMin],
    [HPRecoveryMax],
    [MPRecoveryMin],
    [MPRecoveryMax],
    [SPRecoveryMin],
    [SPRecoveryMax],
    [Gold],
    [CannotUse],
    [BackupCode1],
    [BackupCode2],
    [ScaleBlink1],
    [ScaleBlink2],
    [ItemUniqueID],
    [EffectBlink1],
    [EffectBlink2],
    [EffectColor1],
    [EffectColor2],
    [EffectColor3],
    [EffectColor4],
    [EnableEffect],
    [Specialization],
    [SpecAbsorbRating],
    [SpecDefenseRating],
    [SpecMovementSpeed],
    [SpecBlockRating],
    [SpecAttackSpeed],
    [SpecCritical],
    [SpecAttackRange],
    [SpecMagicMastery],
	[SpecElementalOrganic],
	[SpecElementalEarth],
	[SpecElementalFire],
	[SpecElementalIce],
	[SpecElementalLightning],
	[SpecElementalPoison],
	[SpecElementalWater],
	[SpecElementalWind],
	[SpecElementalOrganicAtkDiv],
	[SpecElementalEarthAtkDiv],
	[SpecElementalFireAtkDiv],
	[SpecElementalIceAtkDiv],
	[SpecElementalLightningAtkDiv],
	[SpecElementalPoisonAtkDiv],
	[SpecElementalWaterAtkDiv],
	[SpecElementalWindAtkDiv],
	[SpecAddMPDiv],
	[SpecAddHPDiv],
	[SpecAttackRatingDiv],
	[SpecAttackPowerDivMin],
	[SpecAttackPowerDivMax],
    [SpecMPRegen],
    [SpecHPRegen],
    [SpecSPRegen],
    [SpecEvade],
    [SpecPenetration],
    [SpecHPPotionEfficience],
    [SpecMPPotionEfficience],
    [SpecSPPotionEfficience],
    [CraftType],
    [MixEffect],
    [AgeMixIDLevel],
    [Maturing],
    [CurrentMature],
    [MaxMature],
    [CraftProtection1],
    [CraftProtection2],
    [SpecialItemType1],
    [SpecialItemType2],
    [ItemCreateTime],
    [SocketType1],
    [SocketGemType1],
    [SocketGemItem1],
    [SocketType2],
    [SocketGemType2],
    [SocketGemItem2],
    [CostumeItem],
    [Evade],
    [LockedItemType],
    [CurrentQuestMonster],
    [MaxQuestMonster],
    [MixTypeName],
    [PerfectItem],
	[Date])
VALUES
    (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, GETDATE())
)Query"

#define SELECT_CharacterItem_QUERY R"Query(SELECT
	ci.[ID] AS [ID],
    ci.[Size] AS [Size],
    ci.[ItemID] AS [ItemID],
    ci.[Code1] AS [Code1],
    ci.[Code2] AS [Code2],
    ci.[Version] AS [Version],
    ci.[ItemTime] AS [ItemTime],
    ci.[CurrentIntegrity] AS [CurrentIntegrity],
    ci.[MaxIntegrity] AS [MaxIntegrity],
    ci.[ItemName] AS [ItemName],
    ci.[ExtraItemName] AS [ExtraItemName],
    ci.[Weight] AS [Weight],
    ci.[SalePrice] AS [SalePrice],
    ci.[Index] AS [Index],
    ci.[PotionCount] AS [PotionCount],
    ci.[OrganicElement] AS [OrganicElement],
    ci.[EarthElement] AS [EarthElement],
    ci.[FireElement] AS [FireElement],
    ci.[IceElement] AS [IceElement],
    ci.[LightingElement] AS [LightingElement],
    ci.[PoisonElement] AS [PoisonElement],
    ci.[WaterElement] AS [WaterElement],
    ci.[WindElement] AS [WindElement],
    ci.[Sight] AS [Sight],
    ci.[Checksum3] AS [Checksum3],
    ci.[MinDamage] AS [MinDamage],
    ci.[MaxDamage] AS [MaxDamage],
    ci.[AttackRange] AS [AttackRange],
    ci.[AttackSpeed] AS [AttackSpeed],
    ci.[AttackRating] AS [AttackRating],
    ci.[Critical] AS [Critical],
    ci.[AbsorbRating] AS [AbsorbRating],
    ci.[DefenseRating] AS [DefenseRating],
    ci.[BlockRating] AS [BlockRating],
    ci.[MovementSpeed] AS [MovementSpeed],
    ci.[PotionStorage] AS [PotionStorage],
    ci.[MagicMastery] AS [MagicMastery],
    ci.[MPRegen] AS [MPRegen],
    ci.[HPRegen] AS [HPRegen],
    ci.[SPRegen] AS [SPRegen],
    ci.[AddHP] AS [AddHP],
    ci.[AddMP] AS [AddMP],
    ci.[AddSP] AS [AddSP],
    ci.[Level] AS [Level],
    ci.[Strength] AS [Strength],
    ci.[Spirit] AS [Spirit],
    ci.[Talent] AS [Talent],
    ci.[Agility] AS [Agility],
    ci.[Health] AS [Health],
    ci.[HPRecoveryMin] AS [HPRecoveryMin],
    ci.[HPRecoveryMax] AS [HPRecoveryMax],
    ci.[MPRecoveryMin] AS [MPRecoveryMin],
    ci.[MPRecoveryMax] AS [MPRecoveryMax],
    ci.[SPRecoveryMin] AS [SPRecoveryMin],
    ci.[SPRecoveryMax] AS [SPRecoveryMax],
    ci.[Gold] AS [Gold],
    ci.[CannotUse] AS [CannotUse],
    ci.[BackupCode1] AS [BackupCode1],
    ci.[BackupCode2] AS [BackupCode2],
    ci.[ScaleBlink1] AS [ScaleBlink1],
    ci.[ScaleBlink2] AS [ScaleBlink2],
    ci.[ItemUniqueID] AS [ItemUniqueID],
    ci.[EffectBlink1] AS [EffectBlink1],
    ci.[EffectBlink2] AS [EffectBlink2],
    ci.[EffectColor1] AS [EffectColor1],
    ci.[EffectColor2] AS [EffectColor2],
    ci.[EffectColor3] AS [EffectColor3],
    ci.[EffectColor4] AS [EffectColor4],
    ci.[EnableEffect] AS [EnableEffect],
    ci.[Specialization] AS [Specialization],
    ci.[SpecAbsorbRating] AS [SpecAbsorbRating],
    ci.[SpecDefenseRating] AS [SpecDefenseRating],
    ci.[SpecMovementSpeed] AS [SpecMovementSpeed],
    ci.[SpecBlockRating] AS [SpecBlockRating],
    ci.[SpecAttackSpeed] AS [SpecAttackSpeed],
    ci.[SpecCritical] AS [SpecCritical],
    ci.[SpecAttackRange] AS [SpecAttackRange],
    ci.[SpecMagicMastery] AS [SpecMagicMastery],
    ci.[SpecElementalOrganic] AS [SpecElementalOrganic],
    ci.[SpecElementalEarth] AS [SpecElementalEarth],
    ci.[SpecElementalFire] AS [SpecElementalFire],
    ci.[SpecElementalIce] AS [SpecElementalIce],
    ci.[SpecElementalLightning] AS [SpecElementalLightning],
    ci.[SpecElementalPoison] AS [SpecElementalPoison],
    ci.[SpecElementalWater] AS [SpecElementalWater],
    ci.[SpecElementalWind] AS [SpecElementalWind],
    ci.[SpecElementalOrganicAtkDiv] AS [SpecElementalOrganicAtkDiv],
    ci.[SpecElementalEarthAtkDiv] AS [SpecElementalEarthAtkDiv],
    ci.[SpecElementalFireAtkDiv] AS [SpecElementalFireAtkDiv],
    ci.[SpecElementalIceAtkDiv] AS [SpecElementalIceAtkDiv],
    ci.[SpecElementalLightningAtkDiv] AS [SpecElementalLightningAtkDiv],
    ci.[SpecElementalPoisonAtkDiv] AS [SpecElementalPoisonAtkDiv],
    ci.[SpecElementalWaterAtkDiv] AS [SpecElementalWaterAtkDiv],
    ci.[SpecElementalWindAtkDiv] AS [SpecElementalWindAtkDiv],
    ci.[SpecAddMPDiv] AS [SpecAddMPDiv],
    ci.[SpecAddHPDiv] AS [SpecAddHPDiv],
    ci.[SpecAttackRatingDiv] AS [SpecAttackRatingDiv],
    ci.[SpecAttackPowerDivMin] AS [SpecAttackPowerDivMin],
    ci.[SpecAttackPowerDivMax] AS [SpecAttackPowerDivMax],
    ci.[SpecMPRegen] AS [SpecMPRegen],
    ci.[SpecHPRegen] AS [SpecHPRegen],
    ci.[SpecSPRegen] AS [SpecSPRegen],
    ci.[SpecEvade] AS [SpecEvade],
    ci.[SpecPenetration] AS [SpecPenetration],
    ci.[SpecHPPotionEfficience] AS [SpecHPPotionEfficience],
    ci.[SpecMPPotionEfficience] AS [SpecMPPotionEfficience],
    ci.[SpecSPPotionEfficience] AS [SpecSPPotionEfficience],
    ci.[CraftType] AS [CraftType],
    ci.[MixEffect] AS [MixEffect],
    ci.[AgeMixIDLevel] AS [AgeMixIDLevel],
    ci.[Maturing] AS [Maturing],
    ci.[CurrentMature] AS [CurrentMature],
    ci.[MaxMature] AS [MaxMature],
    ci.[CraftProtection1] AS [CraftProtection1],
    ci.[CraftProtection2] AS [CraftProtection2],
    ci.[SpecialItemType1] AS [SpecialItemType1],
    ci.[SpecialItemType2] AS [SpecialItemType2],
    ci.[ItemCreateTime] AS [ItemCreateTime],
    ci.[SocketType1] AS [SocketType1],
    ci.[SocketGemType1] AS [SocketGemType1],
    ci.[SocketGemItem1] AS [SocketGemItem1],
    ci.[SocketType2] AS [SocketType2],
    ci.[SocketGemType2] AS [SocketGemType2],
    ci.[SocketGemItem2] AS [SocketGemItem2],
    ci.[CostumeItem] AS [CostumeItem],
    ci.[Evade] AS [Evade],
    ci.[LockedItemType] AS [LockedItemType],
    ci.[CurrentQuestMonster] AS [CurrentQuestMonster],
    ci.[MaxQuestMonster] AS [MaxQuestMonster],
    ci.[MixTypeName] AS [MixTypeName],
    ci.[PerfectItem] AS [PerfectItem],
	ci.[ItemCountIndex] AS [ItemCountIndex],
	ci.[ItemPosition] AS [ItemPosition],
	ci.[ItemPositionX] AS [ItemPositionX],
	ci.[ItemPositionY] AS [ItemPositionY]
	FROM 
		CharacterItem ci 
	WHERE (ci.[AccountID]=?) AND  (ci.[CharacterID]=?) AND (ci.[CharacterItemDataID]=?))Query"

CSQLCharacterItemHandler::CSQLCharacterItemHandler()
{
}

CSQLCharacterItemHandler::~CSQLCharacterItemHandler()
{
}

bool CSQLCharacterItemHandler::LoadCharacterItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID )
{
	auto time_start = std::chrono::high_resolution_clock::now();

	std::vector<SQLRecordItem> vItemsCopy;
	vItemsCopy.resize( MAX_SQL_BULK_ARRAY_COUNT );

	UINT iTotalItemCount = 0;

	SQLRecordItem * pSQLRecordItem = &vItemsCopy.data()[0];

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( SELECT_CharacterItem_QUERY ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iAccountID );
			pcDB->BindParameterInput( 2, PARAMTYPE_Integer, &iCharacterID );
			pcDB->BindParameterInput( 3, PARAMTYPE_Integer, &eCharacterItemDataID );

			UINT uCurrentBind = 1;

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->iID );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.dwSize );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sItemID );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iChk1 );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iChk2 );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.dwVersion );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.dwTime );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sIntegrity.sCur );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sIntegrity.sMax );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_String, pSQLRecordItem->sRecordItem.sItem.szItemName, _countof( pSQLRecordItem->sRecordItem.sItem.szItemName ) );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_String, pSQLRecordItem->szExtraItemName, _countof( pSQLRecordItem->szExtraItemName ) );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iWeight );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iSalePrice );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iIndex );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iPotionCount );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Organic] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Earth] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Fire] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Ice] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Lighting] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Poison] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Water] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Wind] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iSight );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iChecksum3 );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sDamage.sMin );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sDamage.sMax );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iAttackRange );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iAttackSpeed );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iAttackRating );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iCritical );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fAbsorbRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iDefenseRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fBlockRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fMovementSpeed );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iPotionStorage );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fMagicMastery );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fMPRegen );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fHPRegen );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fSPRegen );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fAddHP );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fAddMP );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.fAddSP );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iLevel );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iStrength );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iSpirit );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iTalent );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iAgility );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iHealth );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sHPRecovery.sMin );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sHPRecovery.sMax );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sMPRecovery.sMin );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sMPRecovery.sMax );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSPRecovery.sMin );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSPRecovery.sMax );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iGold );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.bCanNotUse );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iBackupKey );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iBackupChk );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.iScaleBlink[0] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.iScaleBlink[1] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.iItemUniqueID );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectBlink[0] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectBlink[1] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectColor[0] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectColor[1] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectColor[2] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sEffectColor[3] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.bEnableEffect );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.eSpecialization );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecAbsorbRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecDefenseRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMovementSpeed );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecBlockRating );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackSpeed );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecCritical );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackRange );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMagicMastery );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Organic] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Earth] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Fire] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Ice] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Lighting] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Poison] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Water] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Wind] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Organic] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Earth] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Fire] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Ice] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Lighting] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Poison] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Water] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalAtkDiv[ELEMENTID_Wind] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAddMPDiv );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAddHPDiv );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackRatingDiv );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackPowerDivMin );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackPowerDiv );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMPRegen );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecHPRegen );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Float, &pSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecSPRegen );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecEvade );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecPenetration );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecHPPotionEfficience );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecMPPotionEfficience );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecSPPotionEfficience );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.eCraftType );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.eMixEffect );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sAgeLevel );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.bMaturing );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sMatureBar.sCur );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sMatureBar.sMax );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saCraftProtection[0] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.saCraftProtection[1] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecialItemType[0] );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sSpecialItemType[1] );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.tTime );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[0].eSocketType );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[0].eGemType );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[0].eGemItem );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[1].eSocketType );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[1].eGemType );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.sSocketData[1].eGemItem );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.bCostumeItem );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.bEvade );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Byte, &pSQLRecordItem->sRecordItem.sItem.bLockedItemType );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sQuestMonsters.sCur );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Short, &pSQLRecordItem->sRecordItem.sItem.sQuestMonsters.sMax );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.eMixTypeName );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItem.bPerfectItem );

			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.iItemCount );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.iItemPosition );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItemPosition.iX );
			pcDB->BindData( uCurrentBind++, PARAMTYPE_Integer, &pSQLRecordItem->sRecordItem.sItemPosition.iY );

			if ( pcDB->Execute() )
			{
				while ( pcDB->FetchEx<MAX_SQL_BULK_ARRAY_COUNT>( sizeof( SQLRecordItem ) ) )
				{
					for ( UINT i = 0; i < pcDB->GetRowsFetchedCount(); i++ )
					{
						if ( vItemsCopy[i].szExtraItemName[0] != 0 )
						{
							int iItemStrLen = STRLEN( vItemsCopy[i].sRecordItem.sItem.szItemName );
							int iExtraStrLen = STRLEN( vItemsCopy[i].szExtraItemName );

							if ( (iItemStrLen + iExtraStrLen + 1) < _countof( vItemsCopy[i].sRecordItem.sItem.szItemName ) )
							{
								STRINGCOPYLEN( vItemsCopy[i].sRecordItem.sItem.szItemName + iItemStrLen + 1, (_countof( vItemsCopy[i].sRecordItem.sItem.szItemName ) - (iItemStrLen + 1)), vItemsCopy[i].szExtraItemName );
							}
						}
					}

					vItems.resize( vItems.size() + pcDB->GetRowsFetchedCount() );

					CopyMemory( &vItems.data()[iTotalItemCount], &vItemsCopy.data()[0], sizeof( SQLRecordItem ) * pcDB->GetRowsFetchedCount() );

					iTotalItemCount += pcDB->GetRowsFetchedCount();
				}
			}
		}

		pcDB->Close();
	}

	auto time_end = std::chrono::high_resolution_clock::now();

	auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();

	WRITEERR( "SQLCharacterItemHandler::LoadCharacterItems: %I64d", time_diff );

	vItems.resize( iTotalItemCount );

	return iTotalItemCount > 0;
}

bool CSQLCharacterItemHandler::SaveCharacterItemsBulk( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID )
{
	if ( vItems.size() == 0 )
		return true;

	//Fix Extra Name
	for ( auto & sSQLRecordItem : vItems )
	{
		sSQLRecordItem.szExtraItemName[0] = 0;

		if ( sSQLRecordItem.sRecordItem.sItem.sItemID.ToItemID() == ITEMID_UnionCore )
		{
			int iItemNameLength = STRLEN( sSQLRecordItem.sRecordItem.sItem.szItemName ) + 1;
			if ( iItemNameLength < (_countof( sSQLRecordItem.sRecordItem.sItem.szItemName ) - 1) )
				STRINGCOPY( sSQLRecordItem.szExtraItemName, sSQLRecordItem.sRecordItem.sItem.szItemName + STRLEN( sSQLRecordItem.sRecordItem.sItem.szItemName ) + 1 );
		}

		sSQLRecordItem.iAccountID = iAccountID;
		sSQLRecordItem.iCharacterID = iCharacterID;
		sSQLRecordItem.eCharacterItemDataID = eCharacterItemDataID;
	}

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		UINT uTotalItemCount = vItems.size();

		std::vector<SQLRecordItem> vItemsCopy;
		vItemsCopy.resize( uTotalItemCount < MAX_SQL_BULK_ARRAY_COUNT ? uTotalItemCount : MAX_SQL_BULK_ARRAY_COUNT );
			
		auto psSQLRecordItem = &vItemsCopy[0];

		CopyMemory( psSQLRecordItem, &vItems.data()[0], sizeof( SQLRecordItem ) * vItemsCopy.size() );

		if ( pcDB->UpdateInputArraySize( vItemsCopy.size(), sizeof( SQLRecordItem ) ) )
		{
			if ( pcDB->Prepare( INSERT_CharacterItem_QUERY ) )
			{
				UINT uCurrentBind = 1;

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->iAccountID );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->iCharacterID );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->eCharacterItemDataID );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sItemID );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iChk1 );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iChk2 );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.iItemCount );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.iItemPosition );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItemPosition.iX );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItemPosition.iY );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.dwSize );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.dwVersion );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.dwTime );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sIntegrity.sCur );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sIntegrity.sMax );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_String, psSQLRecordItem->sRecordItem.sItem.szItemName, _countof( psSQLRecordItem->sRecordItem.sItem.szItemName ) );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_String, psSQLRecordItem->szExtraItemName, _countof( psSQLRecordItem->szExtraItemName ) );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iWeight );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iSalePrice );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iIndex );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iPotionCount );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Organic] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Earth] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Fire] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Ice] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Lighting] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Poison] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Water] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saElementalDef[ELEMENTID_Wind] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iSight );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iChecksum3 );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sDamage.sMin );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sDamage.sMax );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iAttackRange );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iAttackSpeed );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iAttackRating );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iCritical );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fAbsorbRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iDefenseRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fBlockRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fMovementSpeed );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iPotionStorage );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fMagicMastery );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fMPRegen );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fHPRegen );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fSPRegen );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fAddHP );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fAddMP );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.fAddSP );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iLevel );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iStrength );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iSpirit );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iTalent );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iAgility );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iHealth );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sHPRecovery.sMin );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sHPRecovery.sMax );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sMPRecovery.sMin );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sMPRecovery.sMax );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSPRecovery.sMin );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSPRecovery.sMax );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iGold );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.bCanNotUse );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iBackupKey );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iBackupChk );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iScaleBlink[0] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iScaleBlink[1] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.iItemUniqueID );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectBlink[0] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectBlink[1] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectColor[0] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectColor[1] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectColor[2] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sEffectColor[3] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.bEnableEffect );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.eSpecialization );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecAbsorbRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecDefenseRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMovementSpeed );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecBlockRating );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackSpeed );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecCritical );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackRange );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMagicMastery );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Organic] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Earth] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Fire] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Ice] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Lighting] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Poison] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Water] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Wind] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Organic] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Earth] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Fire] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Ice] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Lighting] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Poison] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Water] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.saSpecElementalDef[ELEMENTID_Wind] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAddMPDiv );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAddHPDiv );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackRatingDiv );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackPowerDivMin );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecAttackPowerDiv );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecMPRegen );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecHPRegen );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Float, &psSQLRecordItem->sRecordItem.sItem.sSpecData.fSpecSPRegen );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.sSpecData.iSpecEvade );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecPenetration );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecHPPotionEfficience );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecMPPotionEfficience );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecData.sSpecSPPotionEfficience );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.eCraftType );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.eMixEffect );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sAgeLevel );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.bMaturing );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sMatureBar.sCur );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sMatureBar.sMax );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saCraftProtection[0] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.saCraftProtection[1] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecialItemType[0] );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sSpecialItemType[1] );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.tTime );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[0].eSocketType );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[0].eGemType );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[0].eGemItem );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[1].eSocketType );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[1].eGemType );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.sSocketData[1].eGemItem );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.bCostumeItem );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.bEvade );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Byte, &psSQLRecordItem->sRecordItem.sItem.bLockedItemType );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sQuestMonsters.sCur );
				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Short, &psSQLRecordItem->sRecordItem.sItem.sQuestMonsters.sMax );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.eMixTypeName );

				pcDB->BindParameterInput( uCurrentBind++, PARAMTYPE_Integer, &psSQLRecordItem->sRecordItem.sItem.bPerfectItem );

				UINT uTotalProcessedCount = 0;

				UINT uArrayCount = uTotalItemCount - uTotalProcessedCount;
				if ( uArrayCount > MAX_SQL_BULK_ARRAY_COUNT )
					uArrayCount = MAX_SQL_BULK_ARRAY_COUNT;

				while ( uTotalProcessedCount < uTotalItemCount )
				{
					pcDB->Execute();

					uTotalProcessedCount += uArrayCount;

					uArrayCount = uTotalItemCount - uTotalProcessedCount;
					if ( uArrayCount > MAX_SQL_BULK_ARRAY_COUNT )
						uArrayCount = MAX_SQL_BULK_ARRAY_COUNT;

					if ( uArrayCount > 0 )
					{
						CopyMemory( psSQLRecordItem, &vItems.data()[uTotalProcessedCount], sizeof( SQLRecordItem ) * uArrayCount );

						if ( pcDB->UpdateInputArraySize( uArrayCount, sizeof( SQLRecordItem ) ) == FALSE )
							break;
					}
				}
			}
		}

		pcDB->Close();
	}

	return true;
}

bool CSQLCharacterItemHandler::SaveCharacterItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID )
{
	SaveCharacterItemsBulk( iAccountID, iCharacterID, vItems, eCharacterItemDataID );

	return false;
}

bool CSQLCharacterItemHandler::LoadCharacterInventoryItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vSQLRecordItems, int & iIndexStartInventoryRight )
{
	if ( LoadCharacterItems( iAccountID, iCharacterID, vSQLRecordItems, ECharacterItemDataID::CHARACTERITEMDATAID_Inventory ) )
	{
		SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
		if ( pcDB->Open() )
		{
			if ( pcDB->Prepare( "SELECT ItemRightIndex FROM CharacterInventoryInfo WHERE CharacterID=?" ) )
			{
				pcDB->AddData( PARAMTYPE_Integer, "CharacterID", iCharacterID );

				if ( pcDB->Execute() )
				{
					if ( pcDB->Fetch() )
					{
						iIndexStartInventoryRight = pcDB->GetData<int>( "ItemRightIndex" );
					}
				}
			}

			pcDB->Close();
		}

		return vSQLRecordItems.size() > 0;
	}

	return false;
}

bool CSQLCharacterItemHandler::SaveCharacterInventoryItems( const int iAccountID, const int iCharacterID, std::vector<RecordItem> & vRecordItems, const int iIndexStartInventoryRight )
{
	std::vector<SQLRecordItem> vItems;

	DeleteInventoryCharacterItems( iAccountID, iCharacterID );

	for ( auto & sRecordItem : vRecordItems )
	{
		vItems.push_back( SQLRecordItem( 0, sRecordItem ) );
	}

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		UINT iItemCount = vItems.size();
		UINT iLeftItemCount = iIndexStartInventoryRight >= 0 ? iIndexStartInventoryRight : iItemCount;
		UINT iRightItemCount = iItemCount - iLeftItemCount;

		pcDB->Clear();
		if ( pcDB->Prepare( "INSERT INTO CharacterInventoryInfo([CharacterID], [ItemCount], [ItemCountLeft], [ItemCountRight], [ItemRightIndex]) VALUES(?, ?, ?, ?, ?)" ) )
		{
			pcDB->AddData( PARAMTYPE_Integer, "CharacterID", iCharacterID );
			pcDB->AddData( PARAMTYPE_Integer, "ItemCount", iItemCount );
			pcDB->AddData( PARAMTYPE_Integer, "ItemCountLeft", iLeftItemCount );
			pcDB->AddData( PARAMTYPE_Integer, "ItemCountRight", iRightItemCount );
			pcDB->AddData( PARAMTYPE_Integer, "ItemRightIndex", iIndexStartInventoryRight );

			pcDB->Execute();
		}

		pcDB->Close();
	}

	return SaveCharacterItems( iAccountID, iCharacterID, vItems, ECharacterItemDataID::CHARACTERITEMDATAID_Inventory );
}

bool CSQLCharacterItemHandler::DeleteInventoryCharacterItems( const int iAccountID, const int iCharacterID )
{
	return DeleteCharacterItems( iAccountID, iCharacterID, ECharacterItemDataID::CHARACTERITEMDATAID_Inventory );
}

bool CSQLCharacterItemHandler::DeleteCharacterItems( const int iAccountID, const int iCharacterID, ECharacterItemDataID eCharacterItemDataID )
{
	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "DELETE FROM CharacterItem WHERE ([AccountID]=?) AND ([CharacterID]=?) AND ([CharacterItemDataID]=?)" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iAccountID );
			pcDB->BindParameterInput( 2, PARAMTYPE_Integer, &iCharacterID );
			pcDB->BindParameterInput( 3, PARAMTYPE_Integer, &eCharacterItemDataID );

			pcDB->Execute();
		}

		if ( eCharacterItemDataID == ECharacterItemDataID::CHARACTERITEMDATAID_Inventory )
		{
			pcDB->Clear();
			if ( pcDB->Prepare( "DELETE FROM CharacterInventoryInfo WHERE ([CharacterID]=?)" ) )
			{
				pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );

				pcDB->Execute();
			}
		}

		pcDB->Close();
	}

	return true;
}

bool CSQLCharacterItemHandler::SQLCharacterNameUpdate( const int iCharacterID, const std::string & strCharacterNameNew )
{
	return true;
}

SQLRecordItem::SQLRecordItem()
{
	ZeroMemory( this, sizeof( SQLRecordItem ) );
}

SQLRecordItem::SQLRecordItem( const INT64 iID, const Item & sItem )
{
	this->iID = iID;
	this->sRecordItem.sItem = sItem;
}

SQLRecordItem::SQLRecordItem( const Item & sItem )
{
	CopyMemory( &this->sRecordItem.sItem, &sItem, sizeof( Item ) );
	this->iID = 0;
}

SQLRecordItem::SQLRecordItem( const INT64 iID, const RecordItem & sRecordItem )
{
	CopyMemory( &this->sRecordItem, &sRecordItem, sizeof( RecordItem ) );
	this->iID = iID;
}
