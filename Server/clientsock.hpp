#ifndef  _ClientSock_hpp_
#define _ClientSock_hpp_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <windows.h>
#include <WinSock2.h>
#include <QMutex>
//#pragma comment(lib,"ws2_32.lib")

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
#ifndef MSG_BUFF_SIZE
#define MSG_BUFF_SIZE 102400
#endif // MSG_BUFF_SIZE
#include <QList>
#include <string>
#include <vector>
#include <stdio.h>
#include <mutex>
#include "Tools/toolRandomID.h"
class ClientBase{
private:
    SOCKET _clientfd;
    int UID = 0;
    int RoomID = 0;
public:
    sockaddr_in clientaddr;
    ClientBase(SOCKET sock = INVALID_SOCKET){
        _clientfd = sock;
        creatUID();
    }
    ~ClientBase(){

    }
    int creatUID(){
        UID = ToolRandomID::CreatID();
        return UID;
    }
    SOCKET getsock(){
        return _clientfd;
    }
    void setsock(SOCKET client){
        _clientfd = client;
    }
    char* get_ip(){
        return inet_ntoa(clientaddr.sin_addr);
    }
    unsigned short get_port(){
        return clientaddr.sin_port;
    }
    int getUID(){
        return UID;
    }
    void updateUID(){
        UID = ToolRandomID::CreatID();
    }
    int getRoomID(){
        return RoomID;
    }
    void setRoomID(int RoomID){
        this->RoomID = RoomID;
    }

};

class ClientSock :public ClientBase
{
private:
    char _msgBuff[MSG_BUFF_SIZE];
    char _recvBuff[RECV_BUFF_SIZE];
    int _lastPos = 0;
    long _userid = 0;

public:
    ClientSock(SOCKET sock = INVALID_SOCKET){
        setsock(sock);
        memset(_msgBuff, 0, sizeof(_msgBuff));
        memset(_recvBuff, 0, sizeof(_recvBuff));
        _lastPos = 0;
		clientaddr = {};
        creatUID();
	}
    int getPos(){
		return _lastPos;
	}
    void setPos(int pos){
		_lastPos = pos;
	}
    char* getBuff(){
		return _msgBuff;
	}
    void set_userid(long id){
        _userid = id;
    }
    long get_userid(){
        return _userid;
    }



};
class UDPRoom{
protected:
    int RoomID = 0;
    QList<ClientBase*> _clients;
public:
    UDPRoom(){
//        updateRoomID();
    }
//    UDPRoom(const UDPRoom &old) {
//          old._lock->lock();
//          _lock = new QMutex(); // Need to make new one here.
//           this->RoomID = old.RoomID;
//          this->_clients = old._clients;
//          old._lock->unlock();
//      }
    ~UDPRoom(){

    }
    void pushClient(ClientBase *client){
//        if(RoomID == 0)
//            updateRoomID();
        if(_clients.size() == 0)
            this->RoomID = client->getUID();
        client->setRoomID(RoomID);
        _clients.push_back(client);

    }
    ClientBase* at(int index){
        _clients.at(index);
    }
    int size(){
        return _clients.size();
    }
    int count(){
        return _clients.count();
    }
    int count(ClientBase *const client){
        return _clients.count(client);
    }
    QList<ClientBase*>::iterator end(){
        return _clients.end();
    }
    QList<ClientBase*>::iterator begin(){
        return _clients.begin();
    }
    const QList<ClientBase*> clients(){
        return _clients;
    }
    void clear(){
        if(_clients.count() >0){
            _clients.clear();
        }
    }
    int getRoomID(){
        return RoomID;
    }
    void updateRoomID(){
        RoomID = ToolRandomID::CreatID();
    }
    void removeClient(ClientBase *base){
//        for(auto i: clients){
//            if(i->get_ip() == base->get_ip()){
//            }
//        }
        _clients.removeOne(base);

    }

};

#endif // _ClientSock_hpp_
