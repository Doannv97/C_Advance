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



void load_data(JRB *graph, JRB *graph_id, char filename[]){

	JRB g=(JRB)*graph,g_id=(JRB)*graph_id;

	FILE *fptr=fopen(filename,"r");
	int n,m,u,v,d,i;
	char str[20];

	fscanf(fptr,"%d%d",&n,&m);
	for (i=0; i<n; i++){
		fscanf(fptr,"%s%d",str,&u);
		g_id=add_vertex(g_id,u,str);
	}
	
	for (i=0; i<m; i++){
		fscanf(fptr,"%d%d",&u,&v);
		g=add_edge(g,u,v,1);	
	}
	
	fclose(fptr);
}	
		
	

int main(int argc, char* argv[]){
	
	if (argc==1){
		printf("Nhap tham so dong lenh vao\n");
		return 0;
	}
	if (strcmp(argv[1],"help")==0){
		printf("Supported command: help, i2n, n2i, cc, cr, odg, rel.\n");
		return 0;
	}
	if (strcmp(argv[1],"about")==0){
		printf("C advanced 20142 final exam.\n");
		return 0;
	}

	JRB graph=creat_graph();
	JRB graph_id=creat_graph();
	load_data(&graph,&graph_id,argv[2]);

	int i,output[MAX];

	if (strcmp(argv[1],"i2n")==0){
		int id= atoi(argv[3]);
		printf("%s\n",get_vertex(graph_id,id));
	}

	if (strcmp(argv[1],"n2i")==0){
		printf("%d\n",get_id(graph_id,argv[3]));
	}
		
	if (strcmp(argv[1],"cc")==0){
		int u= atoi(argv[3]), v=atoi(argv[4]);
		if (jrb_find_int(graph,u)==NULL || jrb_find_int(graph,v)==NULL ) return 0;
		JRB node,cur1,tree1,cur2,tree2; int w1,w2;

		node=jrb_find_int(graph,u);
		tree1=(JRB)jval_v(node->val);

		jrb_traverse(cur1, tree1){
			w1=jval_i(cur1->key);
			node=jrb_find_int(graph,v);
			tree2=(JRB)jval_v(node->val);
			jrb_traverse(cur2, tree2){
				w2=jval_i(cur2->key);
				if (w1==w2) printf("%s\n",get_vertex(graph_id,w1));
			}
		}

	}


	if (strcmp(argv[1],"cr")==0){
		int u= atoi(argv[3]), v=atoi(argv[4]);

		JRB node;  int w;
		jrb_traverse(node, graph){
			w=jval_i(node->key);
			if (has_edge(graph,w,u) && has_edge(graph,w,v))	
       				printf("%s\n",get_vertex(graph_id,w));
		}

	}

	if (strcmp(argv[1],"odg")==0){
		int k=atoi(argv[3]);
		
		JRB node; int w;
		jrb_traverse(node, graph){
			w=jval_i(node->key);
			int sum=out_degree(graph,w,output);
			if (sum>k) printf("%s\n",get_vertex(graph_id,w));
		}
	}			

	if (strcmp(argv[1],"rel")==0){
		int u=atoi(argv[3]),v,w;
		JRB node,cur1,tree1,cur2,tree2; 
		node=jrb_find_int(graph,u);
		if (node==NULL) return 0;
		tree1=(JRB)jval_v(node->val);

		jrb_traverse(cur1, tree1){
			v=jval_i(cur1->key);
			printf("%s\n",get_vertex(graph_id,v));
			node=jrb_find_int(graph,v);
			if (node==NULL) break;
			tree2=(JRB)jval_v(node->val);
			jrb_traverse(cur2, tree2){
				w=jval_i(cur2->key);
				if (w!=u) printf("%s\n",get_vertex(graph_id,w));
			}
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
