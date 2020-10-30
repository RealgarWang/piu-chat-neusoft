/****************************************
 * Version: 1.0.0
 * File Name: chat.c
 * Model Name: Client
 * Module Name: ChatUI
 * uCom: i5-6300HQ
 * 
 * Create Date: 2018/09/05
 * Author/Corporation: Zhangqiyang, Zoubowen / BITCS
 * 
 * Abstract Description: You can chat with your friends and add new friends in this UI. 
****************************************/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "chat.h"
#include "friendbox.h"
#include "msg_queue.h"
#include "client.h"

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *addfriend_window;
GtkWidget *friends_listbox;

gchar username[20];
gint useravator;
gchar cur_chat_friend_name[20];

gint avator_selected;

//The struction of the list of friends
GList *friends_list;
typedef struct FriendItem{
	gchar friend_name[MAX_LENGTH];
	GtkWidget *friendbox;
	Msg_queue *msg_queue;
} FriendItem;

//Establish the new instance
FriendItem *utalk_frienditem_new (const gchar *friend_name, GtkWidget *friendbox, Msg_queue *msg_queue){
	FriendItem *frienditem = (FriendItem *)malloc (sizeof(FriendItem));
	strcpy (frienditem->friend_name, friend_name);
	frienditem->friendbox = friendbox;
	frienditem->msg_queue = msg_queue;
	return frienditem;
}

// Execute the function when something is wrong
void server_error_dialog (GtkWidget *window){
	const gchar *msg = "Connect Failed!";
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg);
	gtk_window_set_title(GTK_WINDOW(dialog), "Error");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void error_dialog_z (GtkWidget *window, const char *msg){
    GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", msg);
    gtk_window_set_title(GTK_WINDOW(dialog), "警告");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
//Add new friends
void add_friend (const char *friend_name, gint avator_id, const char *msg, int position){
	gchar path[10];
	sprintf (path, "glade/avators/%d.jpg", avator_id);
	GtkWidget *friendbox_with_msg = utalk_friendbox_with_msg_new (path, friend_name, msg);
	gtk_list_box_insert ((GtkListBox *)friends_listbox, friendbox_with_msg, position);

	Msg_queue *msg_queue = Msg_queue_new ();
	Message *message = Message_new (friend_name, msg);
	FriendItem *frienditem = utalk_frienditem_new (friend_name, friendbox_with_msg, msg_queue);
	friends_list = g_list_insert (friends_list, frienditem, position);
}

//Choose the file you want to upload
void choose_file (GtkWidget *widget, gpointer data){
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("打开文件", (GtkWindow *)window, GTK_FILE_CHOOSER_ACTION_OPEN,
										  "取消", GTK_RESPONSE_CANCEL, "打开", GTK_RESPONSE_ACCEPT, NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT){
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		gchar *filename = gtk_file_chooser_get_filename (chooser);
		g_print ("%s\n", filename);
		send_a_file (filename, cur_chat_friend_name);
		// Open file and send to cur_chat_friend_name
	}
	gtk_widget_destroy (dialog);
}

static GtkFileChooserConfirmation confirm_overwrite (GtkFileChooser *chooser, gpointer data){
	return GTK_FILE_CHOOSER_CONFIRMATION_CONFIRM;
}

// Save the file received
void save_file (){
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("保存文件", (GtkWindow *)window ,GTK_FILE_CHOOSER_ACTION_SAVE,
										  "取消", GTK_RESPONSE_CANCEL, "保存", GTK_RESPONSE_ACCEPT, NULL);
	GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	g_signal_connect (chooser, "confirm-overwrite", G_CALLBACK (confirm_overwrite), NULL);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_ACCEPT){
		char *filename = gtk_file_chooser_get_filename (chooser);
		g_print ("filename: %s\n", filename);
		// Save file here!
		g_free (filename);
	  }
	gtk_widget_destroy (dialog);
}

