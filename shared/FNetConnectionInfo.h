#pragma once

struct FNetConnectionInfo
{
    std::string     strIP;
    std::string     strHostName;
    int             iPort;

    FNetConnectionInfo(const std::string _strIP, const std::string _strHostName, const int _iPort)
        : strIP(_strIP), strHostName(_strHostName), iPort(_iPort)
    {
    }

    FNetConnectionInfo()
        : strIP(""), strHostName(""), iPort(0)
    {
    }

    ~FNetConnectionInfo()
    {
    }
};