#include "tcpserver.h"

TCPServer::TCPServer()
{
    //信号注册自定义类型
    _server = INVALID_SOCKET;
    _recvBuff[RECV_BUFF_SIZE] = {};
}

TCPServer::~TCPServer()
{
    string msg = "";
    close_server();
}
int TCPServer::initial_server()
{
    QString errormsg;
    int result = 0;
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
    _server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (INVALID_SOCKET == _server)
    {
        errormsg = "socket初始化失败...\n";
        result = -1;
    }
    else
    {
        errormsg = "socket初始化成功...\n";
    }
    qDebug()<<errormsg;
    return result;
}
int TCPServer::bind_port(const char* ip, unsigned short port)
{
    QString errormsg;
    int result = 0;
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(port);//host to net unsigned short

#ifdef _WIN32
    if (ip)
    {
        _sin.sin_addr.S_un.S_addr = inet_addr(ip);
    }
    else
    {
        _sin.sin_addr.S_un.S_addr = INADDR_ANY;
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
    result = bind(_server, (sockaddr*)&_sin, sizeof(_sin));
    if (SOCKET_ERROR == result)
    {
        errormsg = "绑定失败...\n";
    }
    else
    {
        errormsg = "绑定成功.\n";
        cell_init();
    }
    qDebug()<<errormsg;
    return result;
}
int TCPServer::listen_port(int n)
{
    int ret = listen(_server, n);
    QString errormsg;

    if (SOCKET_ERROR == ret )//5:五个连接
    {
        errormsg = "监听失败...\n";
    }
    else
    {
        errormsg = "监听成功.\n";
        cell_init();
    }
    qDebug()<<errormsg;

    return ret;
}


void TCPServer::cell_init()
{
    for(int i = 0;i< CELL_COUNT; i++)
    {
        qRegisterMetaType<ClientSock>("ClientSock");

        CellServer *server = new CellServer(_server);
        connect(server, &CellServer::signalMsg, this, &TCPServer::signalRecv);
        connect(server, &CellServer::signalLogOut, this, &TCPServer::signalLogOut);
        _servers.push_back(server);
        server->Start();
    }
}


bool TCPServer::run_select()
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
        int ret = select(_server , &fdRead, &fdWrite, &fdExp, &over_time);
        //int ret = select(_server, &fdRead, &fdWrite, &fdExp, &over_time);

        if (ret < 0)
        {
            errormsg = "select任务结束";
            qDebug()<<errormsg;

            return false;
        }
        if (FD_ISSET(_server, &fdRead))//sock 是否在集合中
        {
            FD_CLR(_server, &fdRead);
            string msg1 = "";
            accept_client(msg1);
        }

        return true;
    }
    return false;
}

ClientSock TCPServer::accept_client(string &errormsg)
{
    int nAddrLen = sizeof(sockaddr_in);
    ClientSock* client = new ClientSock();
#ifdef _WIN32
    client->setsock(accept(_server, (sockaddr*)&client->clientaddr, &nAddrLen));
#else
    client->setsock(accept(_server, (sockaddr*)&client->clientaddr, (socklen_t*)&nAddrLen));
#endif
    if (INVALID_SOCKET == client->getsock())
    {
        printf("错误，接受的客户端无效...");
    }
    else
    {
        qDebug()<<QString("新的客户端:%1:%2").arg( client->get_ip()).arg( client->get_port());
        QByteArray strID = CMD::JsonTool::sendUID(client->getUID());
        emit signalAccept(*client);
        send(client->getsock(), strID, strID.size(), 0);
        new_client_join(client);
    }

    return *client;
}

void TCPServer::new_client_join(ClientSock *client)
{
    //_clients.push_back(client);

    CellServer *minServer = _servers[0];
    for(CellServer *s : _servers)
    {
        if(minServer->get_clients_count() > s->get_clients_count())
            minServer = s;
    }
    minServer->add_client(client);

}


bool TCPServer::is_run()
{
    return _server != INVALID_SOCKET;
}



//int MainServer::send_data(ClientSock *m_client, DataHeader *data)
//{
//    if(is_run())
//    {
//        return send(m_client->getsock(),(const char*)data,data->datalength,0);
//    }
//    return SOCKET_ERROR;
//}


void TCPServer::close_server()
{
    QString errormsg;

    if (_server != INVALID_SOCKET)
    {
#ifdef _WIN32

        for (int n = (int)_clients.size() - 1; n >= 0; n--)
        {
            closesocket(_clients[n]->getsock());
        }
        for (int n = (int)_servers.size() - 1; n >=0; n-- )
        {
            string msg = "";
            _servers[n]->close_server(msg);
        }

        closesocket(_server);
        //清楚windows socket环境
        WSACleanup();
#else

        for (int n = (int)clients.size() - 1; n >= 0; n--)
        {
            close(clients[n]->getsock());
        }
        for (int n = (int)_servers.size() - 1; n >=0; n-- )
        {
            string msg = "";
            _servers[n]->close(msg);
        }
        close(_server);
#endif
        _servers.clear();
        _server = INVALID_SOCKET;
        errormsg = "服务器已关闭";
        qDebug()<<errormsg;
    }
}

