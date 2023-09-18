#pragma once

struct RecordStatusCharacterBuffer
{
	int					iID;

	int					iStatusCount;
};

class CCharacterStatusWindowHandler
{
public:
	CCharacterStatusWindowHandler();
	~CCharacterStatusWindowHandler();

	void						Init();

	void						Render();

	void						Update( float fTime );

	void						RenderText( int iBaseX, int iBaseY );

	void						RenderStatusWindow( int iBaseX, int iBaseY );

	BOOL						OnMouseClick( CMouse * pcMouse );
	void						OnMouseMove( CMouse * pcMouse );

	void						UpdateRequiredStatus();

private:

	static const UINT			MAX_STATUS_RECORD_BUFFER = 10;
	RecordStatusCharacterBuffer	saStatusRecordBuffer[MAX_STATUS_RECORD_BUFFER];

	enum EStatusCounter : int
	{
		STATUSCOUNTER_One,
		STATUSCOUNTER_Ten,
		STATUSCOUNTER_Hundred,
	};
	
	EStatusCounter				eCurrentStatusCounter = STATUSCOUNTER_One;

	enum
	{
		BUTTON_Invalid = -1,
		BUTTON_Strength,
		BUTTON_Spirit,
		BUTTON_Talent,
		BUTTON_Agility,
		BUTTON_Health,
		BUTTON_BackStatus,
		BUTTON_ChangeStatus,

		BUTTON_RequiredStrength,
		BUTTON_RequiredSpirit,
		BUTTON_RequiredTalent,
		BUTTON_RequiredAgility,
		BUTTON_RequiredHealth,

		TOOLTIP_RequiredStrength,
		TOOLTIP_RequiredSpirit,
		TOOLTIP_RequiredTalent,
		TOOLTIP_RequiredAgility,
		TOOLTIP_RequiredHealth,
	};

	DX::Font_ptr				pFont = nullptr;
	UI::Window_ptr				pWindow = nullptr;

	int							iCharacterStatusPointOld = 0;

	bool						bIsButtonsEnabled = false;

	CharacterStatusBaseData		sRequiredCharacterStatusBaseData;

	DWORD						dwaStatusBaseColor[5];

	void						BuildButtonsStatus();
	
	void						DrawTextCharacter( const std::string & strText, const Rectangle2D & rRect, const DX::Color & cColor, UINT uAlign = DT_CENTER );

	DWORD						GetStatusColor( int iStatus );
	DWORD						GetColor( int iColorStatus );


	void						OnBeforeRenderChildren( UIEventArgs eArgs );
	
	void						OnButtonStatusClick( int iButtonID, UIEventArgs eArgs );
	void						OnButtonCloseClick( UIEventArgs eArgs );

	void						RecordBuffer( int iButtonID, int iPoints );

	bool						GetInvalidatedBuffer( RecordStatusCharacterBuffer & sOut );
	
	void						AddStatus( int iButtonID, int iStatusCount );
	void						SubStatus( int iButtonID, int iStatusCount );

	int							GetStatusCounter( EStatusCounter eStatusCounter );

	void						ChangeStatusCounter();
	
	void						ChangeStatusCounterButton( EStatusCounter eStatusCounter );

	void						EnableButtons();
	void						DisableButtons();

	void						GetRequiredStatus( CharacterStatusBaseData & sCharacterStatusBaseData );
};

