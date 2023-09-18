#pragma once

class MapGame
{
public:
	//Constructor
											MapGame();
	virtual								   ~MapGame();

	void									Load();

	void									SetMessageBoxMap( int iMap, BOOL bUsingCore = FALSE );

	void									UpdateBlockedPosition( UnitData * pcUnitData );

    static void                             TeleportMapPosition( EMapID eMapID, int iX, int iZ );
    void                                    TeleportMapPositionHandle( EMapID eMapID, int iX, int iZ );
	
	static void								TeleportMapPositionNear( EMapID eMapID, int iX, int iZ, int * piMapX, int * piMapZ );

	static int								TeleportStartField( int * piX, int * piZ );
	static int								TeleportStartField( EMapID eMapID, int * piX, int * piZ );

	BaseMap									* GetBaseMapGame() { return pcaBaseMap; }

	BaseMap								  * pcaBaseMap;
};