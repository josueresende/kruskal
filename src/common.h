#ifndef __COMMON_H__
#define __COMMON_H__
typedef struct ARESTA
{
	int vertice_origem;
	int vertice_destino;
	int custo;
} Aresta;

typedef struct GRAPH
{
	int n_nodes;
	int n_edges;
	int i_edge;
	Aresta *edges;
} Graph;

typedef struct FILESET
{
	int n_graphs;
	int i_graph;
	Graph *graphs;
} FileSet;
#endif