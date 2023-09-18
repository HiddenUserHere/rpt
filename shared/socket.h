#pragma once

#include "AYObfuscate.h"

class UserData;
class User;


struct SocketPasswordData
{
	const char				* pszChallenge;
	const char				* pszPassword;
};

static const SocketPasswordData SOCKET_PASSWORD_LIST[] =
{
	{ AY_OBFUSCATE( "h3g7DZVbcTxHr2xO" ), AY_OBFUSCATE( "u475MB7TqnpTzdIZ" ) },
	{ AY_OBFUSCATE( "0HOa10Qt0AnIT5b6" ), AY_OBFUSCATE( "AM9tBxjEYveReElt" ) },
	{ AY_OBFUSCATE( "O9tlWHbSuXy4kMBh" ), AY_OBFUSCATE( "23Im06fuxqIl1BUL" ) },
	{ AY_OBFUSCATE( "jbPzww4hgoGgrP1t" ), AY_OBFUSCATE( "JoSLHlyfVxbWnqBI" ) },
	{ AY_OBFUSCATE( "ESMhYWIAVIDnEBfu" ), AY_OBFUSCATE( "cLo8lTjaAH1Zwnd0" ) },
	{ AY_OBFUSCATE( "wFEVAtt6QN1iRLV0" ), AY_OBFUSCATE( "RHUmd04dd1mOGJ37" ) },
	{ AY_OBFUSCATE( "zyTARjJEVrJffX2e" ), AY_OBFUSCATE( "8MBbxZIg18nw2Nbm" ) },
	{ AY_OBFUSCATE( "yWkilmHYRIlDJKJJ" ), AY_OBFUSCATE( "Ui4n0rOxhEkGSb7a" ) },
	{ AY_OBFUSCATE( "IDXoBIXgxJbc82Mq" ), AY_OBFUSCATE( "RjvhEIktGfpd1YbA" ) },
	{ AY_OBFUSCATE( "QYM6qkeL4fczNz3l" ), AY_OBFUSCATE( "JMbnsSbFigcSozmS" ) },
	{ AY_OBFUSCATE( "nmbV33vfe81Cv3Oo" ), AY_OBFUSCATE( "Cedkzjkxln0OPyHR" ) },
	{ AY_OBFUSCATE( "sMyTchdswRyJftkw" ), AY_OBFUSCATE( "JDLPbDwFgx2m3aUa" ) },
	{ AY_OBFUSCATE( "Ou0F5njZUd2Eftmv" ), AY_OBFUSCATE( "6q8RgNEiNJJLWyxd" ) },
	{ AY_OBFUSCATE( "6JK7gWt3ppOmwoee" ), AY_OBFUSCATE( "LVYNA7e3yYSw7DxO" ) },
	{ AY_OBFUSCATE( "sNTcw9ndxMCxwtHI" ), AY_OBFUSCATE( "wuSzgUoITUg62K6M" ) },
	{ AY_OBFUSCATE( "BfYVdAwC3ZRw5jxm" ), AY_OBFUSCATE( "zHPoBi4Eflv2gBSX" ) },
	{ AY_OBFUSCATE( "oklReonDhSBKPWZf" ), AY_OBFUSCATE( "1K0MHZmT8B8xgCzf" ) },
	{ AY_OBFUSCATE( "0wkjkAjYxjMmGCIy" ), AY_OBFUSCATE( "GUDDXdXLyK2zVeje" ) },
	{ AY_OBFUSCATE( "8NEV7mb2W1ADlw4O" ), AY_OBFUSCATE( "391IrOVguarINxQS" ) },
	{ AY_OBFUSCATE( "22BzzkQkxADGUha4" ), AY_OBFUSCATE( "12xRZHLlMqgNI9MW" ) },
	{ AY_OBFUSCATE( "R6UKdyRXrPNQZLcj" ), AY_OBFUSCATE( "UIUI1wKdygW5kSbR" ) },
	{ AY_OBFUSCATE( "hHAlBCto0jyRXaGb" ), AY_OBFUSCATE( "XJaM1ZtheupARk7M" ) },
	{ AY_OBFUSCATE( "Fq5Ef7KTDBafFgi5" ), AY_OBFUSCATE( "42N3MfYwhOQsK1J1" ) },
	{ AY_OBFUSCATE( "2dRt3HddMllzeyWV" ), AY_OBFUSCATE( "ch5XLBETFSUpuPqc" ) },
	{ AY_OBFUSCATE( "As5qKGhmIEB49C2P" ), AY_OBFUSCATE( "GmgjzUwvzYjfQP3c" ) },
	{ AY_OBFUSCATE( "Z1WewQcbUhZ9CmBo" ), AY_OBFUSCATE( "xGJMErT2OCGoIAsM" ) },
	{ AY_OBFUSCATE( "6AzjauYS6CcUTAaJ" ), AY_OBFUSCATE( "k2VMgvJv8e7bO8bc" ) },
	{ AY_OBFUSCATE( "UGnrXB0GuQMU016L" ), AY_OBFUSCATE( "TYfkIUgDL1eFap8e" ) },
	{ AY_OBFUSCATE( "tcYFw7TS3ays1fO6" ), AY_OBFUSCATE( "yVFYdn4UMOUsaCCh" ) },
	{ AY_OBFUSCATE( "iJtBSIKBeOkm4NUH" ), AY_OBFUSCATE( "dNeS56d56X4pf4up" ) },
	{ AY_OBFUSCATE( "6Xb1RwS977z6edj5" ), AY_OBFUSCATE( "DUAmm0aLXQQmzoqO" ) },
	{ AY_OBFUSCATE( "SK6SYTLOJUzvZako" ), AY_OBFUSCATE( "CjmslvkkqqjzptcL" ) },
};


