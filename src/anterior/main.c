#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "files.h"
#include "common.h"
#include "resource.h"
#include "kruskal.h"

void kruskal(Aresta *E, int n_nodes, int n_edges);
void pre_kruskal(int n_nodes, int n_edges);

int main()
{
	FileSet *fileSet = 
		open("resource/inst_teste.dat");
		// open("resource/GrafoCompleto/inst_v100.dat");
		// open("resource/GrafoEsparso/i100gs.txt");
	printf("%d graphs \n", fileSet->n_graphs);
	/*
	for (int g = 0; g < fileSet->n_graphs; g++)
	{
		printf("%d vertices \n", (&fileSet->graphs[g])->n_nodes);
		printf("%d arestas \n", (&fileSet->graphs[g])->n_edges);

		pre_kruskal((&fileSet->graphs[g])->n_nodes, (&fileSet->graphs[g])->n_edges);

		Graph *graph = (Graph *)(&fileSet->graphs[g]);
		clock_t start = clock();
		kruskal(graph->edges, (&fileSet->graphs[g])->n_nodes, (&fileSet->graphs[g])->n_edges);
		clock_t end = clock();
		double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Time measured: %.3f seconds.\n", elapsed);
	} 
}
