#include "udpserver.h"

UDPServer::UDPServer()
{
    qRegisterMetaType<sockaddr_in>("sockaddr_in");
}

UDPServer::~UDPServer()
{
    string msg = "";
    close_server();
}
int UDPServer::initial_server()
{
    QString errormsg;
    //启动in sock 2.x环境
#ifdef  _WIN32
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);
#endif //  _WIN32
    if (INVALID_SOCKET != _server)
    {
        string msg = "";
        close_server();
        errormsg = "旧连接已关闭";
    }
    _server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (INVALID_SOCKET == _server)
    {
        errormsg = "socket初始化失败...\n";
        return -1;
    }
    else
    {
        errormsg = "socket初始化成功...\n";
    }
    qDebug()<<errormsg;
    return 0;
}
int UDPServer::bind_port(const char* ip, unsigned short port)
{
    QString errormsg;
    _addrServer = {};
    _addrServer.sin_family = AF_INET;
    _addrServer.sin_port = htons(port);//host to net unsigned short
#ifdef _WIN32
    if (ip)
    {
        _addrServer.sin_addr.S_un.S_addr = inet_addr(ip);
    }
    else
    {
        _addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
    }
#else
    if (ip)
    {
        _sin.sin_addr.s_addr = inet_addr(ip);
    }
    else
    {
        _sin.sin_addr.s_addr = INADDR_ANY;
    }
#endif
    int ret = bind(_server, (sockaddr*)&_addrServer, sizeof(_addrServer));
    if (SOCKET_ERROR == ret)
    {
        errormsg = "绑定失败...\n";
    }
    else
    {
        errormsg = "绑定成功.\n";
    }
    qDebug()<<errormsg;

    return ret;
}


bool UDPServer::run_select()
{
    QString errormsg;
    if (is_run())
    {
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_server, &fdRead);
        FD_SET(_server, &fdWrite);
        FD_SET(_server, &fdExp);


        //select(int nfds); nfds是指fd_set集合中所有描述符（socket）的范围，而不是数量。
        //即所有文件描述符最大值 + 1 , 在Windows中这个参数可以写0
        timeval over_time = { 1,0 };
        int ret = select(_server + 1, &fdRead, &fdWrite, &fdExp, &over_time);
        if (ret < 0)
        {
            errormsg = "select任务结束";
            qDebug()<<errormsg;

            return false;
        }
        if (FD_ISSET(_server, &fdRead))//sock 是否在集合中
        {
            FD_CLR(_server, &fdRead);

            //accept_client(msg1);
            recv_data();
        }

        return true;
    }
    return false;
}


int UDPServer::recv_data()
{
    sockaddr_in client;
    initAddr(client);
    int nAddrLen = sizeof(sockaddr_in);
    char _recvBuff[RECV_BUFF_SIZE];
    memset(_recvBuff,0, RECV_BUFF_SIZE);
//5 接收客户端数据
    int nLen =recvfrom(_server,_recvBuff, RECV_BUFF_SIZE,0 ,(sockaddr*)&client, &nAddrLen);
    if (nLen <= 0)
    {
        //客户端退出
         //errormsg = "客户端:%d 退出\n",(int)_clientSock->getsock();
        return -1;
    }else{
            QByteArray data(_recvBuff);
            emit signalRecv(data, client);
    }
    return 0;
}

void UDPServer::initAddr(sockaddr_in &addr)
{
    addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);//host to net unsigned short
#ifdef _WIN32
    addr.sin_addr.S_un.S_addr = INADDR_ANY;

#else
    _sin.sin_addr.s_addr = INADDR_ANY;
#endif
}

void UDPServer::analyAddr(const sockaddr_in &sock)
{
    int port = ntohs(sock.sin_port);
     #ifdef __MINGW32__  //windows上打印方式
         printf("ip:port  %s : %d",inet_ntoa(sock.sin_addr),port);
     #else              //linux上打印方式
        struct in_addr in  = sock.sin_addr;
        char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
        //成功的话此时IP地址保存在str字符串中。
        inet_ntop(AF_INET,&in, str, sizeof(str));
        printf("ip:port  %s : %d",str,port);
     #endif
}


bool UDPServer::is_run()
{
    return _server != INVALID_SOCKET;
}

void UDPServer::close_server()
{
    QString errormsg;
    if (_server != INVALID_SOCKET)
    {
#ifdef _WIN32
        closesocket(_server);
        //清楚windows socket环境
        WSACleanup();
#else
        close(_server);
#endif
        _server = INVALID_SOCKET;
        errormsg = "服务器已关闭";
        qDebug()<<errormsg;
    }
}


int UDPServer::sendData(char *ip, int nPort, char *pData, int size)
{
    if(is_run()){
    sockaddr_in ser;	// 服务器端地址
    ser.sin_family = AF_INET;	// IP协议
    ser.sin_port = htons(nPort);	// 端口号
    ser.sin_addr.s_addr = inet_addr(ip);	// IP地址
    int nLen = sizeof(ser);	// 服务器地址长度
    return sendto(_server, (const char*)pData, size, 0, (sockaddr*)&ser, nLen);	// 向服务器发送数据
    }
}

int UDPServer::sendData(sockaddr_in client, char *pData, int size)
{
    if(is_run()){
        int nLen = sizeof(client);	// 服务器地址长度
        return sendto(_server, (const char*)pData, size, 0, (sockaddr*)&client, nLen);	// 向服务器发送数据
    }
}
