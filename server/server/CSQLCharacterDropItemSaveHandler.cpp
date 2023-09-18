#include "stdafx.h"
#include "CSQLCharacterDropItemSaveHandler.h"

CSQLCharacterDropItemSaveHandler::CSQLCharacterDropItemSaveHandler()
{
}

CSQLCharacterDropItemSaveHandler::~CSQLCharacterDropItemSaveHandler()
{
}

bool CSQLCharacterDropItemSaveHandler::LoadCharacterDropItem( const int iCharacterID, std::vector<DropItemData> & vDropItems )
{
	DropItemData sDropItemData;

	vDropItems.clear();

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "SELECT * FROM CharacterDropItemSave WHERE CharacterID=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );

			if ( pcDB->Execute() )
			{
				while ( pcDB->Fetch() )
				{
					sDropItemData.iItemID = pcDB->GetData<int>( "ItemID" );
					sDropItemData.iChk1 = pcDB->GetData<int>( "Code1" );
					sDropItemData.iChk2 = pcDB->GetData<int>( "Code2" );

					vDropItems.push_back( sDropItemData );
				}
			}
		}

		pcDB->Close();
	}

	return vDropItems.size() > 0;
}

bool CSQLCharacterDropItemSaveHandler::SaveCharacterDropItem( const int iCharacterID, const std::vector<DropItemData> & vDropItems )
{
	DeleteCharacterDropItem( iCharacterID );

	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		for ( auto & sDropItemData : vDropItems )
		{
			pcDB->Clear();

			if ( pcDB->Prepare( "INSERT INTO CharacterDropItemSave (CharacterID, ItemID, Code1, Code2) VALUES (?, ?, ?, ?)" ) )
			{
				pcDB->AddData( PARAMTYPE_Integer, "CharacterID", iCharacterID );
				pcDB->AddData( PARAMTYPE_Integer, "ItemID", sDropItemData.iItemID );
				pcDB->AddData( PARAMTYPE_Integer, "Code1", sDropItemData.iChk1 );
				pcDB->AddData( PARAMTYPE_Integer, "Code2", sDropItemData.iChk2 );

				pcDB->Execute();
			}
		}

		pcDB->Close();
	}

	return true;
}

bool CSQLCharacterDropItemSaveHandler::DeleteCharacterDropItem( const int iCharacterID )
{
	SQLConnection * pcDB = SQLCONNECTION( DATABASEID_UserDB );
	if ( pcDB->Open() )
	{
		if ( pcDB->Prepare( "DELETE FROM CharacterDropItemSave WHERE CharacterID=?" ) )
		{
			pcDB->BindParameterInput( 1, PARAMTYPE_Integer, &iCharacterID );

			pcDB->Execute();
		}

		pcDB->Close();
	}

	return true;
}
