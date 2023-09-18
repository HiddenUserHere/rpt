#pragma once

enum ECharacterItemDataID : int
{
	CHARACTERITEMDATAID_Inventory = 1,
	CHARACTERITEMDATAID_Warehouse = 2,
	CHARACTERITEMDATAID_Caravan = 3,
	CHARACTERITEMDATAID_PersonalShopOffline = 4,
	CHARACTERITEMDATAID_ItemBox = 5,
};

struct SQLRecordItem
{
	INT64			iID;

	int				iAccountID;

	int				iCharacterID;

	ECharacterItemDataID eCharacterItemDataID;

	RecordItem		sRecordItem;

	char			szExtraItemName[32];

	SQLRecordItem();

	SQLRecordItem( const INT64 iID, const Item & sItem );

	SQLRecordItem( const Item & sItem );

	SQLRecordItem( const INT64 iID, const RecordItem & sRecordItem );

	~SQLRecordItem()
	{
	};

};

class CSQLCharacterItemHandler
{
public:
	CSQLCharacterItemHandler();
	~CSQLCharacterItemHandler();

	bool					LoadCharacterItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID );

	bool 					SaveCharacterItemsBulk( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID );
	bool 					SaveCharacterItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vItems, ECharacterItemDataID eCharacterItemDataID );

	bool 					LoadCharacterInventoryItems( const int iAccountID, const int iCharacterID, std::vector<SQLRecordItem> & vRecordItems, int & iIndexStartInventoryRight );
	bool 					SaveCharacterInventoryItems( const int iAccountID, const int iCharacterID, std::vector<RecordItem> & vRecordItems, const int iIndexStartInventoryRight );

	bool					DeleteInventoryCharacterItems( const int iAccountID, const int iCharacterID );
	bool					DeleteCharacterItems( const int iAccountID, const int iCharacterID, ECharacterItemDataID eCharacterItemDataID );

	bool 					SQLCharacterNameUpdate( const int iCharacterID, const std::string & strCharacterNameNew );
};