//Receive the file
int recv_file (){
	gchar *msg = "您收到了文件";
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
	gtk_window_set_title (GTK_WINDOW(dialog), "文件");
	gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy (dialog);

	if (result == GTK_RESPONSE_YES){
		agree_recv_file ();
		save_file ();
		return 1;
	}
	else{
		refuse_recv_file ();
		return 0;
	}
}

//Select the font that can use
void select_font (GtkFontChooser *self, gchar *fontname, gpointer user_data){
	GtkWidget *msg_textview = GTK_WIDGET(gtk_builder_get_object (builder, "msg_textview"));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (msg_textview));
	gtk_text_buffer_create_tag (buffer, "msg_font", "font", fontname, NULL);
	g_print ("font %s\n", fontname);
}

//Choose the word font
void choose_font (GtkWidget *widget, gpointer data){
	GtkWidget *dialog = gtk_font_chooser_dialog_new ("选择字体", (GtkWindow *)window);
	gint res = gtk_dialog_run (GTK_DIALOG (dialog));

	gtk_widget_destroy (dialog);
}

//Load the the window of history
void load_chathistory_ui (GtkWidget *widget, gpointer data){
	
	if (strcmp (cur_chat_friend_name, "") == 0) return;

	gchar namelist[MAX_NUM][MAX_LENGTH], msglist[MAX_NUM][MAX_LENGTH];
	gint msg_num = request_chathistory (cur_chat_friend_name, namelist, msglist);
	GtkWidget *chat_history_window = GTK_WIDGET (gtk_builder_get_object(builder, "chat_history_window"));

	GtkWidget *history_close_button = GTK_WIDGET(gtk_builder_get_object(builder, "history_close_button"));
	g_signal_connect_swapped (G_OBJECT(history_close_button), "clicked", G_CALLBACK(gtk_widget_destroy), (GtkWidget *)chat_history_window);

	GtkWidget *chathistory_label = GTK_WIDGET(gtk_builder_get_object(builder, "chathistory_label"));
	char text[20];
	sprintf (text, "与 %s 的历史消息", cur_chat_friend_name);
	gtk_label_set_text ((GtkLabel *)chathistory_label, text);

	GtkTextView *text_view = (GtkTextView *)gtk_builder_get_object(builder, "chat_history_textview");
	gtk_text_view_set_editable (text_view,	FALSE);
	gtk_text_view_set_cursor_visible (text_view, FALSE);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(buffer, &end);

	

	char msg[MAX_LENGTH];
	for (int i = 0; i < msg_num; i++){

		const char *name_color;
		if (strcmp (namelist[i], username) == 0) name_color = "blue_fg";
		else name_color = "green_fg";
		gtk_text_buffer_insert_with_tags_by_name (buffer, &end, namelist[i], -1, name_color, "name_l_margin", "name_t_margin", NULL);
		gtk_text_buffer_insert (buffer, &end, "\n", -1);
		gtk_text_buffer_insert_with_tags_by_name (buffer, &end, msglist[i], -1, "bigger_font", "msg_l_margin", "msg_t_margin", NULL);
		gtk_text_buffer_insert (buffer, &end, "\n", -1);

		// sprintf (msg, "\n%s:\n%s\n", namelist[i], msglist[i]);
		// gtk_text_buffer_insert (chat_history_textbuffer, &end, msg, -1);
	}
	chat_history_window = GTK_WIDGET (gtk_builder_get_object(builder, "chat_history_window"));
	gtk_widget_show_all ((GtkWidget *)chat_history_window);
	chathistory_label = GTK_WIDGET(gtk_builder_get_object(builder, "chathistory_label"));
	gtk_widget_show_all ((GtkWidget *)chathistory_label);
	// gtk_main ();
	// gtk_widget_destroy ((GtkWidget *)chat_history_window);
	// gtk_window_close ((GtkWindow *)chat_history_window);
}

//Response to the request
void friend_request_response (const char *name, int response){
	const gchar *msg = response ? "同意了您的好友请求" : "拒绝了您的好友请求";
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "%s %s", name, msg);
	gtk_window_set_title (GTK_WINDOW(dialog), "好友请求");
	gtk_dialog_run (GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);

	if (response == 0) return;
	add_friend (name, 3, "Hello, let's talk!", 0);
	gtk_widget_show_all (friends_listbox);
}

