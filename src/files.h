#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "common.h"
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

