/****************************************
 * Version: 1.0.0
 * File Name: client.h
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: Declare some functions you need when you want to chat with your friends and add new friends in this UI.
****************************************/

// #ifndef __CLIENT_H
// #define __CLIENT_H


// #include "chat.h"

// extern int init_net (const char *);
// extern int signin (const char *, const char *);
// extern int signup (const char *, const char *);
// extern int inform_net ();

// extern int load_friends_list (char friendlist[][MAX_LENGTH], int *);
// extern int send_msg (const char *, const char *);

// extern int search_friends (const char *, char friendname[][MAX_LENGTH], int *);
// extern int add_friend_request (const char *);

// extern int agree_add_friend(const char* friendname);
// extern int refuse_add_friend(const char* friendname);

// extern int request_chathistory (const char *, char namelist[][MAX_LENGTH], char msglist[][MAX_LENGTH]);

// extern int agree_recv_file ();
// extern int refuse_recv_file ();
// extern int request_useravator ();
// extern int send_a_file (const char *path, const char *friendname);

// extern int send_useravator (int);

// #endif
//********************************************************8

#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define PORT 4565
#define MAXBUFFNUM 100
typedef signed long int INT32S;
typedef unsigned long int INT32U;
typedef unsigned char INT8U;
typedef signed char INT8S;
typedef unsigned short int INT16U;
typedef signed short int INT16S;
typedef float FP32;

typedef struct buffclient{
    INT32S loginresult;
    INT32S regresult;
    INT32S addcontres;
} buffclient;

extern pthread_t client_recvthread_id;
extern sem_t sem;
extern sem_t sem4friend;
extern buffclient buff;
extern INT32S chat_sockfd;
extern INT32S usr_tou[100];
extern char usr_name4contactl[100][30];

int linkToServer(char *server_ip);
void *_clientRecvThread(int recv_sockfd);
INT32S sendBySocket(INT32S socketfd, INT8S *content); //将content内容原封不动地发送出去，0：发送失败，1：发送成功
INT32S sendLogIn(const char *usr_name, const char *usr_pwd);//发送登录用户名，密码，0：失败，1：成功
void sendLogOut(char *usr_name);//发送退出用户名，0：失败，1：成功
INT32S sendRegister(const char *usr_name, const char *usr_pwd, INT32S image_ID);//发送注册，0：失败，1：成功
void sendMsg(char *who, char *msg);//who是发给谁
INT32S sendAddContact(char *who);//who是添加谁
void sendRmvContact(char *who);//who是删除谁
void getLogInResult(INT32S login_result); //process解析出登录结果后调用这个函数，传给我0或1，我来通知图形界面，注意：接受到别人的消息和获得好友列表时不是这样，直接在函数内改图形界面
void getRegResult(INT32S reg_result);
void getADCResult(INT32S adc_result);

void process(char *str);

char *int_to_str(int num);
int str_to_int(char *str);

//int recv_msg(char *username, char *msg);

#endif
