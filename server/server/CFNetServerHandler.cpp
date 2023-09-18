#include "stdafx.h"
#include "CFNetServerHandler.h"

static std::vector<std::string> split_local( const std::string & s, char delim )
{
	std::stringstream ss( s.c_str() );
	std::string item;

	std::vector<std::string> tokens;

	while ( std::getline( ss, item, delim ) )
		tokens.push_back( item );

	return tokens;
}

UINT StringIPToNumberIPV4_FNet( const std::string strIP )
{
	struct SIPDataConverter
	{
		union
		{
			BYTE baIP[4];
			UINT uIP;
		};
	};

	SIPDataConverter sIP;
	sIP.uIP = 0;

	auto vIP = split_local( strIP, '.' );
	if ( vIP.size() == 4 )
	{
		for ( int i = 0; i < 4; i++ )
		{
			if ( (atoi( vIP[i].c_str() ) < 0) || (atoi( vIP[i].c_str() ) > 255) )
			{
				sIP.uIP = 0;
				break;
			}

			sIP.baIP[i] = atoi( vIP[i].c_str() );
		}
	}
	vIP.clear();

	return sIP.uIP;
}

CFNetServerHandler::CFNetServerHandler()
{
}

CFNetServerHandler::~CFNetServerHandler()
{
}

bool CFNetServerHandler::Init()
{
	UsePasswordChallenge();

	if( USE_RAKNET_TCP_TYPE )
		return ListenTCP( SERVER_PORT, 1024 );

	return ListenUDP( SERVER_PORT, 1024 );
}

void CFNetServerHandler::OnNewConnection(INT64 iID, const FNetConnectionInfo & cConnection)
{
	auto psAccept = new SocketServerAccept;
	ZeroMemory( &psAccept->sAddr, sizeof( sockaddr_in ) );
	psAccept->sAddr.sin_family = AF_INET;
	psAccept->sAddr.sin_addr.S_un.S_addr = StringIPToNumberIPV4_FNet( cConnection.strIP );
	psAccept->sAddr.sin_port = htons( (WORD)SERVER_PORT );
	psAccept->iID = iID;

	SendMessageA( SOCKETSERVER->GetHWND(), WM_SOCKETACCEPT, 0, (LPARAM)psAccept );
}

UINT CFNetServerHandler::OnReceive(INT64 iID, std::vector<char> vData, UINT iLengthReceived)
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

			iCurrentData += (wPacketLength + 4);
		}
	}

	return 0;
}

void CFNetServerHandler::OnClientDisconnect(INT64 iID, UINT iErrorCode)
{
}

void CFNetServerHandler::OnDisconnect(INT64 iID, UINT iErrorCode)
{
	if ( auto pcSocketData = SOCKETSERVER->GetSocketDataByID( iID ) )
		WNDPROC_SOCKETCLOSE( SOCKETSERVER->GetHWND(), pcSocketData );
}

void CFNetServerHandler::OnError(UINT iErrorCode)
{
}

void CFNetServerHandler::OnClose(UINT iErrorCode)
{
}

void CFNetServerHandler::Send( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted )
{
	SendData( pcSocketData, psPacket, bEncrypted );
}
