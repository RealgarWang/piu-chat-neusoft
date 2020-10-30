/****************************************
 * Version: 1.0.0
 * File Name: main.c
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: Use the main function to create the sign in and sign up window
****************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
 #include "chat.h"
// #include "client.h"

char server_ip[20] = "192.168.43.163";
GtkWidget *signin_window;
GtkWidget *signup_window;
GtkBuilder *builder;
gint useravator;

//Give the warning to the user
void error_dialog (GtkWidget *window, const char *msg){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg);
    gtk_window_set_title(GTK_WINDOW(dialog), "警告");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Signup
struct REGIST_MASSAGE
{
    GtkWidget* username_text;
    GtkWidget* password_text;
    GtkWidget* repeat_password_text;
} regist_massage;

//Signin
struct LOGIN_MASSAGE
{
    GtkWidget* username_text;
    GtkWidget* password_text;
} login_massage;

//Jump from the signup window to the signin window
void signup_to_signin (GtkWidget *widget, gpointer data){
    gtk_widget_hide (signup_window);
    gtk_widget_show_all (signin_window);
}

//Click the button to send the signup information to the server
void signup_button_on_click (GtkWidget *widget, struct REGIST_MASSAGE* regist_massage){
    const char *username = gtk_entry_get_text(GTK_ENTRY(regist_massage->username_text));
    const char *password = gtk_entry_get_text(GTK_ENTRY(regist_massage->password_text));
    const char *repeat_password = gtk_entry_get_text(GTK_ENTRY(regist_massage->repeat_password_text));
    
    printf("Signup username: %s password: %s.\n", username, password);

    if (strlen (username) <= 0){
        error_dialog (signup_window, "用户名不能为空！");
        return;
    }

    if (strlen (password) <= 0){
        error_dialog (signup_window, "密码不能为空！");
        return;
    }

    if (strcmp (password, repeat_password) != 0){
        error_dialog (signup_window, "两次密码不匹配！");
        return;
    }

    srand((unsigned int)time(NULL));
    int randnum = rand() % 8;
    int succeed = sendRegister (username, password, randnum);
    if(succeed == 0){
        printf ("The username has already existed!\n");
        error_dialog (signup_window, "用户名已经存在！");
        return;
    }
    else{

        puts("Signup successful!\n");
        char *msg = "注册成功！返回登陆界面。";
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(signup_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", msg);
        gtk_window_set_title(GTK_WINDOW(dialog), "成功");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        signup_to_signin (NULL, NULL);
        return;
    }
}

// void choose_button_clicked (GtkWidget *widget, gpointer data){
// 	useravator = avator_selected;

// 	recv_useravator (avator_selected);
// 	send_useravator (avator_selected);

// 	GtkWidget *avator_dialog = GTK_WIDGET (gtk_builder_get_object(builder, "avator_dialog"));
// 	gtk_dialog_response ((GtkDialog *)avator_dialog, GTK_RESPONSE_OK);
// }




//Open the sign up window
GtkWidget * load_signup_window (){
    GtkBuilder *signup_builder = gtk_builder_new_from_file ("./glade/signup.glade");
    if(signup_builder == NULL) return NULL;

    signup_window = GTK_WIDGET(gtk_builder_get_object (signup_builder, "window"));
    g_signal_connect (G_OBJECT(signup_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *signup_button = GTK_WIDGET(gtk_builder_get_object (signup_builder, "signup"));
    g_signal_connect (G_OBJECT(signup_button), "clicked", G_CALLBACK (signup_button_on_click), &regist_massage);

    // GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object (signup_builder, "cancel"));
    // g_signal_connect (G_OBJECT(cancel_button), "clicked", G_CALLBACK (signup_to_signin), NULL);
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    GtkWidget *username_text = GTK_WIDGET(gtk_builder_get_object (signup_builder, "username"));
    GtkWidget *password_text = GTK_WIDGET(gtk_builder_get_object (signup_builder, "password"));
    GtkWidget *repeat_password_text = GTK_WIDGET(gtk_builder_get_object (signup_builder, "verification"));

    regist_massage.username_text = username_text;
    regist_massage.password_text = password_text;
    regist_massage.repeat_password_text = repeat_password_text;

    return signup_window;
}

//Click the button to send the signin information to the server
void signin_button_on_click(GtkWidget *widget, struct LOGIN_MASSAGE* login_massage){
    const char *username = gtk_entry_get_text (GTK_ENTRY(login_massage->username_text));
    const char *password = gtk_entry_get_text (GTK_ENTRY(login_massage->password_text));

    if (strlen (username) == 0){
        printf ("Username can't be empty!\n");
        error_dialog (signin_window, "用户名不能为空！");
        return;
    }
    if (strlen (password) == 0){
        printf ("Password can't be empty!\n");
        error_dialog (signin_window, "密码不能为空！");
        return;
    }

    printf ("Login username: %s password: %s.\n", username, password);

    int succeed = sendLogIn (username, password);

    //int succeed = 1;
    printf("%d\n", succeed);
    
    if (succeed == 0){
        printf ("Username or password incorrect!\n");
        error_dialog (signin_window, "用户名或密码错误！");
        return;
    }

    gtk_widget_hide (signin_window);
    
    GtkWidget *chat_window = load_chat_window (username);
    if (chat_window == NULL){
        printf ("Load chat window failed!\n");
        error_dialog (signin_window, "加载主界面失败！");
        exit(1);
    }
    else{
        printf ("Load chat window successfully!\n");
    }
}

//Jump from the signin window to the signup window
void signin_to_signup(GtkWidget *widget, gpointer data){
    gtk_widget_hide (signin_window);
    gtk_widget_show_all (signup_window);
}

//Open the sign in windows
GtkWidget *load_signin_window (){
    GtkBuilder *signin_builder = gtk_builder_new_from_file ("glade/signin.glade");
    if(signin_builder == NULL) return NULL;
    
    signin_window = GTK_WIDGET(gtk_builder_get_object (signin_builder, "window"));
    g_signal_connect (G_OBJECT(signin_window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // GtkWidget *logo_image = GTK_WIDGET(gtk_builder_get_object (signin_builder, "logo_image"));
    // gtk_image_set_from_file ((GtkImage *)logo_image,"logo.gif");

    GtkWidget *signin_button = GTK_WIDGET(gtk_builder_get_object (signin_builder, "signin"));
    //等待修改，链接聊天页面

    g_signal_connect (G_OBJECT(signin_button), "clicked", G_CALLBACK (signin_button_on_click), &login_massage);

    GtkWidget *signup_button = GTK_WIDGET(gtk_builder_get_object (signin_builder, "regist"));
    g_signal_connect (G_OBJECT(signup_button), "clicked", G_CALLBACK (signin_to_signup), NULL);

    GtkWidget *username_text = GTK_WIDGET(gtk_builder_get_object (signin_builder, "username"));
    GtkWidget *password_text = GTK_WIDGET(gtk_builder_get_object (signin_builder, "password"));

    login_massage.username_text = username_text;
    login_massage.password_text = password_text;

    return signin_window;
}

//Load the ChatUI
int main (int argc, char* argv[]){
    int result = linkToServer (server_ip);
    //int result = 1;
    if(result == 0){
        printf("Connect to server %s failed!\n", server_ip);
        exit(1);
    }
    else{
        printf("Connect to server %s successful!\n", server_ip);
    }
    
    gtk_init (&argc, &argv);

    signin_window = load_signin_window ();
    if(signin_window == NULL){
        printf ("Load signin window failed!\n");
        exit(1);
    }
    else{
        printf ("Load signin window successfully!\n");
    }

    signup_window = load_signup_window ();
    if(signup_window == NULL){
        printf ("Load signup window failed!\n");
        exit(1);
    }
    else{
        printf ("Load signup window successfully!\n");
    }

    gtk_widget_show_all (signin_window);
    gtk_main();
    return 0;
}
