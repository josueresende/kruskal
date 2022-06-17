package br.unirio.edu;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

class Instancia {
    public int[][] grafo;
    public int n_nodes; 
}

public class Kruskal {

    static private final int ARESTA_ORIGEM = 0;
    static private final int ARESTA_DESTINO = 1;
    static private final int ARESTA_CUSTO = 2;

    static public void execute_union_by_rank(int[][] dados, int[] parent, int[] rank) {
        int n_nodes = parent.length;
        int n_edges = dados.length;
        int n_node_t = 0;
        for (int i = 0; i < n_edges; i++) {
            if (n_node_t == (n_nodes - 1)) break;
            int vertice_origem = dados[i][ARESTA_ORIGEM];
            int vertice_destino = dados[i][ARESTA_DESTINO];
            int origem = _find(parent , vertice_origem);
            int destino = _find(parent , vertice_destino);
            if (origem != destino)
            {
                _union_by_rank(parent, rank, origem, destino);
                n_node_t++;
            }
        }
    }
    static private void _union_by_rank(int[] parent, int[] rank, int x, int y) {
        int xset = _find(parent, x);
        int yset = _find(parent, y);
        if (rank[xset] < rank[yset]) {
            parent[xset] = yset;
        } else if (rank[yset] < rank[xset]) {
            parent[yset] = xset;
        } else {
            parent[xset] = yset;
            rank[yset]++;
        }
    }
    static public void execute_union_find(int[][] dados, int[] parent) {
        int n_nodes = parent.length;
        int n_edges = dados.length;
        int n_node_t = 0;
        for (int i = 0; i < n_edges; i++) {
            if (n_node_t == (n_nodes - 1)) break;
            int vertice_origem = dados[i][ARESTA_ORIGEM];
            int vertice_destino = dados[i][ARESTA_DESTINO];
            int origem = _find(parent , vertice_origem);
            int destino = _find(parent , vertice_destino);
            if (origem != destino)
            {
                _union_find(parent, origem, destino);
                n_node_t++;
            }
        }
    }
    static private void _union_find(int[] parent, int x, int y) {
        int xset = _find(parent, x);
        int yset = _find(parent, y);
        parent[xset] = yset;
    }
    static private int _find(int[] parent, int i) {
        if (parent[i] == -1) return i;
        return _find(parent, parent[i]);
    }
    static public void print(int[][] dados) {
        int edges = dados.length;
        System.out.println("edges: " + edges);
        for (int i = 0; i < edges; i++) {
            int origem = dados[i][ARESTA_ORIGEM];
            int destino = dados[i][ARESTA_DESTINO];
            int custo = dados[i][ARESTA_CUSTO];
            System.out.println("origem:" + origem + " destino:" + destino + " custo:" + custo);
        }
    }
    static private void troca(int[][] dados, int x, int y) {
        for (int i : new int[]{ARESTA_ORIGEM,ARESTA_DESTINO,ARESTA_CUSTO}) {
            int aux = dados[x][i];
            dados[x][i] = dados[y][i];
            dados[y][i] = aux;
        }
    }
    static private int dividir(int[][] dados, int x, int y) {
        int i, p = x, pivot = dados[y][ARESTA_CUSTO];
        for (i = x; i < y; i++)
        {
            if (dados[i][ARESTA_CUSTO] <= pivot)
            {
                troca(dados, i, p);
                p++;
            }
        }
        troca(dados, y, p);
        return p;
    }
    static public void quicksort(int[][] dados, int x, int y) {
        if (x >= y)
            return;
        int q = dividir(dados, x, y);
        quicksort(dados, x, q - 1);
        quicksort(dados, q + 1, y);
    }
    static public int[][][] carregar_dados(String arquivo) {
        int[][][] dados = null;
        try {
            int graph = -1;
            int index = 0;
            int nodes = 0;
            int edges = 0;
            BufferedReader br = new BufferedReader(
                    new InputStreamReader(
                            new FileInputStream(
                                    new File(arquivo)),
                            Charset.forName("UTF-8")));
            String line;
            while ((line = br.readLine()) != null) {
                if (line.startsWith("END"))
                    break;
                if (line.startsWith("NB_GRAPHS")) {
                    int total = Integer.parseInt(line.substring(10).trim());
                    dados = new int[total][][];
                    continue;
                }
                if (line.startsWith("UNDIRECTED GRAPH")) {
                    graph++;
                    index = -1;
                    continue;
                }
                if (line.startsWith("LIST_OF_EDGES")) {
                    dados[graph] = new int[edges][];
                    index = 0;
                    continue;
                }

                try {
                    if (index > -1) {
                        int[] valores = new int[3];

                        if (line.indexOf("\t") == -1) {
                            int m = -1, n = -1;

                            m = n + 1;
                            n = m + 5;
                            valores[0] = Integer.parseInt(line.substring(m, n).trim());

                            m = n + 1;
                            n = m + 6;
                            valores[1] = Integer.parseInt(line.substring(m, n).trim());

                            m = n + 1;
                            valores[2] = Integer.parseInt(line.substring(m).trim());

                        } else {
                            for (int x = 0, m = -1, n = -1; x < valores.length; x++) {
                                m = n + 1;
                                n = line.indexOf("\t", m);
                                if (n == -1)
                                    n = line.length();
                                valores[x] = Integer.parseInt(line.substring(m, n).trim());
                            }
                        }
                        dados[graph][index++] = valores;
                    }
                    if (line.startsWith("NB_NODES"))
                        nodes = Integer.parseInt(line.substring(10).trim());
                    if (line.startsWith("NB_EDGES"))
                        edges = Integer.parseInt(line.substring(10).trim());
                } catch (Exception e) {
                    System.out.println(line);
                    e.printStackTrace();
                    break;
                }
            }
            br.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            System.out.println("Arquivo " + arquivo + " carregado.");
        }
        return dados;
    }

    static public void imprimir(int[] numeros) {
        System.out.print("numeros: ");
        for (int i = 0; i < numeros.length; i++) {
            if (i > 0)
                System.out.print(", ");
            System.out.print(numeros[i]);
        }
        System.out.println();
    }
}