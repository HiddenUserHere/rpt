#include "stdafx.h"
#include "CActionBarHandler.h"

#include "CGame.h"

extern BOOL __cdecl OpenPlaySkill( struct Skill * psSkill );

extern BOOL __stdcall PrepareSkillInfoSpecial( Skill * psSkill, SkillBox * psSkillBox, int iIcon, BOOL bSkillTimer );


const unsigned int iNumRows = 2;
const unsigned int iNumColumns = 9;

int iOldResolutionWidth = 0;
int iOldResolutionHeight = 0;

std::vector<std::pair<int, std::string>> vLabels = {
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F4, "F4"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {'4', "4"},
    {'5', "5"},
    {'6', "6"},
    {'7', "7"},
    {'8', "8"},
    {'9', "9"},
    {'0', "0"},
    {VK_OEM_MINUS, "-"},
    {VK_OEM_PLUS, "="},
};

void RenderMountIcon( int iMountID, Rectangle2D rRectMountBox )
{
    static auto cooldownTexture = GRAPHICS->GetTextureFactory()->Create( "game\\textures\\cooldown.png" );
    static auto effect = GRAPHICS->GetShaderFactory()->Create( "game\\scripts\\shaders\\CooldownEffect.fx" );

    auto pImage = MOUNTHANDLER->GetWindow()->GetMountImage( iMountID );
    if ( pImage )
    {
        effect->SetFloat( "CooldownTime", (float)MOUNTHANDLER->GetCooldownMountPercent() / 100.0f );
        effect->SetTechnique( "Cooldown" );

        if ( effect->Begin() >= 0 )
        {
            GRAPHICS->GetDevice()->SetTexture( 1, cooldownTexture->Get() );

            if ( Graphics::Sprite::Default->Begin( TRUE ) )
            {
                if ( rRectMountBox.iWidth == 0 || rRectMountBox.iHeight == 0 )
                    Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(rRectMountBox.iX - (pImage->GetWidth() >> 1) + 4), (float)(rRectMountBox.iY - (pImage->GetHeight() >> 1) + 4) ) );
                else
                    Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(rRectMountBox.iX + (rRectMountBox.iWidth >> 1) - (pImage->GetWidth() >> 1) + 4), (float)(rRectMountBox.iY + (rRectMountBox.iHeight >> 1) - (pImage->GetHeight() >> 1) + 4) ) );

                if ( effect->BeginPass( 0 ) )
                {
                    Graphics::Sprite::Default->Draw( pImage->GetImage()->GetTexture(), Math::Color::White, Math::RectInt( 0, 0, pImage->GetWidth(), pImage->GetHeight() ) );

                    effect->EndPass();
                }

                Graphics::Sprite::Default->PopTransform( 2 );
                Graphics::Sprite::Default->End();
            }

            effect->End();
        }

        int iTime = MOUNTHANDLER->GetCooldownMount();

        if ( iTime > 0 )
        {
            GetGraphicsDevice()->GetDefaultFont()->Draw( std::to_string( iTime / 1000 ), RECT{ rRectMountBox.iX + 1 + 4, rRectMountBox.iY + 1 + 4, rRectMountBox.iX + 1 + pImage->GetWidth() + 4, rRectMountBox.iY + 1 + pImage->GetHeight() + 4 }, DT_CENTER | DT_VCENTER | DT_NOCLIP, DX::Color(255, 110, 110, 110));
            GetGraphicsDevice()->GetDefaultFont()->Draw( std::to_string( iTime / 1000 ), RECT{ rRectMountBox.iX + 4, rRectMountBox.iY + 4, rRectMountBox.iX + pImage->GetWidth() + 4, rRectMountBox.iY + pImage->GetHeight() + 4 }, DT_CENTER | DT_VCENTER | DT_NOCLIP, DX::Color( 255, 255, 255, 255 ) );
        }
    }
}

void RenderSkillIcon( int iX, int iY, EXESurfaceOld * psTexture, int iPercent, int iMastery, bool bScale )
{
    if ( psTexture == nullptr )
        return;

    auto pcTexture = psTexture->sTexture.pcD3DTexture;

    static auto cooldownTexture = GRAPHICS->GetTextureFactory()->Create( "game\\textures\\cooldown.png" );
    static auto effect = GRAPHICS->GetShaderFactory()->Create( "game\\scripts\\shaders\\CooldownEffect.fx" );

    if ( pcTexture )
    {
        effect->SetFloat( "CooldownTime", bScale ? 1.f - ((float)iPercent / 35.f) : 0.0f );
        effect->SetTechnique( "Cooldown" );

        if ( effect->Begin() >= 0 )
        {
            GRAPHICS->GetDevice()->SetTexture( 1, cooldownTexture->Get() );

            if ( Graphics::Sprite::Default->Begin( TRUE ) )
            {
                if ( bScale )
                {
                    float fMultScale = 36.0f;// (float)psTexture->sTexture.iWidth;
                    if ( fMultScale == 0.0f )
                        fMultScale = 36.0f;

                    Graphics::Sprite::Default->PushScaling( Math::Vector2( 36.0f / fMultScale, 36.0f / fMultScale ) );
                }

                Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)iX, (float)iY ) );

                if ( effect->BeginPass( 0 ) )
                {
                    Graphics::Sprite::Default->Draw( pcTexture, Math::Color::White, Math::RectInt( 4, 4, psTexture->sTexture.iWidth, psTexture->sTexture.iHeight ) );
                    effect->EndPass();
                }

                Graphics::Sprite::Default->PopTransform( 2 );
                Graphics::Sprite::Default->End();
            }

            effect->End();
        }

        if ( iMastery < 2 )
            iMastery = 2;

        float fSkillCooldown = ((MAX_MASTERY / (MAX_MASTERY / ((float)iMastery / 2.0f))) * 35) * (1000.0f / 60.0f);
        fSkillCooldown /= 1000.0f;

        fSkillCooldown *= 1.0f - ((float)iPercent / 35.f);

        if ( (int)fSkillCooldown > 0 )
        {
            GetGraphicsDevice()->GetDefaultFont()->Draw( std::to_string( (int)fSkillCooldown ), RECT{ iX + 1 - 2, iY + 1, iX + 1 + psTexture->sTexture.iWidth - 2, iY + 1 + psTexture->sTexture.iHeight }, DT_CENTER | DT_VCENTER | DT_NOCLIP, DX::Color( 255, 110, 110, 110 ) );
            GetGraphicsDevice()->GetDefaultFont()->Draw( std::to_string( (int)fSkillCooldown ), RECT{ iX - 2, iY, iX + psTexture->sTexture.iWidth - 2, iY + psTexture->sTexture.iHeight }, DT_CENTER | DT_VCENTER | DT_NOCLIP, DX::Color( 255, 255, 255, 255 ) );
        }
    }
}

