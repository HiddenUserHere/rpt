#pragma once
class CSQLCharacterDataHandler
{
public:
	CSQLCharacterDataHandler();
	virtual ~CSQLCharacterDataHandler();

	bool					LoadCharacterData( const int iCharacterID, CharacterData & sCharacterData );

	bool 					SaveCharacterData( const int iCharacterID, const CharacterData & sCharacterData );

	bool					SQLCharacterDataExists( const int iCharacterID );

	bool					SQLCharacterNameUpdate( const int iCharacterID, const std::string & strCharacterNameNew );
};