#define SIO_TCP_SET_ACK_FREQUENCY _WSAIOW(IOC_VENDOR,23) 

#define WNDPROC_SOCKETACCEPT( window, socket, address )		( SendMessageW( window, WM_SOCKETACCEPT, (WPARAM)socket, (LPARAM)address ) )
#define WNDPROC_SOCKETPACKET( window, socketdata, packet )	( PostMessageW( window, WM_SOCKETPACKET, (WPARAM)socketdata, (LPARAM)packet ) )
#define WNDPROC_SOCKETCLOSE( window, socketdata )			( PostMessageW( window, WM_SOCKETCLOSE, (WPARAM)socketdata, (LPARAM)NULL ) )

//Lag Grace = 10 minutes
#define MAX_LAGGRACE		1000 * 60 * 10
#define NUM_ENCKEYS			256

class SocketData;

extern const BOOL SOCKET_NODELAY;

extern const int RCVWND_SIZE;
extern const int SNDWND_SIZE;

const BOOL											IOCP_SERVER_TYPE = FALSE;
const BOOL											RAKNET_SERVER_TYPE = TRUE;
const BOOL											USE_RAKNET_TCP_TYPE = FALSE;

struct PacketSending
{
	union
	{
		BOOL	  bInUse;
		UINT	  uCount;
	};
	UINT		  uSize;
	BYTE		  baPacket[MAX_PKTSIZ + 4];
};

struct PacketSendingBulk
{
	union
	{
		BOOL	  bInUse;
		UINT	  uCount;
	};
	UINT		  uSize;
	BYTE		  baPacket[MAX_PKTSIZ];
};

struct PacketReceiving
{
	BOOL		  bInUse;
	BOOL		  bDelete;
	BYTE		  baPacket[MAX_PKTSIZ + 4];
};

struct PacketReserved
{
public:
	PacketReserved( SocketData * _pSocketData, PacketReceiving * _pPacketReceiving ) : pSocketData( _pSocketData ), pPacketReceiving( _pPacketReceiving ) {}

public:
	SocketData		* pSocketData;
	PacketReceiving	* pPacketReceiving;
};

struct PacketKeySet : Packet
{
	BYTE	  baKeySet[NUM_ENCKEYS];
};

struct PacketTime : Packet
{
	DWORD	  dwTime;
	ID		  iID;
	DWORD	  dwReserved1;
	DWORD	  dwReserved2;
};

struct PacketSync : Packet
{
	DWORD	  dwReserved1;
	DWORD	  dwTime;
	DWORD	  dwReserved2;
	DWORD	  dwReserved3;
};

struct PacketSyncLoginGame : Packet
{
	DWORD	  dwReserved1;
	DWORD	  dwTime;
	DWORD	  dwReserved2;
	DWORD	  dwReserved3;

	char				szAccountName[32];
	char				szPassword[32];

	bool				bGameRequest;
};

struct PacketPing : Packet
{
	DWORD	  dwTime;
	DWORD	  dwTick;

	int		  iPing;
};

static UINT StringIPToNumberIPV4( const std::string strIP )
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

	auto vIP = split( strIP, '.' );
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
};

static std::string NumberIPToStringIPV4( const UINT uIP )
{
	in_addr sAddr;
	sAddr.S_un.S_addr = uIP;

	char szIP[32] = { 0 };
	STRINGFORMAT( szIP, "%d.%d.%d.%d", uIP & 0xFF, (uIP >> 8) & 0xFF, (uIP >> 16) & 0xFF, (uIP >> 24) & 0xFF );

	return std::string( szIP );
};

