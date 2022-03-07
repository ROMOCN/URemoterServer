#include "cellserver.h"
CellServer::CellServer(SOCKET sock){
    qRegisterMetaType<ClientSock>("ClientSock");
    _server = sock;
}

CellServer::~CellServer(){
    string msg = "";
    close_server(msg);
}
void CellServer::run_select(){
    while (is_run()){
        if(_clients_temp.size() > 0){
            std::lock_guard<mutex> lg(_lock);
            for(auto client : _clients_temp){
                _clients.push_back(client);
            }
            _clients_temp.clear();
        }
        if(_clients.empty()){
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(_server, &fdRead);
        FD_SET(_server, &fdWrite);
        FD_SET(_server, &fdExp);
        SOCKET maxsock = _clients[0]->getsock();
        for (int n = (int)_clients.size() - 1; n >= 0; n--)
        {
            FD_SET(_clients[n]->getsock(), &fdRead);
            if(maxsock < _clients[n]->getsock())
                maxsock = _clients[n]->getsock();
        }

        //select(int nfds); nfds是指fd_set集合中所有描述符（socket）的范围，而不是数量。
        //即所有文件描述符最大值 + 1 , 在Windows中这个参数可以写0
        timeval over_time = { 1,0 };
        int ret = select(maxsock + 1, &fdRead, &fdWrite, &fdExp, &over_time);

        if (ret < 0)
        {
            //return false;
            break;
        }
        for (int n = 0; n < _clients.size(); n++)
        {
            if (_clients[n] && FD_ISSET(_clients[n]->getsock(), &fdRead))
            {
                string msg2 = "";
                if (-1 == recv_data(_clients[n],msg2))
                {
                    auto iter = _clients.begin() + n;
                    //auto iter = find(clients.begin(), clients.end(), new ClientSock(fdRead.fd_array[n]));
                    if (iter != _clients.end())
                    {
//                        emit signal_connecter(*_clients[n],QString::fromStdString(MYCMD::connect_logout));
                        delete _clients[n];
                        _clients.erase(iter);
                    }
                }
            }
        }

        //return true;
    }
    //return false;
}






int CellServer::recv_data(ClientSock* _clientSock,string &errormsg)
{
//5 接收客户端数据
    memset(_clientSock->getBuff(), 0, RECV_BUFF_SIZE);
    int nLen = (int)recv(_clientSock->getsock(), _clientSock->getBuff(), RECV_BUFF_SIZE, 0);
    if (nLen <= 0)
    {
        //客户端退出
        emit signalLogOut(*_clientSock);
         QString error = QString("客户端离线:%1:%2").arg( _clientSock->get_ip()).arg( _clientSock->get_port());
         qDebug()<<error;

        return -1;
    }
    ClientSock client = *_clientSock;
    emit signalMsg(client);

    return 0;
}


//void Cell_Server::big_data_factory(string data, ClientSock *client, ENUM_CMD cmd_kind)
//{
//    int len = data.length();
//    double d = ((double)len / BigData::get_data_maxsize());
//    int count = ceil(d);
//    char *datac = new char[len];
//    strncpy(datac,data.c_str(),data.length());
//    for(int i = 0; i < count ; i++)
//    {
//        BigData bigdata = BigData(cmd_kind);
//        bigdata.data_count = count;
//        bigdata.data_index = i;
//        if(len > BigData::get_data_maxsize() )
//        {
//            strncpy_s(bigdata.data, BigData::get_data_maxsize() ,datac, BigData::get_data_maxsize() );
//            send_data(client,&bigdata);
//            memcpy(datac , datac + BigData::get_data_maxsize() , BigData::get_data_maxsize() );
//        }else
//        {
//            strncpy_s(bigdata.data, BigData::get_data_maxsize() , datac, strlen(datac));
//            send_data(client,&bigdata);
//            break;
//        }

//    }

//}
void CellServer::close_server(string &errormsg)
{
    if (_server != INVALID_SOCKET)
    {
#ifdef _WIN32
        for (int n = (int)_clients.size() - 1; n >= 0; n--)
        {
            closesocket(_clients[n]->getsock());
        }
        closesocket(_server);
        //清楚windows socket环境
        WSACleanup();
#else

        for (int n = (int)clients.size() - 1; n >= 0; n--)
        {
            close(clients[n]->getsock());
        }
        close(_server);
#endif
        _server = INVALID_SOCKET;
        errormsg = "服务器已关闭";
    }
}
