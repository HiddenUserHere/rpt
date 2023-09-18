#include "stdafx.h"
#include "UIControllerGeneral.h"
#include "TestUI.h"
#include "CDamageInfo.h"

CUIControllerGeneral::CUIControllerGeneral()
{
	pcCharacterStatusWindowHandler = new CCharacterStatusWindowHandler();
}

CUIControllerGeneral::~CUIControllerGeneral()
{
	DELET( pcCharacterStatusWindowHandler );
}

void CUIControllerGeneral::CallpFunction( DWORD pFunction, DWORD pClass )
{
	tfnCallUIDraw CallUIDraw = (tfnCallUIDraw)pFunction;
	CallUIDraw( pClass );
}

void CUIControllerGeneral::Init()
{
	pcCharacterStatusWindowHandler->Init();

	pcSkillAutobar = UI::ImageLoader::LoadImage( "game\\images\\skills\\autoskill.png" );
}

BOOL CUIControllerGeneral::OnMouseClick( CMouse * pcMouse )
{
	BOOL bRet = FALSE;

	bRet = pcCharacterStatusWindowHandler->OnMouseClick( pcMouse );

	return bRet;
}

void CUIControllerGeneral::OnMouseMove( CMouse * pcMouse )
{
	pcCharacterStatusWindowHandler->OnMouseMove( pcMouse );
}

void CUIControllerGeneral::Update( float fTime )
{
	pcCharacterStatusWindowHandler->Update( fTime );

	fAutoSkillTimeRotation += fTime;
	if( fAutoSkillTimeRotation > 1000.0f )
		fAutoSkillTimeRotation = 0.0f;

	if ( ENABLE_SKILL_LOOP )
	{
		if ( SELECTED_SKILL_MOUSE_INDEX == 1 )
		{
			if ( SKILL_LEFTSIDE && (SKILL_LEFTSIDE->bActive == FALSE) )
				ENABLE_SKILL_LOOP = FALSE;
		}
		else if ( SELECTED_SKILL_MOUSE_INDEX == 2 )
		{
			if ( SKILL_RIGHTSIDE && (SKILL_RIGHTSIDE->bActive == FALSE) )
				ENABLE_SKILL_LOOP = FALSE;
		}

		if ( MAP_TYPE == MAPTYPE_Town )
			ENABLE_SKILL_LOOP = FALSE;
	}


}

