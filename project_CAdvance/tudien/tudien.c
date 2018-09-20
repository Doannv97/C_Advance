///gcc tudien.c -o tudien `pkg-config gtk+-3.0 --cflags --libs`  bt-5.0.0/lib/libbt.a -w

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
#include "bt-5.0.0/inc/btree.h"

enum {
  	LIST_ITEM = 0,
  	N_COLUMNS
};

BTA* sdx_book;
BTA* book;
GtkListStore *list;
GtkWidget *text_mean;
GtkWidget *text_search;
GtkWidget *list_suggest;

int check;


/*Code soundex*/

/* for ASCII only */
char code[128] = { 0 };
void add_code(const char *s, int c){
	while (*s) {
		code[(int)*s] = code[0x20 ^ (int)*s] = c;
		s++;
	}
}
void init_soundex(){
	static const char *cls[] =
		{ "AEIOU", "", "BFPV", "CGJKQSXZ", "DT", "L", "MN", "R", 0};
	int i;
	for (i = 0; cls[i]; i++)
		add_code(cls[i], i - 1);
}
/* returns a static buffer; user must copy if want to save
   result across calls */
char* soundex(const char *s)
{
	static char out[5];
	int c, prev, i;
 
	out[0] = out[4] = 0;
	if (!s || !*s) return out;
 
	out[0] = *s++;
 
	/* first letter, though not coded, can still affect next letter: Pfister */
	prev = code[(int)out[0]];
	for (i = 1; *s && i < 4; s++) {
		if ((c = code[(int)*s]) == prev) continue;
 
		if (c == -1) prev = 0;	/* vowel as separator */
		else if (c > 0) {
			out[i++] = c + '0';
			prev = c;
		}
	}
	while (i < 4) out[i++] = '0';
	return out;
}
void next_sdx(char word[]){
	int i=strlen(word)-1,len=strlen(word);
	while (i>=0){
		if (word[i]!='z'){
			word[i]+=1;
			return;
		}else word[i]='a';
		i--;
	}
}
int check_str(char word1[], char word2[]){
	int i=0,len1=strlen(word1),len2=strlen(word2);
	if (len2<len1) return 0;
	while (i<len1 && i<len2){
		if (word1[i]!=word2[i]) return 0;
		i++;
		if (i==6) break;
	}
	return 1;
}




//Tìm tự cùng độ dài ngay phía sau theo thứ tự từ điển
void next_word(char word[]){
	int i=strlen(word)-1;
	while (i>=0){
		if (word[i]!='z'){
			word[i]+=1;
			word[i+1]='\0';
			return;
		}
		i--;
	}
}


//So sánh 2 tự theo tứ thự từ điển 
int stringcmp(char a[], char b[]){
	int i=0, lena=strlen(a), lenb=strlen(b);
	while (1){
		if (a[i]<b[i]) return -1;
		else if (a[i]>b[i]) return 1;
		else{
			i++;
			if (i==lena && i==lenb) return 0;
			if (i==lena) return -1;
			if (i==lenb) return 1;
		}
	}
}



//Hiện thị vào text_mean 
void set_text_mean(char *text) {
	GtkTextBuffer *buffer;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_mean));
	if (buffer == NULL) {
		printf("Get buffer fail!");
		buffer = gtk_text_buffer_new(NULL);
	}
	gtk_text_buffer_set_text(buffer, text, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_mean), buffer);
}


//Thêm vào danh sách gợi ý
void add_to_list(const gchar *str) {
  	GtkTreeIter iter;
  	gtk_list_store_append(list, &iter);
  	gtk_list_store_set(list, &iter, LIST_ITEM, str, -1);
}
//Xóa danh sách gợi ý  
void remove_all_list(GtkWidget *widget, gpointer data) {
  	gtk_list_store_clear(list);
}


//Tạo cửa sổ thông báo
//parent: cửa sổ cha, type: loại thông báo, mms: chủ đề, content: nội dung 
void Show_message(GtkWidget * parent , GtkMessageType type,  char * mms, char * content) {
	GtkWidget *mdialog;
	mdialog = gtk_message_dialog_new(GTK_WINDOW(parent),GTK_DIALOG_DESTROY_WITH_PARENT,
	                		type,GTK_BUTTONS_OK,"%s", mms);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(mdialog), "%s",  content);
	gtk_dialog_run(GTK_DIALOG(mdialog));
	gtk_widget_destroy(mdialog);
}
//Xóa cửa sổ
void destroy_something(GtkWidget * widget, gpointer gp) {
	gtk_widget_destroy(gp);
}

