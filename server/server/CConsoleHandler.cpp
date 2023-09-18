#include "stdafx.h"
#include "CConsoleHandler.h"

CConsoleHandler::CConsoleHandler()
{
}

CConsoleHandler::~CConsoleHandler()
{
	Shutdown();
}

bool CConsoleHandler::Init()
{
	bool bResult = false;

	bEnabled = false;

	if ( AllocConsole() )
	{
		if ( (freopen_s( &pFileConsoleHandler, "CONOUT$", "w", stdout ) == 0) &&
			(freopen_s( &pFileConsoleHandler, "CONOUT$", "w", stderr ) == 0) &&
			(freopen_s( &pFileConsoleHandler, "CONIN$", "r", stdin ) == 0) )
		{
			//Set UTF-8 codepage
			SetConsoleOutputCP( CP_UTF8 );

			setvbuf( stdout, nullptr, _IOFBF, 1000 );

			//Light Blue
			SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 9 );

			bEnabled = true;

			bResult = true;
		}
		else
		{
			FreeConsole();
		}
	}

	return bResult;
}

void CConsoleHandler::Shutdown()
{
	if ( bEnabled )
	{
		if ( pFileConsoleHandler )
		{
			fclose( pFileConsoleHandler );
			pFileConsoleHandler = nullptr;
		}

		FreeConsole();

		bEnabled = false;
	}
}

void CConsoleHandler::SetTitle( const std::string & strTitle )
{
	if ( bEnabled )
		SetConsoleTitleA( strTitle.c_str() );
}
