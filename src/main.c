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

typedef struct GRAPH
{
    int nb_edges; // arestas
    int nb_nodes; // vertices
    Aresta *arestas;
} Graph;

typedef struct DATASET
{
    Graph *grafos;
    int nb_graphs;
} Dataset;

int *parent;
int *rank;

double get_time_in_seconds()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (double)t.tv_sec + 1.0e-9 * t.tv_nsec;
}

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
    if (x >= y)
        return;
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
int _find(int parent[], int i)
{
    if (parent[i] == -1)
        return i;
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
void _union(int parent[], int x, int y)
{
    int xset = _find(parent, x);
    int yset = _find(parent, y);
    parent[xset] = yset;
}
void print_rank(int n_nodes)
{
    printf("--\n");
    printf("P ");
    for (int i = 0; i < n_nodes; i++)
        printf("%3d ", parent[i]);
    printf("\n");

    printf("R ");
    for (int i = 0; i < n_nodes; i++)
        printf("%3d ", rank[i]);
    printf("\n");
}
void kruskal(Aresta *E, int n_nodes, int n_edges)
{
    int vertice_origem, vertice_destino, origem, destino, custo;

    int n_node_t = 0;
    for (int n_edge = 0; n_edge < n_edges; n_edge++)
    {
        if (n_node_t == (n_nodes - 1))
            break;
        vertice_origem = E[n_edge]._origem;
        vertice_destino = E[n_edge]._destino;
        custo = E[n_edge]._custo;

        origem = _find(parent, vertice_origem);
        destino = _find(parent, vertice_destino);

        // printf("find(%d) = %d ", vertice_origem, origem);
        // printf("find(%d) = %d ", vertice_destino, destino);
        // printf("\n");

        if (origem != destino)
        {
            // printf("Vertices in different trees. Add edge to tree: Union(%d, %d) \n", origem, destino);
            _union(parent, origem, destino);
            n_node_t++;
            // } else {
            // printf("Vertices in the same tree. Skip edge \n");
        }
    }
}
void run(char *nomeDoArquivo)
{
    Dataset *dataset = abrir(nomeDoArquivo);

    int nb_dataset = 0;
    for (int nb_graph = 0; nb_graph < dataset[0].nb_graphs; nb_graph++)
    {
        int n_nodes = dataset[nb_dataset].grafos[nb_graph].nb_nodes;

        rank = malloc(n_nodes * sizeof(int));
        for (int i = 0; i < n_nodes; i++)
            rank[i] = 0;
        parent = malloc(n_nodes * sizeof(int));
        for (int i = 0; i < n_nodes; i++)
            parent[i] = -1;

        quicksort(
            dataset[nb_dataset].grafos[nb_graph].arestas,
            0,
            dataset[nb_dataset].grafos[nb_graph].nb_edges - 1);

        double start = get_time_in_seconds();
        kruskal(
            dataset[nb_dataset].grafos[nb_graph].arestas,
            dataset[nb_dataset].grafos[nb_graph].nb_nodes,
            dataset[nb_dataset].grafos[nb_graph].nb_edges);
        double end = get_time_in_seconds();

        printf("ARQUIVO %s | GRAPH %d/%d | ARESTAS %d | VERTICES %d | %f segundos \n",
               nomeDoArquivo,
               (nb_graph + 1),
               dataset[0].nb_graphs,
               dataset[nb_dataset].grafos[nb_graph].nb_edges,
               dataset[nb_dataset].grafos[nb_graph].nb_nodes,
               end - start);

        // print_rank(n_nodes);
    }
}
int main()
{
    char nomeDoArquivo[100];
    for (int i = 1; i <= 10; i++)
    {
        sprintf(nomeDoArquivo, "resource/GrafoCompleto/inst_v%d00.dat", i);
        run(nomeDoArquivo);
    }
    for (int i = 1; i <= 10; i++)
    {
        sprintf(nomeDoArquivo, "resource/GrafoEsparso/i%d00gs.txt", i);
        run(nomeDoArquivo);
    }
}
