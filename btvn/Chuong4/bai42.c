#include<stdio.h>
#include"libfdr/jrb.h"

int menu(){
	printf("\n\n-------MENU------\n");
	printf("1. Chen 1 so vao cay\n");
	printf("2. Hien thi toan bo so\n");
	printf("3. Xoa cay\n");
	printf("4. Thoat\n");
	printf("Chon: ");
	int c; scanf("%d",&c);
	while (c>4 || c<1){
		printf("NHap lai: "); scanf("%d",&c);
	}
	return c;
}

int main(){
	
	JRB tree=make_jrb(),cur;
	while (1){
		int c=menu();
		switch (c){
			case 1: printf("Nhap so can chen: "); int a; scanf("%d",&a);	
				jrb_insert_int(tree,a,new_jval_i(0));break;
			case 2: 
				jrb_traverse(cur,tree)
					printf("%d\n",jval_i(cur->key));
				break;
			case 3: jrb_free_tree(tree); break;
			case 4: return 0;
		}
	}
}
