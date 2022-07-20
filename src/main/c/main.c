#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define TURNS 5
typedef struct ARESTA
{
    int _origem;
    int _destino;
    int _custo;
} Aresta;

typedef struct GRAPH
{
    double start;
    double end;
    int nb_edges; // arestas
    int nb_nodes; // vertices
    Aresta *arestas;
} Graph;

typedef struct MST
{
    int nb_edges; // arestas
    int nb_nodes; // vertices
    int _custo;
    Aresta *arestas;
} MinimumSpanningTree;

typedef struct DATASET
{
    Graph *grafos;
    int nb_graphs;
} Dataset;

int *parent;
int *rank;

#ifdef _WIN32
double get_time_in_seconds()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + 1.0e-9 * t.tv_nsec;
}
#else
double get_time_in_seconds()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (double)t.tv_sec + 1.0e-9 * t.tv_nsec;
}
#endif
int compare(const char *left, char *right)
{
    return strncmp(left, right, strlen(left));
}

Dataset *abrir(char *nomeDoArquivo)
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
        while (fgets(buffer, MAX_LENGTH, fp))
        {

            if (compare("UNDIRECTED GRAPH", buffer) == 0)
            {
                flag_input = 0;
                nb_graph++;
            }
            else if (compare("END", buffer) == 0)
            {
                flag_input = 0;
                break;
            }
            else if (flag_input == 1)
            { // procede extração de valores
                char *numero;
                char *reffub = (char *)malloc(strlen(buffer));
                int j = -1;
                for (int i = 0; i < strlen(buffer); i++)
                {
                    if (buffer[i] >= 48 && buffer[i] <= 57)
                    {
                        reffub[++j] = buffer[i];
                    }
                    else if (j > -1 && reffub[j] != 32)
                    {
                        reffub[++j] = 32;
                    }
                }
                reffub[++j] = 0;
                numero = strtok(reffub, " ");
                Graph *grafo = (*dataset).grafos + nb_graph;
                Aresta *aresta = (*grafo).arestas + nb_edge++;
                for (int i = 0; i < 3; i++)
                {
                    if (numero == NULL)
                        break;
                    if (i == 0)
                        (*aresta)._origem = atoi(numero);
                    if (i == 1)
                        (*aresta)._destino = atoi(numero);
                    if (i == 2)
                        (*aresta)._custo = atoi(numero);
                    numero = strtok(NULL, " ");
                }
                int origem = (*aresta)._origem;
                int destino = (*aresta)._destino;
                int custo = (*aresta)._custo;
                free(reffub);
            }
            else if (compare("NB_GRAPHS", buffer) == 0)
            {
                (*dataset).nb_graphs = atoi(strdup(buffer + 10));
                (*dataset).grafos = (Graph *)malloc((*dataset).nb_graphs * sizeof(Graph));
                nb_graph = -1;
            }
            else if (compare("NB_NODES", buffer) == 0)
            { // total de vertices
                Graph *grafo = (*dataset).grafos + nb_graph;
                (*grafo).nb_nodes = atoi(strdup(buffer + 10));
            }
            else if (compare("NB_EDGES", buffer) == 0)
            { // total de arestas
                Graph *grafo = (*dataset).grafos + nb_graph;
                (*grafo).nb_edges = atoi(strdup(buffer + 10));
            }
            else if (compare("LIST_OF_EDGES", buffer) == 0)
            { // inicia a entrada dados
                Graph *grafo = (*dataset).grafos + nb_graph;
                int nb_nodes = (*grafo).nb_nodes;
                int nb_edges = (*grafo).nb_edges;
                (*grafo).arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                nb_edge = 0;
                flag_input = 1;
            }
        }
    }
    fclose(fp);
    // */
    return dataset;
}

