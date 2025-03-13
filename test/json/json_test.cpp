#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

string fun1(){
    json js;
    js["msg_type"] = 3;
    js["from"] = "zhansan";
    js["msg"]["zhangsan"] = "hello1!";
    js["msg"]["xy"] = "hello2!";
    js["to"] = "lisi";

    string sendBuf = js.dump(); //json序列化
    cout << sendBuf.c_str() << endl;

    return sendBuf;
}

void fun2(){
    json js;
    vector<int> v1;
    v1.push_back(1);
    v1.push_back(6);
    v1.push_back(8);

    js["list"] = v1;

    map<int,string> m1;
    m1.insert({1,"重庆"});
    m1.insert({6,"开州"});
    m1.insert({8,"云枫街道"});

    js["path"] = m1;

    string sendBuf = js.dump();

    cout << sendBuf.c_str() << endl;
}

int main(){
    auto recBuf = fun1();
    json jsbuf = json::parse(recBuf);     //json反序列化
    cout << jsbuf["msg_type"]<<endl;
    cout << jsbuf["msg"]<<endl;
    cout << jsbuf["from"]<<endl;
    cout << jsbuf["to"]<<endl;

    return 0;
}

