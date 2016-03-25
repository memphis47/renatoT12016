#ifndef _GRAFO_H
#define _GRAFO_H

#include "lista.h"

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados para representar um grafo
// 
// o grafo pode ser
// - direcionado ou não
// - com pesos nas arestas ou não
// 
// além dos vértices e arestas, o grafo tem um nome, que é uma "string"
// 
// num grafo com pesos nas arestas todas as arestas tem peso, que é um long int
// 
// o peso default de uma aresta é 0

typedef struct grafo *grafo;

//------------------------------------------------------------------------------
// devolve o nome do grafo g

char *nome_grafo(grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g é direcionado, ou
//         0, caso contrário

int direcionado(grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g tem pesos nas arestas/arcos,
//      ou 0, caso contrário

int ponderado(grafo g);

//------------------------------------------------------------------------------
// devolve o número de vértices do grafo g

unsigned int n_vertices(grafo g);

//------------------------------------------------------------------------------
// devolve o número de arestas/arcos do grafo g

unsigned int n_arestas(grafo g);

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados que representa um vértice do grafo
// 
// cada vértice tem um nome que é uma "string"

typedef struct vertice *vertice;

//------------------------------------------------------------------------------
// devolve o nome do vertice v

char *nome_vertice(vertice v);

//------------------------------------------------------------------------------
// lê um grafo no formato dot de input, usando as rotinas de libcgraph
// 
// desconsidera todos os atributos do grafo lido exceto o atributo
// "peso" quando ocorrer; neste caso o valor do atributo é o peso da
// aresta/arco que é um long int
// 
// num grafo com pesos todas as arestas/arcos tem peso
// 
// o peso default de uma aresta num grafo com pesos é 0
// 
// todas as estruturas de dados alocadas pela libcgraph são
// desalocadas ao final da execução
// 
// devolve o grafo lido ou
//         NULL em caso de erro 

grafo le_grafo(FILE *input);  

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
// 
// devolve 1 em caso de sucesso ou
//         0 caso contrário
// 
// g é um (void *) para que destroi_grafo() possa ser usada como argumento de
// destroi_lista()

int destroi_grafo(void *g);

//------------------------------------------------------------------------------
// escreve o grafo g em output usando o formato dot, de forma que
// 
// 1. todos os vértices são escritos antes de todas as arestas/arcos 
// 
// 2. se uma aresta tem peso, este deve ser escrito como um atributo 
//    de nome "peso"
//
// devolve o grafo escrito ou
//         NULL em caso de erro 

grafo escreve_grafo(FILE *output, grafo g);

//------------------------------------------------------------------------------
// devolve um grafo igual a g

grafo copia_grafo(grafo g);

//------------------------------------------------------------------------------
// devolve a vizinhança do vértice v
// 
// se direcao == 0, v é um vértice de um grafo não direcionado
//                  e a função devolve sua vizinhanca 
//
// se direcao == -1, v é um vértice de um grafo direcionado e a função
//                   devolve sua vizinhanca de entrada
//
// se direcao == 1, v é um vértice de um grafo direcionado e a função
//                  devolve sua vizinhanca de saída

lista vizinhanca(vertice v, int direcao);

//------------------------------------------------------------------------------
// devolve o grau do vértice v
// 
// se direcao == 0, v é um vértice de um grafo não direcionado
//                  e a função devolve seu grau
//
// se direcao == -1, v é um vértice de um grafo direcionado
//                   e a função devolve seu grau de entrada
//
// se direcao == 1, v é um vértice de um grafo direcionado
//                  e a função devolve seu grau de saída

unsigned int grau(vertice v, int direcao);

//------------------------------------------------------------------------------
// devolve 1, se o conjunto dos vertices em l é uma clique em g, ou
//         0, caso contrário
//
// um conjunto C de vértices de um grafo é uma clique em g 
// se todo vértice em C é vizinho de todos os outros vértices de C em g

int clique(lista l, grafo g);

//------------------------------------------------------------------------------
// devolve 1, se v é um vértice simplicial em g, ou
//         0, caso contrário
//
// um vértice é simplicial no grafo se sua vizinhança é uma clique

int simplicial(vertice v, grafo g);

//------------------------------------------------------------------------------
// devolve 1, se g é um grafo cordal ou
//         0, caso contrário
//
// um grafo (não direcionado) G é cordal 
// se e somente se 
// existe uma permutação 
//     (v_1, ..., v_n) de V(G)
// tal que
//     v_i é simplicial em G - v_1 - ... - v_{i-1}

int cordal(grafo g);
#endif
