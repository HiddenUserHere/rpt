#pragma once

#include "FNetRakNetHandlerImpl.h"

class CFNetServerHandler : public FNetRakNetHandlerImpl
{
public:
	CFNetServerHandler();
	virtual ~CFNetServerHandler();

	bool					Init() override;

	void					OnNewConnection( INT64 iID, const FNetConnectionInfo & cConnection ) override;

	UINT					OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) override;

	void					OnClientDisconnect( INT64 iID, UINT iErrorCode ) override;

	void					OnDisconnect( INT64 iID, UINT iErrorCode ) override;

	void					OnError( UINT iErrorCode ) override;

	void					OnClose( UINT iErrorCode ) override;

	void					Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted );
};

