#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"libfdr/jrb.h"
#include"libfdr/dllist.h"

#define MAX 100
#define MAXW 1000000

JRB creat_graph();
void drop_graph(JRB *graph);


JRB add_vertex(JRB graphid, int id, char* name);
char* get_vertex(JRB graphid,int id);
int get_id(JRB graphit, char*name);


JRB add_edge(JRB graph,int v1, int v2, int weight);

int out_degree(JRB graph, int vertex, int *output);
int in_degree(JRB graph, int v, int *output);

int has_edge(JRB graph,int v1, int v2);
int get_weight(JRB graph,int v1, int v2);

int DFS(JRB graph, int start, int *kq);
int BFS(JRB graph, int start,int finish, int *kq);

int DAG(JRB graph, int* kq);

int dijkstra(JRB graph,int s, int f,int *kq);



void load_data(JRB *graph, JRB *graph_id){

	JRB g=(JRB)*graph,g_id=(JRB)*graph_id;


	FILE *fptr=fopen("products.txt","r");
	int u,A[MAX],n,i,j,d;
	char str[MAX],c;
	while (1){
		fscanf(fptr,"%d%s",&u,str);
		if (feof(fptr)!=0) break;
		g_id=add_vertex(g_id,u,str);
	}
	fclose(fptr);


	fptr=fopen("orderhistory","r");
	while (1){
		n=0;
		while (1){
			fscanf(fptr,"%s",str);
			if (feof(fptr)!=0) break;
			A[n++]=atoi(str);
			c=fgetc(fptr);
			if (c=='\n' || feof(fptr)!=0) break;
			
	
		}
		if (feof(fptr)!=0) break;
		for (i=0; i<n-1; i++)
			for (j=i+1; j<n; j++){
				
				if (has_edge(g,A[i],A[j])) d=get_weight(g,A[i],A[j])+1; else d=1;
				g=add_edge(g,A[i],A[j],d);	
				g=add_edge(g,A[j],A[i],d);
			}
				
	}
	fclose(fptr);
}	
		
int menu(){
	printf("\n\n-------MENU------\n");
	printf("1. In danh sach san pham\n");
	printf("2. In danh sach giao dich\n");
	printf("3. Hien thi muc do lien quan giua 2 san pham\n");
	printf("4. In danh sach cac san pham lien quan\n");
	printf("5. Hien thi moi quan he giua 2 san pham\n");
	printf("6. Thoat\n");
	printf("Chon: ");
	int c; scanf("%d",&c);
	while (c>6 || c<1){
		printf("NHap lai: "); scanf("%d",&c);
	}
	return c;
}


int indanhsachsanpham(JRB graph_id){
	JRB node;
	jrb_traverse(node, graph_id){
		printf("Ma san pham: %d\n",jval_i(node->key));
		printf("Ten san pham: %s\n\n",jval_s(node->val));
	}
}
	
int indanhsachgiaodich(JRB graph_id){
	int u,A[MAX],n,i,j,d;
	char str[MAX],c;
	FILE *fptr=fopen("orderhistory","r");
	while (1){
		n=0;
		while (1){
			fscanf(fptr,"%s",str);
			if (feof(fptr)!=0) break;
			A[n++]=atoi(str);
			c=fgetc(fptr);
			if (c=='\n' || feof(fptr)!=0) break;
		}
		if (feof(fptr)!=0) break;
		for (i=0; i<n; i++) printf("%s ",get_vertex(graph_id,A[i]));
		printf("\n");
	}
	fclose(fptr);
}

void kiemtramucdolienquan(JRB graph){
	int u,v;
	printf("Nhap 2 ma san pham can kiem tra: "); scanf("%d%d",&u,&v);
	if (has_edge(graph,u,v)) printf("Do lien quan giua 2 san pham: %d\n",get_weight(graph,u,v));
	else printf("-1\n");
}

void sanphamlienquan(JRB graph,JRB graph_id){
	int i,u,output[MAX],j;
	JRB node,tree;
	printf("Nhap ma san pham: "); scanf("%d",&u);
	int sum=out_degree(graph,u,output);
	for (i=0; i<sum-1; i++)
		for (j=i+1; j<sum; j++)
			if (get_weight(graph,u,output[i])<get_weight(graph,u,output[j])){
				int temp=output[i];
				output[i]=output[j];
				output[j]=temp;
				
			}
	printf("Cac san pham lien quan: ");
	for (i=0; i<sum; i++){
		printf("%s ",get_vertex(graph_id,output[i]));
		if (i<sum-1) printf(",");
	}
}


