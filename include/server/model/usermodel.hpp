#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"
#include <iostream>
#include <string>

using namespace std;

//USER表的数据操作类
class UserModel{
public:
    // USER表的插入操作
    bool insert(User &user);

    // USER表的查询操作
    User query(int id);

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();
};

#endif