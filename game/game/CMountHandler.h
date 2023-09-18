#pragma once

#include "CMountWindowHandler.h"

enum EMountMessage : int
{
    MOUNTMESSAGE_None,
    MOUNTMESSAGE_ErrorEnterNPC,
    MOUNTMESSAGE_ErrorWalk,
    MOUNTMESSAGE_ErrorBC,

};

struct MountRarityData
{
    EMountType          eMountType;
    EMountRareType      eMountRareType;

    MountRarityData( EMountType _eMountType, EMountRareType _eMountRareType )
    {
        eMountType = _eMountType;
        eMountRareType = _eMountRareType;
    };

    ~MountRarityData() {};
};

class CMountInstance
{
public:
    CMountInstance();
    virtual ~CMountInstance();

    int                 GetID() { return iID; }
    void                SetID( int _iID ) { iID = _iID; };

    int                 GetOwnerID() { return iOwnerID; }
    void                SetOwnerID( int _iOwnerID ) { iOwnerID = _iOwnerID; }

    EMountType          GetMountType() { return eMountType; }
    void                SetMountType( EMountType _eMountType ) { eMountType = _eMountType; }

    EMountRareType      GetMountRareType() { return eMountRareType; }
    void                SetMountRareType( EMountRareType _eMountRareType ) { eMountRareType = _eMountRareType; }

    UnitData            * GetUnitData() { return pcUnitData; };
    void                SetUnitData( UnitData * _pcUnitData ) { pcUnitData = _pcUnitData; }

private:
    EMountType          eMountType;
    EMountRareType      eMountRareType;

    int                 iID = -1;

    int                 iOwnerID = -1;

    UnitData            * pcUnitData = nullptr;
};


class CMountHandler
{
public:
    CMountHandler();
    virtual ~CMountHandler();

    CMountWindowHandler * GetWindow() { return pcWindow; }

    static void		    SetCharacterAnimation( ECharacterClass eCharacterClass, EMountType eMountType, EAnimationType eAnimationType, int iCharacterAnimationID );

    void                Init();

    bool                AddMount( UnitData * pcUnitData, EMountType eMountType );

    bool                RemoveMount( UnitData * pcUnitData, UINT uCooldownTimeMS = 500, bool bReUseAnimation = true );

    void                Render( UnitData * pcUnitData );
    
    void                Update( UnitData * pcUnitData );

    void                Update( float fTime );

    void                UpdateAnimation( UnitData * pcUnitData, EAnimationType eAnimationType );

    int                 GetAnimationID( UnitData * pcUnitData, EAnimationType eAnimationType );

    void                OnRenderCharacter( UnitData * pcUnitData );

    bool                IsRiding( UnitData * pcUnitData );

    EMountType          GetMountType( UnitData * pcUnitData );
    int                 GetMountModelHeight( UnitData * pcUnitData );

    void                ShowMessage( EMountMessage iMessage );

    BOOL                OnKeyPress( CKeyboard * pcKeyboard );
    BOOL                OnKeyChar( CKeyboard * pcKeyboard );

    bool                CanMount( UnitData * pcUnitData );

    void                HandlePacket( PacketMountUserList * psPacket );

    void                HandlePacket( PacketNewMount * psPacket );

    void                SaveMounts();

    bool                HasMount( EMountType eMountType );

    int                 UpdateMovementSpeed();

    int                 UpdateEvade();

    EMountRareType      GetMountRareByType( EMountType eType );

    UINT                GetCooldownMount();
    UINT				GetCooldownMountPercent();
    void                SetCooldownMount( UINT uTimeMS );

private:
    friend class CMountWindowHandler;

    void                Mount();

    CMountWindowHandler * pcWindow = nullptr;

    std::vector<CMountInstance *> vInstances;

    std::vector<MountRarityData> vRarityData;

    UINT                iWheelID = 1;

    UnitData            * GetNewUnitData( const std::string strBodyFile, Point3D sPosition );

    CMountInstance      * GetInstanceByOwnerID( int iOwnerID );

    std::tuple<EXEMesh *, EXEMesh *>    GetMeshes( UnitData * pcUnitData, UnitData * pcUnitDataMount );

    const static MountModelPath  & GetMount( EMountType eMountType );

    const static MountCharacterAnimation                & GetCharacterAnimationID( EMountType eMountType, ECharacterClass eCharacterClass, EAnimationType eAnimationType );

    void                OnSpawnMount( UnitData * pcUnitData, UnitData * pcUnitDataMount, EMountType eMountType );

    void                ProcessPacket( PacketMountUserList * psPacket );
};

