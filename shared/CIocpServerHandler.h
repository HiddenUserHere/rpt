#pragma once

#include "FNetHandlerImpl.h"
#include "CIocpImpl.h"

namespace IOCP
{

class CIocpServerHandler : public FNetHandlerImpl
{
public:
    CIocpServerHandler();

    ~CIocpServerHandler();



    void        OnNewConnection( INT64 iID, const FNetConnectionInfo & cConnection ) override;

    UINT        OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) override;

    void        OnClientDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnDisconnect( INT64 iID, UINT iErrorCode ) override;

    void        OnError( UINT iErrorCode ) override;

    void        OnClose( UINT iErrorCode ) override;

};

};