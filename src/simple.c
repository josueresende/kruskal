#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct ARESTA
{
	int _origem;
	int _destino;
	int _custo;
} Aresta;

typedef struct ADJACENCIA
{
	int _destino;
	int _custo;
    struct Adjacencia* proximo;
} Adjacencia;

typedef struct GRAPH
{
    int nb_edges; // arestas
    int nb_nodes; // vertices
    Aresta *arestas;
    Adjacencia *vertices;
} Graph;

typedef struct DATASET
{
    Graph *grafos;
    int nb_graphs;
} Dataset;

int *parent;
int *rank;

int compare(const char *left, char *right) 
{
	return strncmp(left, right, strlen(left));
}

Dataset* abrir(char *nomeDoArquivo) 
{
    int nb_graph = -1;
    int nb_edge = -1;

    Dataset *dataset = malloc(sizeof(Dataset));
    (*dataset).nb_graphs = 0;

	FILE *fp;
	fp = fopen(nomeDoArquivo, "r");
	if (fp == NULL)
	{
		printf("nao foi possivel abrir %s", nomeDoArquivo);
		exit(1);
	}
	{
		const unsigned MAX_LENGTH = 256;
		char buffer[MAX_LENGTH];
		int flag_input = 0;
		while (fgets(buffer, MAX_LENGTH, fp)) {

			if (compare("UNDIRECTED GRAPH", buffer) == 0) {
				flag_input = 0;
				nb_graph++;
			} else if (compare("END", buffer) == 0) {
                flag_input = 0;
				break;
			} else if (flag_input == 1) { // procede extração de valores
				char *numero;
                char *reffub = (char *)malloc(strlen(buffer));
                int j = -1;
                for (int i = 0; i < strlen(buffer); i++) 
                {
                    if (buffer[i] >= 48 && buffer[i] <= 57) {
                        reffub[++j] = buffer[i];
                    } else if (j > -1 && reffub[j] != 32) {
                        reffub[++j] = 32;
                    }
                }
                reffub[++j] = 0;
                numero = strtok(reffub, " ");
                Graph *grafo = (*dataset).grafos + nb_graph;
                Aresta *aresta = (*grafo).arestas + nb_edge++;
				for (int i = 0; i < 3; i++)
				{
					if (numero == NULL) break;
					if (i == 0) (*aresta)._origem = atoi(numero);
					if (i == 1) (*aresta)._destino = atoi(numero);
					if (i == 2) (*aresta)._custo = atoi(numero);
					numero = strtok(NULL, " ");
                }
                // tratando adjacencias para exibicao
                Adjacencia *adjacencia;
                int origem = (*aresta)._origem;
                int destino = (*aresta)._destino;
                int custo = (*aresta)._custo;
                {
                    adjacencia = (*grafo).vertices + origem;
                    if ((*adjacencia)._destino != -1) {
                        while ((*adjacencia).proximo != NULL) adjacencia = (*adjacencia).proximo;
                        (*adjacencia).proximo = malloc(sizeof(Adjacencia));
                        adjacencia = (Adjacencia *)((*adjacencia).proximo);
                    }
                    (*adjacencia).proximo = NULL;
                    (*adjacencia)._custo = custo;
                    (*adjacencia)._destino = destino;
                }
                {
                    adjacencia = (*grafo).vertices + destino;
                    if ((*adjacencia)._destino != -1) {
                        while ((*adjacencia).proximo != NULL) adjacencia = (*adjacencia).proximo;
                        (*adjacencia).proximo = malloc(sizeof(Adjacencia));
                        adjacencia = (Adjacencia *)((*adjacencia).proximo);
                    }
                    (*adjacencia).proximo = NULL;
                    (*adjacencia)._custo = custo;
                    (*adjacencia)._destino = origem;
                }
			} else if (compare("NB_GRAPHS", buffer) == 0) {
				(*dataset).nb_graphs = atoi(strdup(buffer+10));
                (*dataset).grafos = (Graph *)malloc((*dataset).nb_graphs * sizeof(Graph));
                nb_graph = -1;
			} else if (compare("NB_NODES", buffer) == 0) { // total de vertices
                Graph *grafo = (*dataset).grafos + nb_graph;
                (*grafo).nb_nodes = atoi(strdup(buffer+10));
			} else if (compare("NB_EDGES", buffer) == 0) { // total de arestas
                Graph *grafo = (*dataset).grafos + nb_graph;
				(*grafo).nb_edges = atoi(strdup(buffer+10));
			} else if (compare("LIST_OF_EDGES", buffer) == 0) { // inicia a entrada dados
                Graph *grafo = (*dataset).grafos + nb_graph;
				int nb_nodes = (*grafo).nb_nodes;
				int nb_edges = (*grafo).nb_edges;
                (*grafo).arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                (*grafo).vertices = malloc(nb_nodes * sizeof(Adjacencia));
                for (int i = 0; i < nb_nodes; i++) {
                    Adjacencia *adjacencia = (*grafo).vertices + i;
                    (*adjacencia)._destino = -1;
                    (*adjacencia)._custo = -1;
                    (*adjacencia).proximo = NULL;
                }
                
                nb_edge = 0;
				flag_input = 1;
			}
		}
	}
	fclose(fp);
// */
    return dataset;
}

