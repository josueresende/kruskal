#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "resource.h"

void write()
{ /*
	int i, n = 0;
	FILE *fp;
	fp = fopen("stu.dat", "wb");
	if (fp == NULL)
	{
		printf("can't create file");
		getch();
		exit(1);
	}
	printf("\n\tHow Many Records You Want to Enter:=");
	scanf("%d", &n);
	for (i = 0; i < n; i++)
	{
		printf("\nEnter Employee ID := ");
		scanf("%d", &s.id);
		printf("\nEnter Employee Name := ");
		scanf("%s", s.name);
		flushall();
		printf("\nEnter the Salary:=");
		scanf("%f", &s.sal);
		printf("\n*****************\n");
		fwrite(&s, sizeof(s), 1, fp);
	}
	fclose(fp);
*/ }

void read()
{ /*
	FILE *fp;
	fp = fopen("stu.dat", "rb");
	if (fp == NULL)
	{
		printf("can't read file");
		getch();
		exit(1);
	}
	while (fread(&s, sizeof(s), 1, fp) == 1)
	{
		printf("\nEmployee ID := %d", s.id);
		printf("\nEmployee Name := %s", s.name);
		flushall();
		printf("\nSalary:= %f", s.sal);
		printf("\n********************\n");
	}
	fclose(fp);
*/ }

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
	printf("%d graphs \n", fileSet->n_graphs);
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
