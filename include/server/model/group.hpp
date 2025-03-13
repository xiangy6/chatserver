#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

// Allgroup表的ORM类
class Group
{
public:
    Group(int id = -1, string groupname = "", string groupdesc = "")
    {
        this->id = id;
        this->groupname = groupname;
        this->groupdesc = groupdesc;
    }

    void setid(int id) { this->id = id; }
    void setgpname(string groupname) { this->groupname = groupname; }
    void setgpdesc(string groupdesc) { this->groupdesc = groupdesc; }

    int getid() { return this->id; }
    string getgpname() { return this->groupname; }
    string getgpdesc() { return this->groupdesc; }
    vector<GroupUser> &getUsers() {return this->users;}


private:
    int id;
    string groupname;
    string groupdesc;
    vector<GroupUser> users;

};

#endif