//Receive the request of new friends
int recv_friend_request (const char *name){
	// gchar *msg = "有用户请求添加您为好友";
	// window = GTK_WIDGET (gtk_builder_get_object(builder, "window"));
	// GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, "%s", msg);
	// gtk_window_set_title (GTK_WINDOW(dialog), "Friend Request");
	// gint result = gtk_dialog_run (GTK_DIALOG(dialog));
	// gtk_widget_destroy (dialog);

	// if (result == GTK_RESPONSE_NO){
	// 	refuse_add_friend (name);
	// 	return 0;
	// }

	g_print ("friend added!\n");
	add_friend (name, 3, "Hello, let's talk!", 0);

	friends_listbox = GTK_WIDGET(gtk_builder_get_object(builder, "friends_listbox"));
	gtk_widget_show_all (friends_listbox);
	agree_add_friend (name);
	return 1;
}

//Execute the function when button "add" was clicked
void add_friend_button_clicked (GtkWidget *widget, gpointer data){
	// GtkWidget *found_friends_listbox = GTK_WIDGET(gtk_builder_get_object (builder, "found_friends_listbox"));
	// GList *list	= gtk_list_box_get_selected_rows ((GtkListBox *)found_friends_listbox);
	// if (list == NULL) return;
	// GtkListBoxRow *row = (GtkListBoxRow *)(list->data);
	// GtkWidget *friendbox = (GtkWidget *)(gtk_container_get_children ((GtkContainer *)row)->data);
	// const char *name = utalk_friendbox_get_name (friendbox);

	// gint result = add_friend_request (name);

	// gchar msg[30];
	// if (result == -1) sprintf (msg, "%s 已经是您的好友了。", name);
	// else sprintf (msg, "向 %s 的好友申请已发送。", name);
	// GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(addfriend_window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", msg);
	// gtk_window_set_title(GTK_WINDOW(dialog), "好友请求");
	// gtk_dialog_run(GTK_DIALOG(dialog));
	//gtk_widget_destroy(dialog);
}

//Execute the function when button "search" was clicked
void search_button_clicked (GtkWidget *widget, gpointer data){
	GtkWidget *searchentry = GTK_WIDGET(gtk_builder_get_object(builder, "searchentry"));
	const gchar *text = gtk_entry_get_text ((GtkEntry *)searchentry);
	g_print ("Search text = %s\n", text);

	int photo_id[MAX_NUM];
	char friendlist[20][MAX_LENGTH];
	gint friends_num = search_friends (text, friendlist, photo_id);
	if(friends_num == 0){
		error_dialog_z(widget,"User does not exist");
	}
	else{
		// sem_wait(&sem4friend);
		load_friends_list_from_server();
	}
	// GtkWidget *found_friends_listbox = GTK_WIDGET(gtk_builder_get_object (builder, "found_friends_listbox"));
	// GList *list = gtk_container_get_children ((GtkContainer *)found_friends_listbox);
	// for (GList *l = list; l != NULL; l = l->next){
	// 	gtk_container_remove ((GtkContainer *)found_friends_listbox, (GtkWidget *)(l->data));
	// }
	// for (int i = 0; i < friends_num; i++){
	// 	char path[20];
	// 	sprintf(path, "glade/avators/%d.jpg", photo_id[i]);
	// 	g_print("path: %s\n", path);
	// 	GtkWidget *friendbox = utalk_friendbox_new (path, friendlist[i]);

	// 	//xiao bug
	// 	gtk_list_box_insert ((GtkListBox *)found_friends_listbox, friendbox, -1);
	// }



	
}

