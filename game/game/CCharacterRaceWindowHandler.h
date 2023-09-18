#pragma once
class CCharacterRaceWindowHandler
{
public:
	CCharacterRaceWindowHandler();
	virtual ~CCharacterRaceWindowHandler();

	void							Init();

	void							Render();

	BOOL							OnMouseClick( CMouse * pcMouse );

	void							OnMouseMove( CMouse * pcMouse );

	void							OnResolutionChanged();

	void							Open();
	void							Close();

private:

	enum
	{
		WINDOWID_Main,

		BUTTON_Morion,
		BUTTON_Tempskron,

		WINDOW_Morion,
		WINDOW_Tempskron,

		IMAGE_Morionbox,
		IMAGE_Tempskronbox,
	};

	UI::Window_ptr					pWindow;

	void							OnTempskronButtonClick( UIEventArgs eArgs );
	void							OnMorionButtonClick( UIEventArgs eArgs );

	std::string						GetImagePath( const std::string & strName );

	void							OnHoverTempskronButton( bool bHover, UIEventArgs eArgs );
	void							OnHoverMorionButton( bool bHover, UIEventArgs eArgs );

	void							UpdateWindowPosition();

	CCharacterScreen				* GetCharacterScreen();
};

