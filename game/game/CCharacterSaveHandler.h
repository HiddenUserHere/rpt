#pragma once
class CCharacterSaveHandler
{
public:
	CCharacterSaveHandler();

	~CCharacterSaveHandler();

	void					HandlePacket( PacketCharacterRecordDataSplit * psPacket );

	static BOOL				ResetPotionSave();

	static BOOL				ValidateSkillInfo();

	void					ProcessPacket();

private:
	CCharacterSaveData    * pcCharacterSaveData = nullptr;

	bool					HandlePacketBlock( PacketCharacterRecordDataSplit * psPacket );

	bool					ProcessPacket( PacketCharacterRecordData * psPacket );

	bool 				    HandleData( CharacterData & sCharacterData, ItemData * pcaItemDataLeft, ItemData * pcaItemDataRight, ItemData * pcItemDataMouse = nullptr );
	bool 				    ProcessData( CharacterData & sCharacterData, ItemData * pcaItemDataLeft, ItemData * pcaItemDataRight, ItemData * pcItemDataMouse = nullptr );

	bool					ProcessSaveData( CharacterSave & sCharacterSave );

	bool					HandleSaveData( CharacterSave & sCharacterSave );

	static BOOL				ResetSavePotions( CharacterSave * psCharacterSave );

	void					SendHackUserPacket( int iCode, int iParam = 0 );
	void					SendItemCopyHackUserPacket( int iItemCount );

	static int	 			AddPotionRecord( EItemID iItemID, int iPotionCount );

	static BOOL				LoadOldQuests( int iQuestID, DWORD * pdwBufferQuest );

	static BOOL				UpdateQuestsBoard();

	static int				GetQuestCodeBuffer( DWORD * pdwBufferQuest );

	bool					ProcessRecordSkill( RecordSkill & sRecordSkill );

	static BOOL				HandleRecordSkill( RecordSkill * psRecordSkill, DWORD dwQuestFlag );

	static BOOL				SetForceOrb( int iItemID, int iTime );
};