char* get_time(){
	 time_t rawtime;
  	struct tm * timeinfo;
  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );

	static char result[26];
  	sprintf(result, "%d/%d/%4d %.2d:%.2d\n",
    	timeinfo->tm_mday,timeinfo->tm_mon+1,timeinfo->tm_year+1900,
	timeinfo->tm_hour,timeinfo->tm_min);
  	return result;

}

void print_file(char *str1, char* str2){

	FILE *fptr;
	if ((fptr=fopen("history.txt","a"))==NULL)
	{
		printf("Khong mo duoc file lich su");
		return 0;
	}
	static char result[50];
	sprintf(result,"%s \"%s\"   -   %s",str1,str2,get_time());
	fputs(result,fptr);
	fclose(fptr);
}

void get_text_list_his(char* str,char result[50] ){
	int i=0,j=0,len=strlen(str); 
	while(str[i++]!='"');
	while(str[i]!='"')
		result[j++]=str[i++];
}


//Khởi tạo list gợi ý, list soundex 
void init_list_suggest(GtkWidget *list_suggest) {
 	GtkCellRenderer    *renderer;
 	GtkTreeViewColumn  *column;
  	GtkListStore       *store;

  	renderer = gtk_cell_renderer_text_new();
  	column = gtk_tree_view_column_new_with_attributes("Gợi ý",
          	renderer, "text", LIST_ITEM, NULL);
  	gtk_tree_view_append_column(GTK_TREE_VIEW(list_suggest), column);
  	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
  	gtk_tree_view_set_model(GTK_TREE_VIEW(list_suggest), GTK_TREE_MODEL(store));
  	g_object_unref(store);
}

//Chọn trong danh sách gợi ý để hiển thị 
void selection_list_suggest(GtkWidget *widget, gpointer selection){
    
  	GtkTreeIter iter;
  	GtkTreeModel *model;
  	gchar *value;

  	if (gtk_tree_selection_get_selected(
      		GTK_TREE_SELECTION(selection), &model, &iter)) {

    		gtk_tree_model_get(model, &iter, LIST_ITEM, &value,  -1);
		char word[50];

		char mean[10000];
		int size; 
		if(btsel(book,word,mean,10000,&size)==0) 
			set_text_mean(mean);
		else
			set_text_mean(" Bạn đã xóa từ này ");		
	//	gtk_entry_set_text(text_search, word);
    		g_free(value);
  	}
}

//Xóa danh sách gợi ý
void remove_all_list_suggest(GtkWidget *widget, gpointer data) {
	check=1;
  	GtkListStore *store;
  	GtkTreeIter  iter;
  	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_suggest)));  
  	gtk_list_store_clear(store);
	check=0;
}

//Thêm vào danh sách gợi ý
void add_to_list_suggest(gchar *str){
	check=1;
	GtkListStore *store;
  	GtkTreeIter iter;
  	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_suggest)));
  	gtk_list_store_append(store, &iter);
 	gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
	check=0;			
}

	
static void suggest_word( GtkWidget *widget,GtkWidget *entry ){
  	const gchar *entry_text;
  	entry_text = gtk_entry_get_text (GTK_ENTRY (entry));

	char word[20],mean[10000],nword[20];
	int size,n=0; 
	strcpy(nword,entry_text);
	btsel(book,nword,mean,0,&size);
	next_word(nword); 
	while(btseln(book,word,mean,0,&size)==0){
		if (stringcmp(word,nword)>=0) break;
		add_to_list(word);
		n++;
		if (n>18) break;
	}	
}

//Tìm từ được soundex 
void fix_word(char word[20]){
	char *sdx;
	int size;
	char word2[20]=" ",sdx2[10];

	if (word[0]=='\0') return;
	sdx=soundex(word);
	strcpy(sdx2,sdx);
	strcat(sdx2,"zzz");
	strcat(sdx,"aaa");
	btsel(sdx_book,sdx,word,0,&size);
	while(btseln(sdx_book,sdx,word,20,&size)==0){
		if (strcmp(sdx,sdx2)==1) break;
		if (check_str(word2,word)==0){
			add_to_list_suggest(word);
			strcpy(word2,word);
		}
	}	
	
}


