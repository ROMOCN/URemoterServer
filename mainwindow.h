#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <thread>
#include "Server/tcpserver.h"
#include "Server/udpserver.h"
#include "Tools/toolRandomID.h"
class MainWindow : public QObject
{
    Q_OBJECT
protected:
    TCPServer *tcpServer = nullptr;
    UDPServer *udpServer = nullptr;
    std::thread tcpThread, udpThread;
    bool stop = false;
    QMap<int, UDPRoom> rooms;
    QList<ClientBase*> clientsInRoom;

//    QList<ClientSock> clientsOnline;
public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow();
    QString getLocalIP() const;
signals:
    /*将跨线程信号转为同线程信号*/
    void signalTCPRecv(ClientSock client);
    void signalUDPRecv(QByteArray data,sockaddr_in addr);

private:
    void init();
    void slotTCPRecv(ClientSock client);
    void slotUDPRecv(QByteArray data,sockaddr_in addr);
    void slotTCPLogOut(ClientSock client);
    void createRoom(ClientSock client);
    void joinRoom(ClientSock client);
    void leaveRoom(ClientSock client);
    void closeRoom(ClientSock client);
    void msgFactory();
};

#endif // MAINWINDOW_H
