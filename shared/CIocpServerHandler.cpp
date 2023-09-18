#include "stdafx.h"
#include "CIocpServerHandler.h"
#include "CIocpServer.h"

namespace IOCP
{

CIocpServerHandler::CIocpServerHandler()
{
}

CIocpServerHandler::~CIocpServerHandler()
{
}

void CIocpServerHandler::OnNewConnection( INT64 iID, const FNetConnectionInfo & cConnection )
{
    //for ( int i = 1; i <= 50; i++ )
    //{

    //    char szBuffer[512] = {0};

    //    sprintf_s( szBuffer, 512, "%d\n", i );


    //    std::vector<char> v( szBuffer, szBuffer + strlen( szBuffer ) );

    //    pcHandler->Send( iID, v );
    //}
}

UINT CIocpServerHandler::OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived )
{
    return 1;
}

void CIocpServerHandler::OnClientDisconnect( INT64 iID, UINT iErrorCode )
{
}

void CIocpServerHandler::OnDisconnect( INT64 iID, UINT iErrorCode )
{
}

void CIocpServerHandler::OnError( UINT iErrorCode )
{
}

void CIocpServerHandler::OnClose( UINT iErrorCode )
{
}

};