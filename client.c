#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t client_recvthread_id;
sem_t sem;
sem_t sem4friend;
buffclient buff;
INT32S chat_sockfd;
INT32S usr_tou[100];
char usr_name4contactl[100][30];
INT32S contactfreep=0;

int linkToServer(char *server_ip)
{
    struct sockaddr_in pin_addr;

    memset(&pin_addr, 0, sizeof(pin_addr));
    pin_addr.sin_family = AF_INET;
    pin_addr.sin_addr.s_addr = inet_addr(server_ip);
    pin_addr.sin_port = htons(PORT);

    if ((chat_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return 0;
    }

    if (connect(chat_sockfd, (struct sockaddr *)&pin_addr, sizeof(pin_addr)) == -1)
    {
        perror("connect");
        return 0;
    }

    if(pthread_create(&client_recvthread_id,NULL,_clientRecvThread,chat_sockfd)!=0)
    {
        perror("thread create");
        return 0;
    }

    sem_init(&sem, 0, 0);
    sem_init(&sem4friend,0,0);

    return 1;
}

void *_clientRecvThread(int recv_sockfd)
{
    char buff_thread[MAXBUFFNUM] = {0};

    while(1){
        memset(buff_thread, 0, sizeof(buff_thread) / sizeof(buff_thread[0]));
        if(recv(recv_sockfd,buff_thread,MAXBUFFNUM,0)==-1)
        {
            perror("receive");
            exit(1);
        }

        //TO-DO:process
        printf("@@@@%s\n#####",buff_thread);
        process(buff_thread);

        sleep(1);
    }
}

//将content内容原封不动地发送出去，0：发送失败，1：发送成功
INT32S sendBySocket(INT32S socketfd, INT8S *content)
{
    puts(content);
    printf("strlen content:%d\n",strlen(content));
    if (send(socketfd, content, strlen(content), 0)==-1)
    {
        perror("sendbysocket");
        return 0;
    }
    else
    {
        return 1;
    }
}

//发送用户名，密码，0：失败，1：成功
INT32S sendLogIn(const char *usr_name, const char *usr_pwd)
{
    INT32S login_res;

    char content[80] = {'l', 'g', 'i', 0};
    strcat(content, "$");
    strcat(content, usr_name);
    strcat(content, "$");
    strcat(content, usr_pwd);
    strcat(content, "$");
    sendBySocket(chat_sockfd, content);

    sem_wait(&sem);//等待另一个线程接受登录结果

    login_res = buff.loginresult;

    return login_res;
}

//发送退出用户名，0：失败，1：成功
void sendLogOut(char *usr_name)
{
    char content[40] = {'l', 'g', 'o', 0};
    strcat(content, "$");
    strcat(content, usr_name);
    strcat(content, "$");

    sendBySocket(chat_sockfd, content);
}

//发送注册，0：失败，1：成功
INT32S sendRegister(const char *usr_name, const char *usr_pwd, INT32S image_ID)
{
    char content[80] = {'r', 'e', 'g', '$', 0};
    strcat(content, usr_name);
    strcat(content, "$");
    strcat(content, usr_pwd);
    strcat(content, "$");
    char tempimgid[2]={0};
    tempimgid[0]=image_ID+'0';
    tempimgid[1]=0;
    strcat(content, tempimgid);
    strcat(content, "$");
    sendBySocket(chat_sockfd, content);

    sem_wait(&sem);

    INT32S reg_result = buff.regresult;

    return reg_result;
}

//who是发给谁
void send_msg(char *who, char *msg)
{
    char content[110] = {'s', 'd', 'm', '$', 0};
    strcat(content, who);
    strcat(content, "$");
    strcat(content, msg);
    strcat(content, "$");
    sendBySocket(chat_sockfd, content);
}

//who是添加谁
INT32S sendAddContact(char *who)
{
    char content[50] = {'a', 'd', 'c', '$'};
    strcat(content, who);
    strcat(content, "$");

    sendBySocket(chat_sockfd, content);

    sem_wait(&sem);

    INT32S adc_result = buff.addcontres;

    return adc_result;
}

//who是删除谁
void sendRmvContact(char *who)
{
    char content[50] = {'r', 'm', 'c', '$', 0};
    strcat(content, who);
    strcat(content, "$");
    sendBySocket(chat_sockfd, content);
}

//process解析出登录结果后调用这个函数，传给我0或1，我来通知图形界面，注意：接受到别人的消息和获得好友列表时不是这样，直接在函数内改图形界面
void getLogInResult(INT32S login_result)
{
    buff.loginresult = login_result;
    sem_post(&sem);
}

void getRegResult(INT32S reg_result)
{
    buff.regresult = reg_result;
    sem_post(&sem);
}

void getADCResult(INT32S adc_result)
{
    buff.addcontres = adc_result;
    sem_post(&sem);
}

// void process(char *xiaoxi)
// {
    
//     switch (xiaoxi[0])
//     {
//         case 'l':
//             //登录结果
//             int i;
//             for(i=0;xiaoxi[i]!='$'&&xiaoxi[i];i++)
//             {
//                 ;
//             }
//             i++;
//             char temp[2] = {0};
//             temp[0] = xiaoxi[i];
//             temp[1] = 0;
//             getLogInResult(str_to_int(temp));
//             break;

//         case 'r':
//             int i;
            
//             for(i = 0; xiaoxi[i]!='$'&&xiaoxi[i] ; i++)
//             {
//                 ;
//             }
//             i++;

//             char temp[11] = {0};
//             for(int j=0;xiaoxi[i]!='$'&&xiaoxi[i];i++,j++)
//             {
//                 temp[j] = xiaoxi[i];
//             }

//             getRegResult(str_to_int(temp));
//             break;

//         case 'a':
//             int i;
//             for (i = 0; xiaoxi[i] != '$' && xiaoxi[i]; i++)
//             {
//                 ;
//             }
//             i++;

//             char temp[2] = {0};
//             temp[0] = xiaoxi[i];
//             temp[1] = 0;
//             getADCResult(str_to_int(temp));
//             break;

//         case 's':
//             if(xiaoxi[1]=='d')
//             {
//                 int i;
//                 for (i = 0; xiaoxi[i] != '$' && xiaoxi[i]; i++)
//                 {
//                     ;
//                 }
//                 i++;

                
//             }
//             else if(xiaoxi[1]=='c')
//             {

//             }

//         default:
//             break;
//     }
// }
//int 转 str
char* int_to_str(int num)
{
    char *str=(char*)malloc(100*sizeof(char));
    char *_str[100];
    memset(str,0,sizeof(str));
    memset(_str,0,sizeof(_str));
    int i=0,j=0;
    if(num==0)
    {
        str[0]=_str[0]='0';
    }
	else if(num<0)
	{
		str[0]=_str[0]='-';
		i++;j++;
		num*=-1;
		while(num>0)
		{
			_str[i++]=num%10+'0';
			num=num/10;
		}
		while(i-1-j>=0)
		{
			str[j]=_str[i-j];
			j++;
		}		
		//-123 i=4 j=1 1=2 2=1 3=0
		//-321 1=3 2=2 3=1
	}
	else
	{
		while(num>0)
		{
			_str[i++]=num%10+'0';
			num=num/10;
		}
		while(i-1-j>=0)
		{
			str[j]=_str[i-1-j];
			j++;
		}		
	}

	
	
    //printf("%s\n%s\n",_str,str);
    return str;
}

//str 转 int
int str_to_int(char* str)
{
    int num=0;
    int len=strlen(str);

    int i=0,flag=1;
	if(str[i]=='-')
	{
		i++;
		flag=-1;
	}
    while(i<len&&str[i]>='0'&&str[i]<='9')
    {
        num*=10;
        num+=str[i++]-'0';
    }
	num=num*flag;
    return num;
}

void process(char *str)
{
    char *cmd=(char*)malloc(sizeof(char)*5);
    cmd[0] = str[0];
    cmd[1] = str[1];
    cmd[2] = str[2];
    cmd[3]=0;
    char *cd1 = (char *)malloc(sizeof(char) * 110);
    memset(cd1, 0, sizeof(cd1));
    int Currentlocation = 4;
    int LengthOfCd1 = 0;
    while (str[Currentlocation] != '$')
    {
        cd1[LengthOfCd1++] = str[Currentlocation++];
    }
    if (!strcmp(cmd, "reg"))
    {
        getRegResult(str_to_int(cd1));
    }
    else if (!strcmp(cmd, "adc"))
    {
        int add_successful = (int)(cd1[0] - '0');
        getADCResult(add_successful);
    }
    else if (!strcmp(cmd, "lgi"))
    {
        getLogInResult(cd1[0] - '0');
    }
    else
    {
        Currentlocation++;
        char *cd2 = (char *)malloc(sizeof(char) * 110);
        memset(cd2, 0, sizeof(cd2));
        int LengthOfCd2 = 0;
        while (str[Currentlocation] != '$')
        {
            cd2[LengthOfCd2++] = str[Currentlocation++];
        }
        if (!strcmp(cmd, "sdm"))
        {
            //gui的函数，传入cd1：张三，cd2：哈哈
            recv_msg(cd1, cd2);
        }
        else
        {
            printf("zhunbei cd3\n");
            Currentlocation++;
            char *cd3 = (char *)malloc(sizeof(char) * 10);
            memset(cd3, 0, sizeof(cd3));
            int LengthOfCd3 = 0;
            while (str[Currentlocation] != '$')
            {
                cd3[LengthOfCd3++] = str[Currentlocation++];
            }
            /*****
             * TO-DO填数组，检测结尾，信号量用新的一个
             * ****/
            printf("cd1:%s,cd2:%s,cd3:%s\n",cd1,cd2,cd3);
            if(strcmp(cd1,"-1")==0)
            {
                //end
                puts("haoyouliebiao-1");
                usr_tou[contactfreep] = -1;
                contactfreep = 0;
                sem_post(&sem4friend);
            }
            else
            {
                strcpy(usr_name4contactl[contactfreep], cd2);
                usr_tou[contactfreep] = str_to_int(cd3);
                contactfreep++;
            }
            printf("before free cd3\n");
            free(cd3);
            printf("free cd3\n");
        }
        printf("before free cd2\n");
        free(cd2);
        printf("free cd2\n");
    }

    printf("before free cd1\n");
    free(cd1);
    printf("free cd1\n");
    free(cmd);
    printf("free cmd\n");
}