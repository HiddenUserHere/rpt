#pragma once

enum EStarterTeleportMapInfoType
{
	TELEPORTINFOTYPE_StarterTown,
	TELEPORTINFOTYPE_StarterEXP,
	TELEPORTINFOTYPE_StarterHunt,
	TELEPORTINFOTYPE_StarterPVP,
	TELEPORTINFOTYPE_StarterSolo,
	TELEPORTINFOTYPE_StarterMana,
};

struct StarterTeleportMapInfo
{
	BOOL					bEnabled;

	EMapID					iMapID;

	ETeleportMapInfoType	iMapTypeFirst;
	ETeleportMapInfoType	iMapTypeSecond;
};


static const StarterTeleportMapInfo saStarterTeleportMapInfo[] =
{
	{ TRUE, MAPID_RicartenTown, TELEPORTINFOTYPE_Town, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_PhillaiTown, TELEPORTINFOTYPE_Town, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_NaviskoTown, TELEPORTINFOTYPE_Town, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_DarkSanctuary, TELEPORTINFOTYPE_EXP, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_CursedTempleF1, TELEPORTINFOTYPE_EXP, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_CursedTempleF2, TELEPORTINFOTYPE_EXP, TELEPORTINFOTYPE_Town },
	{ TRUE, MAPID_RailwayOfChaos, TELEPORTINFOTYPE_EXP, TELEPORTINFOTYPE_Town },
};

class CStarterCoreHandler
{
public:
	CStarterCoreHandler();
	virtual ~CStarterCoreHandler();

	void					Init();

	BOOL							OnMouseClick( class CMouse * pcMouse );
	void							OnMouseMove( class CMouse * pcMouse );
	BOOL							OnMouseScroll( class CMouse * pcMouse );

	void							OnResolutionChanged();

	void							Render();

	void							Update( float fTime );

	void							Open( ItemData * pcItemData );
	void							Close();

	BOOL							IsOpen();

private:

	enum
	{
		WINDOWID_Main,
		IMAGEID_Minimap,
		IMAGEID_MapTown,
		IMAGEID_MapEXP,
		IMAGEID_MapHunt,
		IMAGEID_MapPVP,
		IMAGEID_MapSolo,
		IMAGEID_MapMana,
		TEXTID_MapName,
		TEXTID_MapType,
		TEXTID_MapLevelText,
		TEXTID_MapLevel,

		BUTTONID_Teleport,

		LISTID_Map,
		WINDOWID_Map = 100,

		TEXTID_MapNameList	= 500,
		TEXTID_MapLevelList = 800,
	};

	void					BuildWindow();

	void					OnClickMap( int iMapID, int iIndex, UIEventArgs eArgs );
	void					OnClickTeleport( UIEventArgs eArgs );
	void					OnAcceptTeleport( int iMapID, UIEventArgs eArgs );

	void					OnButtonCloseClick( UIEventArgs eArgs );

	int						GetMapTypeImageID( ETeleportMapInfoType iType );
	const char				* GetMapTypeImageFile( ETeleportMapInfoType iType );

	UI::Window_ptr			pWindow;

	EItemID					iItemID = ITEMID_None;
	int						iChk1 = 0;
	int						iChk2 = 0;

	int						iMapID = -1;
};

