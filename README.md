# chatserver
基于muduo网络库的C++集群聊天服务器与客户端源码，支持nginx TCP负载均衡，使用数据库mysql，跨服务器通信采用发布-订阅的redis消息队列



**chatserver:**

![image-20250313110101985](https://github.com/xiangy6/chatserver/blob/main/img/chatserver.png)

**chatclient:**

![image-20250313110441368](https://github.com/xiangy6/chatserver/blob/main/img/chatclient.png)

### 1.代码文件描述

网络层代码与业务层代码低耦合

|              | 代码文件                                                     | 描述                                                         |
| :----------: | :----------------------------------------------------------- | ------------------------------------------------------------ |
|    网络层    | chatserver.hpp & chatserver.cpp                              | 使用muduo网络库，启动server服务，处理网络连接与断开，以及链接的读写操作 |
|    业务层    | chatservice.hpp & chatservice.cpp                            | 响应聊天服务器的一系列业务操作                               |
|    数据层    | friendmodel.hpp&cpp<br />groupmodel.hpp&cpp<br />offlinemessagemodel.hpp&cpp<br />usermodel.hpp&cpp<br />db.hpp&cpp | 提供维护好友信息、群组信息、存储离线消息的接口，以及数据库交互接口 |
| 跨服务器通信 | redis.hpp&cpp                                                | 采用发布-订阅模式的redis消息队列，使不同服务器之间可以相互通信 |



### 2.数据库表设计

一共有User、Friend、AllGroup、GroupUser、OfflineMessage五张表。

![image-20250313104338944](https://github.com/xiangy6/chatserver/blob/main/img/tables.png)

```sql
REATE TABLE User (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(50) NOT NULL UNIQUE,
    password VARCHAR(50) NOT NULL,
    state ENUM('online', 'offline') DEFAULT 'offline'
);

CREATE TABLE Friend (
    userid INT NOT NULL,
    friendid INT NOT NULL,
    PRIMARY KEY (userid, friendid),
    FOREIGN KEY (userid) REFERENCES User(id),
    FOREIGN KEY (friendid) REFERENCES User(id)
);

CREATE TABLE AllGroup (
    id INT PRIMARY KEY AUTO_INCREMENT,
    groupname VARCHAR(50) NOT NULL,
    groupdesc VARCHAR(200) DEFAULT ''
);

CREATE TABLE GroupUser (
    groupid INT,
    userid INT NOT NULL,
    grouprole ENUM('creator', 'normal') DEFAULT 'normal',
    PRIMARY KEY (groupid, userid),
    FOREIGN KEY (groupid) REFERENCES AllGroup(id),
    FOREIGN KEY (userid) REFERENCES User(id)
);

CREATE TABLE OfflineMessage (
    userid INT NOT NULL,
    message VARCHAR(500) NOT NULL,
    FOREIGN KEY (userid) REFERENCES User(id)
);
```



### 3.nginx服务配置

在/usr/local/nginx/conf中配置nginx.conf，配置如下：

两个服务分别运行在6000和6001端口，权重配置为1，即采用轮询方式（轮流提供服务）。

![image-20250313103530565](https://github.com/xiangy6/chatserver/blob/main/img/nginx.png)

| 第三方软件 | 版本             |
| ---------- | ---------------- |
| mysql      | 8.0.41           |
| nginx      | 1.12.2           |
| json       | 3.7.3            |
| redis      | 7.0.2            |
| muduo      | *Copyright* 2010 |

| hiredis    | *Copyright* 2015 |