void CUIControllerGeneral::Render()
{
	WRITEDWORD( DrawOffsetArray, 2 );
	//CallpFunction( pfnCharStatusDraw, pcCharStatus );
	//CallpFunction( pfnCharStatusDrawText, pcCharStatus );
	pcCharacterStatusWindowHandler->Render();

	WRITEDWORD( DrawOffsetArray, 2 );
	CallpFunction( pfnSkillDraw, pcSkill );

	WRITEDWORD( DrawOffsetArray, 2 );
	CallpFunction( pfnInventoryDraw, pcInventory );
	CallpFunction( pfnInventoryDrawText, pcInventory );

	HUDHANDLER->GetInventoryWindow()->Render();

	WRITEDWORD( DrawOffsetArray, 2 );
	CallpFunction( pfnInterfaceDraw, pcInterface );
	CallpFunction( pfnInterfaceDrawText, pcInterface );

	if ( Game::GetGameMode() == GAMEMODE_InGame )
	{
		if ( ISSHOW_INTERFACE )
		{
			if ( ENABLE_SKILL_LOOP )
			{
				if ( SELECTED_SKILL_MOUSE_INDEX == 1 )
					UI::ImageRender::Render( pcSkillAutobar, 352, RESOLUTION_HEIGHT - 57, pcSkillAutobar->GetWidth(), pcSkillAutobar->GetHeight(), 0, 0, -1, 1.0f, (fAutoSkillTimeRotation * 360.0f) / 1000.0f );
				else if ( SELECTED_SKILL_MOUSE_INDEX == 2 )
					UI::ImageRender::Render( pcSkillAutobar, 406, RESOLUTION_HEIGHT - 57, pcSkillAutobar->GetWidth(), pcSkillAutobar->GetHeight(), 0, 0, -1, 1.0f, (fAutoSkillTimeRotation * 360.0f) / 1000.0f );
			}
		}

		GAMESCREEN->Render2D();
	}

	DAMAGEINFOHANDLER->Render();

	WRITEDWORD( DrawOffsetArray, 0 );
	TIMERSKILLHANDLER->DrawTimer();

	WRITEDWORD( DrawOffsetArray, 0 );
	CallpFunction( pfnShopDraw, pcShop );
	CallpFunction( pfnShopDrawText, pcShop );

	CallpFunction( pfnTradeDraw, pcTrade );
	CallpFunction( pfnTradeDrawText, pcTrade );

	CallpFunction( pfnSocketItemDraw, pcItemSocket );

	WRITEDWORD( DrawOffsetArray, 0 );
	CallpFunction( pfnHelpDraw, pcHelp );
	CallpFunction( pfnHelpDrawText, pcHelp );

	CallpFunction( pfnWarpGateDraw, pcWapgate );
	CallpFunction( pfnWarpGateDrawText, pcWapgate );

	CALL( pfnInterfaceEffectDraw );
	WRITEDWORD( DrawOffsetArray, 0 );
	CALL( pfnClanMenuDraw );

	WRITEDWORD( DrawOffsetArray, 2 );
	CallpFunction( pfnSkillTextDraw, pcSkill );

	WRITEDWORD( DrawOffsetArray, 0 );
	CallpFunction( pfnHelpPetDraw, pcHelpPet );
	CallpFunction( pfnHelpPetDrawText, pcHelpPet );

	CallpFunction( pfnPhoenixPetDraw, pcPhoenixPet );
	CallpFunction( pfnPhoenixPetDrawText, pcPhoenixPet );

	CallpFunction( pfnSodDraw, pcSod );
	CallpFunction( pfnSodDrawText, pcSod );

	CallpFunction( pfnSiegeDraw, pcSiege );
	CallpFunction( pfnSiegeDrawText, pcSiege );

	CallpFunction( pfnQuestDraw, pcQuest );

	CallpFunction( pfnPremiumDraw, pcPremiumItem );
	CallpFunction( pfnPremiumDrawText, pcPremiumItem );

	CallpFunction( pfnMessageBoxDraw, pcMessageBox );
	CallpFunction( pfnMessageBoxDrawText, pcMessageBox );

	WRITEDWORD( DrawOffsetArray, 0 );

	//Render Skills
	if( READDWORD(0x0367E83C) > 0 )
	{
		//FSP
		if ( READDWORD( 0x0367E054 ) > 0 )
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "%d", CHARACTERGAME->GetFSP() ), RECT{ 229, (RESOLUTION_HEIGHT + 56) - WINDOW_SKILLMOVE + 175, 0, 0 }, DT_NOCLIP, DX::Color( 255, 255, 255 ) );
		else
		{
			//SP & EP
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "%d", READDWORD( 0x03673B70 ) ), RECT{ 201, (RESOLUTION_HEIGHT + 56) - WINDOW_SKILLMOVE + 175, 0, 0 }, DT_NOCLIP, DX::Color( 255, 255, 255 ) );
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "%d", READDWORD( 0x03673B74 ) ), RECT{ 244, (RESOLUTION_HEIGHT + 56) - WINDOW_SKILLMOVE + 175, 0, 0 }, DT_NOCLIP, DX::Color( 255, 255, 255 ) );
		}
	}

	if( READDWORD(0x367DE9C) > 0 )
	{
		if ( CHARACTERGAME->GetFSP() > 0 )
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "Regnum Skill Points: %d", CHARACTERGAME->GetFSP() ), RECT{ 112, 275, 0, 0 }, DT_NOCLIP, DX::Color( 230, 170, 40 ) );

		int iPoints = READDWORD( 0x03673B70 ) + READDWORD( 0x03673B74 ) + CHARACTERGAME->GetFSP();
		if ( iPoints <= 0 )
		{
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "Not enough skill points", CHARACTERGAME->GetFSP() ), RECT{ 112, 235, 0, 0 }, DT_NOCLIP, DX::Color( 220, 0, 0 ) );
			GetGraphicsDevice()->GetDefaultFont()->Draw( FormatString( "to train new skills", CHARACTERGAME->GetFSP() ), RECT{ 112, 255, 0, 0 }, DT_NOCLIP, DX::Color( 220, 0, 0 ) );
		}
	}
}

void CUIControllerGeneral::OnDrawClanInfo( int iX )
{

}