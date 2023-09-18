#pragma once

#include "EXEFace.h"
#include "EXEFrame.h"
#include "EXEMesh.h"
#include "EXEModel.h"
#include "EXERender.h"
#include "EXEVertex.h"
#include "EXEModelDataHandler.h"
#include "EXECharacter.h"

#define GETWORDPT(decoded, line)			((char*)CALL_WITH_ARG2(0x004780A0, (DWORD)decoded, (DWORD)line))
#define GETWORDVALUEPT(buffer, param)		((char*)CALL_WITH_ARG2(0x004780F0, (DWORD)buffer, (DWORD)param))

#define DAMAGESINGLE_COUNT					(*(int*)0x00A04764)

#define BATTLETIME							(*(DWORD*)0x00CF4764)
#define BATTLETIME_QUIT						(*(DWORD*)0x00CF4768)

#define WINDOW_GAMEDEATH					(*(BOOL*)0x035E10F8)

#define GETSERVER_SOCKET(id)				((SocketData*)CALL_WITH_ARG1(0x0061EB00, id))

#define PRECISIONTARGET_RANGE				(*(BOOL*)0x00A046AC)

#define SPARKSHIELD_MODEL					(*(PTModel**)0x00CF47EC)
#define DIVINESHIELD_MODEL					(*(PTModel**)0x00CF47F0)
#define GODLYSHIELD_MODEL					(*(PTModel**)0x00CF47F4)
#define PROJECTILE_MODEL					(*(PTModel**)0x00CE3F88)

#define BLESSCASTLE_BLUE_CROWN_MODEL					(*(PTModel**)0x00CF47FC)
#define BLESSCASTLE_GOLDEN_CROWN_MODEL					(*(PTModel**)0x00CF47F8)

#define EXETEXTUREMANAGER_DEFAULT			(*(EXETextureManager**)0x03500CE8)

#define MESSAGEBOX_SHOWALWAYS				(*(BOOL*)0x00A17598)

#define ACCOUNT_NAME_MAX_LENGTH				32
#define ACCOUNT_NAME						((char*)0x039033E8)

#define NIGHT_MODE							(*(BOOL*)0x00CF475C)

#define ETHERCORE_USE_ITEMID				((EItemID)READDWORD(0x00CF47BC))
#define SET_ETHERCORE_USE_ITEMID			(*(DWORD*)0x00CF47BC)

#define ITEMDATA_USED_ITEM					(*(ItemData**)0x035EB670)

#define RESTART_BUTTON_ID					(*(int*)0x035E1114)

#define REVIVE_TIME							(*(int*)0x0209EA4C)

#define PACKET_VIRTUAL_LIFE					(*(PacketSimple**)0x03A976B8)

#define ENABLE_SKILL_LOOP					(*(BOOL*)0x00CF48B0)
#define AUTOMATIC_SKILL_ATTACK				(*(BOOL*)0x00CF48AC)

#define SELECTED_SKILL_MOUSE_INDEX			(*(int*)0x00CF4898)

#define GAMECONFIG_MAPLIGHT_VECTOR			((Point3D*)0x00CDE130)

#define GAMECONFIG_MAPLIGHT_BRIGHT			(*(int*)0x00CDE128)
#define GAMECONFIG_MAPLIGHT_CONTRAST		(*(int*)0x00CDE12C)

#define XOREXP_LOW 							(*(int*)0x0209EA24)
#define XOREXP_HIGH 						(*(int*)0x0209EA28)
#define XORHP_NUM							(*(int*)0x0209EA2C)

struct EXESurfaceOld
{
	int						iReferenceCount;
	EXETextureHandle		sTexture;
	void					* psCache;
};


enum EClanStatusInfo
{
	CLANSTATUSINFO_Disabled,
	CLANSTATUSINFO_Updating,
	CLANSTATUSINFO_Done,
	CLANSTATUSINFO_Error,
};

enum RestartButtonID
{
	RESTARTBUTTONID_None,
	RESTARTBUTTONID_Field,
	RESTARTBUTTONID_Town,
	RESTARTBUTTONID_Exit,
};

struct ClanInfo
{
	ClanInfoHeader			sHeader;

	EXESurfaceOld			* psTexture16;
	EXESurfaceOld			* psTexture32;

	DWORD					dwTime;
	int						iStatus;   //0: disabled, 1: updating info..., 2: done!, 3: Error
	int						iSaveNum;
};