void suggest_list_word(char entry_text[]){

	char word[20],mean[10000],nword[20];
	int size,n=0; 
	strcpy(nword,entry_text);
	btsel(book,nword,mean,0,&size);
	next_word(nword); 
	while(btseln(book,word,mean,0,&size)==0){
		if (stringcmp(word,nword)>=0) break;
		add_to_list_suggest((char*)word);
		n++;
		if (n>30) break;
	}	
}	



//Hàm tìm kiếm từ
static void search_word(GtkWidget *widget,GtkWidget *data){
	char *entry_text;
  	entry_text = gtk_entry_get_text (GTK_ENTRY (text_search));
	char word[20],mean[10000],nword[20];
	int size,n=0; 
	if(btsel(book,entry_text,mean,10000,&size)==0){
		set_text_mean(mean);
		suggest_list_word(entry_text);
		print_file("Bạn đã tìm kiếm từ ",entry_text);
	}
	else{
		set_text_mean("Từ này không có trong từ điển\n\nGợi ý:\n- Chọn chức năng thêm từ\n- Xem các từ gợi ý bên trái");
		fix_word(entry_text);
	}
}


void on_changed(GtkWidget *widget, gpointer label) {
	if (check) return;
  	GtkTreeIter iter;
  	GtkTreeModel *model;
  	gchar *value;

  	if (gtk_tree_selection_get_selected(
      	GTK_TREE_SELECTION(widget), &model, &iter)) {
    		gtk_tree_model_get(model, &iter, LIST_ITEM, &value,  -1);
    		gtk_entry_set_text(text_search, value);
    		g_free(value);
  }
}


//Thêm một từ vào từ điển
static void add_to_dict(GtkWidget *widget,gpointer data){
	GtkWidget *view;
	GtkTextIter start,end;
  	GtkTextBuffer *buffer;
	const gchar *entry_text;
	char *mean,mean2[10000]; int size;

  	entry_text = gtk_entry_get_text (GTK_ENTRY (((GtkWidget**)data)[0]));	
	view=((GtkWidget**)data)[1];
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
 	gtk_text_buffer_get_start_iter(buffer,&start);
  	gtk_text_buffer_get_end_iter(buffer,&end);
  	mean=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
	
	if (entry_text[0]=='\0' || mean[0]=='\0'){
		Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_WARNING, 
			"Cảnh báo!", "Không được bỏ trống phần nào.");
		return 0;
	}

	if(btsel(book,entry_text,mean2,0,&size)==0) 
		Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_ERROR, 
			"Xảy ra lỗi!","Từ này đã có trong từ điển.");
	else{
		if (btins(book,entry_text,mean,10000)==0){	
			Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_INFO, "Thành công!",
					             "Đã thêm từ vào trong từ điển.");
			print_file("Bạn đã thêm từ ",entry_text);
		}
	}

		
}
	
	
//Tạo form cho cửa sổ chức năng tìm kiếm
static void insert_word(GtkWidget *widget,GtkWidget *no_data){
	GtkWidget *window;
	GtkWidget *button1,*button2;
	GtkWidget *label1,*label2;
	GtkWidget *entry;
	GtkWidget *view;
	GtkWidget *fixed;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), " Thêm từ ");
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	
	view=gtk_text_view_new();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
	GtkWidget *scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scroll),view);

	label1 = gtk_label_new("NHập từ tiếng anh");
	label2 = gtk_label_new("NHập nghĩa");	
	entry = gtk_entry_new ();

	button1 = gtk_button_new_with_label ("Thêm từ");
	button2 = gtk_button_new_with_label ("Thoát");
	
	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_fixed_put(GTK_FIXED(fixed),label1,0,0);
	gtk_fixed_put(GTK_FIXED(fixed),entry,0,20);
	gtk_fixed_put(GTK_FIXED(fixed),label2,0,60);
	gtk_fixed_put(GTK_FIXED(fixed),scroll,0,90);
	gtk_fixed_put(GTK_FIXED(fixed),button1,70,260);
	gtk_fixed_put(GTK_FIXED(fixed),button2,210,260);

	gtk_widget_set_size_request(entry, 300, 30);
	gtk_widget_set_size_request(scroll, 350, 120);
	gtk_widget_set_size_request(button1, 90, 50);
	gtk_widget_set_size_request(button2, 90, 50);

	//Tạo khối dữ liệu để truyền vào hàm
	GtkWidget * data[3];
	data[0] = entry;
	data[1] = view;
	data[2] = window;	


	g_signal_connect (button1, "clicked",G_CALLBACK (add_to_dict),data);
	g_signal_connect (button2, "clicked",G_CALLBACK(destroy_something),window);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(destroy_something), window);
 
	gtk_widget_show_all(window);
	gtk_main();
}

