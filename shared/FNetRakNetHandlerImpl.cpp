#include "stdafx.h"
#include "FNetRakNetHandlerImpl.h"

static INT64 ID_FNET_IMPL_WHEEL = 0;

bool FNetRakNetHandlerImpl::ListenTCP( int iPort, int iMaxIncommingConnections )
{
	pTCP = new SLNet::TCPInterface;

	bIsServer = true;

	iPortInternal = iPort;

	//Only Single Thread is Supported
	UINT iNumThreads = 1;

	if ( pTCP->Start( iPort, iMaxIncommingConnections, iMaxIncommingConnections, THREAD_PRIORITY_HIGHEST ) )
	{
		for( UINT i = 0; i < iNumThreads; i++ )
			vWorkerThreads.push_back( CreateThread( NULL, KB64, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThread, this, 0, NULL ) );

		hWorkerConnectionsThread = CreateThread( NULL, KB64, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThreadConnections, this, 0, NULL );

		return true;
	}

	return false;
}


bool FNetRakNetHandlerImpl::ListenUDP( int iPort, int iMaxIncommingConnections, const std::string strPassword )
{
	pPeer = SLNet::RakPeerInterface::GetInstance();

	bIsServer = true;

	iPortInternal = iPort;

	//Only Single Thread is Supported
	UINT iNumThreads = 1;

	//Password Challenge
	if ( pTwoWayAuthentication == nullptr )
	{
		pTwoWayAuthentication = new SLNet::TwoWayAuthentication;
		pPeer->AttachPlugin( pTwoWayAuthentication );
	}

	for ( const auto & p : vPasswordChallenges )
	{
		pTwoWayAuthentication->AddPassword( p.first.c_str(), p.second.c_str() );
	}

	SLNet::SocketDescriptor sSocketDescriptor;
	sSocketDescriptor.port = (short)iPort;
	sSocketDescriptor.socketFamily = AF_INET;

	if ( pPeer->Startup( iMaxIncommingConnections, &sSocketDescriptor, 1 ) == SLNet::RAKNET_STARTED )
	{
		pPeer->SetOccasionalPing( true );
		pPeer->SetTimeoutTime( 10000, SLNet::UNASSIGNED_SYSTEM_ADDRESS );

		pPeer->SetMaximumIncomingConnections( iMaxIncommingConnections );

		if( strPassword.length() > 0 )
			pPeer->SetIncomingPassword( strPassword.c_str(), strPassword.length() );

		for ( UINT i = 0; i < iNumThreads; i++ )
			vWorkerThreads.push_back( CreateThread( NULL, KB64, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThread, this, 0, NULL ) );

		return true;
	}

	SLNet::RakPeerInterface::DestroyInstance( pPeer );
	pPeer = nullptr;

	if ( pTwoWayAuthentication )
	{
		delete pTwoWayAuthentication;
		pTwoWayAuthentication = nullptr;
	}

	return false;
}

bool FNetRakNetHandlerImpl::ConnectUDP( const char * pszIP, int iPort, const std::string strPassword, const UINT iConnectTimeOut )
{
	pPeer = SLNet::RakPeerInterface::GetInstance();

	iInternalID = ++ID_FNET_IMPL_WHEEL;

	bIsServer = false;

	SetConnected( false );

	iPortInternal = iPort;

	//Password Challenge
	if ( pTwoWayAuthentication == nullptr )
	{
		pTwoWayAuthentication = new SLNet::TwoWayAuthentication;
		pPeer->AttachPlugin( pTwoWayAuthentication );
	}

	for ( const auto & p : vPasswordChallenges )
	{
		pTwoWayAuthentication->AddPassword( p.first.c_str(), p.second.c_str() );
	}

	SLNet::SocketDescriptor sSocketDescriptor;
	sSocketDescriptor.port = 0;

	if ( pPeer->Startup( 1, &sSocketDescriptor, 1 ) == SLNet::RAKNET_STARTED )
	{
		const char * pszPassword = strPassword.length() > 0 ? strPassword.c_str() : nullptr;

		vWorkerThreads.push_back( CreateThread( NULL, MB1, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThread, this, 0, NULL ) );

		if ( pPeer->Connect( pszIP, iPort, pszPassword, strPassword.length() ) == SLNet::CONNECTION_ATTEMPT_STARTED )
		{			
			//Wait for connection
			UINT uTime = GetTickCount();
			while ( IsConnected() == false )
			{
				if ( (GetTickCount() - uTime) > iConnectTimeOut )
				{
					for ( auto & hThread : vWorkerThreads )
					{
						TerminateThread( hThread, 0 );
						CloseHandle( hThread );
					}

					vWorkerThreads.clear();

					SLNet::RakPeerInterface::DestroyInstance( pPeer );
					pPeer = nullptr;

					if ( pTwoWayAuthentication )
					{
						delete pTwoWayAuthentication;
						pTwoWayAuthentication = nullptr;
					}

					ClearPasswordChallenges();

					return false;
				}

				Sleep( 100 );
			}

			return true;
		}
	}

	SLNet::RakPeerInterface::DestroyInstance( pPeer );
	pPeer = nullptr;

	return false;
}

