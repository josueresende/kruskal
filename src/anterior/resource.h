#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "common.h"

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
			} else
			if (compare("END", buffer) == 0) {
				break;
			} else
			if (b_input_value == 1) { // procede extração de valores
				char *numero;
				{
					char *reffub = (char *)malloc(strlen(buffer));
					int j = -1;
					for (int i = 0; i < strlen(buffer); i++) 
					{
						if (buffer[i] >= 48 && buffer[i] <= 57) reffub[++j] = buffer[i];
						if (j > -1 && buffer[i] == 32 && reffub[j] != 32) reffub[++j] = 32;
					}
					reffub[++j] = 0;
					numero = strtok(reffub, " ");
				}
				int i_edge = ((Graph *) &fileSet->graphs[fileSet->i_graph])->i_edge++;
				for (int i = 0; i < 3; i++)
				{
					if (numero == NULL) break;
					if (i == 0)
						((Aresta *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->vertice_origem = atoi(numero);
					if (i == 1)
						((Aresta *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->vertice_destino = atoi(numero);
					if (i == 2)
						((Aresta *) &(((Graph *) &fileSet->graphs[fileSet->i_graph])->edges[i_edge]))->custo = atoi(numero);
					numero = strtok(NULL, " ");
				}
			} else
			if (compare("NB_GRAPHS", buffer) == 0) {
				fileSet->i_graph = -1;
				fileSet->n_graphs = atoi(strdup(buffer+10));
				fileSet->graphs = (Graph *)malloc(fileSet->n_graphs * sizeof(Graph));
			} else
			if (compare("NB_NODES", buffer) == 0) { // total de vertices
				((Graph *) &fileSet->graphs[fileSet->i_graph])->n_nodes = atoi(strdup(buffer+10));
			} else
			if (compare("NB_EDGES", buffer) == 0) { // total de arestas
				((Graph *) &fileSet->graphs[fileSet->i_graph])->n_edges = atoi(strdup(buffer+10));
			} else
			if (compare("LIST_OF_EDGES", buffer) == 0) { // inicia a entrada dados
				int n_nodes = ((Graph *) &fileSet->graphs[fileSet->i_graph])->n_nodes;
				int n_edges = ((Graph *) &fileSet->graphs[fileSet->i_graph])->n_edges;
				((Graph *) &fileSet->graphs[fileSet->i_graph])->i_edge = 0;
				((Graph *) &fileSet->graphs[fileSet->i_graph])->edges = (Aresta *)malloc(n_edges * sizeof(Aresta));
				b_input_value = 1;
			}
		}
	}
	fclose(fp);
// */
}
FileSet* open(char *filename) 
{
	printf("file: %s\n", filename);
	FileSet* fileSet = (FileSet *)malloc(sizeof(FileSet));
	{
		clock_t start = clock();
		read_from_text(fileSet, filename);
		clock_t end = clock();
		double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
		printf("read_from_text: time measured: %.3f seconds.\n", elapsed);
	}
	// printf("%d graphs \n", fileSet->n_graphs);
	/*
	for (int g = 0; g < fileSet->n_graphs; g++)
	{
		printf("%d nodes \n", (&fileSet->graphs[g])->n_nodes);
		printf("%d edges \n", (&fileSet->graphs[g])->n_edges);
		Graph *graph = (Graph *)(&fileSet->graphs[g]);
		for (int e = 0; e < graph->n_edges; e++)
		{
			printf("%04d - u:%d v:%d w:%d \n", e, (&graph->edges[e])->u, (&graph->edges[e])->v, (&graph->edges[e])->w);
		}
	} // */
    return fileSet;
}
#endif
