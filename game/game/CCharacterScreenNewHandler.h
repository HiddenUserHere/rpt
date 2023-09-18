#pragma once

struct CharacterSelectModelData
{
	const std::string				strBodyModelName;
	const std::string				strInitialBodyModelName;

	const std::string				strWeaponCode;
	const std::string				strShieldCode;
	
	const std::vector<std::string>	vHairModelNames;
};

struct CharacterSelectNewCharacterData
{
	ECharacterClass					iClass;
	ECharacterRace					iRace;

	const std::vector<CharacterSelectModelData> vModelData;
};


class CCharacterScreenNewHandler
{
public:
	CCharacterScreenNewHandler();

	~CCharacterScreenNewHandler();

	void						Init();

	BOOL						Shutdown();

	void						Open();
	void						Close();

	void						Render();

	void						Update( float fTime );

	BOOL						OnMouseClick( CMouse * pcMouse );

	void						OnMouseMove( CMouse * pcMouse );

	BOOL						OnKeyChar( CKeyboard * pcKeyboard );

	BOOL						OnKeyPress( CKeyboard * pcKeyboard );

	void						UpdateNewCharacters( ECharacterScreenStep iStep );

	void						OnSelectCreateCharacter( UnitData * pcUnitData );

	void						ProcessCreateCharacter();

	void						OnResolutionChanged();

	const std::string			GetCharacterInitialModelPath( ECharacterClass eCharacterClass );

private:

	const UINT					MAX_HAIRS = 3;

	enum
	{
		WINDOW_Main,
		WINDOW_Mainbox,
		UNITMESH_Mainbox,
		INPUT_Name,
		
		WINDOW_Hair = 500,
		WINDOW_Hair1,
		WINDOW_Hair2,
		WINDOW_Hair3,
		WINDOW_Hair4,
		WINDOW_Hair5,

		BUTTON_Hair1,
		BUTTON_Hair2,
		BUTTON_Hair3,
		BUTTON_Hair4,
		BUTTON_Hair5,

		UNITMESH_Hair = 1000,
		UNITMESH_Hair1,
		UNITMESH_Hair2,
		UNITMESH_Hair3,
		UNITMESH_Hair4,
		UNITMESH_Hair5,
		
	};

	UI::Window_ptr				pWindow = nullptr;

	bool						bBlockNewCharactersCreate = false;
	
	static const std::vector<CharacterSelectNewCharacterData> vNewCharacterData;

	void						OnButtonSelectHairClick( int iHairID, UIEventArgs eArgs );

	void						ResetCharacterHair();

	void						SetNewCharacters( ECharacterRace iRace );

	const std::string			GetCharacterModelPath( const std::string strFileName );

	CCharacterScreen			* GetCharacterScreen();

	const std::string			GetImagePath( const std::string strFileName );

	void						OnCallbackProcessCreateCharacter();

	const CharacterSelectNewCharacterData & GetCharacterData( ECharacterClass iClass );
};

