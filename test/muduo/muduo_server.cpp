/*
muduo网络库为用户提供了两个主要的类，分别是
TCPServer:用于编写服务器的
TCPClient:用于编写客户端的
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>

using namespace std;
using namespace muduo;
using namespace muduo::net;

//基于muduo网络库开发的服务器程序
/*
1.组合TcpServer对象
2.创建EventLoop事件循环的指针
3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4.在服务器类的构造函数中，注册 处理连接回调函数 和 处理读写事件的回调函数
5.设置合适的服务端线程数量，muduo库会自己分配io线程和工作线程
*/

class ChatServer{
public:
        ChatServer (EventLoop* loop,
        const InetAddress& listenAddr,
        const string& nameArg)
        : _server(loop,listenAddr,nameArg),_loop(loop)
        {
            // 给服务器注册用户连接的创建和断开回调
            _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));

            // 给服务器注册用户读写事件回调
            _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

            //设置服务器端的线程数量 (1个io线程，3个worker线程)
            _server.setThreadNum(4);
            
        }

        void start(){
            _server.start();
        }   //开启事件循环

private:
        void onConnection(const TcpConnectionPtr &conn)
        {
            if(conn->connected()){
                cout << conn->peerAddress().toIpPort() << " ->" << conn->localAddress().toIpPort() <<"state: Online"<< endl;
            }
            else{
                cout << conn->peerAddress().toIpPort() << " ->" << conn->localAddress().toIpPort() <<"state: Offline"<< endl;
                conn ->shutdown();
            }
        }   //专门处理用户的连接创建和断开

        void onMessage(const TcpConnectionPtr &conn,   //连接
            Buffer* buffer,    //缓冲区
            Timestamp time) //接收到数据的时间信息
        {
            string buf = buffer->retrieveAllAsString();
            cout << "recv data:"<<buf << "time:"<<time.toString()<<endl;
            conn->send(buf);
        }   //专门处理用户的读写事件

        TcpServer _server;  //1.创建_server
        EventLoop *_loop;   //2. 等同于创建epoll

};

int main(){
    EventLoop loop;
    InetAddress addr("192.168.28.4",6000);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();
    cout << "Server running..." << endl;
    loop.loop();

    return 0;
}