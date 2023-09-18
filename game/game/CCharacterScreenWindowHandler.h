#pragma once

#include "CCharacterRaceWindowHandler.h"
#include "CCharacterScreenNewHandler.h"

class CCharacterScreenWindowHandler
{
public:
	CCharacterScreenWindowHandler();
	virtual ~CCharacterScreenWindowHandler();

	void							Init();

	void							Render();

	void							Update( float fTime );

	BOOL							OnMouseClick( CMouse * pcMouse );

	void							OnMouseMove( CMouse * pcMouse );

	BOOL							OnKeyPress( CKeyboard * pcKeyboard );

	BOOL							OnKeyChar( CKeyboard * pcKeyboard );

	void							OnResolutionChanged();

	void							UpdateCharacterInfo( const CharacterScreenCharacterData & sCharacterData );

	void							HideCharacterInfo();

	void							OnCharacterStep( ECharacterScreenStep eStep );

	void							UpdateDeleteButton( bool bShow );

	void							SetTemporaryBlockActions( float fDuration );

	void							RemoveTemporaryBlockActions() { fTemporaryBlockActions = 0.0f; }

	const std::string				GetCharacterInitialBodyPath( ECharacterClass eCharacterClass );

private:

	friend class CCharacterRaceWindowHandler;
	friend class CCharacterScreenNewHandler;
	friend class CCharacterScreenModel;

	enum
	{
		WINDOW_Main,
		WINDOW_Bottom,
		BUTTON_NewCharacter,
		BUTTON_Create,
		BUTTON_Select,
		BUTTON_Back,
		BUTTON_Delete,
		BUTTON_Exit,

		RECTBOX_Bottom,

		TEXT_CharacterInfoLeft = 300,
		TEXT_CharacterInfoRight = 400,

		IMAGE_MainboxLeft = 500,
		IMAGE_MainboxRight,
		IMAGE_MainboxTop,
		IMAGE_MainboxBottom,

		IMAGE_MainboxTopLeft,
		IMAGE_MainboxTopRight,
		IMAGE_MainboxBottomLeft,
		IMAGE_MainboxBottomRight,

		IMAGE_MainboxBackgroundTop,
		IMAGE_MainboxBackgroundBottom,

		WINDOW_Titlebox,
		IMAGE_TitleboxSelectChar,
		IMAGE_TitleboxSelectTribe,
		IMAGE_TitleboxSelectClass,
		IMAGE_TitleboxSelectFace,

		WINDOW_CharacterInfo,
		IMAGE_CharacterClan,
		TEXT_CharacterClanName,
		TEXT_CharacterName,
		TEXT_CharacterLevel,
		TEXT_CharacterTribe,
		TEXT_CharacterClass,
	};

	UI::Image									* pcCharacterInfoImageNoClan;
	UI::Image									* pcCharacterInfoImageClan;

	struct CharacterInfoText
	{
		std::string strText;

		DWORD		dwColor;

		BOOL		bBold;
	};


	std::unordered_map<int, UI::Image *>		mCharacterScreenImages;
	

	UI::Window_ptr					pWindow = nullptr;

	std::vector<CharacterInfoText>	vTextLeft;
	std::vector<CharacterInfoText>	vTextRight;

	CCharacterRaceWindowHandler * pcCharacterRaceWindowHandler = nullptr;
	CCharacterScreenNewHandler	* pcCharacterScreenNewHandler = nullptr;

	float							fTemporaryBlockActions = 0.0f;

	bool							IsBlockedActions() { return fTemporaryBlockActions > 0.0f; }

	void							ClearText();
	void							AddString( const std::string strText, BOOL bBold, DX::Color cColor, bool bLeft );

	void							BuildCharacterText();

	void							OnButtonNewCharacterClick( UIEventArgs eArgs );

	void							OnButtonSelectClick( UIEventArgs eArgs );
	void							OnButtonBackClick( UIEventArgs eArgs );
	void							OnButtonDeleteClick( UIEventArgs eArgs );
	void							OnButtonCreateClick( UIEventArgs eArgs );
	void							OnButtonExitClick( UIEventArgs eArgs );

	void							UpdateWindowPosition();

	class CCharacterScreen			* GetCharacterScreen();

	void							UpdateButtonsMainbox( bool bShowCreateButton, bool bShowSelectButton, bool bShowBackButton, bool bShowExitButton );

	void							UpdateButtonsPosition();

	std::string						GetImagePath( const std::string & strImageName );

	void							ShowTitle( int iTitleID );

	void							OnDeleteButtonConfirm();
};