class SocketData
{
public:
	//Attributes
	SocketData			* pcThis;

	//Status Flags
	bool				  bInUse;
	bool				  bConnecting;
	bool				  bConnected;
	bool				  bReserved;

	//This Socket Information
	SOCKET				  s;
	UserData			* ud;
	User				* u;

	//Remote Host
	char				  szIP[32];
	long				  lIP;
	int					  iPort;

	//Time last Received Packet
	DWORD				  dwTimeLastPacketReceived;

	//Receiving Packets
	UINT				  iDefReceivePacket;
	UINT				  iMaxReceivePacket;
	HANDLE				  hReceiveThread;
	HANDLE				  hReceiveThreadSignal;
	CMutex				* pcReceiveMutex;
	UINT				  iReceiveWheel;
	PacketReceiving		* psaPacketReceiving;

	//Sending Packets
	UINT				  iDefSendPacket;
	UINT				  iMaxSendPacket;
	HANDLE				  hSendEvent;
	HANDLE				  hSendThread;
	HANDLE				  hSendThreadSignal;
	CMutex				* pcSendMutex;
	UINT				  iSendWheel;
	UINT				  iSendingWheel;
	PacketSending		* psaPacketSending;
	BOOL				  bBlockSend;
	DWORD				  dwBulkThreadId;
	PacketSendingBulk	  sBulkPacket;

	//Encryption
	bool				  bKeySet;
	BYTE				  baKeySet[NUM_ENCKEYS];
	BYTE				  bNextKey;
	int					  iIPIndex;

	//Pinging
	bool				  bPing;
	DWORD				  dwPingTime;
	int					  iPing;

	//Counting
	UINT				  iCountIN;
	UINT				  iCountINB;

	UINT				  iCountOUT;
	UINT				  iCountOUTB;

	UINT				  iAmountIN;
	UINT				  iAmountINB;
	
	UINT				  iAmountOUT;
	UINT				  iAmountOUTB;

	DWORD				  dwTimeIN;

	//Error Handling
	int					  iRecvRet;
	int					  iRecvEC;
	int					  iSendRet;
	int					  iSendEC;

	BOOL				  bKeepAlive;

	//WSA
	WSABUF				  * psWSABuf;
	OVERLAPPED			  * psOverlapped;

	INT64				  iConnectionID = -1;
	bool				  bServerClient = false;

public:
	//Operations
	SocketData();
	~SocketData();

	void				  Prepare( UINT iMaxSendPacket, UINT iMaxReceivePacket );

	//Initializing
	void				  Init();
	void				  UnInit();

	//Connection
	bool				  Connect( const char * pszIP, int iPort );
	void				  Open( SOCKET s, sockaddr_in * addr );
	void				  Ping( DWORD dwTime );
	void				  Close();

	//Receiving
	PacketReceiving		* FreePacketReceiving();
	void				  FreePacketReceiving( PacketReceiving * pp );

	PacketReceiving		* ReceivePacket();

	//Sending
	PacketSending		* NextPacketSending();
	void				  FreePacketSending( PacketSending * pp );

private:
	//Inner Packet Sending Methods
	BOOL				  Send( BYTE * pbData, UINT uSize );
	BOOL				  SendData( BYTE * pbData, UINT uSize );

	//Automatic Bulk Sending
	PacketSending		* FindAutomaticBulk( UINT uSizeNeeded );

public:
	//Packet Sending Method that should only be used from Sending Thread
	BOOL				  Send( PacketSending * pp );

	//Packet Sending Methods that should be used from outside
	BOOL				  SendPacket( Packet * p, BOOL bEncrypted = FALSE );
	BOOL				  SendPacketBlank( PKTHDR iHeader, BOOL bEncrypted = FALSE );

	//Forced Bulk Sending
	BOOL				  StartBulkSend();
	BOOL				  FlushBulkSend();
	BOOL				  StopBulkSend();

	//Encryption
	void				  SendKeySet( BYTE bObfuscator );
	void				  ReceiveKeySet( PacketKeySet * p );

	BYTE				  GetNextIPIndex();
	BYTE				  ValIPIndex( BYTE iIndex, Packet * p );
	void				  EncryptPacket( Packet * p );
	void				  DecryptPacket( Packet * p );
	bool				  PacketEncrypted( Packet * p );

	//Getters & Setters
	void				  SetPing( int i ) { iPing = i; bPing = false; }
	int					  GetPing( DWORD dwTime );

	void				  UpdateTime( DWORD dwTime );
};