//Load the window of adding friends
void load_addfriend_ui (){
	addfriend_window = GTK_WIDGET (gtk_builder_get_object(builder, "addfriend_window"));

	GtkWidget *search_button = GTK_WIDGET(gtk_builder_get_object(builder, "search_button"));
	g_signal_connect (G_OBJECT(search_button), "clicked", G_CALLBACK(search_button_clicked), NULL);

	GtkWidget *add_friend_button = GTK_WIDGET(gtk_builder_get_object(builder, "add_friend_button"));
	g_signal_connect (G_OBJECT(add_friend_button), "clicked", G_CALLBACK(add_friend_button_clicked), NULL);

	GtkWidget *chathistory_close_button = GTK_WIDGET(gtk_builder_get_object(builder, "chathistory_close_button"));
	g_signal_connect (G_OBJECT(chathistory_close_button), "clicked", G_CALLBACK(gtk_window_close), NULL);

	gtk_widget_show_all (addfriend_window);
}

//Update the window to show the message
void update_msg_textview (const gchar *name, const gchar *msg){
	GtkWidget *msg_textview = GTK_WIDGET(gtk_builder_get_object (builder, "msg_textview"));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (msg_textview));
	GtkTextIter iter;
	GtkTextIter end;
	gtk_text_buffer_get_end_iter(buffer, &end);

	const char *name_color;
	if (strcmp (name, username) == 0) name_color = "blue_fg";
	else name_color = "green_fg";
	gtk_text_buffer_insert_with_tags_by_name (buffer, &end, name, -1, name_color, "name_l_margin", "name_t_margin", NULL);
	gtk_text_buffer_insert (buffer, &end, "\n", -1);
	gtk_text_buffer_insert_with_tags_by_name (buffer, &end, msg, -1, "bigger_font", "msg_l_margin", "msg_t_margin", NULL);
	gtk_text_buffer_insert (buffer, &end, "\n", -1);
}

//Select someone to talk
void friend_selected (GtkListBox *box, GtkListBoxRow *row, gpointer user_data){
	// Get the friend selected
	GtkWidget *friendbox = (GtkWidget *)(gtk_container_get_children ((GtkContainer *)row)->data);
	const gchar *name = utalk_friendbox_with_msg_get_name (friendbox);
	
	// Set chatwith_label and cur_chat_friend_name
	GtkWidget *chatwith_label = GTK_WIDGET (gtk_builder_get_object(builder, "chatwith_label"));
	gtk_label_set_text ((GtkLabel *)chatwith_label, name);
	strcpy (cur_chat_friend_name, name);
	
	// Clear msg_textview
	GtkWidget *msg_textview = GTK_WIDGET (gtk_builder_get_object(builder, "msg_textview"));
	GtkTextBuffer *msg_textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (msg_textview));
	gtk_text_buffer_set_text (msg_textbuffer, "", -1);

	// Find the frienditem
	FriendItem *frienditem = NULL;
	for (GList *l = friends_list; l != NULL; l = l->next){
		frienditem = (FriendItem *)(l->data);
		if (strcmp(frienditem->friend_name, name) == 0) break;
	}

	// Clear the Msg_queue
	while (!Msg_queue_is_empty (frienditem->msg_queue)){
		Message msg = Msg_queue_front (frienditem->msg_queue);
		Msg_queue_pop (frienditem->msg_queue);
		update_msg_textview(msg.name, msg.text);
	}
}

//Receive message from the server
void recv_msg (const char *friendname, const char *msg){
	FriendItem *frienditem = NULL;
	for (GList *l = friends_list; l != NULL; l = l->next){
		frienditem = (FriendItem *)(l->data);
		if (strcmp(frienditem->friend_name, friendname) == 0) break;
	}
	if (frienditem == NULL){
		server_error_dialog (window);
		return;
	}

	utalk_friendbox_with_msg_set_msg (frienditem->friendbox, msg);

	if (strcmp(friendname, cur_chat_friend_name) == 0){
		update_msg_textview(friendname, msg);
	}
	else{
		Msg_queue_push (frienditem->msg_queue, Message_new (friendname, msg));
	}
}

//Get the message
gchar *get_type_textview_msg (){
	GtkWidget *type_textview = GTK_WIDGET (gtk_builder_get_object(builder, "type_textview"));
	GtkTextBuffer *type_textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (type_textview));
	
	GtkTextIter start, end;
	gtk_text_buffer_get_start_iter (type_textbuffer, &start);
	gtk_text_buffer_get_end_iter (type_textbuffer, &end);

	gchar *msg = gtk_text_buffer_get_text (type_textbuffer, &start, &end, TRUE);
	gtk_text_buffer_set_text (type_textbuffer, "", -1);
	return msg;
}

