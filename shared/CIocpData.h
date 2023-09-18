#pragma once

#include "CIocpConnectionManager.h"
#include "FNetHandlerImpl.h"

namespace IOCP
{

class CIocpData
{
public:
    CIocpData();
    ~CIocpData();

    INT64       GetNextID();

    HANDLE      sShutdownEvent;
    HANDLE      sIoCompletionPort;
    SOCKET      sSocket;

    CIocpConnectionManager  cConnectionManager;

    FNetHandlerImpl * pHandler;

    CIocpBase   cAcceptBase;

    LPFN_ACCEPTEX   pfnAcceptEx;

    UINT        iBufferSize;

private:
    std::mutex  sMutex;

    INT64       iID;
};

};