#include "friendmodel.hpp"
#include "db.hpp"
#include <iostream>

using namespace std;

void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into Friend values('%d','%d')", userid,friendid);

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
} // 添加好友关系

vector<User> FriendModel::query(int userid)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select User.id,User.name,User.state from User inner join Friend on Friend.friendid = User.id where Friend.userid=%d", userid);
    vector<User> vec;
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // 将userid用户的所有好友消息放入vec后返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
} // 返回好友列表