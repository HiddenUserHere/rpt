#include "stdafx.h"
#include "CFNetClientHandler.h"

CFNetClientHandler::CFNetClientHandler()
{
}

CFNetClientHandler::~CFNetClientHandler()
{
}

bool CFNetClientHandler::Init( const char * pszIP, const UINT iPort )
{
	UsePasswordChallenge();

	if ( USE_RAKNET_TCP_TYPE )
		return ConnectTCP( pszIP, iPort );

	return ConnectUDP( pszIP, iPort );
}

void CFNetClientHandler::Start()
{

}

void CFNetClientHandler::Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted )
{
	SendData( pcSocketData, psPacket, bEncrypted );
}

UINT CFNetClientHandler::OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived )
{
	if ( auto pcSocketData = SOCKETSERVER->GetSocketDataByID( iID ) )
	{
		UINT iTotalLength = vData.size();
		UINT iCurrentData = 0;

		while ( iCurrentData < iTotalLength )
		{
			WORD wPacketLength = *(WORD *)&vData.data()[iCurrentData + 4];

			if ( (wPacketLength < 8) || (wPacketLength > iTotalLength) )
			{
				WRITEDBG( "Invalid Packet Length: %d", wPacketLength );
				break;
			}

			PacketReceiving * p = new PacketReceiving;
			p->bDelete = TRUE;
			p->bInUse = TRUE;
			CopyMemory( p->baPacket, &vData.data()[iCurrentData + 4], wPacketLength );

			Packet * psPacket = (Packet *)p->baPacket;
			if ( pcSocketData->PacketEncrypted( psPacket ) )
				pcSocketData->DecryptPacket( psPacket );

			PostMessageA( SOCKETSERVER->GetHWND(), WM_SOCKETPACKET, (WPARAM)pcSocketData, (LPARAM)p );

			iCurrentData += wPacketLength + 4;
		}
	}

	return 0;
}

void CFNetClientHandler::OnClientDisconnect( INT64 iID, UINT iErrorCode )
{
}

void CFNetClientHandler::OnDisconnect( INT64 iID, UINT iErrorCode )
{
	if ( auto pcSocketData = SOCKETSERVER->GetSocketDataByID( iID ) )
	{
		WNDPROC_SOCKETCLOSE( SOCKETSERVER->GetHWND(), pcSocketData );
	}
}

void CFNetClientHandler::OnError( UINT iErrorCode )
{
}

void CFNetClientHandler::OnClose( UINT iErrorCode )
{
}