void RenderItemIcon( int iX, int iY, EXESurfaceOld * psSurface, float fPercent )
{
    return;

    if ( (psSurface == nullptr) || (psSurface->sTexture.pcD3DTexture == nullptr) )
        return;

    auto pcTexture = psSurface->sTexture.pcD3DTexture;

    static auto cooldownTexture = GRAPHICS->GetTextureFactory()->Create( "game\\textures\\cooldown.png" );
    static auto effect = GRAPHICS->GetShaderFactory()->Create( "game\\scripts\\shaders\\CooldownEffect.fx" );

    if ( pcTexture )
    {
        effect->SetFloat( "CooldownTime", fPercent );
        effect->SetTechnique( "Cooldown" );

        if ( effect->Begin() >= 0 )
        {
            GRAPHICS->GetDevice()->SetTexture( 1, cooldownTexture->Get() );

            if ( Graphics::Sprite::Default->Begin( TRUE ) )
            {
                Graphics::Sprite::Default->PushScaling( Math::Vector2( 1.f, 1.f ) );
                Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)iX, (float)iY ) );

                if ( effect->BeginPass( 0 ) )
                {
                    Graphics::Sprite::Default->Draw( pcTexture, Math::Color::White );
                    effect->EndPass();
                }

                Graphics::Sprite::Default->PopTransform( 2 );
                Graphics::Sprite::Default->End();
            }

            effect->End();
        }
    }
}

CActionBarHandler::CActionBarHandler()
{
    pWindow1 = std::make_shared<UI::Window>( Rectangle2D( 0, 0, WINDOW_ACTIONBAR_WIDTH, WINDOW_ACTIONBAR_HEIGHT ) );
}

CActionBarHandler::~CActionBarHandler()
{
    CLEARPTR( pImageSlotHover );
	CLEARPTR( pImageLeftSkill );
	CLEARPTR( pImageRightSkill );
	CLEARPTR( pImageLeftRightSkill );
}

void CActionBarHandler::RecordSaveData( ActionBarData & sData )
{
    int iIndex = 0;

    for ( const auto & slots : vSlots )
    {
        for ( const auto & slot : slots )
        {
            if ( slot.bActive )
            {
                if ( slot.sItemData.bValid )
                {
                    ItemID sItemID = slot.sItemData.sBaseItemID;
                    sData.iaBuffer[iIndex] = sItemID.ToInt();
                }
                else if ( slot.iSkillID > 0 )
                {
                    if ( slot.iSkillMousePos == 1 )
                        sData.iaBuffer[iIndex] = (0x69010000 | slot.iSkillID);
                    else if ( slot.iSkillMousePos == 2 )
                        sData.iaBuffer[iIndex] = (0x69020000 | slot.iSkillID);
                }
                else if ( slot.iTaunt > 0 )
                {
                    sData.iaBuffer[iIndex] = (slot.iTaunt | 0xF1000000);
                }
                else if ( slot.iMount > 0 )
                {
                    sData.iaBuffer[iIndex] = (slot.iMount | 0xF2000000);
                }
            }
            else
            {
                sData.iaBuffer[iIndex] = -1;
            }

            iIndex++;
        }
    }

    sData.bLocked = IsLocked();
	sData.bOpen = IsOpen();
	sData.bSlotIndex = iSelectedTab;
    sData.bSwapped = bSwapMode;

    sData.sWindowX = pWindow1->GetRect().iX;
	sData.sWindowY = pWindow1->GetRect().iY;
}

void CActionBarHandler::RestoreSaveData( ActionBarData & sData )
{
    SKILLMANAGERHANDLER->ResetSkillsShortcut();

    CopyMemory( &sCacheData, &sData, sizeof( ActionBarData ) );

    if( sData.bOpen )
		Open();
	else
		Close();

	SetSelectedTabID( sData.bSlotIndex );

    if ( sData.sWindowX != -1 && sData.sWindowY != -1 )
    {   
		if ( (sData.sWindowX + pWindow1->GetWidth()) > RESOLUTION_WIDTH )
			sData.sWindowX = RESOLUTION_WIDTH - pWindow1->GetWidth() - 8;

        if( sData.sWindowX < 0 )
            sData.sWindowX = RESOLUTION_WIDTH - pWindow1->GetWidth() - 8;

        if ( (sData.sWindowY + pWindow1->GetHeight()) > RESOLUTION_HEIGHT )
            sData.sWindowY = (int)((float)RESOLUTION_HEIGHT * 0.75f) - (pWindow1->GetHeight() >> 1);

        if ( sData.sWindowY < 0 )
            sData.sWindowY = (int)((float)RESOLUTION_HEIGHT * 0.75f) - (pWindow1->GetHeight() >> 1);

        pWindow1->SetPosition( sData.sWindowX, sData.sWindowY );
    }
    else
        pWindow1->SetPosition( (int)((float)RESOLUTION_WIDTH * 0.75f) - (pWindow1->GetWidth() >> 1), (int)((float)RESOLUTION_HEIGHT * 0.75f) - (pWindow1->GetHeight() >> 1) );

    sCacheData.sWindowX = pWindow1->GetRect().iX;
    sCacheData.sWindowY = pWindow1->GetRect().iY;

    SetLock( sData.bLocked );

    bSwapMode = sData.bSwapped;
}

void CActionBarHandler::LoadCache()
{
    int iTab = 0;

    int iIndex = 0;

    for ( auto & slots : vSlots )
    {
        for ( auto & slot : slots )
        {
            if ( sCacheData.iaBuffer[iIndex] != -1 )
            {
                //skill
                if ( (sCacheData.iaBuffer[iIndex] & 0xFF000000) == 0x69000000 )
                {
                    slot.bActive = true;
                    slot.sItemData.bValid = false;
                    slot.iQuantity = 0;
                    slot.iSkillID = (sCacheData.iaBuffer[iIndex] & 0x0000FFFF);

                    int iUseMousePos = (sCacheData.iaBuffer[iIndex] & 0x00FF0000);

                    if ( iUseMousePos == 0x00010000 )
                        slot.iSkillMousePos = 1;
                    else if ( iUseMousePos == 0x00020000 )
                        slot.iSkillMousePos = 2;

                    SetSkillSlot( slot.iKey, slot.iSkillID, slot.iSkillMousePos, iTab );

                    if ( iTab == iSelectedTab )
                        SKILLMANAGERHANDLER->SetSkillShortcut( slot.iSkillID, slot.iKey, slot.iSkillMousePos );
                }
                //Taunt
                else if ( (sCacheData.iaBuffer[iIndex] & 0xFF000000) == 0xF1000000 )
                {
                    slot.bActive = true;
                    slot.sItemData.bValid = false;
                    slot.iQuantity = 0;
                    slot.iSkillID = -1;
                    slot.iMount = -1;
                    slot.iTaunt = (sCacheData.iaBuffer[iIndex] & 0x00FFFFFF);
                }
                else if ( (sCacheData.iaBuffer[iIndex] & 0xFF000000) == 0xF2000000 )
                {
                    slot.bActive = true;
                    slot.sItemData.bValid = false;
                    slot.iQuantity = 0;
                    slot.iSkillID = -1;
                    slot.iTaunt = -1;
                    slot.iMount = (sCacheData.iaBuffer[iIndex] & 0x00FFFFFF);
                }
                else
                {
                    if ( auto pcItemData = ITEMHANDLER->GetItemInventoryByCode( (EItemID)sCacheData.iaBuffer[iIndex] ) )
                    {
                        slot.iTaunt = -1;
                        slot.iSkillID = -1;
                        slot.bActive = true;


                        CopyMemory( &slot.sItemData, pcItemData, sizeof( ItemData ) );

                        slot.sItemData.sItem.iChk1 = -1;
                        slot.sItemData.sItem.iChk1 = -2;
                    }
                }
            }

            iIndex++;
        }

        iTab++;
    }

    OnSwapMode( bSwapMode );

    pWindow1->SetPosition( sCacheData.sWindowX, sCacheData.sWindowY );
}

