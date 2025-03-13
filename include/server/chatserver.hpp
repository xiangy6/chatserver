#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;

class ChatServer
{
public:
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const string &nameArg); // ChatServer的构造函数

    void start(); // 启动server服务

private:
    void onConnection(const TcpConnectionPtr &); // 处理连接与断开
    void onMessage(const TcpConnectionPtr &,
                   Buffer *,
                   Timestamp time); // 处理连接的读写操作
    TcpServer _server;
    EventLoop *_loop;
};

#endif