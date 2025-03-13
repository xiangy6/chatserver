#include "usermodel.hpp"
#include "db.hpp"
#include <iostream>

using namespace std;

// USER表的插入操作
bool UserModel::insert(User &user)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name,password,state) values('%s','%s','%s')",
            user.getname().c_str(), user.getpwd().c_str(), user.getstate().c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            // 获取插入成功的用户数据生成的主键userid
            user.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// USER表的查询操作
User UserModel::query(int id)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id=%d", id);
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setpwd(row[2]);
                user.setstate(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update User set state='%s' where id=%d", user.getstate().c_str(), user.getid());
    Mysql mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

void UserModel::resetState()
{
    char sql[1024] = "update User set state='offline' where state='online'";
    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}