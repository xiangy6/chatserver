#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "usermodel.hpp"
#include "json.hpp"
#include "offlinemsgmodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

using namespace muduo;
using namespace muduo::net;
using namespace std;

using json = nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json *js ,Timestamp)>;

// 聊天服务器的业务类
class ChatService
{
public:
    static ChatService* instance(); //获取单例对象的接口函数

    void login(const TcpConnectionPtr &conn, json *js ,Timestamp time); //处理登录业务

    void loginout(const TcpConnectionPtr &conn, json *js ,Timestamp time); //处理下线业务

    void reg(const TcpConnectionPtr &conn, json *js ,Timestamp time);   //处理注册业务

    void onechat(const TcpConnectionPtr &conn, json *js ,Timestamp time);   //一对一聊天业务

    void addfriend(const TcpConnectionPtr &conn, json *js ,Timestamp time); //添加好友

    void createGroup(const TcpConnectionPtr &conn, json *js ,Timestamp time); //创建群组

    void addGroup(const TcpConnectionPtr &conn, json *js ,Timestamp time);  //加入群组

    void groupChat(const TcpConnectionPtr &conn, json *js ,Timestamp time); //群组聊天

    MsgHandler getHandler(int msgid);   //获取消息对应的处理器

    void clientCloseException(const TcpConnectionPtr &conn);    //处理客户端异常退出

    void reset();

    void handlerRedisSubscribeMessage(int userid,string msg);
private:
    ChatService();

    unordered_map<int,MsgHandler> _msgHandlerMap;   //存储msgid和其对应的业务处理方法

    unordered_map<int,TcpConnectionPtr> _userconnMap;   // 存储在线用户的通信连接

    mutex _connmutex;    // 定义互斥锁，保证_userconnMap的线程安全
    UserModel _usermodel;   //数据操作对象
    offlineMsg _offlineMsgmodel;//离线数据操作对象
    FriendModel _friendmodel;   //添加好友操作对象
    GroupModel _groupmodel;     //群组操作对象
    Redis _redis;   //redis操作对象
};

#endif