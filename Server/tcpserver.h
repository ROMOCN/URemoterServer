#ifndef  MainServer_h_
#define MainServer_h_

#ifdef _WIN32
    #define FD_SETSIZE 1024
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <WinSock2.h>


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

#ifndef CELL_COUNT
#define CELL_COUNT 4
#endif // CELL_COUNT
#include <QObject>
#include <QDebug>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "clientsock.hpp"
#include "cellserver.h"

using namespace std;
class TCPServer :public QObject
{

    Q_OBJECT
private:
    SOCKET _server;
    std::vector<ClientSock*> _clients;
    std::vector<CellServer*> _servers;

    /// <summary>
    /// 接收缓冲区
    /// </summary>
    char _recvBuff[RECV_BUFF_SIZE];
public:
    TCPServer();
    virtual ~TCPServer();
    //初始化socket
    int initial_server();
    //绑定端口号
    int bind_port(const char* ip, unsigned short port);
    //监听端口号
    int listen_port(int n = 0);
    //关闭socket
    void close_server();
    //处理网络信息
    bool run_select();
    //是否运行
    inline bool is_run();
signals:
    void signalRecv(ClientSock client);
    void signalAccept(ClientSock client);
    void signalLogOut(ClientSock client);

private:
    ClientSock accept_client(string &errormsg);
//    int send_data(ClientSock *m_client, DataHeader *data);
    void test_json();
    void cell_init();
    void new_client_join(ClientSock *client);

};


#endif // MainServer_h_
