package br.unirio.edu;

import java.lang.management.ManagementFactory;
import java.lang.management.ThreadMXBean;

public class Main {
    static ThreadMXBean mx = ManagementFactory.getThreadMXBean();
    static int[][][] grafo_teste = Kruskal.carregar_dados("resource/inst_teste.dat");
    static int[][][] grafo_completo__100 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v100.dat");
    static int[][][] grafo_completo__200 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v200.dat");
    static int[][][] grafo_completo__300 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v300.dat");
    static int[][][] grafo_completo__400 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v400.dat");
    static int[][][] grafo_completo__500 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v500.dat");
    static int[][][] grafo_completo__600 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v600.dat");
    static int[][][] grafo_completo__700 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v700.dat");
    static int[][][] grafo_completo__800 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v800.dat");
    static int[][][] grafo_completo__900 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v900.dat");
    static int[][][] grafo_completo_1000 = Kruskal.carregar_dados("resource/GrafoCompleto/inst_v1000.dat");
    static int[][][] grafo_esparso___100 = Kruskal.carregar_dados("resource/GrafoEsparso/i100gs.txt");
    static int[][][] grafo_esparso___200 = Kruskal.carregar_dados("resource/GrafoEsparso/i200gs.txt");
    static int[][][] grafo_esparso___300 = Kruskal.carregar_dados("resource/GrafoEsparso/i300gs.txt");
    static int[][][] grafo_esparso___400 = Kruskal.carregar_dados("resource/GrafoEsparso/i400gs.txt");
    static int[][][] grafo_esparso___500 = Kruskal.carregar_dados("resource/GrafoEsparso/i500gs.txt");
    static int[][][] grafo_esparso___600 = Kruskal.carregar_dados("resource/GrafoEsparso/i600gs.txt");
    static int[][][] grafo_esparso___700 = Kruskal.carregar_dados("resource/GrafoEsparso/i700gs.txt");
    static int[][][] grafo_esparso___800 = Kruskal.carregar_dados("resource/GrafoEsparso/i800gs.txt");
    static int[][][] grafo_esparso___900 = Kruskal.carregar_dados("resource/GrafoEsparso/i900gs.txt");
    static int[][][] grafo_esparso__1000 = Kruskal.carregar_dados("resource/GrafoEsparso/i1000gs.txt");

    static public void main(String... args) {
        execute_1(grafo_completo__100, 100);
        execute_2(grafo_completo__100, 100);
        execute_1(grafo_completo_1000, 1000);
        execute_2(grafo_completo_1000, 1000);
        // execute(grafo_esparso___100, 100);
        // execute(grafo_teste, 8);
    }
    static private void execute_2(int[][][] data, int n_nodes) {
        for (int i = 0; i < data.length; i++) {
            int[] rank = new int[n_nodes];
            for (int j = 0; j < n_nodes; j++) {
                rank[j] = j;
            }
            int[] parent = new int[n_nodes];
            for (int j = 0; j < n_nodes; j++) {
                parent[j] = -1;
            }
            Kruskal.quicksort(data[i], 0 , data[i].length - 1);
            long s = mx.getCurrentThreadCpuTime();
            Kruskal.execute_union_by_rank(data[i], parent, rank);
            long e = mx.getCurrentThreadCpuTime();
            System.out.println(((e - s) * 1e-9) + " seconds");
        }
    }
    static private void execute_1(int[][][] data, int n_nodes) {
        for (int i = 0; i < data.length; i++) {
            int[] parent = new int[n_nodes];
            for (int j = 0; j < n_nodes; j++) {
                parent[j] = -1;
            }
            Kruskal.quicksort(data[i], 0 , data[i].length - 1);
            long s = mx.getCurrentThreadCpuTime();
            Kruskal.execute_union_find(data[i], parent);
            long e = mx.getCurrentThreadCpuTime();
            System.out.println(((e - s) * 1e-9) + " seconds");
            // for (int j = 0; j < n_nodes; j++) {
            //     System.out.print(String.format("[%d]:%d ", j, parent[j]));
            // }
            // System.out.println();
        }
    }
   
}
