#pragma once
class CIocpGameHandler : public FNetHandlerImpl
{
public:
    CIocpGameHandler();
    ~CIocpGameHandler();

    void        Init( SocketData * sd );
    void        UnInit();

    void			Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted = FALSE );

    UINT        OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) override;

    void        OnClientDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnError( UINT iErrorCode ) override;

    void        OnClose( UINT iErrorCode ) override;

    IOCP::CIocpClient * pcClient = nullptr;

    SocketData * pcSocketData = nullptr;
};

