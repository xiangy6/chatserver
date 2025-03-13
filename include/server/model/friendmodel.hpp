#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>

using namespace std;

// 维护好友信息的操作接口
class FriendModel
{
public:
    void insert(int userid, int friendid); // 添加好友关系

    vector<User> query(int userid); // 返回好友列表
};

#endif