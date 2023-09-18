#pragma once

#define PKT_DATAUNIT_SZ 8096

static const int FREE_EXP_EVENT_LEVEL = 145;

#define MOVELOCATION_X  (*(int*)0x077BFC54)
#define MOVELOCATION_Y  (*(int*)0x077BFC58)
#define MOVELOCATION_Z  (*(int*)0x077BFC5C)

//Packet Header
struct SPlayDataGroup : Packet
{
	int		iDataCount;
	DWORD	dwCodeMask;
};

class UnitServer
{
private:
	IMPFNC										pfnGetTop10DamageUnitData	= 0x0055A3D0;
	IMPFNC										pfnUnitSwapper				= 0x0054FEA0;
	IMPFNC										pfnUnitDamageSkill			= 0x0054FAB0;
	IMPFNC										pfnUnitDataByIDMap			= 0x0054CCA0;


	UnitData									* pcaUnitData		= NULL;
	Unit										* pcaUnit			= NULL;

public:

	enum EUnitFollowType : int
	{
		UNITFOLLOWTYPE_DontFollow				= 0,
		UNITFOLLOWTYPE_Die						= 1,
		UNITFOLLOWTYPE_Follow					= 2,
	};

	Unit										** pcaUnitInGame = NULL;
	UINT										uUnitsInGame = 0;

	enum
	{
		MOTIONID_Death							= 0x120,
		MOTIONID_Skill							= 0x150,
	};

	UnitServer();

	UnitData *									GetUnit( UINT uEnemyID );

	static UnitData								* UnitDataByIDMap( int iID, int iMapID );
	Unit										* UnitDataToUnit( UnitData * pcUnitData );

	Unit *										GetUnitByExclusiveUser( UnitData * pcUnitData );

	struct CharacterData						* GetCharacterDataByEffect( DWORD dwEffect, int iClass = 0 );
	struct CharacterData						* GetCharacterDataByEffectLevel( DWORD dwEffect, int iLevel );
	struct CharacterData						* GetCharacterDataByName( const char * pszName );

	struct UnitInfo								* GetUnitInfoByName( const char * pszName );

	int											GetArrayPosition( UnitData * pcUnitData );

	INT64										GetExpLevelDifference( int iLevel1, int iLevel2, INT64 iExp );

	INT64										GetExp( UnitData * pcUnitData );
	INT64										GetExp( const CharacterData * psCharacterData );

	BOOL										SetMotionFromCode( UnitData * pcUnitData, int iMotionCode );
	void										SendBossMessage( BaseMap * pcBaseMap, UINT pUnit );
	
	BOOL										OnUnitNpcClick( UserData * pcUserData, Unit * pcUnit );
	void										OnSendExp( UserData * pcUserData, UnitData * pcUnitData );
	void										OnSetDrop( UserData * pcUserData, UnitData * pcUnitData );

	void										OnSetTargetPosition( UserData * pcUserData, UnitData * pcUnitData );

	BOOL										IsMultiplyHPMonter( UnitData * pcUnitData );

	static BOOL									GetTop10Damage( UnitData * pcUnitData, AttackDamageData* psaDamageData );

	BOOL										HandleItemDrop( UserData * pcUserData, UnitData * pcUnitData, Map::Loot::LootItem * psItemLoot );

	BOOL										HandleKill( UnitData * pcUnitData, UserData * pcUserData );

	void										MainUnitData( UnitData * pcUnitData );

	static void									SkillUnitDamage( UnitData * pcUnitData, UserData * pcUserData );

	static void									UnitSwapper( UnitData * pcUnitData, CharacterData * psCharacterData, UnitInfo * psUnitInfo );

	BOOL										OnUnitChanger( UnitData * pcUnitData );

	BOOL										UpdateUnitData( UnitData * pcUnitData );

	void										UpdateCharacterData( User* pcUser, UnitData * pcUnitData );

	void										UpdateUnit( Unit * pcUnit );

	void										Update();

	EUnitFollowType								GetUnitFollowType( UnitData * pcUnitData );

	BOOL										MainServerUnitData( UnitData * pcUnitData );

	AttackDamageData *							GetUserInDamageList( UnitData * pcUnitData, User * pcUser );

	int											GetTotalHP( UnitData * pcUnitData );

	void										ProcessPacketKillUnitID( User * pcUser, int iID, int iMapID );

	void										OnHandleUnitDataBufferNew( Unit * pcUnit, PacketPlayData * psPacket );

	BOOL										HandleUnitDamageTop( UserData * pcUserData, UnitData * pcUnitData, int iDamage );

	int											GetUnitDataHPRegen( UnitData * pcUnitData );

	int											GetUnitDataSpawnFlagCount( UnitData * pcUnitData );

	static void									GetMoveLocation( int iX, int iY, int iZ, int iAngleX, int iAngleY, int iAngleZ );

	static UnitData *							CreateMonsterFromSkill( int iSkillID, int iX, int iY, int iZ, UserData * pcUserData, UnitData * pcUnitDataTarget );

private:
	BOOL										IsSafeStuckUnit( UnitData * pcUnitData );

	BOOL										MainUnitMonsterData( UnitData * pcUnitData );

	BOOL										MainUnitNPCData( UnitData * pcUnitData );

	void										ProcessQuestPacket( UserData * pcUserData, UnitData * pcUnitData, QuestCodeSkillUnit iQuestCode, int iQuestParam );

	void										SQLSaveItemRare( User * pcUser, Unit * pcUnit, Item * psItem );

	void										OnBeforeCrystalKill( UnitData * pcUnitData );

	UnitData									* DoDamageAreaSkillOnUnits( UnitData * pcUnitData );

	UnitData									* DoRemoveStunAreaSkillOnUnits( UnitData * pcUnitData, int iRange );

	UnitData									* GetUnitDataAround( UnitData * pcUnitData, int iDistance );

	void										SaveFrames( UnitData * pcUnitData );
	void										SaveFramesNPC( UnitData * pcUnitData );

	int											DefendChance( int iDefenseRating, int iAttackRating );

	int											GetDistanceDouble( int iX1, int iZ1, int iX2, int iZ2 );



};

