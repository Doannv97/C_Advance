#include<stdio.h>
#include<stdlib.h>
#include"libfdr/jrb.h"

int menu(){
	printf("\n\n-------MENU------\n");
	printf("1. Chen 1 so \n");
	printf("2. Hien thi toan bo so\n");
	printf("3. Sua 1 so\n");
	printf("4. Xoa 1 so\n");
	printf("5. Thoat\n");
	printf("Chon: ");
	int c; scanf("%d",&c);
	while (c>5 || c<1){
		printf("NHap lai: "); scanf("%d",&c);
	}
	return c;
}

JRB delete_node(JRB tree){
	char name[10]; JRB cur;
	printf("Nhap ten nguoi can xoa: "); scanf("%s",name); 
	cur=jrb_find_str(tree,name);
	if (cur==NULL){
		printf("Khong ton tai");
		return tree;
	}
	jrb_delete_node(cur);
	return tree;
}

JRB insert_node(JRB tree){
	char name[10],phone[10];
	printf("Nhap ten: ");  scanf("%s",name);
	printf("Nhap sdt: "); scanf("%s",phone);
	jrb_insert_str(tree,strdup(name),new_jval_s(strdup(phone)));
	return tree;
}

void print_tree(JRB tree){
	JRB cur;
	jrb_traverse(cur,tree)
		printf("%s  %s\n",jval_s(cur->key),jval_i(cur->val));
}


JRB change_node(JRB tree){
	char name[10],phone[10]; JRB cur;
	printf("NHap ten can sua: "); scanf("%s",name);
	cur=jrb_find_str(tree,name);
	if (cur==NULL){
		printf("Khong ton tai");
		return tree;
	}
	printf("Nhap lai sdt moi: "); scanf("%s",phone);
	jrb_delete_node(cur);
	jrb_insert_str(tree,strdup(name),new_jval_s(strdup(phone)));
	return tree;
}
	
	

int main(){
	
	JRB tree=make_jrb(),cur;

	while (1){
		int c=menu();
		switch (c){
			case 1: tree=insert_node(tree); break;
			case 2: print_tree(tree); break;
			case 3: change_node(tree); break;
			case 4:	delete_node(tree); break;
			case 5: jrb_free_tree(tree); return 0;
		}
	}
}