struct ClanGameData
{
	int  iPosition; //?

	int	 iGold;
	int  iAbility;






	char clanJoinID[64];
	char clanJoinCharID[64];
	int  clanJoinJob, clanJoinLevel;

	char chipCharID[64];


	char  szJoinClanName[64];
	char  szJoinClanChipName[64];



	char szName[20];
	char explanation[128];


	char stats[2];
	char sec[30];
	char szRegiD[50];
	char  limitD[30];
	char  DelActive[10];
	char  gserver[30];
	char  szID[64];
	char  szCharacterName[64];
	char  ClanZang_ID[64];
	char  ClanZang_Name[64];
	char  ClanMark[64];

	int intClanMark;
	int nLastDay;
	BOOL bIsOneDay;
	int	nMinute;
	int	nHour;

	int nCurrentClanWon_Num;
	int nClanWon_Max;







	char  clanWon[110][30];
	int	 clanWonStrLen[110];
	char  szClanMarkName[50];
	int   nTotalClanWonCount;
	char  PFlag[5];
	char  LeaveClanFlag[5];
	int	 nKFlag;
	int   nKFlag_Count;
	char  OldChipChaName[64];
	char  SubChip[64];
	char	 OldSubChip[64];
	char	 isSubChip[2];


	UnitData * pcTarget;
	
	EXESurfaceOld	* psClanIcon;
	EXESurfaceOld	* psClanIcon16;

	char  StartMessageFlag[1];

	DWORD CNFlag;
};

struct EXERenderCacheData
{
	EXERender		cRender;
	PTMesh			cMesh;
};

struct EXEStageModel
{
	EXEModel				* pcModelBone;
	EXEModel				* pcModel;

	Point3D					sPosition;
	Point3D					sAngle;

	EXEStageModel			* psParent;

	int						iSum;

	DWORD					dwTimeUpdate;
};

static const DWORD							pdwSkillBuff = 0x03693AD8;

IMPFNC										pfnSetDynamicLight		= 0x00450D60;
IMPFNC										pfnSetCameraCharacter	= 0x0041E9C0;
IMPFNC										pfnSetCastleMaster		= 0x004292B0;
IMPFNC										pfnGetDamageFromArea	= 0x0040FB40;
IMPFNC										pfnRenderDropItemView	= 0x00428FB0;
IMPFNC										pfnSetupCamera			= 0x004019C0;
IMPFNC										pfnSetupCameraMatrix	= 0x0048B540;
IMPFNC										pfnSetDynamicLights		= 0x0040C050;
IMPFNC										pfnRenderSky			= 0x005AA330;
IMPFNC										pfnRenderStage			= 0x00450EB0;
IMPFNC										pfnRender3DAlpha		= 0x00454210;
IMPFNC										pfnRenderModelShadow	= 0x004548A0;
IMPFNC										pfnUpdateUITextures		= 0x00417EF0;
IMPFNC										pfnRenderEffects		= 0x005769F0;
IMPFNC										pfnRender3DEngine		= 0x00525C00;
IMPFNC										pfnRenderModel2D		= 0x00454950;
IMPFNC										pfnRenderZoomMap		= 0x0040B780;
IMPFNC										pfnFakeRenderGameState	= 0x00417FE0;
IMPFNC										pfnRenderGameState		= 0x004181D0;
IMPFNC										pfnUpdateMovementCamera = 0x004168A0;
IMPFNC										SetDynamicLight2		= 0x0040C080;
IMPFNC										pfnClearRenderDropView	= 0x00428EF0;

class EXERenderCache
{
protected:
	static std::vector<EXERenderCacheData*>  vRender;


public:
	static void								FinalRender();
	static void								FinalRenderHandler( PTMesh * pcThis );
	static void								RenderCacheData();
};

static BOOL									AddUnitQueueID( int iID );

void										RenderDropItem( DropItemView * ps );
void										ClearRenderDropView( DropItemView * ps );

void										RenderCharacterHP( UnitData * pcUnitData, int iX, int iY );

void										RenderShadowMap();

void										RenderCharacterChangingBar( UnitData * pcUnitData, int iX, int iY, int iMin, int iMax );

