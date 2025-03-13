#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "user.hpp"


// 群组用户，多一个role角色，其他信息从User类继承
class GroupUser : public User
{
public:
    void setRole(string role) { this->role = role; }

    string getRole() { return this->role; }

private:
    string role;
};

#endif