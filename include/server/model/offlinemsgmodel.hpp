#ifndef OFFLINEMSGMODEL_H
#define OFFLINEMSGMODEL_H

#include <string>
#include <vector>
using namespace std;

// 提供存储离线消息方法的接口
class offlineMsg
{
public:
    void insert(int userid, string msg); // 存储用户的离线消息

    void remove(int userid);// 删除用户的离线消息

    vector<string> query(int userid);// 查询用户的离线消息
};

#endif