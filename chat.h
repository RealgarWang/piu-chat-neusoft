/****************************************
 * Version: 1.0.0
 * File Name: chat.h
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: Declare some functions you need when you want to chat with your friends and add new friends in this UI. 
****************************************/

#ifndef __CHAT_H
#define __CHAT_H

#include <gtk/gtk.h>

#define MAX_LENGTH 21
#define MAX_NUM 30

extern GtkWidget *load_chat_window (const char *recv_username);

extern void recv_useravator (int avator_id);
extern void recv_msg (const char *friendname, const char *msg);
extern int recv_friend_request (const char *name);
extern void friend_request_response (const char *name, int response);
extern void ok_button_clicked (GtkWidget *widget, gpointer data);

#endif

