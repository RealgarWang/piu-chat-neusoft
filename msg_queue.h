/****************************************
 * Version: 1.0.0
 * File Name: msg_queue.h
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: Define the structure Message and the queue for the message and extern functions would  be used
****************************************/

#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H

#include <stdlib.h>
#include <string.h>

#define NAME_MAX_LENGTH 20
#define TEXT_MAX_LENGTH 50

//The linklist for the message, contain the pointer and the node with the name of the user and text would be send
typedef struct Message{
    struct Message *prev, *next;
    char name[NAME_MAX_LENGTH];
    char text[TEXT_MAX_LENGTH];
} Message;

//The queue for the message
typedef struct Msg_queue{
    struct Message *head, *tail;
    int num;
} Msg_queue;

//Some functions which would be define in the msg_queue.c
extern Message *Message_new (const char *name, const char *text);
extern Msg_queue *Msg_queue_new ();
extern void Msg_queue_push (Msg_queue *msg_queue, Message *msg);
extern Message Msg_queue_front (Msg_queue *msg_queue);
extern void Msg_queue_pop (Msg_queue *msg_queue);
extern int Msg_queue_get_num (Msg_queue *msg_queue);
extern int Msg_queue_is_empty (Msg_queue *msg_queue);

#endif
