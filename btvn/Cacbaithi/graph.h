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
