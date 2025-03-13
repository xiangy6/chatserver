#ifndef PUBLIC_H
#define PUBLIC_H

/*
server和client的公共文件
*/

// {"msgid":1,"id":1,"password":"123456"}
// {"msgid":1,"id":3,"password":"111111"}
// {"msgid":7,"id":3,"friendid":1}
// {"msgid":5,"from":1,"to":3,"msg":"hello~"}
enum MsgType
{
    LOGIN_MSG = 1,  //登录消息
    LOGIN_MSG_ACK,  //登录响应消息
    REG_MSG,     //注册消息
    REG_MSG_ACK,  //注册响应消息
    ONE_CHAT_MSG, //聊天消息
    ADD_FRIEND_MSG,  //添加好友消息

    CREATE_GROUP_MSG,  //创建群聊消息
    ADD_GROUP_MSG,  //加入群聊消息
    GROUP_CHAT_MSG,  //群聊消息
    LOGINOUT_MSG,

};

#endif