//Execute the function when button "send" was clicked
void send_button_clicked (GtkWidget *widget, gpointer data){
	printf("ClickOK\n");
	if (strcmp (cur_chat_friend_name, "") == 0) return;
	gchar *msg = get_type_textview_msg ();
	if (strlen (msg) <= 0) return;
	printf("%s\n", msg);
	//Sending message default succeed
	int succeed = 1;
	send_msg (cur_chat_friend_name, msg);
	//!!!!!!!!!!!!!!!!!!!!!!!
	if (succeed == 0) {
		server_error_dialog (window);
		return;
	}
	else {
		g_print ("Send message successfully. Message: %s\n", msg);
	}

	update_msg_textview (username, msg);

	FriendItem *frienditem = NULL;
	for (GList *l = friends_list; l != NULL; l = l->next){
		frienditem = (FriendItem *)(l->data);
		if (strcmp(frienditem->friend_name, cur_chat_friend_name) == 0) break;
	}
	utalk_friendbox_with_msg_set_msg (frienditem->friendbox, msg);
}

//Read from the native image database
void recv_useravator (int avator_id){
	GtkWidget *useravator_image = GTK_WIDGET (gtk_builder_get_object(builder, "useravator_image"));
	gchar path[20];
	sprintf (path, "glade/avators/%d.jpg", avator_id);
	gtk_image_set_from_file ((GtkImage *)useravator_image, path);
}

//Execute the function when button "ok" was clicked
void ok_button_clicked (GtkWidget *widget, gpointer data){
	useravator = avator_selected;

	recv_useravator (avator_selected);
	send_useravator (avator_selected);

	GtkWidget *avator_dialog = GTK_WIDGET (gtk_builder_get_object(builder, "avator_dialog"));
	gtk_dialog_response ((GtkDialog *)avator_dialog, GTK_RESPONSE_OK);
}

//Select the avator
void select_avator (GtkWidget *widget, gpointer data){
	avator_selected = (int)widget;
	g_print ("avator id = %d\n", avator_selected);
}

//Set the framework of changing avator
void set_avator_eventbox (){
	for (int i = 1; i <= 9; i++){
		char eventbox_name[10];
		sprintf (eventbox_name, "eventbox%d", i);
		g_print ("box name %s", eventbox_name);
		GtkWidget *eventbox = GTK_WIDGET(gtk_builder_get_object(builder, eventbox_name));
		gint id = i;
		g_signal_connect_swapped (G_OBJECT(eventbox), "button_press_event", G_CALLBACK(select_avator), id);		
	}
}

//change the user's avator
void change_avator (GtkWidget *widget, gpointer data){
	GtkWidget *avator_dialog = GTK_WIDGET (gtk_builder_get_object(builder, "avator_dialog"));
	
	set_avator_eventbox ();

	GtkWidget *ok_button = GTK_WIDGET(gtk_builder_get_object(builder, "ok_button"));
	g_signal_connect (G_OBJECT(ok_button), "clicked", G_CALLBACK(ok_button_clicked), NULL);

	gtk_dialog_run ((GtkDialog *)avator_dialog);
	gtk_widget_destroy (avator_dialog);
	g_print ("avator id = %d\n", useravator);
}

//Send the message of changing the user's avator to the server
void update_avators (){
	for (int i = 1; i <= 9; i++){
		char image_name[10];
		sprintf (image_name, "image%d", i);
		char path_name[10];
		sprintf (path_name, "glade/avators/%d.jpg", i);
		GtkWidget *image = GTK_WIDGET(gtk_builder_get_object(builder, image_name));
		gtk_image_set_from_file ((GtkImage *)image, path_name);
	}
}

