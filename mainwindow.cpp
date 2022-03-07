#include "mainwindow.h"

MainWindow::MainWindow(QObject *parent) : QObject(parent)
{
    init();
}

MainWindow::~MainWindow()
{
    stop = true;
}
QString MainWindow::getLocalIP() const
{
    QString ip = "";
//    QString localHostName = "";
//    localHostName = QHostInfo::localHostName();
//    QHostInfo info = QHostInfo::fromName(localHostName);
//    qDebug()<<info.addresses();
//    foreach(QHostAddress address,info.addresses()){
//         if(address.protocol() == QAbstractSocket::IPv4Protocol){
//            ip = address.toString();
//         }
//    }
//    qDebug() << ip;
    return ip;
}


void MainWindow::init()
{
    qRegisterMetaType<ClientSock>("ClientSock");
    qRegisterMetaType<sockaddr_in>("sockaddr_in");

    tcpServer = new TCPServer();
    tcpServer->initial_server();
    udpServer = new UDPServer();
    udpServer->initial_server();

    connect(tcpServer, &TCPServer::signalRecv, this, &MainWindow::signalTCPRecv);
    connect(tcpServer, &TCPServer::signalLogOut, this, &MainWindow::slotTCPLogOut);
    connect(udpServer, &UDPServer::signalRecv, this, &MainWindow::signalUDPRecv);
    connect(this, &MainWindow::signalTCPRecv, this, &MainWindow::slotTCPRecv);
    connect(this, &MainWindow::signalUDPRecv, this, &MainWindow::slotUDPRecv);

    int ret =tcpServer->bind_port("0.0.0.0", 8000);
    if (SOCKET_ERROR != ret){
        tcpServer->listen_port();
        tcpThread = std::thread([=](){
            while(!stop)
            {
                tcpServer->run_select();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    ret = udpServer->bind_port("0.0.0.0", 8001);
    if (SOCKET_ERROR != ret){
        udpThread = std::thread([=](){
            while(!stop)
            {
                udpServer->run_select();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
}

void MainWindow::slotTCPRecv(ClientSock client)
{
    QByteArray data(client.getBuff());

    const QString cmd = "ENUM_CMD";
    const QString streamType = "ENUM_STREAM";
    ENUM_CMD c = (ENUM_CMD)CMD::JsonTool::analyseID(data, cmd);
    switch (c) {
    case CMD_CREATROOM:
    {
        createRoom(client);
    }
        break;
    case CMD_JOINROOM:
    {
        int roomid = CMD::JsonTool::analyseID(client.getBuff(),"RoomID");
        client.setRoomID(roomid);
        joinRoom(client);
    }
        break;
    case CMD_LEAVEROOM:
    {
        leaveRoom(client);
    }
        break;
    case CMD_CLOSEROOM:
    {
        closeRoom(client);
    }
        break;
    }

    qDebug()<<client.getUID();
}

void MainWindow::slotUDPRecv(QByteArray data, sockaddr_in addr)
{
    std::string ip = inet_ntoa(addr.sin_addr);
    unsigned short port = addr.sin_port;
    int roomId = 0;
    for(auto i : clientsInRoom){
        if(i->get_ip() == ip){
            roomId = i->getRoomID();
            break;
        }
    }
    if(roomId!=0 && rooms.contains(roomId)){
        for(auto i :rooms.find(roomId)->clients()){
            unsigned short curPort = i->get_port();
            std::string curIP = i->get_ip();
            if(ip == curIP)//&& curPort == port
                continue;
            udpServer->sendData(i->get_ip(), 8888, data.data(), data.size());
        }
    }

}

void MainWindow::slotTCPLogOut(ClientSock client)
{
    leaveRoom(client);
}

void MainWindow::createRoom(ClientSock client)
{
    UDPRoom room;
    ClientBase *cbase = (ClientBase*)&client;
    room.pushClient(cbase);
    rooms.insert(room.getRoomID(), room);
    clientsInRoom.push_back(cbase);
}
/**
 * @brief MainWindow::joinRoom 输入房主UID根据UID查找clientsInRoom 找到RoomID
 * @param client
 */
void MainWindow::joinRoom(ClientSock client)
{
    ClientBase *cbase = (ClientBase*)&client;
    if(rooms.contains(cbase->getRoomID())){
        clientsInRoom.push_back(cbase);
        rooms.find(cbase->getRoomID()).value().pushClient(cbase);
    }
}

void MainWindow::leaveRoom(ClientSock client)
{
    for(auto i : clientsInRoom){
        if( i->get_ip() == client.get_ip()){
            ClientBase *cbase = i;
            if(rooms.contains(cbase->getRoomID())){
                rooms.find(cbase->getRoomID()).value().removeClient(cbase);
                UDPRoom room = rooms.find(cbase->getRoomID()).value();
                if(room.count() == 0){
                    rooms.erase(rooms.find(cbase->getRoomID()));
                }
            }
            if(clientsInRoom.contains(cbase)){
                clientsInRoom.removeOne(cbase);
            }

            break;
        }
    }
}

void MainWindow::closeRoom(ClientSock client)
{
    rooms.remove(client.getRoomID());
}