void CActionBarHandler::SetSelectedTabID( int iID )
{
    iSelectedTab = iID;

    for ( int i = 0; i < 3; i++ )
    {
        auto pTabButton = pWindow1->GetElement<UI::CheckBox>( i );
        if ( pTabButton )
        {
            pTabButton->SetSelected( i == iID ? TRUE : FALSE );
        }
    }
}

BOOL CActionBarHandler::Init()
{
    BuildWindow();

    pImageSlotHover = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\slothover.png" );
	pImageLeftSkill = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\leftskill.png" );
	pImageRightSkill = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\rightskill.png" );
    pImageLeftRightSkill = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\leftrightskill.png" );

    pWindow1->Hide();

    return TRUE;
}

void CActionBarHandler::Render()
{
    if ( Game::GetGameMode() != GAMEMODE_InGame )
        return;

    if ( pWindow1 )
    {
        if ( !pWindow1->IsOpen() )
            return;

        static auto pLockButton = pWindow1->GetElement<UI::CheckBox>( 101 );
        static auto pCloseButton = pWindow1->GetElement<UI::Button>( 100 );
        static auto pSwapButton = pWindow1->GetElement<UI::Button>( 102 );

        if ( pWindow1->IsHover() || pWindow1->IsSelected() )
        {
            pLockButton->Show();
            pCloseButton->Show();
            pSwapButton->Show();

            for ( int i = 0; i < 3; i++ )
            {
                auto pTabButton = pWindow1->GetElement<UI::CheckBox>( i );
                pTabButton->Show();
            }
        }
        else
        {
            pLockButton->Hide();
            pCloseButton->Hide();
            pSwapButton->Hide();

            for ( int i = 0; i < 3; i++ )
            {
                auto pTabButton = pWindow1->GetElement<UI::CheckBox>( i );
                pTabButton->Hide();
            }
        }

        pWindow1->Render( 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );

        auto pWindowTab = pWindow1->GetElement<UI::Window>( iSelectedTab + 500 );

        //Render Slots
        int iIndex = 0;
        for ( auto & slot : vSlots[iSelectedTab] )
        {
            const auto sSlotBox = Rectangle2D( pWindow1->GetRect().iX + pWindowTab->GetRect().iX, pWindow1->GetRect().iY + pWindowTab->GetRect().iY, 0, 0 ) + slot.pSlot->GetRect();

            if ( slot.pSlot && slot.bActive )
            {
                RECT rScissorRect = RECT{sSlotBox.iX + 4, sSlotBox.iY + 4, sSlotBox.iX + sSlotBox.iWidth - 4, sSlotBox.iY + sSlotBox.iHeight - 4};

                //Render slot
                if ( slot.sItemData.bValid )
                {
                    GRAPHICS->GetDevice()->SetScissorRect( &rScissorRect );
                    GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

                    int iItemsQuantity = ITEMHANDLER->GetItemAmountByCode( slot.sItemData.sBaseItemID.ToItemID() );

                    slot.iQuantity = iItemsQuantity;

                    const int iX = (41 >> 1) - (slot.sItemData.iWidth >> 1);
                    const int iY = (41 >> 1) - (slot.sItemData.iHeight >> 1);

                    RenderItemIcon( sSlotBox.iX + iX, sSlotBox.iY + iY, (EXESurfaceOld *)slot.sItemData.pcInventoryTexture, iItemsQuantity == 0 ? 1.f : 0.f );

                    if ( pImageSlotHover )
                    {
                        if ( Graphics::Sprite::Default->Begin( TRUE ) )
                        {
                            Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(sSlotBox.iX + 4), (float)(sSlotBox.iY + 4) ) );
                            Graphics::Sprite::Default->Draw( pImageSlotHover->GetTexture(), Math::Color::White);
                            Graphics::Sprite::Default->PopTransform();
                            Graphics::Sprite::Default->End();
                        }
                    }

                    GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

                    const char * pszItemsQuantity = FormatString( "%d", iItemsQuantity );

                    Math::RectInt sTextRect;
                    sTextRect.x = sSlotBox.iX + 4;
                    sTextRect.y = sSlotBox.iY + 22;
                    sTextRect.w = sTextRect.x + 32;
                    sTextRect.h = sTextRect.y + 38;

                    Graphics::Font::Default->Draw( pszItemsQuantity, STRLEN( pszItemsQuantity ), sTextRect, DT_NOCLIP | DT_RIGHT, Math::Color::White );
                }
                else if ( (slot.iSkillID > 0) && (slot.iSkillID < 50) )
                {
                    if ( auto psSkill = SKILLMANAGERHANDLER->GetSkillByIndex( slot.iSkillID - 1 ) )
                    {
                        GRAPHICS->GetDevice()->SetScissorRect( &rScissorRect );
                        GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

                        int iPercent = psSkill->GageLength;

                        RenderSkillIcon( sSlotBox.iX, sSlotBox.iY, SKILLS_ICON[slot.iSkillID - 1], iPercent, psSkill->Mastery, true );

                        if ( pImageSlotHover )
                        {
                            if ( Graphics::Sprite::Default->Begin( TRUE ) )
                            {
                                Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(sSlotBox.iX + 4), (float)(sSlotBox.iY + 4) ) );
                                Graphics::Sprite::Default->Draw( pImageSlotHover->GetTexture(), Math::Color::White);
                                Graphics::Sprite::Default->PopTransform();
                                Graphics::Sprite::Default->End();
                            }
                        }

                        GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

                        if ( SKILLMANAGERHANDLER->IsRightSkill( slot.iSkillID - 1 ) && SKILLMANAGERHANDLER->IsLeftSkill( slot.iSkillID - 1 ) )
                        {
                            if ( pImageLeftRightSkill )
                            {
                                if ( Graphics::Sprite::Default->Begin( TRUE ) )
                                {
                                    Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(sSlotBox.iX - 1), (float)sSlotBox.iY ) );
                                    Graphics::Sprite::Default->Draw( pImageLeftRightSkill->GetTexture(), Math::Color::White);
                                    Graphics::Sprite::Default->PopTransform();
                                    Graphics::Sprite::Default->End();
                                }
                            }
                        }
                        else if ( SKILLMANAGERHANDLER->IsLeftSkill( slot.iSkillID - 1 ) )
                        {
                            if ( pImageLeftSkill )
                            {
                                if ( Graphics::Sprite::Default->Begin( TRUE ) )
                                {
                                    Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(sSlotBox.iX - 1), (float)sSlotBox.iY ) );
                                    Graphics::Sprite::Default->Draw( pImageLeftSkill->GetTexture(), Math::Color::White);
                                    Graphics::Sprite::Default->PopTransform();
                                    Graphics::Sprite::Default->End();
                                }
                            }
                        }
                        else if ( SKILLMANAGERHANDLER->IsRightSkill( slot.iSkillID - 1 ) )
                        {
                            if ( pImageRightSkill )
                            {
                                if ( Graphics::Sprite::Default->Begin( TRUE ) )
                                {
                                    Graphics::Sprite::Default->PushTranslation( Math::Vector2( (float)(sSlotBox.iX - 1), (float)sSlotBox.iY ) );
                                    Graphics::Sprite::Default->Draw( pImageRightSkill->GetTexture(), Math::Color::White);
                                    Graphics::Sprite::Default->PopTransform();
                                    Graphics::Sprite::Default->End();
                                }
                            }
                        }
                    }
                }
               /* else if ( slot.iTaunt > 0 )
                {
                    TAUNTHANDLER->RenderTaunt( slot.iTaunt, sSlotBox.iX, sSlotBox.iY );

                    GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
                }*/
                else if ( slot.iMount > 0 )
                {
                    GRAPHICS->GetDevice()->SetScissorRect( &rScissorRect );
                    GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );

                    Rectangle2D sNewBox = sSlotBox;
                    sNewBox.iX -= 4;
                    sNewBox.iY -= 4;

                    RenderMountIcon( slot.iMount, sNewBox );

                    GRAPHICS->GetDevice()->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );
                }

                //Render info
                if ( sSlotBox.Inside( MOUSEHANDLER->GetPosition() ) )
                {
                    if ( slot.sItemData.bValid )
                    {
                        slot.sItemData.sPosition.iX = sSlotBox.iX;
                        slot.sItemData.sPosition.iY = sSlotBox.iY;

                        //sinShowItemInfoFlag = 1;
                        ITEMHANDLER->ShowItemInfo( &slot.sItemData, FALSE );
                    }
                    else if ( slot.iSkillID > 0 )
                    {
                        if ( auto psSkill = SKILLMANAGERHANDLER->GetSkillByIndex( slot.iSkillID - 1 ) )
                        {
                            SkillBox sSkillBox;
                            sSkillBox.BoxRect.left = sSlotBox.iX;
							sSkillBox.BoxRect.top = sSlotBox.iY;

                            PrepareSkillInfoSpecial( psSkill, &sSkillBox, 0, FALSE );

                            SKILLINFOBOX->SetFreeBox( TRUE );
                            SKILLINFOBOX->SetShowBox();
                        }
                    }
                }
            }

            //Render Label
            const char * pszLabel = slot.strLabel.c_str();

            Graphics::Font::Default->Draw( sSlotBox.iX + 6, sSlotBox.iY + 4, pszLabel, DT_NOCLIP, Math::Color( 0, 0, 0 ) );
            Graphics::Font::Default->Draw( sSlotBox.iX + 5, sSlotBox.iY + 3, pszLabel, DT_NOCLIP, Math::Color::White );

            iIndex++;
        }

        //Render Selected Skill
        if ( iSelectedSkillIcon > 0 )
        {
            if ( auto psSkill = SKILLMANAGERHANDLER->GetSkillByIndex( iSelectedSkillIcon - 1 ) )
            {
                int iPercent = psSkill->GageLength;

                RenderSkillIcon( MOUSEHANDLER->GetPosition()->iX - (SKILLS_ICON[iSelectedSkillIcon - 1]->sTexture.iWidth >> 1), MOUSEHANDLER->GetPosition()->iY - (SKILLS_ICON[iSelectedSkillIcon - 1]->sTexture.iHeight >> 1), 
                    SKILLS_ICON[iSelectedSkillIcon - 1], 0, psSkill->Mastery, false );
            }
        }
        //else if ( iSelectedTaunt > 0 )
        //{
        //    TAUNTHANDLER->RenderTaunt( iSelectedTaunt, MOUSE->GetPosition().x, MOUSE->GetPosition().y );
        //}
        else if ( iSelectedMount > 0 )
        {
            RenderMountIcon( iSelectedMount, Rectangle2D( MOUSEHANDLER->GetPosition()->iX, MOUSEHANDLER->GetPosition()->iY ) );
        }
        else if ( sSelectedItemData.bValid )
        {
            RenderItemIcon( MOUSEHANDLER->GetPosition()->iX - (sSelectedItemData.iWidth >> 1), MOUSEHANDLER->GetPosition()->iY - (sSelectedItemData.iHeight >> 1), (EXESurfaceOld *)sSelectedItemData.pcInventoryTexture, 0.0f);
        }
        else if ( ITEMHANDLER->GetMouseItem()->bValid )
        {
            //RenderItemIcon( MOUSEHANDLER->GetPosition()->iX, MOUSEHANDLER->GetPosition()->iY, (EXESurfaceOld *)ITEMHANDLER->GetMouseItem()->pcInventoryTexture, 1.0f );
        }
    }
}

