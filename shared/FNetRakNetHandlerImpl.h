#pragma once

#include "FNetConnectionInfo.h"
#include "FNetHandlerImpl.h"
#include "socket.h"

//RakNet
#include <slikenet/MessageIdentifiers.h>

#include <slikenet/peerinterface.h>
#include <slikenet/TCPInterface.h>
#include <slikenet/statistics.h>
#include <slikenet/types.h>
#include <slikenet/BitStream.h>
#include <slikenet/sleep.h>
#include <slikenet/TwoWayAuthentication.h>
#include <map>
#include <vector>
#include <mutex>

#ifdef _DEBUG
#pragma comment(lib, "SLikeNetLibStatic_d.lib")
#else
#pragma comment(lib, "SLikeNetLibStatic.lib")
#endif

class FNetRakNetHandlerImpl : public FNetHandlerImpl
{
private:
    UINT				iPortInternal = 0;

    INT64   	        iInternalID = 0;

    bool				bIsServer = false;

    SLNet::RakPeerInterface * pPeer = nullptr;
    SLNet::TCPInterface * pTCP = nullptr;

    std::map<INT64, SLNet::SystemAddress> mConnections;

    std::vector<HANDLE> vWorkerThreads;
    HANDLE              hWorkerConnectionsThread = INVALID_HANDLE_VALUE;

    std::mutex 		    mConnectionMutex;

    SLNet::TwoWayAuthentication * pTwoWayAuthentication = nullptr;
    std::vector<std::pair<std::string, std::string>> vPasswordChallenges;

    bool                bIsConnected = false;


public:

    static const UINT   FNET_DEFAULT_MULTI_THREADS = 2;
    static const UINT   FNET_DEAULT_CONNECT_TIMEOUT = 10000;

    static const UINT   FNET_PACKET_ID = ID_USER_PACKET_ENUM;

    //5 Minutes
    static const UINT   FNET_DEFAULT_BAN_TIMEMS = (60 * 1000 * 5);


    INT64               GetInternalID() { return iInternalID; };

    bool                ListenTCP( int iPort, int iMaxIncommingConnections );
    bool                ListenUDP( int iPort, int iMaxIncommingConnections, const std::string strPassword = "" );

    bool                ConnectUDP( const char * pszIP, int iPort, const std::string strPassword = "", const UINT iConnectTimeOut = FNET_DEAULT_CONNECT_TIMEOUT );
    bool                ConnectTCP( const char * pszIP, int iPort );

    void				UnInit() override;

    void                SendData( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted );

    void                ClearPasswordChallenges();
    void                AddPasswordChallenge( const std::string strChallenge, const std::string strPassword );

    void                UsePasswordChallenge();

private:
    static void WINAPI	WorkerThread( FNetRakNetHandlerImpl * pcThis );
    static void WINAPI	WorkerThreadConnections( FNetRakNetHandlerImpl * pcThis );

    SLNet::Packet *     ReceiveInternal();
    void                SendInternal( INT64 iID, char * pbData, UINT iLength );
    void                OnNewConnectionInternal( INT64 iID, SLNet::SystemAddress sSystemAddress, const FNetConnectionInfo & cConnection );
    void                OnLostConnectionInternal( INT64 iID );

    void                Challenge( INT64 iID );
    void                OnChallengeFailInternal( INT64 iID, SLNet::Packet * pPacket );

    bool                GetConnection( INT64 iID, SLNet::SystemAddress & sSystemAddress );

    INT64			    GetID( SLNet::Packet * pPacket );
    INT64			    GetID( const SLNet::SystemAddress & sSystemAddress );

    UINT			    GetPort() { return iPortInternal; };

    void			    OnHandledPacket( SLNet::Packet * pPacket );

    bool                IsServer() { return bIsServer; };

    bool				IsUsingChallenges() { return vPasswordChallenges.size() > 0; }

    bool                IsConnected() { return bIsConnected; };
    void                SetConnected( bool bConnected ) { bIsConnected = bConnected; };
};
