#pragma once
class CSQLCharacterSaveHandler
{
public:
	CSQLCharacterSaveHandler();
	virtual ~CSQLCharacterSaveHandler();

	bool 					LoadCharacterSave( const int iCharacterID, CharacterSave & sCharacterSave );

	bool					SaveCharacterSave( const int iCharacterID, CharacterSave & sCharacterSave );

	bool					SQLCharacterSaveExists( const int iCharacterID );

	bool 					SQLCharacterNameUpdate( const int iCharacterID, const std::string & strNewCharacterName );

	bool					SQLGetLastPosition( const int iCharacterID, int & iLastPositionX, int & iLastPositionZ );
};

