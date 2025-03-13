#include "offlinemsgmodel.hpp"
#include "db.hpp"
#include <iostream>

using namespace std;

void offlineMsg::insert(int userid, string msg)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into OfflineMessage values('%d','%s')", userid, msg.c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
} // 存储用户的离线消息

void offlineMsg::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from OfflineMessage where userid=%d", userid);

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
} // 删除用户的离线消息

vector<string> offlineMsg::query(int userid)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from OfflineMessage where userid=%d", userid);
    vector<string> vec;
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            // 将userid用户的所有离线消息放入vec后返回
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
} // 查询用户的离线消息