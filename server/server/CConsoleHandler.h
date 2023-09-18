#pragma once
class CConsoleHandler
{
private:
	FILE 			* pFileConsoleHandler = nullptr;

	bool 			bEnabled = false;

public:
	CConsoleHandler();

	~CConsoleHandler();

	bool			Init();

	void			Shutdown();

	void 		    SetTitle( const std::string & strTitle );

};

