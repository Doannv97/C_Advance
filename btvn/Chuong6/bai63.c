#include<stdio.h>
#include"libfdr/jrb.h"

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

int main(){
	int n,m,v1,v2,i;
	JRB graph=make_jrb();
	printf("nhap so dinh,so canh: "); scanf("%d%d",&n,&m);
	for (i=0; i<m; i++){
		scanf("%d%d",&v1,&v2);
		graph=addEdge(graph,v1,v2);
		graph=addEdge(graph,v2,v1);
	}


	for (i=1; i<=n; i++){
		printf("Dinh %d: ",i);
		JRB node=jrb_find_int(graph,i);
		if (node!=NULL){
			JRB tree=(JRB)jval_v(node->val);
			jrb_traverse(node,tree)
				printf("%d ",jval_i(node->key));
		}
		printf("\n");
	}
	
	JRB node;
	jrb_traverse(node,graph)
		jrb_free_tree(jval_v(node->val));

}