bool FNetRakNetHandlerImpl::ConnectTCP( const char * pszIP, int iPort )
{
	pTCP = new SLNet::TCPInterface;

	iInternalID = ++ID_FNET_IMPL_WHEEL;

	bIsServer = false;

	iPortInternal = iPort;

	if ( pTCP->Start( 0, 0, THREAD_PRIORITY_HIGHEST ) )
	{
		if ( pTCP->Connect( pszIP, iPort ) != SLNet::UNASSIGNED_SYSTEM_ADDRESS )
		{
			vWorkerThreads.push_back( CreateThread( NULL, MB1, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThread, this, 0, NULL ) );
			hWorkerConnectionsThread = CreateThread( NULL, MB1, (LPTHREAD_START_ROUTINE)&FNetRakNetHandlerImpl::WorkerThreadConnections, this, 0, NULL );

			return true;
		}
	}

	delete pTCP;
	pTCP = nullptr;

	return false;
}

void FNetRakNetHandlerImpl::UnInit()
{
	if ( pPeer )
	{
		pPeer->Shutdown( 1500 );
		SLNet::RakPeerInterface::DestroyInstance( pPeer );
		pPeer = nullptr;
	}

	if ( pTCP )
	{
		//Close all connections
		{
			std::lock_guard<std::mutex> lock( mConnectionMutex );

			for ( auto & p : mConnections )
			{
				pTCP->CloseConnection( p.second );
			}

			mConnections.clear();
		}

		pTCP->Stop();
		delete pTCP;
		pTCP = nullptr;
	}

	for ( auto & hThread : vWorkerThreads )
	{
		TerminateThread( hThread, 0 );
		CloseHandle( hThread );
	}

	vWorkerThreads.clear();

	if ( hWorkerConnectionsThread != INVALID_HANDLE_VALUE )
	{
		TerminateThread( hWorkerConnectionsThread, 0 );
		CloseHandle( hWorkerConnectionsThread );
		hWorkerConnectionsThread = INVALID_HANDLE_VALUE;
	}

	if ( pTwoWayAuthentication )
	{
		delete pTwoWayAuthentication;
		pTwoWayAuthentication = nullptr;
	}

	ClearPasswordChallenges();
}


void FNetRakNetHandlerImpl::SendData( SocketData * pcSocketData, Packet * psPacket, BOOL bEncrypted )
{
	if ( pcSocketData )
	{
		PacketSending * p = new PacketSending;
		CopyMemory( p->baPacket + 4, psPacket, psPacket->iLength );

		//RakNet Opcode
		p->baPacket[0] = FNET_PACKET_ID;

		Packet * psNewPacket = (Packet *)&p->baPacket[4];

		if ( psNewPacket->iHeader != PKTHDR_KeySet )
		{
			psNewPacket->iEncKeyIndex = 0;
			psNewPacket->iEncrypted = bEncrypted ? 1 : 0;
		}

		if ( psNewPacket->iEncrypted == 1 )
			pcSocketData->EncryptPacket( psNewPacket );

		SendInternal( pcSocketData->iConnectionID, (char *)p->baPacket, psNewPacket->iLength + 4 );

		DELET( p );
	}
}

void FNetRakNetHandlerImpl::ClearPasswordChallenges()
{
	vPasswordChallenges.clear();
}

void FNetRakNetHandlerImpl::AddPasswordChallenge( const std::string strChallenge, const std::string strPassword )
{
	if ( strChallenge.length() > 0 && strPassword.length() > 0 )
	{
		vPasswordChallenges.push_back( std::make_pair( strChallenge, strPassword ) );
	}
}

