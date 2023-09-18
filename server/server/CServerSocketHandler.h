#pragma once


class CServerSocketHandler : public FNetHandlerImpl
{
private:

	static BOOL			bInit;

public:
	CServerSocketHandler();
	~CServerSocketHandler();

	bool			Init() override;

	static void			Send( User * pcUser, Packet * psPacket, BOOL bEncrypted = FALSE );
	static void			Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted = FALSE );

	void        OnNewConnection( INT64 iID, const FNetConnectionInfo & cConnection ) override;

	UINT        OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) override;

	void        OnClientDisconnect( INT64 iID, UINT iErrorCode ) override;

	void        OnDisconnect( INT64 iID, UINT iErrorCode ) override;

	void        OnError( UINT iErrorCode ) override;

	static void		Disconnect( SocketData * pcSocketData );

private:
	static IOCP::CIocpServer * pcServer;
};

