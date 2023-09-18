#pragma once
class CUserInterface
{
public:
	CUserInterface();
	virtual ~CUserInterface();

	BOOL				Init();
	BOOL				Shutdown();

	void				Render();

	void				Update( float fTime );

private:
};