//Get the information of friends from the server
void load_friends_list_from_server (){
	// Clear
	friends_listbox = GTK_WIDGET(gtk_builder_get_object(builder, "friends_listbox"));
	GList *list = gtk_container_get_children ((GtkContainer *)friends_listbox);
	for (GList *l = list; l != NULL; l = l->next){
		gtk_container_remove ((GtkContainer *)friends_listbox, (GtkWidget *)(l->data));
	}

	int photo_id[MAX_NUM];
	char friends_name_list[MAX_NUM][MAX_LENGTH];

	sem_wait(&sem4friend);
	int friends_num = load_friends_list (friends_name_list, photo_id);
	g_print ("Load friends list done. friends counting %d.\n", friends_num);

	g_print ("in chat: %s\n", friends_name_list[0]);

	for (int i = 0; i < friends_num; i++)
	{
		add_friend (friends_name_list[i], photo_id[i], "", -1);
	}
	gtk_widget_show_all (friends_listbox);
}

//Consistently refresh the list of friends
void refresh_friends_list (GtkWidget *widget, gpointer data){
	load_friends_list_from_server ();
}

//test the programe
void test (GtkWidget *widget, gpointer data){
	recv_friend_request ("dwqe");
}

// Set the menu on the top of whole window
void set_test_menu (){
	// GtkWidget *quit_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "quit_menuitem"));
	// g_signal_connect (G_OBJECT(quit_menuitem), "activate", G_CALLBACK(gtk_main_quit), NULL);

	// GtkWidget *v_rev_msg_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "v_rev_msg_menuitem"));
	// g_signal_connect (G_OBJECT(v_rev_msg_menuitem), "activate", G_CALLBACK(v_rev_msg), NULL);

	// GtkWidget *v_rev_friend_request_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "v_rev_friend_request_menuitem"));
	// g_signal_connect (G_OBJECT(v_rev_friend_request_menuitem), "activate", G_CALLBACK(v_rev_friend_request), NULL);

	// GtkWidget *v_friend_request_ac_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "v_friend_request_ac_menuitem"));
	// g_signal_connect (G_OBJECT(v_friend_request_ac_menuitem), "activate", G_CALLBACK(v_friend_request_ac), NULL);

	// GtkWidget *v_friend_request_ref_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "v_friend_request_ref_menuitem"));
	// g_signal_connect (G_OBJECT(v_friend_request_ref_menuitem), "activate", G_CALLBACK(v_friend_request_ref), NULL);

	GtkWidget *test_button = GTK_WIDGET (gtk_builder_get_object(builder, "test_button"));
	g_signal_connect (G_OBJECT(test_button), "activate", G_CALLBACK(test), NULL);
}

