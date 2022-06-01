#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "kruskal.h"

typedef struct FILESET
{
	int n_graphs;
	int i_graph;
	Graph *graphs;
} FileSet;
#ifndef __unix__
char *strndup(char *str, int chars)
{
    char *buffer;
    int n;

    buffer = (char *) malloc(chars +1);
    if (buffer)
    {
        for (n = 0; ((n < chars) && (str[n] != 0)) ; n++) buffer[n] = str[n];
        buffer[n] = 0;
    }

    return buffer;
}
#endif
int compare(const char *left, char *right) 
{
	return strncmp(left, right, strlen(left));
}

void read_from_text(FileSet *fileSet, char *filename)
{
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("can't read file");
		exit(1);
	}
	{
		const unsigned MAX_LENGTH = 256;
		char buffer[MAX_LENGTH];
		int b_input_value = 0;
		while (fgets(buffer, MAX_LENGTH, fp)) {

			if (compare("UNDIRECTED GRAPH", buffer) == 0) {
				b_input_value = 0;
				fileSet->i_graph++;
				// printf("graph: %d\n", fileSet->i_graph);
			} else
			if (compare("END", buffer) == 0) {
				break;
			} else
			if (b_input_value == 1) {
				char *numero;
				{
					char *reffub = malloc(strlen(buffer));
					int j = -1;
					for (int i = 0; i < strlen(buffer); i++) 
					{
						// printf("%d ", buffer[i]);
						if (buffer[i] >= 48 && buffer[i] <= 57) reffub[++j] = buffer[i];
						if (j > 0 && buffer[i] == 32 && reffub[j] != 32) reffub[++j] = 32;
					}
					reffub[++j] = 0;
					numero = strtok(reffub, " ");
				}
				int i_edge = ((Graph *) &fileSet->graphs[fileSet->i_graph])->i_edge;
				for (int i = 0; i < 3; i++)
				{
					if (numero == NULL) break;
					if (i == 0)
						((Edge *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->u = atoi(numero);
					if (i == 1)
						((Edge *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->v = atoi(numero);
					if (i == 2)
						((Edge *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->w = atoi(numero);
					numero = strtok(NULL, " ");
				}
				((Graph *) &fileSet->graphs[fileSet->i_graph])->i_edge++;
			} else
			if (compare("NB_GRAPHS", buffer) == 0) {
				fileSet->i_graph = -1;
				fileSet->n_graphs = atoi(strdup(buffer+10));
				fileSet->graphs = (Graph *)malloc(fileSet->n_graphs * sizeof(Graph));
			} else
			if (compare("NB_NODES", buffer) == 0) {
				((Graph *) &fileSet->graphs[fileSet->i_graph])->n_nodes = atoi(strdup(buffer+10));
			} else
			if (compare("NB_EDGES", buffer) == 0) {
				((Graph *) &fileSet->graphs[fileSet->i_graph])->n_edges = atoi(strdup(buffer+10));
			} else
			if (compare("LIST_OF_EDGES", buffer) == 0) {
				int n_nodes = ((Graph *) &fileSet->graphs[fileSet->i_graph])->n_nodes;
				int n_edges = ((Graph *) &fileSet->graphs[fileSet->i_graph])->n_edges;
				//printf("list of edges\nedges: %d\nnodes: %d\n", n_edges, n_nodes);
				((Graph *) &fileSet->graphs[fileSet->i_graph])->i_edge = 0;
				((Graph *) &fileSet->graphs[fileSet->i_graph])->edges = (Edge *)malloc(n_edges * sizeof(Edge));
				b_input_value = 1;
			}
		}
	}
	fclose(fp);
// */
}