void moilienhe(JRB graph,JRB graph_id){

	int u,v,output[MAX],i;
	printf("Nhap 2 san pham can kiem tra: "); scanf("%d %d",&u,&v);
	
	int d=dijkstra(graph,u,v,output);
	if (d==MAXW) {printf("Hai san pham khong co lien quan\n"); return;}
	printf("Hai san pham co lien quan:  ");
	for (i=1; i<=output[0]; i++){
		printf("%s",get_vertex(graph_id,output[i]));
		if (i<output[0]) printf(" - ");
	}
}

int main(int argc, char* argv[]){
	
	JRB graph=creat_graph();
	JRB graph_id=creat_graph();
	load_data(&graph,&graph_id);

	while (1){
		int c=menu();
		switch (c){
			case 1: indanhsachsanpham(graph_id); break;
			case 2: indanhsachgiaodich(graph_id); break;
			case 3: kiemtramucdolienquan(graph); break;
			case 4: sanphamlienquan(graph,graph_id); break;
			case 5: moilienhe(graph,graph_id); break;
			case 6: return 0;
		}
	}
}





int dijkstra(JRB graph,int s, int f,int *kq){
	int distance[MAX],trace[MAX];
	int i,u,v,n,output[MAX],min_dis;
	
	Dllist cur,node_min,queue;
	queue=new_dllist();
	dll_append(queue,new_jval_i(s));
	
	for (i=0; i<MAX; i++) distance[i]=MAXW;
	distance[s]=0;

	while (!dll_empty(queue)){
		node_min= dll_first(queue);
		int min=jval_i(node_min->val);
		jrb_traverse(cur,queue){
			u=jval_i(cur->val);
			if (distance[u]<distance[min]){
				node_min=cur;
				min=u;
			}
		}
		
		dll_delete_node(node_min);
		u=min;
		if (u==f) break;
		
		n= out_degree(graph,u,output);
		for (i=0; i<n; i++){
			v=output[i];
			int dis_v=distance[u]+get_weight(graph,u,v);
			if (dis_v<distance[v]){
				distance[v]=dis_v;
				trace[v]=u;

				int dk=1;
				jrb_traverse(cur,queue)
					if (jval_i(cur->val)==v) dk=0;
				if (dk) dll_append(queue,new_jval_i(v));
			}
		}
	}
	

	if (distance[f]==MAXW) return MAXW;
	int sum=0,path[MAX],dis=distance[f];
	while (1){
		sum++;
		path[sum]=f;
		if (f==s) break;
		f=trace[f];	
	}		
	
	kq[0]=sum;
	for (i=1; i<=sum; i++) kq[sum-i+1]=path[i];
	free_dllist(queue);
	return dis;
}


int DAG(JRB graph, int* kq){
	int u,n=0;
	Dllist node,stack; 
	JRB tree,cur;
	
	int kiemtracungvao(int v){
		int sum,output[MAX];
		sum=in_degree(graph,v,output);
		if (sum==0) return 1;else return 0;
	}

	stack=new_dllist();
	jrb_traverse(cur,graph)
		if (kiemtracungvao(jval_i(cur->key))) dll_append(stack,cur->key);

	while (!dll_empty(stack)){
		node= dll_last(stack);
		u=jval_i(node->val);
		dll_delete_node(node);
		kq[n++]=u;
		int j,sum,output[MAX];
		sum=out_degree(graph,u,output);
		for (j=0; j<sum; j++){
			int v=output[j];
			cur=jrb_find_int(graph,u);
			tree=(JRB)jval_v(cur->val);
			cur=jrb_find_int(tree,v);
			jrb_delete_node(cur);
			if (kiemtracungvao(v)) dll_append(stack,new_jval_i(v));
		}
	}
	
	int sum=0,output[MAX]; 
	jrb_traverse(cur,graph) sum+=out_degree(graph,u,output);
	if (sum==0) return 1; else return 0;
}
	

int has_edge(JRB graph,int v1, int v2){
	if(graph == NULL) return 0;
	JRB node =jrb_find_int(graph,v1);                           
    	if(node == NULL)  return 0;
	JRB tree=(JRB)jval_v(node->val);
	if (jrb_find_int(tree,v2)) return 1; else return 0;
}


