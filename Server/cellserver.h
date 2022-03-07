#ifndef CELLSERVER_H
#define CELLSERVER_H

#ifdef _WIN32
#define FD_SETSIZE 1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <QObject>

#else
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR			(~1)
#endif // _WIN32

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif // RECV_BUFF_SIZE

#include <QObject>
#include <QDebug>
#include <string>
#include <string.h>
#include <mutex>
#include <thread>
#include <math.h>
#include "clientsock.hpp"
#include "sockcmd.h"

using namespace std;
using namespace CMD;
class CellServer :public QObject
{
    Q_OBJECT
protected:
    SOCKET _server = INVALID_SOCKET;
    vector<ClientSock*> _clients;
    vector<ClientSock*> _clients_temp;//缓冲
    mutex _lock;
    std::thread *_thread = nullptr;
    /// <summary>
    /// 接收缓冲区
    /// </summary>
    char _recvBuff[RECV_BUFF_SIZE];
private:
    int recv_data(ClientSock* _clientSock,string &errormsg);
    void run_select();
public:
    CellServer(SOCKET sock = INVALID_SOCKET);
    ~CellServer();
    void close_server(string &errormsg);
    bool is_run()
    {
        return _server != INVALID_SOCKET;
    }
    void Start()
    {
        _thread =new std::thread(&CellServer::run_select,this);
        _thread->detach();
    }
    void add_client(ClientSock *client)
    {
        std::lock_guard<mutex> lg(_lock);
        _clients_temp.push_back(client);
    }
    size_t get_clients_count()
    {
        return _clients.size() + _clients_temp.size();
    }
    int send_data(ClientSock *m_client, DataHeader *data)
    {
        if(is_run())
        {
            return send(m_client->getsock(),(const char*)data,data->datalength,0);
        }
        return SOCKET_ERROR;
    }
signals:
    void signalMsg(ClientSock client);
    void signalLogOut(ClientSock client);
};

#endif // CELLSERVER_H
