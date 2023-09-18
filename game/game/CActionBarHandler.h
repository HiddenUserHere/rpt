#pragma once

struct ActionBarSlot
{
    int iID;
    bool bActive;
    int iSkillID;
    int iTaunt;
    int iMount;
    int iSkillMousePos;
    int iQuantity;
    ItemData sItemData;
    UI::Window_ptr pSlot;
    std::string strLabel;
    int iKey;
};

class CActionBarHandler
{
public:
    CActionBarHandler();
    ~CActionBarHandler();

    void RecordSaveData( ActionBarData & sData );
    void RestoreSaveData( ActionBarData & sData );

    void LoadCache();

    void SetSelectedTabID( int iID );
    int GetSelectedTabID() { return iSelectedTab; }

    BOOL Init();
    void Render();
    void Update( float fElapsedTime );

    bool Open();
    void Close();
    bool IsOpen();

    void OnToggleLock();
    void SetLock( bool bLock );
    bool IsLocked() { return pWindow1->CanMove() ? false : true; };
    
    void OnSelectTab( int iTabID );
    void OnSwapMode( bool bSwap );
    void OnResolutionChanged();

    bool CanUseItem( int iItemID );

    void SetSkillSlot( int iKey, int iSkillID, int iMousePos, int iTab = -1 );

    int GetSelectedSkill() { return iSelectedSkillIcon; }
    int GetSelectedTaunt() { return iSelectedTaunt; }
    int GetSelectedMount() { return iSelectedMount; }

    void OnMouseMove( CMouse * pcMouse );
    BOOL OnMouseClick( CMouse * pcMouse );

    BOOL OnKeyDown( CKeyboard * pcKeyboard );

    UI::Window_ptr GetWindow() { return pWindow1; }
private:

    const int                       WINDOW_ACTIONBAR_PADDING_SLOTX = 6;
    const int                       WINDOW_ACTIONBAR_PADDING_SLOTY = 8;

    const int                       WINDOW_ACTIONBAR_WIDTH = 424;
    const int                       WINDOW_ACTIONBAR_HEIGHT = 96;

    void BuildWindow();

    void OnToggleSwap();

    UI::Window_ptr pWindow1;

    EMouseEvent iCurrentMouseEvent = (EMouseEvent)0;
    DWORD dwLastMouseEventTime = 0;

    EMouseEvent iOldMouseEvent = (EMouseEvent)0;
    ItemData sSelectedItemData;
    int iSelectedSkillIcon = -1;
    int iSelectedTaunt = -1;
    int iSelectedMount = -1;
    int iSelectedTab = 0;
    std::array<std::vector<ActionBarSlot>, 3> vSlots;

    UI::Image * pImageSlotHover = nullptr;
    UI::Image * pImageLeftSkill = nullptr;
    UI::Image * pImageRightSkill = nullptr;
    UI::Image * pImageLeftRightSkill = nullptr;

    UI::Image * pImageActionBarHorizontal = nullptr;
    UI::Image * pImageActionBarVertical = nullptr;

    ActionBarData       sCacheData;

    bool bSwapMode = false;

    DWORD           dwLastPressedKey = 0;
    DWORD           dwLastPressedKeyTime = 0;
};
