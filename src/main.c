#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "files.h"

/*
** referencia https://github.com/abhichand26/kruskal-mst
** Abhishek Chand
*/

int main()
{
	FileSet *fileSet =
		open("resource/GrafoCompleto/inst_v100.dat");
		// open("resource/GrafoEsparso/i100gs.txt");
	printf("%d graphs \n", fileSet->n_graphs);
	//*
	for (int g = 0; g < fileSet->n_graphs; g++)
	{
		printf("%d nodes \n", (&fileSet->graphs[g])->n_nodes);
		printf("%d edges \n", (&fileSet->graphs[g])->n_edges);
		Graph *graph = (Graph *)(&fileSet->graphs[g]);
		Kruskal(graph->edges, (&fileSet->graphs[g])->n_nodes, (&fileSet->graphs[g])->n_edges);
		// for (int e = 0; e < graph->n_edges; e++)
		// {
		// 	printf("%04d - u:%d v:%d w:%d \n", e, (&graph->edges[e])->u, (&graph->edges[e])->v, (&graph->edges[e])->w);
		// }
	} // */
	/*
	{
		clock_t start = clock();
		// Kruskal(E, n, e);
		clock_t end = clock();
		double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
		printf("Time measured: %.3f seconds.\n", elapsed);
	} // */
}
