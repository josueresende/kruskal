#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define TURNS 5
#define GAP 0
#define DEBUG 0
typedef struct ARESTA
{
    int _origem;
    int _destino;
    int _custo;
} Aresta;

typedef struct SUBSET
{
    int parent;
    int rank;
} SubSet;
typedef struct GRAPH
{
    int nb_edges; // arestas
    int nb_nodes; // vertices
    Aresta *arestas;
} Graph;

typedef struct MST
{
    int nb_edges; // arestas
    int _custo;
    double ms_time_i; // marcador tempo
    double qs_time_i; // marcador tempo
    double uf_time_i; // marcador tempo
    double time_f; // marcador tempo
    Aresta *arestas;
} MinimumSpanningTree;

typedef struct DATASET
{
    Graph *grafos;
    int nb_graphs;
} Dataset;

int    simple_parent[1000];
SubSet better_subset[1000];

double get_time_in_seconds()
{
    struct timespec t;
#ifdef _WIN32
    clock_gettime(CLOCK_MONOTONIC, &t);
#else
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
#endif
    return (double)t.tv_sec + (1.0e-9 * t.tv_nsec);
}

int compare(const char *left, char *right)
{
    return strncmp(left, right, strlen(left));
}

Dataset abrir(char *nomeDoArquivo)
{
    int nb_graph = -1;
    int nb_edge = -1;

    Dataset dataset;
    dataset.nb_graphs = 0;

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
                Graph *grafo = dataset.grafos + nb_graph;
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
                dataset.nb_graphs = atoi(strdup(buffer + 10));
                dataset.grafos = (Graph *)malloc(dataset.nb_graphs * sizeof(Graph));
                nb_graph = -1;
            }
            else if (compare("NB_NODES", buffer) == 0)
            { // total de vertices
                Graph *grafo = dataset.grafos + nb_graph;
                (*grafo).nb_nodes = atoi(strdup(buffer + 10));
            }
            else if (compare("NB_EDGES", buffer) == 0)
            { // total de arestas
                Graph *grafo = dataset.grafos + nb_graph;
                (*grafo).nb_edges = atoi(strdup(buffer + 10));
            }
            else if (compare("LIST_OF_EDGES", buffer) == 0)
            { // inicia a entrada dados
                Graph *grafo = dataset.grafos + nb_graph;
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
void quicksort(Aresta *A, int x, int y) // O(log n)
{
    if (x >= y)
        return;
    int q = quicksort_dividir(A, x, y);
    quicksort(A, x, q - 1);
    quicksort(A, q + 1, y);
}

int qsort_compare_aresta(const void * a, const void * b)
{
  Aresta *A = (Aresta *)a;
  Aresta *B = (Aresta *)b;
  return (A->_custo - B->_custo);
}

void simple_makeSet(int n_nodes) // O(n)
{
    for (int i = 0; i < n_nodes; i++) {
        simple_parent[i] = i;
    }
}

int simple_find(int i) // O(1)
{
    return simple_parent[i];
}

void simple_union(int n_nodes, int x, int y) // O(n)
{
    for (int n_node = 0; n_node < n_nodes; n_node++)
    {
        if (simple_parent[n_node] == x) {
            simple_parent[n_node] = y;
        }
    }
}

void kruskal_union_find(MinimumSpanningTree *MST, Aresta *E, int n_nodes, int n_edges)
{
    // makeset
    MST->ms_time_i = get_time_in_seconds();
    simple_makeSet(n_nodes);
    // quicksort
    MST->qs_time_i = get_time_in_seconds();
    quicksort(E, 0, n_edges - 1);
    // qsort (E, n_edges - 1, sizeof(Aresta), qsort_compare_aresta);
    // union-find
    MST->uf_time_i = get_time_in_seconds();
    for (int n_edge = 0; n_edge < n_edges; n_edge++)
    {
        int origem  = simple_find(E[n_edge]._origem);
        int destino = simple_find(E[n_edge]._destino);
        if (origem != destino)
        {
            simple_union(n_nodes, origem, destino);
            MST->arestas[MST->nb_edges++] = E[n_edge];
            MST->_custo += E[n_edge]._custo;
        }
    }
    MST->time_f = get_time_in_seconds();
}

void better_makeSet(int n_nodes)
{
    for (int i = 0; i < n_nodes; i++) {
        better_subset[i].parent = i;
        better_subset[i].rank   = 0;
    }
}

int better_find(int i)
{
    if (better_subset[i].parent != i) {
        return better_find(better_subset[i].parent);
    }
    return better_subset[i].parent;
}

void better_union(int x, int y)
{
    x = better_find(x);
    y = better_find(y);
    if (better_subset[x].rank > better_subset[y].rank)
        better_subset[y].parent = x;
    else if (better_subset[x].rank < better_subset[y].rank)
        better_subset[x].parent = y;
    else 
    {
        better_subset[x].parent = y;
        better_subset[y].rank++;
    }
}

void kruskal_union_by_rank(MinimumSpanningTree *MST, Aresta *E, int n_nodes, int n_edges)
{
    // makeset
    MST->ms_time_i = get_time_in_seconds();
    better_makeSet(n_nodes);
    // quicksort
    MST->qs_time_i = get_time_in_seconds();
    quicksort(E, 0, n_edges - 1);
    // qsort (E, n_edges - 1, sizeof(Aresta), qsort_compare_aresta);
    // union by rank
    MST->uf_time_i = get_time_in_seconds();
    for (int n_edge = 0; n_edge < n_edges; n_edge++)
    {
        int origem  = better_find(E[n_edge]._origem);
        int destino = better_find(E[n_edge]._destino);
        if (origem != destino)
        {
            better_union(origem, destino);
            MST->arestas[MST->nb_edges++] = E[n_edge];
            MST->_custo += E[n_edge]._custo;
        }
    }
    MST->time_f = get_time_in_seconds();

}

int nb_header = 0;
void run(char *nomeDoArquivo, char *nomeDaInstancia)
{
    Dataset dataset = abrir(nomeDoArquivo);

    for (int nb_graph = 0; nb_graph < dataset.nb_graphs; nb_graph++)
    {
        int n_nodes = dataset.grafos[nb_graph].nb_nodes;

        int nb_edges = dataset.grafos[nb_graph].nb_edges;

        MinimumSpanningTree MST;
        MST.arestas = (Aresta*)malloc((nb_edges)*sizeof(Aresta));

        double delta_1 = 0;
        int    custo_1 = 0;
        // /*
        { // union-find
            double soma = 0;
            for (int turn = 0; turn < (TURNS + GAP); turn++)
            {
                MST.nb_edges = 0;
                MST._custo = 0;

                // -> COPIAR ARESTAS
                Aresta *arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                for (int nb_edge = 0; nb_edge < nb_edges; nb_edge++) 
                {
                    Aresta *aresta_o = dataset.grafos[nb_graph].arestas + nb_edge;
                    Aresta *aresta_d = arestas + nb_edge;
                    (*aresta_d)._origem = (*aresta_o)._origem;
                    (*aresta_d)._destino = (*aresta_o)._destino;
                    (*aresta_d)._custo = (*aresta_o)._custo;
                }

                kruskal_union_find(
                    &MST,
                    arestas,
                    dataset.grafos[nb_graph].nb_nodes,
                    dataset.grafos[nb_graph].nb_edges
                );

                custo_1 = MST._custo;

                double delta = (MST.time_f - MST.ms_time_i);
                if (turn >= GAP) soma += delta;
                free(arestas);
            }
            delta_1 = soma / TURNS;
            if (DEBUG == 1) 
            {
                printf("%s MST SIMPLE ", nomeDaInstancia);
                for (int n_edge = 0; n_edge < MST.nb_edges; n_edge++)
                {
                    printf("%d-%d=%d | ", MST.arestas[n_edge]._origem, MST.arestas[n_edge]._destino, MST.arestas[n_edge]._custo);
                }
                printf("CUSTO=%d \n", MST._custo);
            }
        }
        // */
        double delta_2 = 0;
        int    custo_2 = 0;
        { // union-by-rank
            double soma = 0;
            for (int turn = 0; turn < (TURNS + GAP); turn++)
            {
                MST.nb_edges = 0;
                MST._custo = 0;

                Aresta *arestas = (Aresta *)malloc(nb_edges * sizeof(Aresta));
                for (int nb_edge = 0; nb_edge < nb_edges; nb_edge++) 
                {
                    Aresta *aresta_o = dataset.grafos[nb_graph].arestas + nb_edge;
                    Aresta *aresta_d = arestas + nb_edge;
                    (*aresta_d)._origem = (*aresta_o)._origem;
                    (*aresta_d)._destino = (*aresta_o)._destino;
                    (*aresta_d)._custo = (*aresta_o)._custo;
                }

                kruskal_union_by_rank(
                    &MST,
                    arestas,
                    dataset.grafos[nb_graph].nb_nodes,
                    dataset.grafos[nb_graph].nb_edges
                );

                custo_2 = MST._custo;

                double delta = (MST.time_f - MST.ms_time_i);
                if (turn >= GAP) soma += delta;
                free(arestas);
            }
            delta_2 = soma / TURNS;
            if (DEBUG == 1) 
            {
                printf("%s MST BETTER ", nomeDaInstancia);
                for (int n_edge = 0; n_edge < MST.nb_edges; n_edge++)
                {
                    printf("%d-%d=%d | ", MST.arestas[n_edge]._origem, MST.arestas[n_edge]._destino, MST.arestas[n_edge]._custo);
                }
                printf("CUSTO=%d \n", MST._custo);
            }
        }
        // */
        free(MST.arestas);

        double mlogn = dataset.grafos[nb_graph].nb_edges * log10(dataset.grafos[nb_graph].nb_nodes);

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
            dataset.nb_graphs,
            (nb_graph + 1),
            dataset.grafos[nb_graph].nb_nodes,
            dataset.grafos[nb_graph].nb_edges,
            delta_1,
            delta_2,
            mlogn,
            (custo_1 != custo_2 ? 0 : custo_1)
        );
        fprintf(arq, "\"%s_%d_%d\", %d, %d, %f, %f, %f, %d \n",  
            nomeDaInstancia, 
            dataset.nb_graphs, 
            (nb_graph + 1), 
            dataset.grafos[nb_graph].nb_nodes, 
            dataset.grafos[nb_graph].nb_edges, 
            delta_1,
            delta_2,
            mlogn,
            (custo_1 != custo_2 ? 0 : custo_1)
        );
        fclose(arq);
    }
}

int main()
{

    char nomeDoArquivo[100];
    char nomeDaInstancia[100];

    if (DEBUG == 1) {
        int i = 3;
        sprintf(nomeDoArquivo, "resource/inst_test%d.dat", i);
        sprintf(nomeDaInstancia, "teste%d", i);
        run(nomeDoArquivo, nomeDaInstancia);
    } else {
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
    // */
}
