#include "graph.h"

void load_data(JRB *graph, JRB *graph_id, char filename[]){

	JRB g=(JRB)*graph,g_id=(JRB)*graph_id;


	FILE *fptr=fopen(filename,"r");
	int n,m,u,v,d,i;
	char str[20];

	fscanf(fptr,"%d",&n);
	for (i=0; i<n; i++){
		fscanf(fptr,"%d%s",&u,str);
		g_id=add_vertex(g_id,u,str);
	}
	
	fscanf(fptr,"%d",&m);
	for (i=0; i<m; i++){
		fscanf(fptr,"%d%d%d",&u,&v,&d);
		g=add_edge(g,u,v,d);	
		g=add_edge(g,v,u,d);
	}
	
	fclose(fptr);
}	
		
	

int main(int argc, char* argv[]){
	
	if (argc==1){
		printf("Nhap tham so dong lenh vao\n");
		return 0;
	}
	if (strcmp(argv[1],"-h")==0){
		printf("CAdvance-HK20162\n");
		return 0;
	}

	JRB graph=creat_graph();
	JRB graph_id=creat_graph();
	load_data(&graph,&graph_id,argv[2]);

	int i,n,output[MAX];

	if (strcmp(argv[1],"-v")==0){
		JRB node; int u; 
		jrb_traverse(node, graph){
			u=jval_i(node->key);
			printf("%d %s\n",u,get_vertex(graph_id,u));
		}
	}
	
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
}
