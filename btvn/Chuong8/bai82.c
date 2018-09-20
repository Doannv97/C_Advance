#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"libfdr/jrb.h"
#include"libfdr/dllist.h"

JRB creat_graph();
void drop_graph(JRB *graph);

JRB addVertex(JRB graphid, int id, char* name);
char* getVertex(JRB graphid,int id);
int getid(JRB graphit, char*name);

JRB addEdge(JRB graph,int v1, int v2);
int get_adjacent_vertices(JRB graph, int vertex, int *output);
int indegree(JRB graph, int v, int *output);
int hasEdge(JRB graph,int v1, int v2);
void DFS(JRB graph, int start, int sum);
int DAG(JRB graph, int* kq);

int main(){
	int n,m,v1,v2,i,id; char name[10];
	JRB graph=creat_graph();
	JRB graphid=creat_graph();

/*
	printf("nhap so dinh,so canh: "); scanf("%d%d",&n,&m);
	printf("Nhap ten va id cac dinh:\n");
	for (i=0; i<n; i++){
		scanf("%d%s",&id,name);
		graph=addVertex(graph,id,name);
	}

	printf("Nhap cac canh:\n");
	for (i=0; i<m; i++){
		scanf("%d%d",&v1,&v2);
		graph=addEdge(graph,v1,v2);
	}

*/
	n=4; m=5;
	graphid=addVertex(graphid,1,"HN");
	graphid=addVertex(graphid,2,"HCM");
	graphid=addVertex(graphid,3,"DN");
	graphid=addVertex(graphid,4,"HP");

	graph=addEdge(graph,1,2);
	graph=addEdge(graph,1,3);
	graph=addEdge(graph,3,2);
	graph=addEdge(graph,4,3);
	graph=addEdge(graph,4,1);


	for (i=1; i<=n; i++){
		printf("Dinh %d: ",i);
		int j,sum,output[20];
		sum=get_adjacent_vertices(graph,i,output);
		for (j=0; j<sum; j++) printf("%d ",output[j]);
		printf("\n");
	}
	
	printf("\n\n");
	for (i=1; i<=n; i++){
		printf("Dinh %d: ",i);
		int j,sum,output[20];
		sum=indegree(graph,i,output);
		for (j=0; j<sum; j++) printf("%d ",output[j]);
		printf("\n");
	}

	int kq[20];
	printf("%d\n",DAG(graph,kq));
	for (i=0; i<n; i++) printf("%d ",kq[i]);
	drop_graph(&graph);	
	
}

int DAG(JRB graph, int* kq){
	int u,n=0;
	Dllist node,stack; 
	JRB tree,cur;
	
	int kiemtracungvao(int v){
		int sum,output[20];
		sum=indegree(graph,v,output);
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
		int j,sum,output[20];
		sum=get_adjacent_vertices(graph,u,output);
		for (j=0; j<sum; j++){
			int v=output[j];
			cur=jrb_find_int(graph,u);
			tree=(JRB)jval_v(cur->val);
			cur=jrb_find_int(tree,v);
			jrb_delete_node(cur);
			if (kiemtracungvao(v)) dll_append(stack,new_jval_i(v));
		}
	}
	
	int sum=0,output[20]; 
	jrb_traverse(cur,graph) sum+=get_adjacent_vertices(graph,u,output);
	if (sum==0) return 1; else return 0;

}
	

int hasEdge(JRB graph,int v1, int v2){
	if(graph == NULL) return 0;
	JRB node =jrb_find_int(graph,v1);                           
    	if(node == NULL)  return 0;
	JRB tree=(JRB)jval_v(node->val);
	if (jrb_find_int(tree,v2)) return 1; else return 0;
}
	

int indegree(JRB graph, int v, int *output){
	if(graph == NULL) return 0;

    	int total = 0;
	JRB cur;
	jrb_traverse(cur,graph){
		JRB tree=(JRB)jval_v(cur->val);
		if (jrb_find_int(tree,v)) output[total++] = jval_i(cur->key);	
	}
   	return total;
}

	

int getid(JRB graphid, char*name){
	JRB cur;	
	jrb_traverse(cur,graphid)
		if (strcmp(name,jval_s(cur->val))==0)
			return jval_i(cur->key); 
	return -1;
}

char* getVertex(JRB graphid,int id){
	JRB node=jrb_find_int(graphid,id);
	return jval_s(node->val);
}

JRB addVertex(JRB graphid, int id, char* name){
	jrb_insert_int(graphid,id,new_jval_s(name));
	return graphid;
}
	

JRB creat_graph(){
	return make_jrb();
}

JRB addEdge(JRB graph,int v1, int v2){
	JRB node=jrb_find_int(graph,v1);
	if (node==NULL){
		node=make_jrb();
		jrb_insert_int(graph,v1,new_jval_v(node));
		jrb_insert_int(node,v2,new_jval_i(1));
	}
	else{
		JRB tree=(JRB)jval_v(node->val);
		jrb_insert_int(tree,v2,new_jval_i(1));
	}
	return graph;
}

int get_adjacent_vertices(JRB graph, int vertex, int *output){
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

void DFS(JRB graph, int start, int sum){
	int visited[20]={};
	int n,u,v,output[20],i;
	Dllist node,stack;

	stack=new_dllist();
	dll_append(stack,new_jval_i(start));

	while (!dll_empty(stack)){
		node= dll_last(stack);
		u=jval_i(node->val);
		dll_delete_node(node);
		if (!visited[u]){
			visited[u]=1;
			n= get_adjacent_vertices(graph,u,output);
			for (i=0; i<n; i++){
				v=output[i];
				if (!visited[v]) dll_append(stack,new_jval_i(v));
			}
		}
	}
	for (i=1; i<=sum; i++) if (visited[i]) printf("%d ",i); printf("\n");
}