void troca(Aresta *x, Aresta *y)
{
	Aresta temp;
	temp = *x;
	*x = *y;
	*y = temp;
}
int dividir(Aresta *A, int x, int y)
{
	int i, p = x, pivot = A[y]._custo;
	for (i = x; i < y; i++)
	{
		if (A[i]._custo <= pivot)
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
int find(int n, int u)
{
	int i = 0;
    int A[n];
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
void unionbyrank(int n, int u, int v)
{
	int x, y;
	x = find(n, u);
	y = find(n, v);
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
void kruskal(Aresta *E, int n_nodes, int n_edges)
{
	quicksort(E, 0, n_edges - 1);

	int vertice_origem, vertice_destino, origem, destino, custo;

	int n_node_t = 0;
	for (int n_edge = 0; n_edge < n_edges; n_edge++)
	{
		// printf("%04d | origem: %d | destino: %d | custo: %d \n", n_edge, E[n_edge]._origem, E[n_edge]._destino, E[n_edge]._custo);
		
        if (n_node_t == (n_nodes - 1)) break;
		vertice_origem = E[n_edge]._origem;
		vertice_destino = E[n_edge]._destino;
		custo = E[n_edge]._custo;

		origem = find(n_nodes, vertice_origem);
		destino = find(n_nodes, vertice_destino);

		// printf("%04d - find(%d) = %d - find(%d) = %d - custo: %d \n", n_edge, vertice_origem, origem, vertice_destino, destino, custo);

        if (origem != destino) 
        {
            unionbyrank(n_nodes, origem, destino);
            n_node_t++;
        }
	}

	// for (int i = 0; i < n_nodes; i++) printf("%d ", parent[i]);
	// printf("\n");
}

int main()
{
    Dataset *dataset = abrir("resource/inst_teste.dat");
    printf("GRAPHS %d \n", dataset[0].nb_graphs);

    int nb_dataset = 0;
    for (int nb_graph = 0; nb_graph < dataset[0].nb_graphs; nb_graph++)
    {
        printf("ARESTAS %d \n", dataset[nb_dataset].grafos[nb_graph].nb_edges);
        printf("VERTICES %d \n", dataset[nb_dataset].grafos[nb_graph].nb_nodes);
        {
            for (int nb_node = 0; nb_node < dataset[nb_dataset].grafos[nb_graph].nb_nodes; nb_node++)
            {
                Adjacencia *adjacencia = (dataset[nb_dataset].grafos[nb_graph].vertices) + nb_node;
                printf("[%d] -> ", nb_node);
                while (adjacencia != NULL)
                {
                    if ((*adjacencia)._destino > -1) {
                        printf("[%d] %d", (*adjacencia)._destino, (*adjacencia)._custo);
                        if ((*adjacencia).proximo != NULL) printf(" -> ");
                    }
                    adjacencia = (*adjacencia).proximo;
                }
                printf("\n");
            }
        }
        int n_nodes = dataset[nb_dataset].grafos[nb_graph].nb_nodes;
        parent = malloc(n_nodes * sizeof(int));
        for (int i = 0; i < n_nodes; i++) parent[i] = i;
        rank = malloc(n_nodes * sizeof(int));
        for (int i = 0; i < n_nodes; i++) rank[i] = 0;
        kruskal(
            dataset[nb_dataset].grafos[nb_graph].arestas,
            dataset[nb_dataset].grafos[nb_graph].nb_nodes,
            dataset[nb_dataset].grafos[nb_graph].nb_edges
        );
        
        printf("parent ");
        for (int i = 0; i < n_nodes; i++) printf("%d ", parent[i]);
        printf("\n");

        printf("rank ");
        for (int i = 0; i < n_nodes; i++) printf("%d ", rank[i]);
        printf("\n");
    }
    
}
