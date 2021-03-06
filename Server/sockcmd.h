#ifndef SOCKCMD_H
#define SOCKCMD_H
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
namespace CMD
{
    enum ENUM_CMD: short{
        CMD_NULL = 0,
        CMD_GETUID = 18,
        CMD_SENDUID,
        CMD_CREATROOM,
        CMD_JOINROOM,
        CMD_LEAVEROOM,
        CMD_CLOSEROOM,
    };
    enum ENUM_STREAM :short{
        STREAM_VIDEO = 101,
        STREAM_FILE = 103,
    };

    class DataHeader{
    public:
        DataHeader(){

        }
        ~DataHeader(){

        }
        ENUM_CMD cmd;
        int datalength;
    };
    class Data: public DataHeader{
    public:
        Data(){
            datalength = sizeof(Data);
            cmd = CMD_NULL;
        }
        ~Data(){

        }
    };
    class JsonTool{
    public:
        QByteArray static sendUID(int UID){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_SENDUID);
            json.insert("UID", UID);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }

        QByteArray static getUID(){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_GETUID);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static creatRoom(int UID){
            QJsonObject json;
            json.insert("UID",UID);
            json.insert("ENUM_CMD",ENUM_CMD::CMD_CREATROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static joinRoom(int RoomID){
            QJsonObject json;
            json.insert("RoomID",RoomID);
            json.insert("ENUM_CMD",ENUM_CMD::CMD_JOINROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static leaveRoom(){
            QJsonObject json;
            json.insert("ENUM_CMD",ENUM_CMD::CMD_LEAVEROOM);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);
        }
        QByteArray static sendStream(QByteArray data, ENUM_STREAM streamType = ENUM_STREAM::STREAM_VIDEO){
            QJsonObject json;
            json.insert("Data",QString(data));
            json.insert("Size",data.size());
            json.insert("ENUM_STREAM",streamType);
            QJsonDocument doc;
            doc.setObject(json);
            return doc.toJson(QJsonDocument::Compact);

        }

        QByteArray static analyseData(QByteArray byteArray, QString type){
            QJsonParseError jsonError;
            QByteArray baseData = NULL;
            QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // ????????? JSON ??????
            if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // ?????????????????????
                QJsonObject object = doucment.object();  // ???????????????
                if (object.contains(type)) {  // ??????????????? key
                    QJsonValue value = object.value(type);  // ???????????? key ????????? value
                    if (value.isString()) {  // ?????? value ??????????????????
                        QString strName = value.toString();  // ??? value ??????????????????
                        baseData = QByteArray::fromBase64(strName.toLatin1());
                    }
                }
            }
            return baseData;
        }

        int static analyseID(QByteArray byteArray, QString type){
            QJsonParseError jsonError;
            int ID = 0;
            QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // ????????? JSON ??????
            if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // ?????????????????????
                QJsonObject object = doucment.object();  // ???????????????
                if (object.contains(type)) {  // ??????????????? key
                    QJsonValue value = object.value(type);  // ???????????? key ????????? value
                    if (value.isDouble()) {  // ?????? value ??????????????????
                        ID = value.toDouble();  // ??? value ??????????????????
                    }
                }
            }
            return ID;
        }
    };

}

#endif // SOCKCMD_H
