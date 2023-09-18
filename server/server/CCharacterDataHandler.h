#pragma once
#include "CSQLCharacterSaveHandler.h"
#include "CSQLCharacterItemHandler.h"
#include "CSQLCharacterDataHandler.h"
#include "CSQLCharacterDropItemSaveHandler.h"

enum ECharacterDataSaveFolder
{
	CHARACTERDATASAVEFOLDER_Normal,
	CHARACTERDATASAVEFOLDER_Backup,
	CHARACTERDATASAVEFOLDER_BattleRoyale,
	CHARACTERDATASAVEFOLDER_Warehouse,
};


class CCharacterDataHandler
{
private:

	const bool USE_SQL_CHARACTER_DATA = true;

	CSQLCharacterSaveHandler *			pcSQLCharacterSaveHandler = nullptr;
	CSQLCharacterItemHandler *			pcSQLCharacterItemHandler = nullptr;
	CSQLCharacterDataHandler *			pcSQLCharacterDataHandler = nullptr;
	CSQLCharacterDropItemSaveHandler *	pcSQLCharacterDropItemSaveHandler = nullptr;

	enum EResultSaveData : int
	{
		RESULTSAVEDATA_Pending = 0,
		RESULTSAVEDATA_Success = 1,

		RESULTSAVEDATA_InvalidatedCharacter = -1,
		REUSLTSAVEDATA_LevelGreaterThanServer = -2,

		RESULTSAVEDATA_WrongPacketCount = -4,
		RESULTSAVEDATA_InvalidRecordItem = -5,
		RESULTSAVEDATA_NegativeEXP = -6,
		RESULTSAVEDATA_InvalidatedStatusPoint = -8,
		RESULTSAVEDATA_EXPHack = -9,
		RESULTSAVEDATA_InvalidatedSkillPoint = -10,
		RESULTSAVEDATA_InvalidatedWeight = -11,
		RESULTSAVEDATA_InvalidatedLevelOld = -12,
		RESULTSAVEDATA_InvalidatedEXPNotUsed = -13,
		RESULTSAVEDATA_InvalidatedClass = -14,
		RESULTSAVEDATA_InvalidCharacterNameSave = -15,

		RESULTSAVEDATA_WrongPacketCount2 = -16,
		RESULTSAVEDATA_WrongPacketCount3 = -17,
		RESULTSAVEDATA_WrongPacketCount4 = -18,
		RESULTSAVEDATA_WrongPacketCount5 = -19,
	};

public:
	CCharacterDataHandler();
	virtual ~CCharacterDataHandler();

	CSQLCharacterDataHandler *			GetSQLCharacterDataHandler() const { return pcSQLCharacterDataHandler; }
	CSQLCharacterSaveHandler *			GetSQLCharacterSaveHandler() const { return pcSQLCharacterSaveHandler; }
	CSQLCharacterItemHandler *			GetSQLCharacterItemHandler() const { return pcSQLCharacterItemHandler; }
	CSQLCharacterDropItemSaveHandler *	GetSQLCharacterDropItemSaveHandler() const { return pcSQLCharacterDropItemSaveHandler; }

	bool						IsUseSQLCharacterData() const { return USE_SQL_CHARACTER_DATA; }

	BOOL						LoadCharacterInventory( User * pcUser, PacketCharacterRecordData & sPacket );
	BOOL						LoadCharacterInventoryNew( User * pcUser, PacketCharacterRecordData & sPacket );

	BOOL						LoadCharacterDataFile( std::string strFile, PacketCharacterRecordData & sPacket );
	BOOL						LoadCharacterDataSQL( std::string strCharacterName, PacketCharacterRecordData & sPacket );
	BOOL						LoadCharacterData( std::string strCharacterName, PacketCharacterRecordData & sPacket, bool bUseSQL = true );

	void						HandlePacket( User * pcUser, PacketSelectCharacter * psPacket );

	void						HandlePacket( User * pcUser, PacketCharacterRecordDataSplit * psPacket );

	EResultSaveData				ReceiveCharacterSaveData( User * pcUser, PacketCharacterRecordDataSplit * psPacket );

	void						ProcessPacket( User * pcUser, PacketCharacterRecordData * psPacket );

	static BOOL					SetLoginKey( CharacterData * psCharacterData );

	DWORD						GetLoginKey( CharacterData * psCharacterData, DWORD dwHashName );

	static BOOL					ValidateStatusPoints( User * pcUser, CharacterData & sCharacterData, UINT iQuestFlag );
	static BOOL					ValidateSkillPoints( int iCharacterLevel, const RecordSkill & sRecordSkill, UINT iQuestFlag, int * piOut = nullptr );

	static std::string			GetCharacterDataSaveFolderFile( const std::string strCharacterName, ECharacterDataSaveFolder eFolder );
	static std::string			GetCharacterDataSaveFolder( ECharacterDataSaveFolder eFolder );

	void	 					DoImport();

private:

	BOOL						ProcessSaveData( User * pcUser, PacketCharacterRecordData & sPacket, bool bUseSQL = true );
	BOOL						ProcessSaveDataToFile( User * pcUser, PacketCharacterRecordData & sPacket );
	BOOL						ProcessSaveDataToSQL( User * pcUser, PacketCharacterRecordData & sPacket );
	BOOL						ProcessSQLSave( const int iAccountID, const int iCharacterID, PacketCharacterRecordData & sPacket );

	BOOL						SaveDropItems( UserData * pcUserData, PacketCharacterRecordData & sPacket );

	BOOL						InventoryServerCheck( User * pcUser, PacketCharacterRecordData & sPacket );
	BOOL						InventoryServerCheckNew( User * pcUser, PacketCharacterRecordData & sPacket );

	BOOL						IsValidRecordItemData( const PacketCharacterRecordData & sPacket );

	static int					GetSkillPointQuest( int iLevel, UINT iQuestFlag );
	static int					GetStatusPointQuest( User * pcUser, int iLevel, UINT iQuestFlag );

	static BOOL					SaveForceOrb( User * pcUser, PacketCharacterRecordData & sPacket );

	static BOOL					SaveServerPotion( User * pcUser, PacketCharacterRecordData & sPacket );

	static BOOL					SendBlessCastleSkill( UserData * pcUserData );

	static bool					SaveBinaryData( const std::string & strFileName, const char * pbData, UINT iSize );
};