//Sửa 1 từ trong từ điển 
static void edit_to_dict(GtkWidget *widget,gpointer data){
	GtkWidget *view;
	GtkWidget *deldialog;
	GtkTextIter start,end;
  	GtkTextBuffer *buffer;
	const gchar *entry_text;
	char *mean,mean2[10000]; int size;

  	entry_text = gtk_entry_get_text (GTK_ENTRY (((GtkWidget**)data)[0]));
	view=((GtkWidget**)data)[1];
	buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
 	gtk_text_buffer_get_start_iter(buffer,&start);
  	gtk_text_buffer_get_end_iter(buffer,&end);
  	mean=gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
	

	if (entry_text[0]=='\0' || mean[0]=='\0'){
		Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_WARNING, 
			"Cảnh báo!", "Không được bỏ trống phần nào.");
		return 0;
	}


	if (btsel(book,entry_text,mean2,0,&size)!=0)
		Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_ERROR, 
			"Xảy ra lỗi!","Từ này chưa có trong từ điển.");		
	else{	
		// Tạo thông báo xác nhận
		deldialog = gtk_message_dialog_new(GTK_WINDOW(((GtkWidget**)data)[2]),
	                  GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,
	                  GTK_BUTTONS_YES_NO,"Sửa: \"%s\"?", entry_text);
		gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(deldialog), 
			"Bạn thực sự muốn sửa từ \"%s\" không?",entry_text);
	
		

		int result = gtk_dialog_run(GTK_DIALOG(deldialog));
		if (result == GTK_RESPONSE_YES){
			if (btupd(book, entry_text, mean, 10000)==0){
				Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_INFO, 
						"Thành công!", "Đã sửa từ trong từ điển");
				print_file("Bạn đã sửa nghĩa từ ",entry_text);
			}
		}
		gtk_widget_destroy(deldialog);
	}
	
}
		

//Tạo form cho cửa sổ chức năng sửa từ 
static void edit_word(GtkWidget *widget,GtkWidget *DATA){
	GtkWidget *window;
	GtkWidget *button1,*button2;
	GtkWidget *label1,*label2;
	GtkWidget *entry;
	GtkWidget *view;
	GtkWidget *scroll;
	GtkWidget *fixed; 

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), " Sua từ ");
	gtk_window_set_default_size(GTK_WINDOW(window), 400, 350);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	
	view=gtk_text_view_new();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view),GTK_WRAP_WORD_CHAR);
	scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scroll),view);

	label1 = gtk_label_new("NHập từ tiếng anh can sua");
	label2 = gtk_label_new("NHập nghĩa");	

	entry = gtk_entry_new ();
	gtk_entry_set_text(entry,gtk_entry_get_text (GTK_ENTRY (DATA)));

	button1 = gtk_button_new_with_label ("Sửa từ");
	button2 = gtk_button_new_with_label ("Thoát");
	
	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_fixed_put(GTK_FIXED(fixed),label1,0,0);
	gtk_fixed_put(GTK_FIXED(fixed),entry,0,20);
	gtk_fixed_put(GTK_FIXED(fixed),label2,0,60);
	gtk_fixed_put(GTK_FIXED(fixed),scroll,0,90);
	gtk_fixed_put(GTK_FIXED(fixed),button1,70,260);
	gtk_fixed_put(GTK_FIXED(fixed),button2,210,260);

	gtk_widget_set_size_request(entry, 300, 30);
	gtk_widget_set_size_request(scroll, 350, 120);
	gtk_widget_set_size_request(button1, 90, 50);
	gtk_widget_set_size_request(button2, 90, 50);

	GtkWidget * data[3];
	data[0] = entry;
	data[1] = view;
	data[2] = window;	

	g_signal_connect (button1, "clicked",G_CALLBACK (edit_to_dict),data);
	g_signal_connect (button2, "clicked",G_CALLBACK(destroy_something),window);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(destroy_something), window); 

	gtk_widget_show_all(window);
	gtk_main();
}


