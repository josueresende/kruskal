#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__
#include "common.h"

void troca(Aresta *x, Aresta *y)
{
	Aresta temp;
	temp = *x;
	*x = *y;
	*y = temp;
}
int dividir(Aresta *A, int x, int y)
{
	int i, p = x, pivot = A[y].custo;
	for (i = x; i < y; i++)
	{
		if (A[i].custo <= pivot)
		{
			troca(&A[i], &A[p]);
			p++;
		}
	}
	troca(&A[y], &A[p]);
	return p;
}
void quicksort(Aresta *A, int x, int y)
{
	if (x >= y) return;
	int q = dividir(A, x, y);
	quicksort(A, x, q - 1);
	quicksort(A, q + 1, y);
}
#endif