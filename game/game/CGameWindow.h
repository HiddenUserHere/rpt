#pragma once

#include "CGame.h"

#define GAMEWINDOW				(CGameWindow::GetInstance())

#define GAME					(GAMEWINDOW->GetGame())


class CGameWindow :
	public CWindow
{
public:
	CGameWindow();
	virtual ~CGameWindow();

	UINT					  Init();
	BOOL					  Shutdown();
	BOOL					  Run();

	static CGameWindow		* GetInstance() { return pcInstance; }

	CGame					* GetGame() { return pcGame; }


protected:

	static void				  HookMouseClicks();


	LRESULT					  WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void					  Loop();

	bool					  ResetCheck();

	void					  Render();

	void					  Update( double fTime );

	BOOL					  Quit();

	BOOL					  bRender;

	CGame					* pcGame;

	static CGameWindow		* pcInstance;

	int						iWindowInactiveFrameSleepTime = 0;

	UINT					uTimerID;

	CTimer					* pcTimer;

	bool					bMouseInsideWindow = false;

	HIMC					hImc;

	BOOL					bActiveIME = FALSE;

	void					SetIME( bool b );

};