BOOL										Render3D( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

BOOL										Render3DAlpha();

void WINAPI									Render( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

BOOL										IsCreatedNewRenderTarget();

static void									SetStageDynamicLight( Stage * pcStage, int iX, int iY, int iZ );

void										SetCharacterCamera( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

static void									SetClanMaster( DWORD dwID );

static int									GetDamageFromAreaSkill( UnitData * pcUnitData, UnitData * pcTarget, int iDestX, int iDestY, int iDestZ );

int											SetupCamera( struct EXECameraTrace * psCamera, int * iX, int * iY, int * iZ );

int											SetupCameraMatrix( EXECameraTrace * psCamera, int iX, int iY, int iZ, int iDX, int iDY, int iDZ );

void										RenderSky( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

int											RenderStage( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

int											RenderModelShadow( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );
void										UpdateUITextures();

void										RenderEffects( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

void										RenderModel2D( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

void										Render3DEngine( int iX, int iY, int iZ, int iAX, int iAY, int iAZ );

void										RenderZoomMap();

void										PrepareUnitDataHUD( UnitData * p );
void										RenderUnitDataHUD();

void										UpdateMovementCamera();

void										FakeRenderGameState();
BOOL										RenderGameState();

void										SetDynamicLights();
BOOL __cdecl								SetDynamicLight( int iX, int iY, int iZ, int iR, int iG, int iB, int iA, int iRange );
BOOL __cdecl								SetDynamicLight( int iType, int iX, int iY, int iZ, int iR, int iG, int iB, int iA, int iRange );

int __stdcall								RenderStageMesh( EXEStageModel * psStageModel );

void										DrawMessageMiddle( const char * pszText );
void										DrawCharacterMessage( int iX, int iY, char * pszText, int iLineSize, DWORD dwClan, char * pszClanName, DWORD dwColor, BOOL bSelected, int iBellatra );

BOOL										LoadUnitData( UnitData * pcUnitData, const char * pszBodyModel, const char * pszHeadModel, Point3D * psPosition, Point3D * psAngle );

int											DrawGameInterface3D();

BOOL										RenderUnitData( UnitData * pcUnitData, bool bRenderUnitDataBody, bool bRenderUnitDataHead, bool bRenderUnitDataWeapon );

BOOL										ClientUnitDataUpdate( UnitData * pcUnitData );

BOOL __cdecl								SendSingleDamageUnit( UnitData * pcUnitData, int iDamageMin, int iDamageMax, int iState, int iResistance, int iSkillID, BOOL bChance, SocketData * pcSocketData );
BOOL __cdecl								SendAreaDamageUnit( int iX, int iY, int iZ, UnitData * pcUnitData, int iDamageMin, int iDamageMax, int iState, int iResistance, int iSkillID );


BOOL __cdecl								FreeUnitData( UnitData * pcUnitData );

int	__cdecl 								SaveCharacterFrames( UnitData * pcUnitData );
int	__cdecl									SaveCharacterFramesEx( UnitData * pcUnitData );

BOOL __cdecl								DeleteEXEModelData( EXEModelData * psModelData );

void										RenderBlessCastleCrown( UnitData * pcUnitData );

void										UpdateEquippedItems( UnitData * pcUnitData, short saNewEquippedItems[4] );

#define CAMERACHARACTER						(SetCharacterCamera)

#define RENDERDISTANCE_LIMIT				(*(BOOL*)0x00CF48C4)

static std::string ws2s( const std::wstring & wstr )
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes( wstr );
}


static void GetMoveLocation( int iX, int iY, int iZ, int iAX, int iAY, int iAZ )
{
	CALL_WITH_ARG6( 0x0048AF40, iX, iY, iZ, iAX, iAY, iAZ );
}

NAKED static void StartGlobalEffect( int iX, int iY, int iZ, int iWidth, int iHeight, int iEffectID )
{
    JMP( 0x00574190 );
}

void RenderPTModel( PTModel * pcModel, X3D::Vector3 sPosition, X3D::Vector3 sAngle, X3D::Vector3 sCameraPosition = X3D::Vector3( 0,0,0 ), X3D::Vector3 sCameraAngle = X3D::Vector3( 0, 0, 0 ) );

void OnDrawLightImage( Stage * pcStage, int iX, int iY, int iZ );

BOOL __stdcall SetBoneMeshes( EXEModel * pcModel );

void __cdecl MainUnitDataSelf( UnitData * pcUnitData );

Point3D GetPolygonAngle( Point3D sPoint1, Point3D sPoint2, Point3D sPoint3 );

BOOL __cdecl TraceAttackPlayGate();