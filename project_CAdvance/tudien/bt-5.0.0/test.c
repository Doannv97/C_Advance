#include<stdio.h>
#include<string.h>
#include"inc/btree.h"

void printdata(BTA* book){
	char name[80],phone[20];
	int size;
	
   	btsel(book,"",phone,10,&size);//dua con tro ve truoc node dau tien
   	while(btseln(book,name,phone,10,&size)==0)
		printf("%-50s%sd\n",name,phone);
   
}

int menu(){
	printf("\n\n-------------MENU-----------\n");
	printf("1. Them thong tin\n");
	printf("2. Tiem kiem: \n");
	printf("3. In danh sach\n");	
	printf("4. Xoa thong tin\n");
	printf("5. Thoat\n");
	printf("Nhap: ");
	int c; scanf("%d",&c);
	return c;
}

int main(){
	int i,n,size;
	char name[80],phone[20];

	BTA* book;btinit();
	book=btopn("mybook",0,0);
	if(!book){
		printf("Create my book\n");
		book=btcrt("my book",0,0);
	}
	
	while (1){
		int c=menu();
		switch (c){
			case 1: printf("Nhap ten: "); scanf("%s",name);
				printf("Sdt: "); scanf("%s",phone);
				btins(book,name,phone,20);
 				break;
			case 2: printf("Nhap ten can tim: "); scanf("%s",name); 
				if (btsel(book,name,phone,20,&size)==0)
					printf("%s %s\n",name,phone);
				else  printf("Khong tim thay\n");
				break;
			case 3: printdata(book);
								
				break;
			case 4: printf("Nhap ten can xoa: "); scanf("%s",name); 
				btdel(book,name);
				break;
			case 5: return 0;
		}
	}
	
}
