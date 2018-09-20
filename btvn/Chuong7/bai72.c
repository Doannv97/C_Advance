#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"libfdr/jrb.h"
#include"libfdr/dllist.h"


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

int main(){
	int n,m,v1,v2,i;
	JRB graph=creat_graph();
	printf("nhap so dinh,so canh: "); scanf("%d%d",&n,&m);
	for (i=0; i<m; i++){
		scanf("%d%d",&v1,&v2);
		graph=addEdge(graph,v1,v2);
		graph=addEdge(graph,v2,v1);
	}

	for (i=1; i<=n; i++){
		printf("Dinh %d: ",i);
		int j,sum,output[20];
		sum=get_adjacent_vertices(graph,i,output);
		for (j=0; j<sum; j++) printf("%d ",output[j]);
		printf("\n");
	}
	

	DFS(graph,1,n);
	drop_graph(&graph);
	
	
}
