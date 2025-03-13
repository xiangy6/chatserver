#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// USER表的ORM类
class User
{
public:
    User(int id = -1, string name = "", string password = "", string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }

    void setid(int id) { this->id = id; }
    void setname(string name) { this->name = name; }
    void setpwd(string password) { this->password = password; }
    void setstate(string state) { this->state = state; }

    int getid() { return this->id; }
    string getname() { return this->name; }
    string getpwd() { return this->password; }
    string getstate() { return this->state; }

protected:
    int id;
    string name;
    string password;
    string state;
};

#endif