void FNetRakNetHandlerImpl::UsePasswordChallenge()
{
	ClearPasswordChallenges();

	for ( UINT i = 0; i < _countof( SOCKET_PASSWORD_LIST ); i++ )
	{
		AddPasswordChallenge( SOCKET_PASSWORD_LIST[i].pszChallenge, SOCKET_PASSWORD_LIST[i].pszPassword );
	}
}

void FNetRakNetHandlerImpl::WorkerThread( FNetRakNetHandlerImpl * pcThis )
{
	while ( true )
	{
		SLNet::Packet * pPacket = nullptr;
		while ( pPacket = pcThis->ReceiveInternal() )
		{
			switch ( pPacket->data[0] )
			{
				//Client Stuff
				case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_SUCCESS:
					pcThis->SetConnected( true );
					break;

				//Client Stuff
				case ID_CONNECTION_ATTEMPT_FAILED:
				case ID_NO_FREE_INCOMING_CONNECTIONS:
				case ID_CONNECTION_BANNED:
				case ID_INVALID_PASSWORD:
				case ID_DISCONNECTION_NOTIFICATION:
				case ID_CONNECTION_LOST:
					if ( pcThis->IsServer() == false )
						pcThis->SetConnected( false );

					pcThis->OnLostConnectionInternal( pcThis->GetID( pPacket ) );
					break;

				//Client Stuff
				case ID_TWO_WAY_AUTHENTICATION_INCOMING_CHALLENGE_FAILURE:
					pcThis->SetConnected( false );
					break;

				case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_FAILURE:
				case ID_TWO_WAY_AUTHENTICATION_OUTGOING_CHALLENGE_TIMEOUT:
					pcThis->OnChallengeFailInternal( pcThis->GetID( pPacket ), pPacket );
					break;

				case ID_NEW_INCOMING_CONNECTION:
				{
					FNetConnectionInfo sConnectionInfo;
					sConnectionInfo.strIP = pPacket->systemAddress.ToString( false );
					sConnectionInfo.iPort = pcThis->GetPort();
					sConnectionInfo.strHostName = pPacket->systemAddress.ToString( false );

					pcThis->OnNewConnectionInternal( pcThis->GetID( pPacket ), pPacket->systemAddress, sConnectionInfo );
				}
				break;

				case FNET_PACKET_ID:
					pcThis->OnReceive( pcThis->GetID( pPacket ), std::vector<char>( pPacket->data, pPacket->data + pPacket->length ), pPacket->length );
					break;

				default:
					break;
			}

			pcThis->OnHandledPacket( pPacket );
		}

		Sleep( 1 );
	}
}

void __stdcall FNetRakNetHandlerImpl::WorkerThreadConnections( FNetRakNetHandlerImpl * pcThis )
{
	while ( true )
	{
		if ( pcThis->pTCP )
		{
			if ( pcThis->IsServer() )
			{
				//New Connections
				{
					SLNet::SystemAddress sSystemAddress = pcThis->pTCP->HasNewIncomingConnection();
					if ( sSystemAddress != SLNet::UNASSIGNED_SYSTEM_ADDRESS )
					{
						FNetConnectionInfo sConnectionInfo;
						sConnectionInfo.strIP = sSystemAddress.ToString( false );
						sConnectionInfo.iPort = pcThis->GetPort();
						sConnectionInfo.strHostName = sSystemAddress.ToString( false );

						pcThis->OnNewConnectionInternal( pcThis->GetID( sSystemAddress ), sSystemAddress, sConnectionInfo );
					}
				}

				//Lost Connection
				{
					SLNet::SystemAddress sSystemAddress = pcThis->pTCP->HasLostConnection();
					if ( sSystemAddress != SLNet::UNASSIGNED_SYSTEM_ADDRESS )
					{
						FNetConnectionInfo sConnectionInfo;
						sConnectionInfo.strIP = sSystemAddress.ToString( false );
						sConnectionInfo.iPort = pcThis->GetPort();
						sConnectionInfo.strHostName = sSystemAddress.ToString( false );

						pcThis->OnLostConnectionInternal( pcThis->GetID( sSystemAddress ) );
					}
				}
			}
		}
		//Yield
		Sleep( 1 );
	}
}

