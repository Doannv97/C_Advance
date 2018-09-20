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



void load_data(JRB *graph, JRB *graph_id, JRB *graph_id2, char filename[]){

	JRB g=(JRB)*graph,g_id=(JRB)*graph_id,g_id2=(JRB)*graph_id2;

	FILE *fptr=fopen(filename,"r");
	int n,m,u,v,d,i,num=0;
	char str1[20],str2[20];

	
	do{
		fscanf(fptr,"%s",str1);
		if (strcmp(str1,"...")==0) break;
		num++; n=0;

		char c;
		while (c=fgetc(fptr)!='"'); 
		do{
			
			c=fgetc(fptr);  
			if (c=='"') break;
			str2[n++]=c;
		}
		while(1);
		str2[n]='\0';
		
		g_id=add_vertex(g_id,num,str1);
		g_id2=add_vertex(g_id2,num,str2);
	}
	while (1);
	

	do{
		fscanf(fptr,"%s",str1);
		if (strcmp(str1,"...")==0) break;
		fscanf(fptr,"%s%d",str2,&d);
		g=add_edge(g,get_id(g_id,str1),get_id(g_id,str2),d);
	}
	while (1);
			
	fclose(fptr);
}	
		
	

int main(int argc, char* argv[]){

	JRB graph=creat_graph();
	JRB graph_id=creat_graph();
	JRB graph_id2=creat_graph();

	load_data(&graph,&graph_id,&graph_id2,"test.txt");

	int i,n,output[MAX];


		JRB node; int u; 
		jrb_traverse(node, graph_id){
			u=jval_i(node->key);
			printf("%d %s %s\n",u,jval_s(node->val),get_vertex(graph_id2,u));
		}
	
		JRB tree,cur; int v;
		jrb_traverse(node, graph){
			u=jval_i(node->key);
			tree=(JRB)jval_v(node->val);
			jrb_traverse(cur, tree){
				v=jval_i(cur->key);
				printf("%s %s %d\n",get_vertex(graph_id2,u),get_vertex(graph_id2,v),jval_i(cur->val));
			}
		}

/*	
	if (strcmp(argv[1],"-w")==0){
		int u= atoi(argv[3]), v=atoi(argv[4]);
		int d=get_weight(graph,u,v);
		if (d==MAXW) printf("-1\n"); else printf("%d\n",d);
	}
		
	if (strcmp(argv[1],"-p")==0){
		int u= atoi(argv[3]), v=atoi(argv[4]);
		int d=dijkstra(graph,u,v,output);
		printf("%d\n",d);
		for (i=1; i<=output[0]; i++)
			printf("%d %s\n",output[i],get_vertex(graph_id,output[i]));
	}

	if (strcmp(argv[1],"-n")==0){
		int u=atoi(argv[3]);
		int sum=out_degree(graph,u,output);
		printf("%d\n",sum);
		for (i=0 ;i<sum; i++){
			int v=output[i];
			printf("%d %s %d\n",v,get_vertex(graph_id,v),get_weight(graph,u,v));
		}
	}			

	if (strcmp(argv[1],"-s")==0){
		int sum=0,g[MAX];
		while (argv[3+sum]!='\0') {
			g[sum]=atoi(argv[3+sum]);
			sum++;
		}
		printf("%d\n",sum);
		for (i=0; i<sum; i++) printf("%d %s\n",g[i],get_vertex(graph_id,g[i]));

		int check[MAX]={0};
		for (i=0; i<sum; i++) check[g[i]]=1;
		JRB node,cur,tree; int u,v,m=0; 
		jrb_traverse(node, graph){
			u=jval_i(node->key);
			if (check[u]){
				tree=(JRB)jval_v(node->val);
				jrb_traverse(cur, tree){
					v=jval_i(cur->key);
					if (check[v] && u<v) m++;
				}
			}
		}
		printf("%d\n",m);
		jrb_traverse(node, graph){
			u=jval_i(node->key);
			if (check[u]){
				tree=(JRB)jval_v(node->val);
				jrb_traverse(cur, tree){
					v=jval_i(cur->key);
					if (check[v] && u<v) printf("%d %d %d\n",u,v,get_weight(graph,u,v));
				}
			}
		}
	}
*/
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
