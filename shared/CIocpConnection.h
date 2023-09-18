#pragma once

#include "CIocpBase.h"
#include "CIocpSendQueue.h"

namespace IOCP
{

class CIocpConnection
{
public:
    CIocpConnection( SOCKET _s, INT64 _iID, UINT _iBufferSize, const FNetConnectionInfo sConnectionInfo );
    ~CIocpConnection();

    bool        CloseReceive();

    std::shared_ptr<CIocpBase>  CreateSend();

    bool        HasOutstanding();

    const FNetConnectionInfo& GetConnectionInfo() { return sConnectionInfo; }

    bool                IsFirstPacket() { return bReceivedFirstPacket == false; }
    void                SetFirstPacket() { bReceivedFirstPacket = true; }

    SOCKET      sSocket;

    INT64       iID;

    UINT        iBufferSize;

    LONG        iDisconnecting;
    LONG        iSendClosing;
    LONG        iReceiveClosed;

    CIocpBase   sReceive;
    CIocpBase   sDisconnect;

    CIocpSendQueue  cSendQueue;

    FNetConnectionInfo    sConnectionInfo;

    std::mutex  sMutexReceive;
    std::mutex  sMutexSend;

    bool 	  bReceivedFirstPacket;
};

};