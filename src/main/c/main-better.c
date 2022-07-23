#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define TURNS 5
#define GAP 2
#define DEBUG 0
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

int simple_parent[100];

int *better_parent;
int *better_rank;

#ifdef _WIN32
double get_time_in_seconds_platform()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (double)t.tv_sec + (1.0e-6 * t.tv_nsec);
}
#else
double get_time_in_seconds_platform()
{
    struct timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return (double)t.tv_sec + (1.0e-6 * t.tv_nsec);
}
#endif

double get_time_for_scale()
{
    return get_time_in_seconds_platform();
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

void simple_makeSet(int n_nodes)
{
    // simple_parent = malloc(n_nodes * sizeof(int));
    for (int i = 0; i < n_nodes; i++) {
        simple_parent[i] = i;
    }
}

int simple_find(int i) // O(1)
{
    // if (simple_parent[i] == -1) return i;
    // return simple_find(simple_parent[i]);
    return simple_parent[i];
}

void simple_union(int n_nodes, int x, int y) // O(n)
{
    // int xset = simple_find(x);
    // int yset = simple_find(y);
    // simple_parent[xset] = yset;
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
    MST->ms_time_i = get_time_for_scale();
    simple_makeSet(n_nodes);
    // quicksort
    MST->qs_time_i = get_time_for_scale();
    quicksort(E, 0, n_edges - 1);
    // union-find
    MST->uf_time_i = get_time_for_scale();
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
    MST->time_f = get_time_for_scale();
}

void better_makeSet(int n_nodes)
{
    better_rank = malloc(n_nodes * sizeof(int));
    for (int i = 0; i < n_nodes; i++)
        better_rank[i] = 0;
    better_parent = malloc(n_nodes * sizeof(int));
    for (int i = 0; i < n_nodes; i++) {
        better_parent[i] = i;
    }
}

int better_find(int i)
{
    if (better_parent[i] != i) {
        return better_find(better_parent[i]);
    }
    return better_parent[i];
}

void better_union(int x, int y)
{
    if (better_rank[x] > better_rank[y])
        better_parent[y] = x;
    else if (better_rank[x] < better_rank[y])
        better_parent[x] = y;
    else 
    {
        better_parent[x] = y;
        better_rank[y]++;
    }
}

void kruskal_union_by_rank(MinimumSpanningTree *MST, Aresta *E, int n_nodes, int n_edges)
{
    // makeset
    MST->ms_time_i = get_time_for_scale();
    better_makeSet(n_nodes);
    // quicksort
    MST->qs_time_i = get_time_for_scale();
    quicksort(E, 0, n_edges - 1);
    // union by rank
    MST->uf_time_i = get_time_for_scale();
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
    MST->time_f = get_time_for_scale();

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

        MST->arestas = (Aresta*)malloc((nb_edges-1)*sizeof(Aresta));

        int total = 0;
        double delta_1 = 0;
        double delta_2 = 0;
        // /*
        { // union-by-rank
            double soma = 0;
            for (int turn = 0; turn < (TURNS + GAP); turn++)
            {
                MST->nb_edges = 0;
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

                kruskal_union_by_rank(
                    MST,
                    arestas,
                    dataset[nb_dataset].grafos[nb_graph].nb_nodes,
                    dataset[nb_dataset].grafos[nb_graph].nb_edges
                );

                total = MST->_custo;

                double delta = (MST->time_f - MST->ms_time_i);
                if (turn >= GAP) soma += delta;
                free(better_rank);
                free(better_parent);
                free(arestas);
            }
            delta_2 = soma / TURNS;
            if (DEBUG == 1) 
            {
                printf("%s MST BETTER ", nomeDaInstancia);
                for (int n_edge = 0; n_edge < MST->nb_edges; n_edge++)
                {
                    printf("%d-%d=%d | ", MST->arestas[n_edge]._origem, MST->arestas[n_edge]._destino, MST->arestas[n_edge]._custo);
                }
                printf("CUSTO=%d \n", MST->_custo);
            }
        }
        // */
        free(MST->arestas);
        free(MST);

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