//Xóa từ khỏi từ điển 
static void delete_to_dict(GtkWidget *widget,gpointer data){

	const gchar *entry_text;
	char *mean,mean2[10000]; int size;

  	entry_text = gtk_entry_get_text (GTK_ENTRY (((GtkWidget**)data)[0]));
	
	if (entry_text[0]=='\0'){
		Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_WARNING, 
			"Cảnh báo!", "Nhập từ cần xóa vào.");
		return 0;
	}

	if (btsel(book,entry_text,mean2,10000,&size)!=0){
		Show_message(((GtkWidget**)data)[2],GTK_MESSAGE_ERROR, 
			"Xảy ra lỗi!","Không tìm thấy từ này trong từ điển.");
		return 0;
	}


	//Tạo cửa sổ thông báo yêu cầu xác nhận
	GtkWidget *deldialog;
	deldialog = gtk_message_dialog_new(GTK_WINDOW(((GtkWidget**)data)[2]),
	                                   GTK_DIALOG_DESTROY_WITH_PARENT,
	                                   GTK_MESSAGE_QUESTION,
	                                   GTK_BUTTONS_YES_NO,
	                                   "Xóa: \"%s\"?", entry_text);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(deldialog), 			"Bạn thực sự muốn xóa từ \"%s\" chứ?",entry_text);

	int result = gtk_dialog_run(GTK_DIALOG(deldialog));
	if (result == GTK_RESPONSE_YES){
		if (btdel(book, entry_text)==0){
			Show_message(((GtkWidget**)data)[2], GTK_MESSAGE_INFO, 
					"Thành công!", "Da xoa khỏi từ điển");
			print_file("Bạn đã xóa từ ",entry_text);
		}
	}
	gtk_widget_destroy(deldialog);	
}


//Tạo form cửa sổ xóa từ
static void delete_word(GtkWidget *widget,GtkWidget *DATA){
	GtkWidget *window;
	GtkWidget *button1,*button2;
	GtkWidget *label1,*label2;
	GtkWidget *entry;
	GtkWidget *fixed;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), " Thêm từ ");
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 200);
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	
	label1 = gtk_label_new("NHập từ cần xóa");
	entry = gtk_entry_new ();
	gtk_entry_set_text(entry,gtk_entry_get_text (GTK_ENTRY (DATA)));
	button1 = gtk_button_new_with_label ("Xoá");
	button2 = gtk_button_new_with_label ("Thoát");
	
	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_fixed_put(GTK_FIXED(fixed),label1,0,0);
	gtk_fixed_put(GTK_FIXED(fixed),entry,0,20);
	gtk_fixed_put(GTK_FIXED(fixed),button1,40,100);
	gtk_fixed_put(GTK_FIXED(fixed),button2,180,100);

	gtk_widget_set_size_request(entry, 300, 30);
	gtk_widget_set_size_request(button1, 90, 50);
	gtk_widget_set_size_request(button2, 90, 50);

	GtkWidget *data[3];
	data[0] = entry;
	data[2] = window;	

	g_signal_connect (button1, "clicked",G_CALLBACK (delete_to_dict),data);
	g_signal_connect (button2, "clicked",G_CALLBACK(destroy_something),window);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(destroy_something), window); 
	
	gtk_widget_show_all(window);
	gtk_main();
}



void init_list_his(GtkWidget *list_his) {
 	GtkCellRenderer    *renderer;
 	GtkTreeViewColumn  *column;
  	GtkListStore       *store;

  	renderer = gtk_cell_renderer_text_new();
  	column = gtk_tree_view_column_new_with_attributes("Lịch sử hoạt động",
          	renderer, "text", LIST_ITEM, NULL);
  	gtk_tree_view_append_column(GTK_TREE_VIEW(list_his), column);
  	store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING);
  	gtk_tree_view_set_model(GTK_TREE_VIEW(list_his), GTK_TREE_MODEL(store));
  	g_object_unref(store);
}


