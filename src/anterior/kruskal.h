#ifndef __KRUSKAL_H__
#define __KRUSKAL_H__
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "quicksort.h"

#define M 1000

int rank[M];
int parent[M];

//*
void printMST(Aresta *T, int n)
{
	int i, cost = 0;
	printf("Selected Edges:\no   d   c\n");
	for (i = 0; i < n; i++)
	{
		printf("%d  %d  %d\n", T[i].vertice_origem, T[i].vertice_destino, T[i].custo);
		cost += T[i].custo;
	}
	printf("Cost = %d\n", cost);
}

int find(int u)
{
	int i = 0, A[M];
	while (u != parent[u])
	{
		A[i] = u;
		u = parent[u];
		i++;
	}
	i--;
	while (i >= 0)
	{
		parent[A[i]] = u;
		i--;
	}
	return u;
}

void unionbyrank(int u, int v)
{
	int x, y;
	x = find(u);
	y = find(v);
	if (rank[x] < rank[y])
		parent[x] = y;
	else if (rank[y] < rank[x])
		parent[y] = x;
	else
	{
		parent[x] = y;
		rank[y]++;
	}
}
// */
/*
void Kruskal(Edge *E, int n, int e)
{
	int i, j, x, y, u, v;
	Edge *T;
	T = (Edge *)malloc((n - 1) * sizeof(Edge));
	QuickSort(E, 0, e - 1);
	// printMST(E,e);
	j = 0;
	for (i = 0; i < e; i++)
	{
		if (j == n - 1)
			break;
		u = E[i].u;
		v = E[i].v;
		x = find(u);
		y = find(v);
		if (x != y)
		{
			T[j] = E[i];
			unionbyrank(x, y);
			j++;
		}
	}

	printMST(T, n - 1);
}
*/
void pre_kruskal(int n_nodes, int n_edges)
{
	for (int i = 0; i < M; i++) rank[i] = 0;
	for (int i = 0; i < M; i++) parent[i] = i;
}

typedef struct CHAIN
{
	int vertice;
	int custo;
	struct Chain *proximo;
} Chain;

void matriz_adjacencias(Aresta *E, int n_nodes, int n_edges)
{
	Chain* vertices = (Chain *)malloc(n_nodes * sizeof(Chain));
	for (int n_edge = 0; n_edge < n_edges; n_edge++) {
		int vertice_origem = (&E[n_edge])->vertice_origem;
		int vertice_destino = (&E[n_edge])->vertice_destino;
		int custo = (&E[n_edge])->custo;
		// ((Chain *) &vertices[vertice_origem])->
	}
	
	// for (int n_edge = 0; n_edge < n_edges; n_edge++)
	// {
	// }

}
void kruskal(Aresta *E, int n_nodes, int n_edges)
{
	int vertice_origem, vertice_destino, origem, destino, custo;
	quicksort(E, 0, n_edges - 1);
	int n_node_t = 0;
	printf("arestas: %04d \n", n_edges);
	for (int n_edge = 0; n_edge < n_edges; n_edge++)
	{
		if (n_node_t == (n_nodes - 1)) break;
		vertice_origem = (E+n_edge)->vertice_origem;
		vertice_destino = (E+n_edge)->vertice_destino;
		custo = (E+n_edge)->custo;

		origem = find(vertice_origem);
		destino = find(vertice_destino);

		printf("%04d - find(%d) = %d - find(%d) = %d \n", n_edge, vertice_origem, origem, vertice_destino, destino);

		// printf("%04d - origem:%d destino:%d custo:%d -- ", n_edge, vertice_origem, vertice_destino, custo);
		// printf("%04d - origem:%d destino:%d \n", n_edge, origem, destino, custo);

		if (origem != destino) 
		{
			unionbyrank(origem, destino);
			n_node_t++;
		}
	}
	// for (int i = 0; i < n_nodes; i++) printf("%d ", parent[i]);
	// printf("\n");
}
#endif