SLNet::Packet * FNetRakNetHandlerImpl::ReceiveInternal()
{
	if( pPeer )
		return pPeer->Receive();
	else if( pTCP )
		return pTCP->Receive();

	return nullptr;
}

void FNetRakNetHandlerImpl::SendInternal( INT64 iID, char * pbData, UINT iLength )
{
	//UDP?
	if ( pPeer )
	{
		if ( bIsServer )
		{
			pPeer->Send( pbData, iLength, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, SLNet::RakNetGUID( iID ), false );
		}
		else
		{
			pPeer->Send( pbData, iLength, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true );
		}
	}
	else if ( pTCP )
	{
		if ( bIsServer )
		{
			SLNet::SystemAddress sSystemAddress;

			if( GetConnection( iID, sSystemAddress ) )
				pTCP->Send( pbData, iLength, sSystemAddress, false );
		}
		else
		{
			pTCP->Send( pbData, iLength, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true );
		}
	}
}

void FNetRakNetHandlerImpl::OnNewConnectionInternal( INT64 iID, SLNet::SystemAddress sSystemAddress, const FNetConnectionInfo & cConnection )
{
	std::lock_guard<std::mutex> lock( mConnectionMutex );

	INT64 iConnectionID = iID;

	if( pTCP )
		iConnectionID = SLNet::SystemAddress::ToInteger( sSystemAddress );

	mConnections[iConnectionID] = sSystemAddress;

	OnNewConnection( iConnectionID, cConnection );

	Challenge( iID );
}

void FNetRakNetHandlerImpl::OnLostConnectionInternal( INT64 iID )
{
	OnDisconnect( iID, 0 );

	if( bIsServer )
	{
		if( pTCP )
		{
			SLNet::SystemAddress sSystemAddress;

			if( GetConnection( iID, sSystemAddress ) )
				pTCP->CloseConnection( sSystemAddress );
		}
	}
	else
	{
		if( pTCP )
			pTCP->CloseConnection( SLNet::UNASSIGNED_SYSTEM_ADDRESS );
	}

	{
		std::lock_guard<std::mutex> lock( mConnectionMutex );

		if ( mConnections.find( iID ) != mConnections.end() )
		{
			mConnections.erase( iID );
		}
	}
}

void FNetRakNetHandlerImpl::Challenge( INT64 iID )
{
	if ( pTwoWayAuthentication )
	{
		if ( vPasswordChallenges.size() > 0 )
		{
			const auto & sChallenge = vPasswordChallenges[rand() % vPasswordChallenges.size()];

			pTwoWayAuthentication->Challenge( sChallenge.first.c_str(), SLNet::RakNetGUID(iID));
		}
	}
}

void FNetRakNetHandlerImpl::OnChallengeFailInternal( INT64 iID, SLNet::Packet * pPacket )
{
	//Ban
	if ( pPacket )
	{
		if ( pPacket->systemAddress != SLNet::UNASSIGNED_SYSTEM_ADDRESS )
		{
			pPeer->AddToBanList( pPacket->systemAddress.ToString( false ), FNET_DEFAULT_BAN_TIMEMS );

			pPeer->CloseConnection( pPacket->systemAddress, true );
		}
	}
}

bool FNetRakNetHandlerImpl::GetConnection( INT64 iID, SLNet::SystemAddress & sSystemAddress )
{
	std::lock_guard<std::mutex> lock( mConnectionMutex );

	if( mConnections.find( iID ) != mConnections.end() )
	{
		sSystemAddress = mConnections[iID];
		return true;
	}

	return false;
}

INT64 FNetRakNetHandlerImpl::GetID( SLNet::Packet * pPacket )
{
	INT64 iID = pPacket->guid.g;

	if ( bIsServer )
	{
		if ( pTCP )
			iID = GetID( pPacket->systemAddress );
	}
	else
	{
		iID = iInternalID;
	}

	return iID;
}

INT64 FNetRakNetHandlerImpl::GetID( const SLNet::SystemAddress & sSystemAddress )
{
	return (INT64)SLNet::SystemAddress::ToInteger( sSystemAddress );
}

void FNetRakNetHandlerImpl::OnHandledPacket( SLNet::Packet * pPacket )
{
	if ( pPeer )
		pPeer->DeallocatePacket( pPacket );
	else if ( pTCP )
		pTCP->DeallocatePacket( pPacket );
}