void selection_list_his(GtkWidget *widget, gpointer selection){
    
  	GtkTreeIter iter;
  	GtkTreeModel *model;
  	gchar *value;

  	if (gtk_tree_selection_get_selected(
      		GTK_TREE_SELECTION(selection), &model, &iter)) {

    		gtk_tree_model_get(model, &iter, LIST_ITEM, &value,  -1);
		char word[50];
		get_text_list_his((char*)value,word);

		char mean[10000];
		int size; 
		if(btsel(book,word,mean,10000,&size)==0) 
			set_text_mean(mean);
		else
			set_text_mean(" Bạn đã xóa từ này ");		
		gtk_entry_set_text(text_search, word);
    		g_free(value);
  	}
}

void remove_all_list_his(GtkWidget *widget, gpointer list_his) {
  	GtkListStore *store;
  	GtkTreeIter  iter;
  	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_his)));  
  	gtk_list_store_clear(store);

	FILE *fptr;
	if ((fptr=fopen("history.txt","w"))==NULL)
	{
		printf("Khong mo duoc file lich su");
		return 0;
	}
	fclose(fptr);
}



static void history_word(GtkWidget *widget, gpointer text_search){
	GtkWidget *window;

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), " Lịch sử ");
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 450);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);


	GtkWidget *button1,*button2,*button3;
	GtkWidget *label;
	GtkWidget *fixed;
	GtkWidget *list_his;
	GtkTreeSelection *selection; 

	button1 = gtk_button_new_with_label ("Xem thêm");
	button2 = gtk_button_new_with_label ("Xóa lịch sử");
	button3 = gtk_button_new_with_label ("Thoát");

	list_his = gtk_tree_view_new();
	init_list_his(list_his);
	GtkWidget *scroll=gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scroll),list_his);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_his));


	fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_fixed_put(GTK_FIXED(fixed),scroll,0,0);
	gtk_fixed_put(GTK_FIXED(fixed),button1,15,350);
	gtk_fixed_put(GTK_FIXED(fixed),button2,130,350);
	gtk_fixed_put(GTK_FIXED(fixed),button3,245,350);

	gtk_widget_set_size_request(scroll, 350, 320);
	gtk_widget_set_size_request(button1, 80, 50);
	gtk_widget_set_size_request(button2, 80, 50);
	gtk_widget_set_size_request(button3, 80, 50);
	
	FILE *fptr;
	if ((fptr=fopen("history.txt","r"))==NULL)
	{
		printf("Khong mo duoc file lich su");
		return 0;
	}
	char his[500][100]; int i,sum=0;
	do{
		fgets(his[sum],100,fptr); 
		if (feof(fptr)!=0) break;
		sum++;
				
	}
	while (1);
	fclose(fptr);

	for (i=sum-1; i>=0; i--){
		GtkListStore *store;
  		GtkTreeIter iter;
  		store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list_his)));
  		gtk_list_store_append(store, &iter);
 		gtk_list_store_set(store, &iter, LIST_ITEM, his[i], -1);
		
	}
	
	g_signal_connect (button1, "clicked",G_CALLBACK(selection_list_his),selection);
	g_signal_connect (button1, "clicked",G_CALLBACK(destroy_something),window);
	g_signal_connect (button2, "clicked",G_CALLBACK(remove_all_list_his),list_his);
	g_signal_connect (button3, "clicked",G_CALLBACK(destroy_something),window);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(destroy_something), window); 
	gtk_widget_show_all(window);
	gtk_main();
}