void CActionBarHandler::Update( float fElapsedTime )
{
    if ( Game::GetGameMode() != GAMEMODE_InGame )
        return;

    if ( dwLastMouseEventTime < TICKCOUNT )
    {
        bool bRet = false;

        if ( iCurrentMouseEvent != EMouseEvent::None )
        {
            auto pWindowTab = pWindow1->GetElement<UI::Window>( iSelectedTab + 500 );

            if ( iCurrentMouseEvent == EMouseEvent::ClickUpL )
            {
                if ( iOldMouseEvent == EMouseEvent::ClickDownL || ((ITEMHANDLER->GetMouseItem()) && ITEMHANDLER->GetMouseItem()->bValid) || sSelectedItemData.bValid || iSelectedSkillIcon || iSelectedTaunt > 0 || iSelectedMount > 0 )
                {
                    //Release inside of some slot?
                    for ( auto & slot : vSlots[iSelectedTab] )
                    {
                        if ( slot.pSlot )
                        {
                            const auto sSlotBox = Rectangle2D( pWindow1->GetRect().iX + pWindowTab->GetRect().iX, pWindow1->GetRect().iY + pWindowTab->GetRect().iY, 0, 0) + slot.pSlot->GetRect();

                            if ( sSlotBox.Inside( MOUSEHANDLER->GetPosition() ) )
                            {
                                if ( (ITEMHANDLER->GetMouseItem()) && ITEMHANDLER->GetMouseItem()->bValid )
                                    CopyMemory( &sSelectedItemData, ITEMHANDLER->GetMouseItem(), sizeof( ItemData ) );

                                //Is item?
                                if ( sSelectedItemData.bValid )
                                {
                                    if ( CanUseItem( sSelectedItemData.sBaseItemID.ToInt() ) && pWindow1->CanMove() )
                                    {
                                        for ( auto & checkSlot : vSlots[iSelectedTab] )
                                            if ( checkSlot.bActive && checkSlot.sItemData.bValid && checkSlot.sItemData.sBaseItemID.ToItemID() == sSelectedItemData.sBaseItemID.ToItemID() )
                                            {
                                                checkSlot.bActive = false;
                                                checkSlot.sItemData.bValid = false;
                                                break;
                                            }

                                        CopyMemory( &slot.sItemData, &sSelectedItemData, sizeof( ItemData ) );
                                        if ( (ITEMHANDLER->GetMouseItem()) && ITEMHANDLER->GetMouseItem()->bValid )
                                        {
                                            ITEMHANDLER->ResetItemMouse();
                                            ITEMHANDLER->GetMouseItem()->bValid = false;
                                        }

                                        slot.sItemData.sItem.iChk1 = -1;
                                        slot.sItemData.sItem.iChk1 = -2;
                                        sSelectedItemData.bValid = false;
                                        slot.iSkillID = -1;
                                        slot.iTaunt = -1;
                                        slot.bActive = true;
                                        bRet = true;
                                    }
                                }

                                //Is skill?
                                if ( iSelectedSkillIcon > 0 )
                                {
                                    if ( pWindow1->CanMove() )
                                    {
                                        for ( auto & checkSlot : vSlots[iSelectedTab] )
                                            if ( checkSlot.bActive && checkSlot.iSkillID == iSelectedSkillIcon )
                                            {
                                                checkSlot.bActive = false;
                                                checkSlot.iSkillID = -1;
                                                break;
                                            }

                                        slot.sItemData.bValid = false;
                                        slot.iSkillID = iSelectedSkillIcon;
                                        slot.iTaunt = -1;
                                        slot.iSkillMousePos = 2;
                                        slot.sItemData.bValid = false;
                                        slot.bActive = true;
                                        SKILLMANAGERHANDLER->SetSkillShortcut( slot.iSkillID, slot.iKey, slot.iSkillMousePos );
                                        bRet = true;
                                    }
                                }

                                //Taunt?
                                if ( iSelectedTaunt > 0 )
                                {
                                    if ( pWindow1->CanMove() )
                                    {
                                        for ( auto & checkSlot : vSlots[iSelectedTab] )
                                            if ( checkSlot.bActive && checkSlot.iTaunt == iSelectedTaunt )
                                            {
                                                checkSlot.bActive = false;
                                                checkSlot.iTaunt = -1;
                                                break;
                                            }

                                        slot.sItemData.bValid = false;
                                        slot.iSkillID = -1;
                                        slot.iTaunt = iSelectedTaunt;
                                        slot.sItemData.bValid = false;
                                        slot.bActive = true;
                                        bRet = true;
                                    }
                                }

                                //Mount?
                                if ( iSelectedMount > 0 )
                                {
                                    if ( pWindow1->CanMove() )
                                    {
                                        for ( auto & checkSlot : vSlots[iSelectedTab] )
                                            if ( checkSlot.bActive && checkSlot.iMount == iSelectedMount )
                                            {
                                                checkSlot.bActive = false;
                                                checkSlot.iMount = -1;
                                                break;
                                            }

                                        slot.sItemData.bValid = false;
                                        slot.iSkillID = -1;
                                        slot.iMount = iSelectedMount;
                                        slot.sItemData.bValid = false;
                                        slot.bActive = true;
                                        bRet = true;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                sSelectedItemData.bValid = false;
                iSelectedSkillIcon = -1;
                iSelectedTaunt = -1;
                iSelectedMount = -1;
            }
            else if ( iCurrentMouseEvent == EMouseEvent::ScrollUp )
            {
                for ( auto & slot : vSlots[iSelectedTab] )
                {
                    if ( slot.pSlot )
                    {
                        const auto sSlotBox = Rectangle2D( pWindow1->GetRect().iX + pWindowTab->GetRect().iX, pWindow1->GetRect().iY + pWindowTab->GetRect().iY, 0, 0 ) + slot.pSlot->GetRect();

                        if ( sSlotBox.Inside( MOUSEHANDLER->GetPosition() ) && pWindow1->CanMove() )
                        {
                            if ( slot.bActive )
                            {
                                slot.bActive = false;
                                slot.sItemData.bValid = false;
                                bRet = true;

                                if ( slot.iSkillID != -1 )
                                    SKILLMANAGERHANDLER->SetSkillShortcut( 0, slot.iSkillID, 0 );
                                break;
                            }
                        }
                    }
                }
            }
            else if ( iCurrentMouseEvent == EMouseEvent::ClickDownR )
            {
                for ( auto & slot : vSlots[iSelectedTab] )
                {
                    if ( slot.pSlot )
                    {
                        const auto sSlotBox = Rectangle2D( pWindow1->GetRect().iX + pWindowTab->GetRect().iX, pWindow1->GetRect().iY + pWindowTab->GetRect().iY, 0, 0 ) + slot.pSlot->GetRect();

                        if ( sSlotBox.Inside( MOUSEHANDLER->GetPosition() ) )
                        {
                            if ( slot.bActive )
                            {
                                if ( slot.iSkillID != -1 )
                                {
                                    SKILLMANAGERHANDLER->SetUseSkillSlot( slot.iSkillID, slot.iSkillMousePos == 1 ? SKILLTYPE_Left : SKILLTYPE_Right );
                                    bRet = true;
                                    break;
                                }
                                /*else if ( slot.iTaunt > 0 )
                                {
                                    TAUNTHANDLER->UseTauntAnimationByButtonIndex( slot.iTaunt );
                                    bRet = true;
                                    break;
                                }*/
                            }
                        }
                    }
                }
            }
            else if ( iCurrentMouseEvent == EMouseEvent::ClickDownL )
            {
                if ( iSelectedSkillIcon == -1 )
                {
                    if ( SKILLMANAGERHANDLER->GetOnlySkillHoverLeveled( false ) )
                        iSelectedSkillIcon = SKILL_INDEXBUTTON;
                }

                if ( iSelectedMount == -1 )
                    iSelectedMount = MOUNTHANDLER->GetWindow()->GetMountHoverID();
               
                if ( (iSelectedMount != -1) || (iSelectedSkillIcon != -1) )
                    bRet = true;

                for ( auto & slot : vSlots[iSelectedTab] )
                {
                    if ( slot.pSlot )
                    {
                        const auto sSlotBox = Rectangle2D( pWindow1->GetRect().iX + pWindowTab->GetRect().iX, pWindow1->GetRect().iY + pWindowTab->GetRect().iY, 0, 0 ) + slot.pSlot->GetRect();

                        if ( sSlotBox.Inside( MOUSEHANDLER->GetPosition() ) && pWindow1->CanMove() )
                        {
                            if ( slot.bActive )
                            {
                                if ( slot.iSkillID != -1 )
                                {
                                    iSelectedSkillIcon = slot.iSkillID;
                                    slot.bActive = false;
                                    SKILLMANAGERHANDLER->ResetSkillShortcut( slot.iSkillID );
                                    slot.iSkillID = -1;
                                    bRet = true;
                                    break;
                                }

                                if ( slot.iTaunt > 0 )
                                {
                                    iSelectedTaunt = slot.iTaunt;
                                    slot.bActive = false;
                                    slot.iTaunt = -1;
                                    bRet = true;
                                    break;
                                }

                                if ( slot.iMount > 0 )
                                {
                                    iSelectedMount = slot.iMount;
                                    slot.bActive = false;
                                    slot.iMount = -1;
                                    bRet = true;
                                    break;
                                }

                                if ( slot.sItemData.bValid )
                                {
                                    CopyMemory( &sSelectedItemData, &slot.sItemData, sizeof( ItemData ) );
                                    slot.bActive = false;
                                    slot.sItemData.bValid = false;
                                    bRet = true;
                                    break;
                                }
                            }
                        }
                    }
                }

                if ( bRet )
                    pWindow1->SetSelected( FALSE );
            }

            iCurrentMouseEvent = EMouseEvent::None;
        }
    }

    if ( GetAsyncKeyState( VK_MENU ) & 0x8000 )
    {
        if ( GetAsyncKeyState( '1' ) & 0x8000 )
        {
            if ( iSelectedTab != 0 )
            {
                OnSelectTab( 0 );

                pWindow1->GetElement<UI::CheckBox>( 0 )->SetSelected( TRUE );
            }
		}
        else if ( GetAsyncKeyState( '2' ) & 0x8000 )
        {
            if ( iSelectedTab != 1 )
            {
                OnSelectTab( 1 );

                pWindow1->GetElement<UI::CheckBox>( 1 )->SetSelected( TRUE );
            }
        }
        else if ( GetAsyncKeyState( '3' ) & 0x8000 )
        {
            if ( iSelectedTab != 2 )
            {
                OnSelectTab( 2 );

                pWindow1->GetElement<UI::CheckBox>( 2 )->SetSelected( TRUE );
            }
        }
    }

    if ( pWindow1 )
        pWindow1->Update( fElapsedTime );
}

bool CActionBarHandler::Open()
{
    if ( pWindow1 )
    {
        pWindow1->Show();
        return true;
    }

    return false;
}

void CActionBarHandler::Close()
{
    if ( pWindow1 )
    {
        pWindow1->Hide();
        pWindow1->SetSelected( FALSE );
    }
}

bool CActionBarHandler::IsOpen()
{
    return pWindow1 ? pWindow1->IsOpen() : false;
}

void CActionBarHandler::OnToggleLock()
{
    pWindow1->ToggleCanMove();

    auto pLockButton = pWindow1->GetElement<UI::CheckBox>( 101 );
    pLockButton->SetSelected( !pWindow1->CanMove() );
}

void CActionBarHandler::SetLock( bool bLock )
{
    if ( bLock )
        pWindow1->SetCanMove( FALSE );
    else
        pWindow1->SetCanMove( TRUE );

    auto pLockButton = pWindow1->GetElement<UI::CheckBox>( 101 );
    pLockButton->SetSelected( !pWindow1->CanMove() );
}

void CActionBarHandler::OnSelectTab( int iTabID )
{
    SKILLMANAGERHANDLER->ResetSkillsShortcut();

    iSelectedTab = iTabID;

    for ( const auto & slot : vSlots[iSelectedTab] )
    {
        if ( slot.bActive && slot.iSkillID > 0 )
            SKILLMANAGERHANDLER->SetSkillShortcut( slot.iSkillID, slot.iKey, slot.iSkillMousePos );
        else
            SKILLMANAGERHANDLER->SetSkillShortcut( 0, slot.iKey, 0 );
    }
}

void CActionBarHandler::OnSwapMode( bool bSwap )
{
    pWindow1->SetWidth( bSwap ? WINDOW_ACTIONBAR_HEIGHT : WINDOW_ACTIONBAR_WIDTH );
    pWindow1->SetHeight( bSwap ? WINDOW_ACTIONBAR_WIDTH : WINDOW_ACTIONBAR_HEIGHT );

    pWindow1->SetImage( bSwap ? pImageActionBarVertical : pImageActionBarHorizontal );

    //Invert Width Height
    if ( bSwap )
    {
        for ( int iTabID = 0; iTabID < 3; iTabID++ )
        {
            UI::Window_ptr pSlotsWindow = pWindow1->GetElement<UI::Window>( iTabID + 500 );
            pSlotsWindow->SetPosition( 0, 38 );
            pSlotsWindow->SetWidth( pWindow1->GetWidth() );
            pSlotsWindow->SetHeight( pWindow1->GetHeight() );

            int iIndex = 0;
            for ( int i = 0; i < iNumRows; i++ )
            {
                for ( int j = 0; j < iNumColumns; j++ )
                {
                    const int iWidth = 41;
                    const int iHeight = 41;
                    const int iX = (iWidth + 1) * i;
                    const int iY = (iHeight + 1) * j;

                    UI::Window_ptr pSlotWindow = pSlotsWindow->GetElement<UI::Window>( iIndex + 750 );
                    pSlotWindow->SetBox( Rectangle2D( iX + WINDOW_ACTIONBAR_PADDING_SLOTX, iY + WINDOW_ACTIONBAR_PADDING_SLOTX, iWidth, iHeight ) );

                    iIndex++;
                }
            }

            UI::CheckBox_ptr pTabButton = pWindow1->GetElement<UI::CheckBox>( iTabID );
            pTabButton->SetPosition( WINDOW_ACTIONBAR_PADDING_SLOTX, (14 * iTabID) + 4 );
        }

        UI::CheckBox_ptr pLockButton = pWindow1->GetElement<UI::CheckBox>( 101 );
        pLockButton->SetPosition( 53, 4 );

        UI::Button_ptr pSwapButton = pWindow1->GetElement<UI::Button>( 102 );
        pSwapButton->SetPosition( 72, 25 );

        UI::Button_ptr pCloseButton = pWindow1->GetElement<UI::Button>( 100 );
        pCloseButton->SetPosition( 70, 5 );

        auto sWindowRect = pWindow1->GetRect();
        pWindow1->SetPosition( sWindowRect.iX + 334, sWindowRect.iY + 6 );
    }
    else
    {
        for ( int iTabID = 0; iTabID < 3; iTabID++ )
        {
            UI::Window_ptr pSlotsWindow = pWindow1->GetElement<UI::Window>( iTabID + 500 );
            pSlotsWindow->SetPosition( 0, 0 );
            pSlotsWindow->SetWidth( pWindow1->GetWidth() );
            pSlotsWindow->SetHeight( pWindow1->GetHeight() );

            int iIndex = 0;
            for ( int i = 0; i < iNumRows; i++ )
            {
                for ( int j = 0; j < iNumColumns; j++ )
                {
                    const int iWidth = 41;
                    const int iHeight = 41;
                    const int iX = (iWidth + 1) * j;
                    const int iY = (iHeight + 1) * i;

                    UI::Window_ptr pSlotWindow = pSlotsWindow->GetElement<UI::Window>( iIndex + 750 );
                    pSlotWindow->SetBox( Rectangle2D( iX + WINDOW_ACTIONBAR_PADDING_SLOTX, iY + WINDOW_ACTIONBAR_PADDING_SLOTY, iWidth, iHeight ) );

                    iIndex++;
                }
            }

            UI::CheckBox_ptr pTabButton = pWindow1->GetElement<UI::CheckBox>( iTabID );
            pTabButton->SetPosition( 384, 50 + (14 * iTabID) );
        }

        UI::CheckBox_ptr pLockButton = pWindow1->GetElement<UI::CheckBox>( 101 );
        pLockButton->SetPosition( 387, 10 );

        UI::Button_ptr pSwapButton = pWindow1->GetElement<UI::Button>( 102 );
        pSwapButton->SetPosition( 406, 31 );

        UI::Button_ptr pCloseButton = pWindow1->GetElement<UI::Button>( 100 );
        pCloseButton->SetPosition( 404, 11 );

        auto sWindowRect = pWindow1->GetRect();
        pWindow1->SetPosition( sWindowRect.iX - 334, sWindowRect.iY - 6 );
    }

    bSwapMode = bSwap;
}

void CActionBarHandler::OnResolutionChanged()
{
    float x = (float)RESOLUTION_WIDTH / (float)iOldResolutionWidth;
    float y = (float)RESOLUTION_HEIGHT / (float)iOldResolutionHeight;

    pWindow1->SetPosition( (int)((float)pWindow1->GetX() * x), (int)((float)pWindow1->GetY() * y) );

    iOldResolutionWidth = RESOLUTION_WIDTH;
    iOldResolutionHeight = RESOLUTION_HEIGHT;
}

bool CActionBarHandler::CanUseItem( int iItemID )
{
    bool bResult = false;

    auto sItemID = ItemID( iItemID );

    switch ( sItemID.ToItemType() )
    {
        case ITEMTYPE_Premium1:
        case ITEMTYPE_Premium2:
        case ITEMTYPE_Premium3:
        case ITEMTYPE_Premium4:
        case ITEMTYPE_Premium5:
        case ITEMTYPE_Crystal:
        case ITEMTYPE_ForceOrb:
        case ITEMTYPE_EtherCore:
        case ITEMTYPE_Chocolate:
            bResult = true;
            break;

		default:
			break;
    }

    return bResult;
}

void CActionBarHandler::SetSkillSlot( int iKey, int iSkillID, int iMousePos, int iTab )
{
    int iTabSlot = iSelectedTab;

    if ( iTab != -1 )
        iTabSlot = iTab;

    for ( auto & slot : vSlots[iTabSlot] )
    {
        if ( slot.bActive && slot.iSkillID == iSkillID )
        {
            slot.bActive = false;
            slot.iSkillID = -1;
            break;
        }
    }

    for ( auto & slot : vSlots[iTabSlot] )
    {
        if ( slot.iKey == iKey )
        {
            slot.bActive = true;
            slot.iSkillID = iSkillID;
            slot.iSkillMousePos = iMousePos;
            break;
        }
    }
}

void CActionBarHandler::OnMouseMove( CMouse * pcMouse )
{
    if ( Game::GetGameMode() != GAMEMODE_InGame )
        return;
    
    if ( pWindow1 )
        pWindow1->OnMouseMove( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
}

BOOL CActionBarHandler::OnMouseClick( CMouse * pcMouse )
{
    if ( Game::GetGameMode() != GAMEMODE_InGame )
        return FALSE;

    BOOL bRet = FALSE;
    
    extern DWORD TIME_SKILL_LOOP_TIMEOUT;
    if ( (pcMouse->GetEvent() == EMouseEvent::DoubleClickL) || (pcMouse->GetEvent() == EMouseEvent::DoubleClickR) )
    {
        if ( pcMouse->IsNoMove() == FALSE )
        {
            if ( SKILL_RIGHTSIDE && (pcMouse->GetEvent() == EMouseEvent::DoubleClickR) )
            {
                ENABLE_SKILL_LOOP = TRUE;

                SELECTED_SKILL_MOUSE_INDEX = 2;

                TIME_SKILL_LOOP_TIMEOUT = TICKCOUNT + 500;
            }
            else if ( SKILL_LEFTSIDE && (pcMouse->GetEvent() == EMouseEvent::DoubleClickL) )
            {
				ENABLE_SKILL_LOOP = TRUE;

				SELECTED_SKILL_MOUSE_INDEX = 1;

				TIME_SKILL_LOOP_TIMEOUT = TICKCOUNT + 500;
			}
        }
    }
    else if ( TIME_SKILL_LOOP_TIMEOUT < TICKCOUNT )
        ENABLE_SKILL_LOOP = FALSE;

    if ( pWindow1 )
    {
        if ( !pWindow1->IsOpen() )
            return FALSE;

        EMouseEvent iEvent = pcMouse->GetEvent();
        iCurrentMouseEvent = iEvent;

        dwLastMouseEventTime = TICKCOUNT + 100;
        if ( (iEvent == EMouseEvent::ClickUpL) || (iEvent == EMouseEvent::ClickUpR) )
			dwLastMouseEventTime = TICKCOUNT;

        if ( pWindow1->GetRect().Inside( pcMouse->GetPosition() ) == false )
            iOldMouseEvent = iEvent;
        else
        {
            iOldMouseEvent = (EMouseEvent)0;

            bRet = TRUE;
        }

        if ( bRet || iEvent == EMouseEvent::ClickUpL )
        {
            return pWindow1->OnMouseClick( pcMouse, 0, 0, RESOLUTION_WIDTH, RESOLUTION_HEIGHT, 0, 0 );
        }
    }

    return bRet;
}

BOOL CActionBarHandler::OnKeyDown( CKeyboard * pcKeyboard )
{
    BOOL bRet = FALSE;

    if ( Game::GetGameMode() != GAMEMODE_InGame )
        return FALSE;

    if ( pcKeyboard->GetEvent() != EKeyboardEvent::KeyDown )
        return FALSE;

    if ( MESSAGEBOX->IsOpen() || HUDHANDLER->IsWindowOldHUDOpen() )
        return FALSE;

    if ( CHATBOX->IsInputFieldActive() )
        return FALSE;

    if ( KEYBOARDHANDLER->IsKeyDown( VK_LCONTROL ) || KEYBOARDHANDLER->IsKeyDown( VK_RCONTROL ) )
        return FALSE;

    if ( toupper( pcKeyboard->GetKey() ) == 'B' )
    {
        if ( !pWindow1->IsOpen() )
            Open();
        else
            Close();
    }

    if ( pWindow1 )
        bRet = pWindow1->OnKeyPress( pcKeyboard );

    if ( SKILLMANAGERHANDLER->SkillKeyDown( pcKeyboard->GetKey() ) )
    {
        bRet = TRUE;
    }

    if ( UNITDATA->GetAnimation() != ANIMATIONTYPE_Die )
    {
        for ( auto & slot : vSlots[iSelectedTab] )
        {
            if ( slot.pSlot && slot.bActive )
            {
                if ( slot.iKey == pcKeyboard->GetKey() )
                {
                    //Is item?
                    if ( slot.sItemData.bValid )
                    {
                        if ( slot.iQuantity > 0 )
                            ITEMHANDLER->RightClickSimulate( slot.sItemData.sBaseItemID.ToItemID() );
                        else
                            TITLEBOX( "There is no Item found on this action shortcut!" );
                    }
                    else if ( slot.iSkillID > 0 )
                    {
                        SKILLMANAGERHANDLER->SetUseSkillSlot( slot.iSkillID, slot.iSkillMousePos == 1 ? SKILLTYPE_Left : SKILLTYPE_Right );

                        if ( (dwLastPressedKey == pcKeyboard->GetKey()) && ((dwLastPressedKeyTime + 300) > TICKCOUNT) )
                        {
                            //if ( UNITDATA_FOCUS && (UNITDATA_FOCUS->bActive) && (UNITDATA_FOCUS->sCharacterData.iType == CHARACTERTYPE_Monster) )
                            ENABLE_SKILL_LOOP = TRUE;

                            if ( slot.iSkillMousePos == 2 )
                            {
                                if ( (UNITDATA->GetAnimation() != ANIMATIONTYPE_Attack) && (UNITDATA->GetAnimation() != ANIMATIONTYPE_Potion) &&
                                    (UNITDATA->GetAnimation() != ANIMATIONTYPE_Skill) )
                                {
                                    if ( SKILL_RIGHTSIDE )
                                    {
                                        SELECTED_SKILL_MOUSE_INDEX = 2;
                                        OpenPlaySkill( SKILL_RIGHTSIDE );
                                    }
                                }
                            }
                        }
                    }
                    else if ( slot.iMount > 0 )
                    {
                        MOUNTHANDLER->GetWindow()->UseMountByID( slot.iMount );
                    }
                    /*else if ( slot.iTaunt > 0 )
                    {
                        TAUNTHANDLER->UseTauntAnimationByButtonIndex( slot.iTaunt );
                    }*/

                    bRet = TRUE;

                    break;
                }
            }
        }
    }

    dwLastPressedKey = pcKeyboard->GetKey();
    dwLastPressedKeyTime = TICKCOUNT;

    return bRet;
}

void CActionBarHandler::BuildWindow()
{
    iOldResolutionWidth = RESOLUTION_WIDTH;
    iOldResolutionHeight = RESOLUTION_HEIGHT;

    pImageActionBarHorizontal = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\background_horizontal.png" );
    pImageActionBarVertical = UI::ImageLoader::LoadImage( "game\\images\\actionbar\\background_vertical.png" );

    pWindow1->SetPosition( (RESOLUTION_WIDTH >> 1) - (pWindow1->GetRect().iWidth >> 1) + 20, RESOLUTION_HEIGHT - 220 );
    pWindow1->SetImage( pImageActionBarHorizontal );
    pWindow1->SetCanMove( TRUE );
    pWindow1->SetRenderBackgroundWhenHover( TRUE );
    pWindow1->SetMoveBox( Rectangle2D( 0, 0, pWindow1->GetRect().iWidth - 50, pWindow1->GetRect().iHeight ) );

    //if ( GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.x != -1 && GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.y != -1 )
    //{
    //    if ( GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.x >= 0 && GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.x < RESOLUTION_WIDTH &&
    //        GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.y >= 0 && GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.y < RESOLUTION_HEIGHT )
    //    {
    //        pWindow1->SetPosition( GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.x, GAMECONFIG->GetUserInterfaceSettings().sActionBarPosition.y );
    //    }
    //}

    UI::Group_ptr pGroup = std::make_shared<UI::Group>();
    pWindow1->AddElement( pGroup );

    for ( int iTabID = 0; iTabID < 3; iTabID++ )
    {
        UI::Window_ptr pSlotsWindow = std::make_shared<UI::Window>( Rectangle2D( 0, 0, pWindow1->GetWidth(), pWindow1->GetHeight() ) );
        pSlotsWindow->SetID( iTabID + 500 );
        pSlotsWindow->Hide();
        pWindow1->AddElement( pSlotsWindow );

        int iIndex = 0;
        for ( int i = 0; i < iNumRows; i++ )
        {
            for ( int j = 0; j < iNumColumns; j++ )
            {
                const int iWidth = 41;
                const int iHeight = 41;
                const int iX = (iWidth + 1) * j;
                const int iY = (iHeight + 1) * i;

                UI::Window_ptr pSlotWindow = std::make_shared<UI::Window>( Rectangle2D( iX + WINDOW_ACTIONBAR_PADDING_SLOTX, iY + WINDOW_ACTIONBAR_PADDING_SLOTY, iWidth, iHeight ) );
                pSlotWindow->SetID( iIndex + 750 );

                UI::Button_ptr pSlotButton = std::make_shared<UI::Button>( Rectangle2D( 0, 0, iWidth, iHeight ) );
                pSlotButton->SetID( iIndex + 1000 );
                pSlotButton->SetActiveImage( UI::ImageLoader::LoadActiveImage( "game\\images\\actionbar\\slot.png", "game\\images\\actionbar\\slot_.png" ) );
                pSlotWindow->AddElement( pSlotButton );

                //Create Slot
                ActionBarSlot sActionBarSlot;
                sActionBarSlot.bActive = false;
                sActionBarSlot.iID = iIndex;
                sActionBarSlot.iSkillID = -1;
                sActionBarSlot.iTaunt = -1;
                sActionBarSlot.iMount = -1;
                sActionBarSlot.pSlot = pSlotWindow;
                sActionBarSlot.strLabel = vLabels[iIndex].second;
                sActionBarSlot.iKey = vLabels[iIndex].first;
                sActionBarSlot.iQuantity = 0;
                sActionBarSlot.sItemData.bValid = false;
                vSlots[iTabID].push_back( sActionBarSlot );

                pSlotsWindow->AddElement( pSlotWindow );
                iIndex++;
            }
        }

        UI::CheckBox_ptr pTabButton = std::make_shared<UI::CheckBox>( Rectangle2D( 384, 50 + (14 * iTabID), 34, 12 ) );
        pTabButton->SetID( iTabID );
        pTabButton->SetImage( UI::ImageLoader::LoadImage( "game\\images\\actionbar\\tab%d.png", iTabID + 1 ), UI::ImageLoader::LoadImage( "game\\images\\actionbar\\tab%d_.png", iTabID + 1 ) );
        pTabButton->SetEvent( UI::Event::Build( std::bind( &CActionBarHandler::OnSelectTab, this, iTabID ) ) );
        pTabButton->SetGroup( pGroup );
        pTabButton->SetSelected( iTabID == 0 ? TRUE : FALSE );
        pTabButton->Hide();
        pWindow1->AddElement( pTabButton );

        if ( iTabID == 0 )
            pSlotsWindow->Show();
    }

    //Swap Button
    UI::Button_ptr pSwapButton = std::make_shared<UI::Button>( Rectangle2D( 406, 31, 12, 15 ) );
    pSwapButton->SetID( 102 );
    pSwapButton->SetActiveImage( UI::ImageLoader::LoadActiveImage( "game\\images\\actionbar\\btnswap.png", "game\\images\\actionbar\\btnswap_.png" ) );
    pSwapButton->SetEvent( UI::Event::Build( std::bind( &CActionBarHandler::OnToggleSwap, this ) ) );
    pSwapButton->Hide();
    pWindow1->AddElement( pSwapButton );

    //Lock Button
    UI::CheckBox_ptr pLockButton = std::make_shared<UI::CheckBox>( Rectangle2D( 387, 10, 12, 15 ) );
    pLockButton->SetID( 101 );
    pLockButton->SetImage( UI::ImageLoader::LoadImage( "game\\images\\actionbar\\btnlock.png" ), UI::ImageLoader::LoadImage( "game\\images\\actionbar\\btnlock_.png" ) );
    pLockButton->SetEvent( UI::Event::Build( std::bind( &CActionBarHandler::OnToggleLock, this ) ) );
    pLockButton->SetSelected( FALSE );
    pLockButton->Hide();
    pWindow1->AddElement( pLockButton );

    //Close Button
    UI::Button_ptr pCloseButton = std::make_shared<UI::Button>( Rectangle2D( 404, 11, 14, 14 ) );
    pCloseButton->SetID( 100 );
    pCloseButton->SetActiveImage( UI::ImageLoader::LoadActiveImage( "game\\images\\actionbar\\btnclose.png", "game\\images\\actionbar\\btnclose_.png" ) );
    pCloseButton->SetEvent( UI::Event::Build( std::bind( &CActionBarHandler::Close, this ) ) );
    pCloseButton->Hide();
    pWindow1->AddElement( pCloseButton );
}

void CActionBarHandler::OnToggleSwap()
{
    OnSwapMode( !bSwapMode );
}
