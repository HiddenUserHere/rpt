#pragma once

class CCharacterScreen;

enum ECharacterScreenStep : int
{
	CHARACTERSCREENSTEP_None,
	
	CHARACTERSCREENSTEP_NewCharacterTribe,
	CHARACTERSCREENSTEP_NewCharacterTempskronClass,
	CHARACTERSCREENSTEP_NewCharacterMorionClass,
	CHARACTERSCREENSTEP_NewCharacterHair,
};

struct CharacterScreenCharacterData
{
	std::string			strName;
	int					iLevel;
	std::string			strClanName;
	ECharacterClass		iClass;

	EXESurfaceOld		* psClanSurface;

	CharacterScreenCharacterData( const std::string _strName, int _iLevel, const std::string _strClanName, ECharacterClass _iClass, EXESurfaceOld * _psClanSurface )
	{
		strName = _strName;
		iLevel = _iLevel;
		strClanName = _strClanName;
		iClass = _iClass;
		psClanSurface = _psClanSurface;
	};

	~CharacterScreenCharacterData() {};
};

class CCharacterScreenModel
{
public:
	CCharacterScreenModel();
	virtual ~CCharacterScreenModel();

	void						Init();

	BOOL						Shutdown();

	void						Render3D();


	ECharacterScreenStep		GetStep() { return iStep; }
	ECharacterScreenStep		GetNewStep() { return iNewStep; }
	ECharacterScreenStep		GetOldStep() { return iStepOld; }


	void						AddCameraAxis( Point3D sPosition );
	void						AddNextCameraAxis( Point3D sPosition );
	
	void						SetNextCamera( Point3D sPosition ) { sNextCameraPosition = sPosition; fCameraTimeUpdate = 0.0f; }
	
	void						SetCameraTimeChangeDuration( float fTime ) { fCameraTimeChangeDuration = fTime; }

	void						Frame();

	void						Update( float fTime );

	void						ClearUserCharacters();
	void						AddUserCharacter( const char * pszName, const char * pszModelFile, const char * pszHeadFile, ECharacterClass iClass, int iLevel, int iClanID, EItemID iItemRightHandID = ITEMID_None, EItemID iItemLeftHandID = ITEMID_None );

	void						ClearNewCharacters();
	void						AddNewCharacter( const char * pszName, const char * pszModelFile, const char * pszHeadFile, ECharacterClass iClass, EItemID iItemRightHandID = ITEMID_None, EItemID iItemLeftHandID = ITEMID_None );

	void						ResetCharacterSelect();
	UnitData					* GetCharacter() { return pcCharacter; };
	void						SetCharacter( UnitData * pcUnitData );
	
	void						ResetCreateCharacter();
	UnitData					* GetCreateCharacter() { return pcCreateCharacter; }
	void						SetCreateCharacter( UnitData * pcUnitData );

	void						SelectNewCharacter();

	void						SetHairCharacterCreate( const std::string strHairModelFile );

	bool						HasSelectedCharacter() { return GetCharacter() != nullptr; }

	bool						CanDeleteSelectedCharacter();
	void						DeleteSelectedCharacter();

	void						MoveCharacter( UnitData * pcUnitData, Point3D sPosition );


	bool						GetCharacterLocked() { return bCharacterLocked; }
	void						SetCharacterLocked( bool b ) { bCharacterLocked = b; }

	bool						CanSelectUserCharacters();
	bool						CanSelectCreateCharacter();
	UnitData					* GetCharacterMouseOver( std::vector<UnitData *> & vCharacters, Point2D * psPoint );

	UnitData					* GetUserCharacterMouseOver( Point2D * psPoint );
	UnitData					* GetCreateCharacterMouseOver( Point2D * psPoint );

	bool						CanBackLoginScreen();

	void						SetStep( ECharacterScreenStep iStep );

	bool						HasUserCharacter();

	void						HandlePacket( PacketCreateCharacter * psPacket );

	void						CreateNewCharacter( const std::string strCharacterName );