int main(int argc, char *argv[]) {

	btinit();
	book=btopn("words.dat",0,0);
	if(!book){
		printf("Khong the mo file!\n");
		return 0;
	}
	
	sdx_book=btopn("soundex_book.dat",0,0);
	if(!sdx_book){
		printf("Khong the mo file!\n");
		return 0;
	}

	init_soundex();

	//Khao bao cac bien	
	GtkWidget *window;
	GtkWidget *label;
	
	GtkWidget *search_button;
	GtkWidget *edit_button;
	GtkWidget *insert_button;
	GtkWidget *delete_button;
	GtkWidget *history_button;
	GtkWidget *exit_button;
	GtkWidget *scrolling;
	GtkTreeSelection *selection_suggest; 


	//Khoi tao cua so man hinh chinh
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Tu dien");
	gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);
	gtk_container_set_border_width(GTK_CONTAINER(window), 15);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  
	

  	
	label = gtk_label_new("NHập từ cần tìm");
	
	//Khởi tạo text_search để nhập từ 
	text_search = gtk_search_entry_new ();
    	gtk_entry_set_max_length (GTK_ENTRY (text_search), 50);	


	//Khởi tạo comple: danh sách các gợi ý khi tìm kiếm  
	GtkEntryCompletion *comple;
	comple = gtk_entry_completion_new();
	gtk_entry_completion_set_text_column(comple, 0);
	list = gtk_list_store_new(10, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
	        G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING, G_TYPE_STRING);
	gtk_entry_completion_set_model(comple, GTK_TREE_MODEL(list));
	gtk_entry_set_completion(GTK_ENTRY(text_search), comple);
	

	//Khởi tạo các nút
	search_button = gtk_button_new_with_label ("Tìm kiếm");
	edit_button = gtk_button_new_with_label ("Sửa từ");
	insert_button = gtk_button_new_with_label ("Thêm từ");
	delete_button = gtk_button_new_with_label ("Xóa từ");
	history_button = gtk_button_new_with_label ("Lịch sử");
	exit_button = gtk_button_new_with_label ("Thoát");
	

	//Khởi tạo text_mean để hiện thị nghĩa khi tìm kiếm
	scrolling=gtk_scrolled_window_new(NULL,NULL);
	text_mean=gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_mean), GTK_WRAP_WORD_CHAR);//Chong tran be ngang
	gtk_container_add(GTK_CONTAINER(scrolling), text_mean);


	list_suggest = gtk_tree_view_new();
	init_list_suggest(list_suggest);
	GtkWidget *suggest=gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(suggest),list_suggest);
	selection_suggest = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_suggest));


	//Định vị trí cho các đối tượng 
	GtkWidget *fixed=gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_fixed_put(GTK_FIXED(fixed),label,10,0);
	gtk_fixed_put(GTK_FIXED(fixed),text_search,10,20);
	gtk_fixed_put(GTK_FIXED(fixed),search_button,200,10);
	gtk_fixed_put(GTK_FIXED(fixed),edit_button,280,10);
	gtk_fixed_put(GTK_FIXED(fixed),insert_button,360,10);
	gtk_fixed_put(GTK_FIXED(fixed),delete_button,440,10);
	gtk_fixed_put(GTK_FIXED(fixed),history_button,520,10);
	gtk_fixed_put(GTK_FIXED(fixed),exit_button,600,10);
	gtk_fixed_put(GTK_FIXED(fixed),scrolling,210,80);
	gtk_fixed_put(GTK_FIXED(fixed),suggest,10,60);


	//Định kích thước cho các đối tượng 
	gtk_widget_set_size_request(text_search, 180, 30);
	gtk_widget_set_size_request(search_button, 75, 45);
	gtk_widget_set_size_request(edit_button, 75, 45);
	gtk_widget_set_size_request(insert_button, 75, 45);
	gtk_widget_set_size_request(delete_button, 75, 45);
	gtk_widget_set_size_request(history_button, 75, 45);
	gtk_widget_set_size_request(exit_button, 75, 45);
	gtk_widget_set_size_request(scrolling, 435, 380);
	gtk_widget_set_size_request(suggest, 180, 380);


	//Tạo các sự kiện 
	g_signal_connect (text_search, "activate",G_CALLBACK (remove_all_list_suggest),NULL);
	g_signal_connect (search_button, "clicked",G_CALLBACK (remove_all_list_suggest),NULL);
	g_signal_connect (text_search, "changed",G_CALLBACK (remove_all_list),NULL);
	g_signal_connect (text_search, "changed",G_CALLBACK (suggest_word),text_search);
	g_signal_connect (text_search, "activate",G_CALLBACK (search_word),text_search);
	g_signal_connect (search_button, "clicked",G_CALLBACK (search_word),text_search);
	g_signal_connect(selection_suggest, "changed", G_CALLBACK(on_changed), NULL);


	g_signal_connect (edit_button, "clicked",G_CALLBACK (edit_word),text_search);
	g_signal_connect (insert_button, "clicked",G_CALLBACK (insert_word),NULL);
	g_signal_connect (delete_button, "clicked",G_CALLBACK (delete_word),text_search);
	g_signal_connect (history_button, "clicked",G_CALLBACK (history_word),text_search);


	g_signal_connect (exit_button, "clicked",G_CALLBACK (gtk_main_quit),NULL);
 	g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(window);
	gtk_main();

	btcls(book);
	btcls(sdx_book);
	
	return 0;
}