int get_weight(JRB graph, int v1, int v2){
	if (has_edge(graph,v1,v2)==0) return 1000000;
	JRB node =jrb_find_int(graph,v1);
	JRB tree=(JRB)jval_v(node->val);
	node =jrb_find_int(tree,v2);
	return jval_i(node->val);
}


	

int in_degree(JRB graph, int v, int *output){
	if(graph == NULL) return 0;

    	int total = 0;
	JRB cur;
	jrb_traverse(cur,graph){
		JRB tree=(JRB)jval_v(cur->val);
		if (jrb_find_int(tree,v)) output[total++] = jval_i(cur->key);	
	}
   	return total;
}

	

int get_id(JRB graphid, char*name){
	JRB cur;	
	jrb_traverse(cur,graphid)
		if (strcmp(name,jval_s(cur->val))==0)
			return jval_i(cur->key); 
	return -1;
}

char* get_vertex(JRB graphid,int id){
	JRB node=jrb_find_int(graphid,id);
	return jval_s(node->val);
}

JRB add_vertex(JRB graphid, int id, char* name){
  	jrb_insert_int(graphid,id,new_jval_s(strdup(name)));
	return graphid;
}
	

JRB creat_graph(){
	return make_jrb();
}

JRB add_edge(JRB graph,int v1, int v2, int weight){
	JRB node=jrb_find_int(graph,v1);
	if (node==NULL){
		node=make_jrb();
		jrb_insert_int(graph,v1,new_jval_v(node));
		jrb_insert_int(node,v2,new_jval_i(weight));
	}
	else{
		JRB tree=(JRB)jval_v(node->val);
		node=jrb_find_int(tree,v2);
		if (node==NULL) jrb_insert_int(tree,v2,new_jval_i(weight));
		else{
			jrb_delete_node(node);
			jrb_insert_int(tree,v2,new_jval_i(weight));
		}
			
	}
	return graph;
}

int out_degree(JRB graph, int vertex, int *output){
    	if(graph == NULL) return 0;
	JRB node =jrb_find_int(graph,vertex);                           
    	if(node == NULL)  return 0;
    	int total = 0;
 	JRB tree = (JRB) jval_v(node->val);
  	jrb_traverse(node, tree)
       		output[total++] = jval_i(node->key);
   	return total;
}

void drop_graph(JRB *graph){
     	JRB g = (JRB) *graph;
     	JRB node;
     	jrb_traverse(node, g)
        	jrb_free_tree(jval_v(node->val));
     	jrb_free_tree(*graph);
     	*graph = NULL;
}

int DFS(JRB graph, int start, int *kq){
	int visited[MAX]={0};
	int n,u,v,output[MAX],i;
	Dllist node,stack;

	stack=new_dllist();
	dll_append(stack,new_jval_i(start));

	while (!dll_empty(stack)){
		node= dll_last(stack);
		u=jval_i(node->val);
		dll_delete_node(node);
		if (!visited[u]){
			visited[u]=1;
			n= out_degree(graph,u,output);
			for (i=0; i<n; i++){
				v=output[i];
				if (!visited[v]) dll_append(stack,new_jval_i(v));
			}
		}
	}
	free_dllist(stack);
	
	int sum=0;
	for (i=1; i<MAX; i++)
		if (visited[i]) kq[sum++]=i;
	return sum;
}

int BFS(JRB graph, int start,int finish, int *kq){
	int visited[MAX]={0},trace[MAX];
	int n,u,v,output[MAX],i,dk=0;
	Dllist node, queue;
	queue=new_dllist();
	dll_append(queue,new_jval_i(start));

	while (!dll_empty(queue)){
		node= dll_first(queue);
		u=jval_i(node->val);
		if (u==finish){
			dk=1; break;
		}
		dll_delete_node(node);
		if (!visited[u]){
			visited[u]=1;
			n= out_degree(graph,u,output);
			for (i=0; i<n; i++){
				v=output[i];
				trace[v]=u;
				if (!visited[v]) dll_append(queue,new_jval_i(v));
			}
		}
	}
	free_dllist(queue);


	if (dk==0) return 0;
	int sum=0,path[MAX];
	while (1){
		path[sum++]=finish;
		if (finish==start) break;
		finish=trace[finish];	
	}
	for (i=0; i<sum; i++) kq[sum-1-i]=path[i];
	return sum;
}
