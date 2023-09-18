#pragma once

#include "Screen.h"

#include "CCharacterScreenModel.h"
#include "CCharacterScreenWindowHandler.h"

#define	CHARACTER_SELECT_NAME			((char*)0x00CF4828)	//Size = 32
#define	CHARACTER_SELECT_BODYMODEL		((char*)0x038FED40) //Size = 64
#define	CHARACTER_SELECT_HEADMODEL		((char*)0x038FEE40) //Size = 64
#define	CHARACTER_SELECT_CLASS			(*(int*)0x00CF4880)
#define	CHARACTER_SELECT_MAPID			(*(int*)0x009C474C)
#define	CHARACTER_SELECT_X				(*(int*)0x03903290)
#define	CHARACTER_SELECT_Z				(*(int*)0x03903294)

struct NewCharacterData
{
	ECharacterClass			iClass;
	int						iStrength;
	int						iSpirit;
	int						iTalent;
	int						iAgility;
	int						iHealth;
};

#define	NEW_CHARACTER_DATA				((NewCharacterData*)0x03903278)

class CCharacterScreen : public CScreen
{
public:
						CCharacterScreen();
						~CCharacterScreen();

	BOOL				Init();
	void				Shutdown();

	void				Render3D();
	void				Render2D();

	EScreen				Frame();

	void				OnMouseMove( class CMouse * pcMouse );
	BOOL				OnMouseClick( class CMouse * pcMouse );

	BOOL				OnKeyPress( CKeyboard * pcKeyboard );
	BOOL				OnKeyChar( CKeyboard * pcKeyboard );

	void				OnResolutionChanged();

	void				SetScreenHalf( int iWidth ) { iScreenHalfWidth = iWidth; }

	ECharacterScreenStep		GetStep() { return pcCharacterScreenModel->GetStep(); }
	ECharacterScreenStep		GetNewStep() { return pcCharacterScreenModel->GetNewStep(); }
	ECharacterScreenStep		GetOldStep() { return pcCharacterScreenModel->GetOldStep(); }

	bool				CanBackLoginScreen() { return pcCharacterScreenModel->CanBackLoginScreen(); }


	void				UpdateScreen( EScreen iScreen );

	static PacketCharacterSelectUser	sPacketCharactersUser;
	static void			HandlePacket( PacketCharacterSelectUser * psPacket );

	CharacterSelectPacketData & GetCharacterData( const char * pszName );

	void				SetCharacterScreenStep( ECharacterScreenStep iStep ) { pcCharacterScreenModel->SetStep( iStep ); pcWindow->OnCharacterStep( iStep ); }

	CCharacterScreenWindowHandler * GetWindow() { return pcWindow; }
	
	CCharacterScreenModel * GetModel() { return pcCharacterScreenModel; }

	void				SelectCharacter( bool bLock );

	void				CreateNewCharacter( UnitData * pcUnitData );

	void				HandlePacket( PacketCreateCharacter * psPacket );

private:

	//char				cPadding001[0x01C4];

	int					iScreenHalfWidth;			//0x1C4

	//char				cPadding002[0x036004];

	int					iTexturePosition;

	BOOL				bLoadingTime = FALSE;

	DWORD				dwLoadingTime = 0;

	CCharacterScreenModel * pcCharacterScreenModel = NULL;

	CCharacterScreenWindowHandler * pcWindow;

	EScreen				iCurrentScreen;

	void				OnSelectedCharacterBox();

	void				SetCharacterUser( CharacterSelectPacketData & sData );

	void				Disconnect();

}; //Size = 0x361CC

