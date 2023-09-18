#pragma once

class ChatGame : public Chat
{
private:
	static const DWORD							pfnAddChatBoxText = 0x0061F600;
public:
												ChatGame();
	virtual										~ChatGame();

	static void									SendChat( BOOL bLoginServer, const char * pszChat );
	static void	WINAPI							RestoreCommand();
	int											AnalyzeChat( char * pszBuff );

	void										PHPing( struct PacketPing * psPacket );

	static void									AddChatBoxText( const char * pszText, EChatColor iColor = CHATCOLOR_Error );
	static void									AddChatBoxTextEx( EChatColor iColor, const char * pszText, ... );

	void										ChatBuffSort();
};

