#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
    #define FD_SETSIZE 1024
    #define WIN32_LEAN_AND_MEAN
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <windows.h>
    #include <WinSock2.h>


#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #define SOCKET int
    #define INVALID_SOCKET (SOCKET)(~0)
    #define SOCKET_ERROR			(~1)
#endif // _WIN32

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 1024 * 64
#endif // RECV_BUFF_SIZE

#ifndef CELL_COUNT
#define CELL_COUNT 4
#endif // CELL_COUNT
#include <QObject>
#include <QDebug>
#include <QWidget>
#include <string.h>
#include <vector>
#include <stdio.h>
#include <math.h>
#include "clientsock.hpp"

using namespace std;
class UDPServer :public QObject
{
    Q_OBJECT
private:
    SOCKET _server =INVALID_SOCKET;
    sockaddr_in _addrServer = {};
    /// <summary>
    /// 接收缓冲区
    /// </summary>
//    char _recvBuff[RECV_BUFF_SIZE] = {};

public:
    UDPServer();
    virtual ~UDPServer();
    //初始化socket
    int initial_server();
    //绑定端口号
    int bind_port(const char* ip, unsigned short port);
    //关闭socket
    void close_server();
    //处理网络信息
    bool run_select();
    //是否运行
    inline bool is_run();
    int recv_data();
    void initAddr(sockaddr_in &addr);
//    int send_data(ClientSock *m_client, DataHeader *data);
    void analyAddr(const sockaddr_in &addr);
    int sendData(char *ip, int nPort, char *pData, int size);
    int sendData(sockaddr_in client, char *pData, int size);
signals:
    void signalRecv(QByteArray data, sockaddr_in client);
};

#endif // SERVER_H