void quicksort_troca(Aresta *x, Aresta *y)
{
    Aresta temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

int quicksort_dividir(Aresta *A, int x, int y)
{
    int i, p = x, pivot = A[y]._custo;
    for (i = x; i < y; i++)
    {
        if (A[i]._custo <= pivot)
        {
            quicksort_troca(&A[i], &A[p]);
            p++;
        } 
    }
    quicksort_troca(&A[y], &A[p]);
    return p;
}
void quicksort(Aresta *A, int x, int y)
{
    if (x >= y)
        return;
    int q = quicksort_dividir(A, x, y);
    quicksort(A, x, q - 1);
    quicksort(A, q + 1, y);
}

int _find(int parent[], int i)
{
    if (parent[i] == -1) return i;
    return _find(parent, parent[i]);
}

void _union_by_rank(int u, int v, int parent[], int rank[])
{
    int x, y;
    x = _find(parent, u);
    y = _find(parent, v);
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

void kruskal_union_by_rank(MinimumSpanningTree *MST, Aresta *E, int n_nodes, int n_edges)
{
    rank = malloc(n_nodes * sizeof(int));
    for (int i = 0; i < n_nodes; i++)
        rank[i] = 0;
    parent = malloc(n_nodes * sizeof(int));
    for (int i = 0; i < n_nodes; i++)
        parent[i] = -1;

    quicksort(E, 0, n_edges - 1);

    int origem, destino;

    for (int n_edge = 0; n_edge < n_edges; n_edge++)
    {
        if (MST->nb_edges == (n_edges - 1))
            break;

        origem = _find(parent, E[n_edge]._origem);
        destino = _find(parent, E[n_edge]._destino);

        if (origem != destino)
        {
            _union_by_rank(origem, destino, parent, rank);
            MST->arestas[MST->nb_edges++] = E[n_edge];
            MST->_custo += E[n_edge]._custo;
        }
    }
}

void disjointSet_makeSet(int parent[], int n_nodes)
{
    for (int i = 0; i < n_nodes; i++)
        parent[i] = i;
}

int disjointSet_find(int parent[], int i)
{
    if (parent[i] == i) return i;
    return disjointSet_find(parent, parent[i]);
}

void disjointSet_union(int parent[], int x, int y)
{
    int xset = disjointSet_find(parent, x);
    int yset = disjointSet_find(parent, y);
    parent[xset] = yset;
}

// O(E log E)
void kruskal_union_find(MinimumSpanningTree *MST, Aresta *E, int n_nodes, int n_edges)
{
    parent = malloc(n_nodes * sizeof(int));
    disjointSet_makeSet(parent, n_nodes); // O(n)

    quicksort(E, 0, n_edges - 1); // O(log E)

    int origem, destino;

    for (int n_edge = 0; n_edge < n_edges; n_edge++) // O(E)
    {
        if (MST->nb_edges == (n_edges - 1))
            break;

        origem = disjointSet_find(parent, E[n_edge]._origem);
        destino = disjointSet_find(parent, E[n_edge]._destino);

        if (origem != destino)
        {
            disjointSet_union(parent, origem, destino);
            MST->arestas[MST->nb_edges++] = E[n_edge];
            MST->_custo += E[n_edge]._custo;
        }
    }
}

int nb_header = 0;
void run(char *nomeDoArquivo, char *nomeDaInstancia)
{
    Dataset *dataset = abrir(nomeDoArquivo);
    int nb_dataset = 0;
    for (int nb_graph = 0; nb_graph < dataset[0].nb_graphs; nb_graph++)
    {
        int n_nodes = dataset[nb_dataset].grafos[nb_graph].nb_nodes;

        int nb_edges = dataset[nb_dataset].grafos[nb_graph].nb_edges;

        MinimumSpanningTree *MST = (MinimumSpanningTree*)malloc(sizeof(MinimumSpanningTree));

        int total = 0;
        double delta_1 = 0;
        { // union-find
            double soma = 0;
            int total = 0;
            for (int turn = 0; turn < TURNS; turn++)
            {
                MST->arestas = (Aresta*)malloc((nb_edges-1)*sizeof(Aresta));
                MST->nb_edges = 0;
                MST->nb_nodes = 0;
                MST->_custo = 0;

                // -> COPIAR ARESTAS
                Aresta *arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                for (int nb_edge = 0; nb_edge < nb_edges; nb_edge++) 
                {
                    Aresta *aresta_o = dataset[nb_dataset].grafos[nb_graph].arestas + nb_edge;
                    Aresta *aresta_d = arestas + nb_edge;
                    (*aresta_d)._origem = (*aresta_o)._origem;
                    (*aresta_d)._destino = (*aresta_o)._destino;
                    (*aresta_d)._custo = (*aresta_o)._custo;
                }
                // COPIAR ARESTAS <-
                // -> kruskal union-find
                dataset[nb_dataset].grafos[nb_graph].start = get_time_in_seconds();
                kruskal_union_find(
                    MST,
                    arestas,
                    dataset[nb_dataset].grafos[nb_graph].nb_nodes,
                    dataset[nb_dataset].grafos[nb_graph].nb_edges
                );
                dataset[nb_dataset].grafos[nb_graph].end = get_time_in_seconds();
                // kruskal union-find <-
                total = MST->_custo;
                double delta = (dataset[nb_dataset].grafos[nb_graph].end - dataset[nb_dataset].grafos[nb_graph].start);
                soma += delta;
                free(parent);
                free(arestas);
            }
            delta_1 = soma / TURNS;
        }
        double delta_2 = 0;
        { // union-by-rank
            double soma = 0;
            for (int turn = 0; turn < TURNS; turn++)
            {
                MST->arestas = (Aresta*)malloc((nb_edges-1)*sizeof(Aresta));
                MST->nb_edges = 0;
                MST->nb_nodes = 0;
                MST->_custo = 0;

                Aresta *arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                for (int nb_edge = 0; nb_edge < nb_edges; nb_edge++) 
                {
                    Aresta *aresta_o = dataset[nb_dataset].grafos[nb_graph].arestas + nb_edge;
                    Aresta *aresta_d = arestas + nb_edge;
                    (*aresta_d)._origem = (*aresta_o)._origem;
                    (*aresta_d)._destino = (*aresta_o)._destino;
                    (*aresta_d)._custo = (*aresta_o)._custo;
                }

                dataset[nb_dataset].grafos[nb_graph].start = get_time_in_seconds();
                kruskal_union_by_rank(
                    MST,
                    arestas,
                    dataset[nb_dataset].grafos[nb_graph].nb_nodes,
                    dataset[nb_dataset].grafos[nb_graph].nb_edges
                );
                dataset[nb_dataset].grafos[nb_graph].end = get_time_in_seconds();
                total = MST->_custo;
                double delta = (dataset[nb_dataset].grafos[nb_graph].end - dataset[nb_dataset].grafos[nb_graph].start);
                soma += delta;
                free(rank);
                free(parent);
                free(arestas);
            }
            delta_2 = soma / TURNS;
        }

        double mlogn = dataset[nb_dataset].grafos[nb_graph].nb_edges * log10(dataset[nb_dataset].grafos[nb_graph].nb_nodes);

        FILE *arq;
        arq = fopen("resultado.csv","a");
        char titulo1[50] = "instancia";
        char titulo2[50] = "n";
        char titulo3[50] = "m";
        char titulo4[50] = "tempo - union-find";
        char titulo5[50] = "tempo - union-by-rank";
        char titulo6[50] = "O(m log n)";
        char titulo7[50] = "Solucao";

        if (nb_header++ == 0) {
            fprintf(arq, "%s, %s, %s, %s, %s, %s, %s\n", titulo1, titulo2, titulo3, titulo4, titulo5, titulo6, titulo7);
            printf("\"instancia\", \"n\", \"m\", \"tempo - union-find\", \"tempo - union-by-rank\", \"O(m log n)\", \"solucao\"\n");
        }

        printf("\"%s_%d_%d\", %d, %d, %f, %f, %f, %d \n",
            nomeDaInstancia,
            dataset[0].nb_graphs,
            (nb_graph + 1),
            dataset[nb_dataset].grafos[nb_graph].nb_nodes,
            dataset[nb_dataset].grafos[nb_graph].nb_edges,
            delta_1,
            delta_2,
            mlogn,
            total
        );
        fprintf(arq, "\"%s_%d_%d\", %d, %d, %f, %f, %f, %d \n",  
            nomeDaInstancia, 
            dataset[0].nb_graphs, 
            (nb_graph + 1), 
            dataset[nb_dataset].grafos[nb_graph].nb_nodes, 
            dataset[nb_dataset].grafos[nb_graph].nb_edges, 
            delta_1,
            delta_2,
            mlogn,
            total
        );
        fclose(arq);
    }
    free(dataset);
}

int main()
{

    char nomeDoArquivo[100];
    char nomeDaInstancia[100];

    for (int i = 1; i <= 10; i++)
    {
        sprintf(nomeDoArquivo, "resource/GrafoCompleto/inst_v%d00.dat", i);
        sprintf(nomeDaInstancia, "GrafoCompleto_%d00", i);
        run(nomeDoArquivo, nomeDaInstancia);
    }
    for (int i = 1; i <= 10; i++)
    {
        sprintf(nomeDoArquivo, "resource/GrafoEsparso/i%d00gs.txt", i);
        sprintf(nomeDaInstancia, "GrafoEsparso_%d00", i);
        run(nomeDoArquivo, nomeDaInstancia);
    }
}