	UINT						GetUserCharacterCount() { return vUserCharacters.size(); }

	UINT						GetMaxUserCharacter() { return MAX_USER_CHARACTERS; }

private:

	const UINT					MAX_USER_CHARACTERS = 6;

	const Point3D				CAMERA_DEFAULT_POSITION = Point3D( 0 << 8, 75 << 8, -(400 << 8) );
	const Point3D				CAMERA_DEFAULT_ANGLE = Point3D( 0, 2048, 0 );

	const Point3D				CAMERA_RACE_AXIS = Point3D( 0 << 8, 75 << 8, -(500 << 8) );
	
	const Point3D				CAMERA_CLASS_AXIS = Point3D( 0 << 8, 75 << 8, -(650 << 8) );

	const Point3D				CAMERA_DEFAULT_LOOKAT = Point3D( 0, 50 << 8, -(110 << 8) );

	const Point3D				CHARACTER_DEFAULT_ANGLE = Point3D( 0, 2048, 0 );
	const Point3D				CHARACTER_USER_POSITION = Point3D( 0, -(6 << 8), -(110 << 8));
	const Point3D				CHARACTER_CREATE_POSITION = Point3D( 0, -(5 << 8), -(328 << 8) );
	const Point3D				CHARACTER_CREATE_MOVE_POSITION = Point3D( 0, 2 << 8, -(418 << 8) );

	Stage						* pcStage			= nullptr;
	Point3D						sCameraPosition;
	Point3D						sNextCameraPosition;
	Point3D						sCameraAngle;
	Point3D						sCharacterCreateOldPosition;
	float						fCameraTimeUpdate			= 0.0f;
	float						fCameraTimeChangeDuration	= 2000.0f; //Duration beetween position changes

	Point3D						sLookAt;

	std::vector<UnitData*>		vCreateCharacters;
	std::vector<UnitData*>		vUserCharacters;
	Point3D						sUserCharactersPosition;
	Point3D						sCreateCharactersPosition;

	UnitData					* pcCharacter		= nullptr;
	bool						bCharacterLocked	= false;
	float						fCharacterSelectTimeUpdate = 0.0f;

	UnitData					* pcCreateCharacter = nullptr;

	ECharacterScreenStep		iStepOld			= CHARACTERSCREENSTEP_None;
	ECharacterScreenStep		iStep				= CHARACTERSCREENSTEP_None;
	ECharacterScreenStep		iNewStep			= CHARACTERSCREENSTEP_None;
	DWORD						dwUpdateTimeStep	= 0;

	bool						bUpdatedClanData	= false;

	void						UpdateCamera( float fTime );
	void						SetCamera( Point3D sPosition, Point3D sLookAt );

	bool						CanRenderUserCharacters();
	void						UpdateUserCharactersPosition();
	void						UpdateCreateCharactersPosition();
	void						UpdateCharactersPosition( std::vector<UnitData *> & vCharacters, Point3D sUpdatePosition, bool bPodiumStyle );
	void						RenderUserCharacters();
	void						RenderNewCharacters();
	void						UpdateUserCharacters( float fTime );
	void						FrameUserCharacters();
	void						UpdateNewCharacters( float fTime );
	void						FrameNewCharacters();

	void						UpdateTextures();

	static BOOL					MainClanData();

	static void					ClanWebStart();

	static void					ClanMenuInit( int iNum );

	CCharacterScreen			* GetCharacterScreen();

	void						ProcessPacket( PacketDeleteCharacter * psPacket );

	void						ProcessPacket( PacketCreateCharacter * psPacket );

	std::vector<UI::Image*>			vImageClasses;
	std::vector<UI::Image *>		vImageClassesText;

	UI::Image					* pcImageBox = nullptr;

	UI::Image					* pcImageTempskron = nullptr;		
	UI::Image					* pcImageMorion = nullptr;

	static BOOL					GetCharacterClan( const char * pszAccountName, const char * pszCharacterName );
};

