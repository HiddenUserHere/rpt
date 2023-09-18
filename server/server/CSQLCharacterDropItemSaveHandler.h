#pragma once
class CSQLCharacterDropItemSaveHandler
{
public:
	CSQLCharacterDropItemSaveHandler();
	~CSQLCharacterDropItemSaveHandler();

public:
	bool                           LoadCharacterDropItem( const int iCharacterID, std::vector<DropItemData> & vDropItems );

	bool                           SaveCharacterDropItem( const int iCharacterID, const std::vector<DropItemData> & vDropItems );

bool							   DeleteCharacterDropItem( const int iCharacterID );
};

