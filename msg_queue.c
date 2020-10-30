/****************************************
 * Version: 1.0.0
 * File Name: msg_queue.c
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: Define the functions for the message
****************************************/
#include "msg_queue.h"
#include <string.h>

//Create a new message to save the text
Message *Message_new (const char *name, const char *text){
    Message *msg = (Message *)malloc (sizeof(Message));
    msg->prev = NULL;
    msg->next = NULL;
    strcpy (msg->name, name);
    strcpy (msg->text, text);
    return msg;
}

//Create a queue for the message like the store
Msg_queue *Msg_queue_new (){
    Msg_queue *msg_queue = (Msg_queue *)malloc (sizeof(Msg_queue));
    msg_queue->head = Message_new ("", "");
    msg_queue->tail = msg_queue->head;
    msg_queue->num = 0;
    return msg_queue;
}

//Push the message into the queue
void Msg_queue_push (Msg_queue *msg_queue, Message *msg){
    msg_queue->tail->next = msg;
    msg->prev = msg_queue->tail;
    msg->next = NULL;
    msg_queue->tail = msg;
    msg_queue->num++;
}

//Get the lastest message in the queue
Message Msg_queue_front (Msg_queue *msg_queue){
    return *(msg_queue->head->next);
}

//Delete the lastest message in the queue
void Msg_queue_pop (Msg_queue *msg_queue){
    if (msg_queue->num <= 0) return;
    Message *msg = msg_queue->head->next;
    msg_queue->head->next = msg->next;
    if (msg->next) msg->next->prev = msg_queue->head;
    if (msg == msg_queue->tail) msg_queue->tail = msg_queue->head;
    free(msg);
    msg_queue->num--;
}

//Get the size of the queue
int Msg_queue_get_num (Msg_queue *msg_queue){
    return msg_queue->num;
}

//Judge the queue is empty or not
int Msg_queue_is_empty (Msg_queue *msg_queue){
    return msg_queue->num == 0;
}
