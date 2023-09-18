#pragma once

#include "FNetConnectionInfo.h"


class FNetHandlerImpl
{
public:
    virtual bool        Init() { return true; };

    virtual void        UnInit() {};

    virtual void        OnNewConnection( INT64 iID, const FNetConnectionInfo & cConnection ) {};

    virtual UINT        OnReceive( INT64 iID, std::vector<char> vData, UINT iLengthReceived ) { return iLengthReceived; };

    virtual void        OnClientDisconnect( INT64 iID, UINT iErrorCode ) {};

    virtual void        OnDisconnect( INT64 iID, UINT iErrorCode ) {};

    virtual void        OnError( UINT iErrorCode ) {};

    virtual void        OnClose( UINT iErrorCode ) {};
};
