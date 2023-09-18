#pragma once
class CFNetClientHandler : public FNetRakNetHandlerImpl
{

public:
    CFNetClientHandler();
    ~CFNetClientHandler();

    bool        Init( const char * pszIP, const UINT iPort );

    void        Start();

    void		Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted = FALSE );

    UINT        OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) override;

    void        OnClientDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnError( UINT iErrorCode ) override;

    void        OnClose( UINT iErrorCode ) override;

};