//Initiate the whole framework, bind the instance
void chat_init (){
	friends_listbox = GTK_WIDGET(gtk_builder_get_object(builder, "friends_listbox"));
	g_signal_connect (G_OBJECT(friends_listbox), "row-selected", G_CALLBACK(friend_selected), NULL);

	GtkWidget *username_label = GTK_WIDGET(gtk_builder_get_object(builder, "username_label"));
	gtk_label_set_text ((GtkLabel *)username_label, username);

	// Add Send button
	GtkWidget *type_overlay = GTK_WIDGET(gtk_builder_get_object(builder, "type_overlay"));
	GtkWidget *send_button = gtk_button_new_with_label ("发送");
	g_signal_connect (G_OBJECT(send_button), "clicked", G_CALLBACK(send_button_clicked), NULL);

	gtk_widget_set_halign (send_button, GTK_ALIGN_END);
	gtk_widget_set_valign (send_button, GTK_ALIGN_END);
	gtk_widget_set_margin_end (send_button, 5);
	gtk_widget_set_margin_bottom (send_button, 5);
	gtk_widget_set_size_request (send_button, 90, 43);
	gtk_overlay_add_overlay ((GtkOverlay *)type_overlay, send_button);

	GtkWidget *msg_textview = GTK_WIDGET(gtk_builder_get_object (builder, "msg_textview"));
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (msg_textview));
	gtk_text_buffer_create_tag (buffer, "blue_fg", "foreground", "blue", NULL);	
	gtk_text_buffer_create_tag (buffer, "green_fg", "foreground", "green", NULL);
	gtk_text_buffer_create_tag (buffer, "bigger_font", "scale", 1.15, NULL);
	gtk_text_buffer_create_tag (buffer, "name_l_margin", "left-margin", 35, NULL);
	gtk_text_buffer_create_tag (buffer, "msg_l_margin", "left-margin", 50, NULL);
	gtk_text_buffer_create_tag (buffer, "name_t_margin", "pixels-above-lines", 30, NULL);
	gtk_text_buffer_create_tag (buffer, "msg_t_margin", "pixels-above-lines", 10, NULL);

	GtkTextView *text_view = (GtkTextView *)gtk_builder_get_object(builder, "chat_history_textview");
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_create_tag (buffer, "blue_fg", "foreground", "blue", NULL);
	gtk_text_buffer_create_tag (buffer, "green_fg", "foreground", "green", NULL);
	gtk_text_buffer_create_tag (buffer, "bigger_font", "scale", 1.15, NULL);
	gtk_text_buffer_create_tag (buffer, "name_l_margin", "left-margin", 35, NULL);
	gtk_text_buffer_create_tag (buffer, "msg_l_margin", "left-margin", 50, NULL);
	gtk_text_buffer_create_tag (buffer, "name_t_margin", "pixels-above-lines", 30, NULL);
	gtk_text_buffer_create_tag (buffer, "msg_t_margin", "pixels-above-lines", 10, NULL);

	// Search and add friend
	GtkWidget *add_friend_menuitem = GTK_WIDGET (gtk_builder_get_object(builder, "add_friend_menuitem"));
	g_signal_connect (G_OBJECT(add_friend_menuitem), "activate", G_CALLBACK(load_addfriend_ui), NULL);

	// Choose file
	GtkWidget *choose_file_button = GTK_WIDGET(gtk_builder_get_object(builder, "choose_file_button"));
	g_signal_connect (G_OBJECT(choose_file_button), "clicked", G_CALLBACK(choose_file), NULL);

	// Choose font
	GtkWidget *choose_font_button = GTK_WIDGET(gtk_builder_get_object(builder, "choose_font_button"));
	g_signal_connect (G_OBJECT(choose_font_button), "clicked", G_CALLBACK(choose_font), NULL);

	// Chat history
	GtkWidget *chat_history_button = GTK_WIDGET(gtk_builder_get_object(builder, "chat_history_button"));
	g_signal_connect (G_OBJECT(chat_history_button), "clicked", G_CALLBACK(load_chathistory_ui), NULL);

	// Change avator
	GtkWidget *change_avator_menuitem = GTK_WIDGET(gtk_builder_get_object(builder, "change_avator_menuitem"));
	g_signal_connect (G_OBJECT(change_avator_menuitem), "activate", G_CALLBACK(change_avator), NULL);

	// 
	GtkWidget *refresh_friends_menuitem = GTK_WIDGET(gtk_builder_get_object(builder, "refresh_friends_menuitem"));
	g_signal_connect (G_OBJECT(refresh_friends_menuitem), "activate", G_CALLBACK(refresh_friends_list), NULL);

	update_avators ();
	set_test_menu ();
}

void logout_server(){
	sendLogOut(username);
	gtk_main_quit();
}

//Load the chat window, the parameter "username" means the user's name
GtkWidget *load_chat_window (const char *recv_username){
	builder = gtk_builder_new_from_file ("glade/chat.glade");
	if(builder == NULL) return NULL;
	
	window = GTK_WIDGET (gtk_builder_get_object(builder, "window"));
	g_signal_connect (G_OBJECT(window), "destroy", G_CALLBACK(logout_server), NULL);

	strcpy (username, recv_username);
	memset (cur_chat_friend_name, 0, sizeof(cur_chat_friend_name));
	useravator = 0;

	//inform_net ();
	chat_init ();
	load_friends_list_from_server ();
	request_useravator ();

	window = GTK_WIDGET (gtk_builder_get_object(builder, "window"));
	gtk_widget_show_all (window);
	return window;
}
