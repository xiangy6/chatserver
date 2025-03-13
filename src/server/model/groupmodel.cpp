#include "groupmodel.hpp"
#include "db.hpp"

using namespace std;

bool GroupModel::createGroup(Group &group)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into AllGroup(groupname,groupdesc) values('%s','%s')",
            group.getgpname().c_str(), group.getgpdesc().c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
} // 创建群组

void GroupModel::addGroup(int userid, int groupid, string role)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser(groupid,userid,grouprole) values('%d','%d','%s')",
            groupid, userid, role.c_str());

    Mysql mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
} // 加入群组

vector<Group> GroupModel::queryGroups(int userid)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup a \
        inner join GroupUser b on a.id=b.groupid where b.userid=%d",
            userid);

    vector<Group> groupVec;
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            // 查出userid所有的群组消息
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setid(atoi(row[0]));
                group.setgpname(row[1]);
                group.setgpdesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    // 查询群组的用户信息
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from User a \
            inner join GroupUser b on b.userid=a.id where b.groupid=%d",
                group.getid());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
} // 查询用户所在群组信息

vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from GroupUser where groupid=%d and userid!=%d",groupid,userid);

    vector<int> idVec;
    Mysql mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
} // 根据指定的groupid查询群组用户列表，主要用于群聊业务