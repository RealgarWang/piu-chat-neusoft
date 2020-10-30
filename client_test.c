// /****************************************
//  * Version: 1.0.0
//  * File Name: client_test.c
//  * Model Name: Client
//  * Module Name: ChatUI
//  * uCom: i5-6300HQ
//  * 
//  * Create Date: 2018/09/05
//  * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
//  * 
//  * Abstract Description: Test the whole programe. 
// ****************************************/

#include <stdio.h>
#include <string.h>
#include "client.h"
#include "chat.h"
#include "client.h"

int request_useravator ()
{
    return 1;
}

int send_a_file (const char *filename, const char *cur_chat_friend_name){
    return 1;
}

// int init_net (const char *server_ip){
//     return 1;
// }

// int signin (const char *username, const char *password){
//     return 1;
// }

// int signup (const char *username, const char *password){
//     return 1;
// }

// int inform_net (){
//     return 1;
// }

int load_friends_list (char friendlist[][30], int *name){
    int i;
    for( i = 0; usr_tou[i] != -1; i++){
        strcpy(friendlist[i], usr_name4contactl[i]);
        name[i] = usr_tou[i];
    }
    return i;
    // strcpy (friendlist[0], "a");
    // name[0] = 1;
    // strcpy (friendlist[1], "b");
    // name[1] = 2;
    // strcpy (friendlist[2], "c");
    // name[2] = 3;
    // strcpy (friendlist[3], "d");
    // name[3] = 4;
    // strcpy (friendlist[4], "e");
    // name[4] = 5;
    // strcpy (friendlist[5], "f");
    // name[5] = 6;
    // strcpy (friendlist[6], "g");
    // name[6] = 7;
    // strcpy (friendlist[7], "h");
    // name[7] = 8;
    // strcpy (friendlist[8], "i");
    // name[8] = 9;
    // return 9;
}

// int send_msg (const char *friendname, const char *msg){
//     return 1;
// }

int search_friends (const char *name, char friendname[][20], int *photo_id){
    int result = add_friend_request(name);
    return result;
}

int add_friend_request (const char *friendname){
    int result = sendAddContact(friendname);
    return result;
}

int request_chathistory (const char *friendname, char namelist[][20], char msglist[][30]){
    sprintf (namelist[0], "User1"); sprintf (msglist[0], "Hello");
    sprintf (namelist[1], "User2"); sprintf (msglist[1], "Hi");
    sprintf (namelist[2], "User1"); sprintf (msglist[2], "How are you?");
    return 3;
}

int agree_add_friend(const char* friendname)
{
    return 1;
}

int refuse_add_friend(const char* friendname)
{
    return 1;
}

int agree_recv_file (){
    return 1;
}

int refuse_recv_file (){
    return 1;
}

int send_useravator (int avator_id){
    return 1;
}

// int recv_msg(char * username, char* msg){
//     return 1;
// }
