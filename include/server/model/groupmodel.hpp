#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// 维护群组信息的操作接口
class GroupModel
{
public:
    bool createGroup(Group &group); // 创建群组

    void addGroup(int userid, int groupid, string role); // 加入群组

    vector<Group> queryGroups(int userid); // 查询用户所在群组信息

    vector<int> queryGroupUsers(int userid, int groupid); // 根据指定的groupid查询群组用户列表，主要用于群聊业务
};

#endif