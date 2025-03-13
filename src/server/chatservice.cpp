#include "chatservice.hpp"
#include "friendmodel.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace muduo;
using namespace placeholders;

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    //单用户操作
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::onechat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addfriend, this, _1, _2, _3)});

    //群组操作
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});

    //连接redis
    if (_redis.connect())
    {
        //上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handlerRedisSubscribeMessage, this, _1, _2));
    }
}

void ChatService::handlerRedisSubscribeMessage(int userid,string msg){
    lock_guard<mutex> lock(_connmutex);
    auto it = _userconnMap.find(userid);
    if (it != _userconnMap.end())
    {
        it->second->send(msg);
        return;
    }
    _offlineMsgmodel.insert(userid,msg);//存储该用户的离线消息
}//从redis消息队列中获取订阅的消息

void ChatService::reset()
{
    _usermodel.resetState();
}

MsgHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &conn, json *js, Timestamp)
        {
            LOG_ERROR << "msgid:" << msgid << " can't find handler!";
        }; // 返回一个默认的处理器,空操作
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
} // 获取消息对应的处理器

void ChatService::login(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    // LOG_INFO << "doing login service~";
    int id = (*js)["id"];
    string pwd = (*js)["password"];

    User user = _usermodel.query(id);
    if (user.getid() == id && user.getpwd() == pwd)
    {
        if (user.getstate() == "online")
        {
            // 已经登录，禁止重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账号已登陆，禁止重复登录";
            conn->send(response.dump());
        }
        else
        {
            { // 登录成功，记录用户连接信息
                lock_guard<mutex> lock(_connmutex);
                _userconnMap.insert({id, conn});
            }

            _redis.subscribe(id); // 用户登录成功后，向redis订阅id通道
            // 登录成功，更新用户状态信息
            user.setstate("online");
            _usermodel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getid();
            response["name"] = user.getname();
            // 查询该用户是否有离线消息
            vector<string> vec = _offlineMsgmodel.query(id);
            if (!vec.empty())
            {
                response["offlineMsg"] = vec;
                _offlineMsgmodel.remove(id); // 读取完离线消息后，将离线消息删除
            }

            vector<User> uservec = _friendmodel.query(id);
            if (!uservec.empty())
            {
                vector<string> vec2;
                for (User &user : uservec)
                {
                    json js;
                    js["id"] = user.getid();
                    js["name"] = user.getname();
                    js["state"] = user.getstate();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            } // 查询用户的好友信息并返回

            vector<Group> groupuserVec = _groupmodel.queryGroups(id);
            if (!groupuserVec.empty())
            {
                // group:[{groupid:[xxx,xxx，xxx,xxx]}]
                vector<string> groupV;
                for (Group &group : groupuserVec)
                {
                    json grpjson;
                    grpjson["id"] = group.getid();
                    grpjson["groupname"] = group.getgpname();
                    grpjson["groupdesc"] = group.getgpdesc();
                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getid();
                        js["name"] = user.getname();
                        js["state"] = user.getstate();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                response["groups"] = groupV;
            } // 查询用户的群组信息

            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误，登录失败";
        conn->send(response.dump());
    }
} // 处理登录业务

void ChatService::loginout(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userconnMap.find(userid);
        if (it != _userconnMap.end())
        {
            _userconnMap.erase(it);
        }
    }

    _redis.unsubscribe(userid); // 在redis中取消订阅的通道

    User user;
    user.setid(userid);
    user.setpwd("");
    user.setname("");
    user.setstate("offline");
    _usermodel.updateState(user);
}

void ChatService::reg(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    // LOG_INFO << "doing reg service~";
    string name = (*js)["name"];
    string pwd = (*js)["password"];

    User user;
    user.setname(name);
    user.setpwd(pwd);
    bool state = _usermodel.insert(user);
    if (state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getid();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["id"] = user.getid();
        conn->send(response.dump());
    }
} // 处理注册业务

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connmutex);
        for (auto it = _userconnMap.begin(); it != _userconnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                user.setid(it->first);
                _userconnMap.erase(it); // 从map表中删除用户的连接信息
                break;
            }
        }
    }

    _redis.unsubscribe(user.getid()); // 在redis中取消订阅的通道

    if (user.getid() != -1)
    {
        user.setstate("offline"); // 更新用户的状态信息
        _usermodel.updateState(user);
    }
} // 处理客户端异常退出

void ChatService::onechat(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int toid = (*js)["to"];
    {
        lock_guard<mutex> lock(_connmutex);
        auto it = _userconnMap.find(toid);
        if (it != _userconnMap.end())
        {
            // toid 在线，转发消息，服务器主动推送消息给toid用户
            it->second->send((*js).dump());
            return;
        }
    }

    User user = _usermodel.query(toid);
    if (user.getstate() == "online") // 用户在其他服务器上登录，使用redis发布消息
    {
        _redis.publish(toid, (*js).dump());
        return;
    }
    // toid不在线，存储离线消息
    _offlineMsgmodel.insert(toid, (*js).dump());
}

void ChatService::addfriend(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int friendid = (*js)["friendid"];

    _friendmodel.insert(userid, friendid); // 存储好友信息

} // 添加好友

void ChatService::createGroup(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    string name = (*js)["groupname"];
    string desc = (*js)["groupdesc"];

    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupmodel.createGroup(group))
    {
        _groupmodel.addGroup(userid, group.getid(), "creator"); // 存储群组创建人信息
    }

} // 创建群组

void ChatService::addGroup(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int groupid = (*js)["groupid"];
    _groupmodel.addGroup(userid, groupid, "normal");
} // 加入群组

void ChatService::groupChat(const TcpConnectionPtr &conn, json *js, Timestamp time)
{
    int userid = (*js)["id"];
    int groupid = (*js)["groupid"];
    vector<int> useridVec = _groupmodel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connmutex);
    for (int id : useridVec)
    {
        auto it = _userconnMap.find(id);
        if (it != _userconnMap.end())
        {
            it->second->send((*js).dump()); // 转发群消息
        }
        else
        {
            User user = _usermodel.query(id);
            if (user.getstate() == "online")
            {
                _redis.publish(id, (*js).dump());
            }
            else
            {
                _offlineMsgmodel.insert(id, (*js).dump()); // 存储离线消息
            }
        }
    }
} // 群组聊天