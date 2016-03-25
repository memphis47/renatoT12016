#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <graphviz/cgraph.h>
#include "grafo.h"
#include "lista.h"

// tamanho máximo da string para representar o peso de uma aresta/arco
#define MAX_STRING_SIZE 256

// número de vértices alocados por vez (para evitar realloc's constantemente)
#define VERTICE_BLOC 64

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

typedef struct grafo{
    char *nome;                      //nome do grafo
    int direcionado;                // 1, se o grafo é direcionado, 0 se não é 
    int ponderado;                  // 1, se o grafo tem pesos nas arestas/arcos, 0 se não é     
    int n_vertices;        //numero de vertices
    int n_arestas;         //numero de arestas
    vertice *vertices;              //apontador para a estrutura de vertices
};

//------------------------------------------------------------------------------
// (apontador para) estrutura de dados que representa um vértice do grafo
// 
// cada vértice tem um nome que é uma "string"

typedef struct vertice{
    char *nome;                     // nome do vertice
    int id;                // id = posição do vertice no vetor de vertices do grafo, serve para facilitar a busca de vertices
    lista lista_vizinhos;                // lista com os vizinhos do vertice, na função de vizinhança você passa uma lista !!!!
    int grau;              // grau do vertice
    int direcao;		    // se for 0, nao é direcionado, se for 1 é de saida, se for -1 é de entrada (ACHOO que é isso)
};

//------------------------------------------------------------------------------
// estrutura dos vizinhos

typedef struct struct_vizinhos{
    long int peso;
    vertice v_origem; 			//vertice de origem
    vertice v_destino;                  //vertice de destino
} *struct_vizinhos;

//------------------------------------------------------------------------------
// cria e devolve um  grafo g

static grafo cria_grafo(const char *nome, int direcionado, int ponderado){
    grafo g = malloc(sizeof(grafo)); //aloca memoria pro grafo

    if(g == NULL){
	printf("Sem memoria para alocas.\n");
    }else{
	g->nome = malloc((strlen(nome) +1) * sizeof(char));
	strcpy(g->nome, nome);
	g->direcionado = direcionado;
	g->ponderado = ponderado;
	g->n_vertices = 0;
	g->n_arestas = 0;
	g->vertices = malloc(VERTICE_BLOC * sizeof(vertice));

	return g;
    }
}

//------------------------------------------------------------------------------
// cria um vizinho e insere na lista de vizinhos do vertice de origem e/ou na de destino tambem
// se ele nao for direcionado

static void cria_vizinhaca(grafo g, vertice v_origem, vertice v_destino, long int peso){
    struct_vizinhos vizinho = malloc(sizeof(struct_vizinhos));

    if(vizinho == NULL){
	printf("Sem memoria para alocas.\n");
    } else{
	vizinho->v_origem = v_origem;
	vizinho->v_destino = v_destino;
	vizinho->peso = peso;
	insere_lista(vizinho, v_origem->lista_vizinhos);
	v_origem->grau++;
	v_destino->grau++;

	if(direcionado(g) == 0){
		struct_vizinhos vizinho2 = malloc(sizeof(struct_vizinhos));
		
    		if(vizinho2 == NULL){
			printf("Sem memoria para alocas.\n");
    		} else{
			vizinho2->v_origem = v_destino;
			vizinho2->v_destino = v_origem;
			vizinho2->peso = peso;
			insere_lista(vizinho, v_destino->lista_vizinhos);
			v_destino->grau++;
			v_origem->grau++;
		}
    	}else{
		struct_vizinhos vizinho3 = malloc(sizeof(struct_vizinhos));
		
    		if(vizinho3 == NULL){
			printf("Sem memoria para alocas.\n");
    		} else{
			vizinho3->v_origem = v_origem;
			vizinho3->v_destino = v_destino;
			vizinho3->peso = peso;
			insere_lista(vizinho, v_origem->lista_vizinhos);
			v_destino->grau++;
			v_origem->grau++;
		}
	
	}
   }

   g->n_arestas++;
}

//------------------------------------------------------------------------------
// destroi um vizinho

static int destroi_vizinho(struct_vizinhos v){
	free(v);
	return 1;
}

//------------------------------------------------------------------------------
// cria, insere no grafo e retorna o vertice
// faltou a direção, eu fiquei em duvida como colocar ela =/
static vertice cria_vertice(grafo g, const char *nome){
	vertice v = malloc(sizeof(struct vertice));

	if(v == NULL){
		printf("Sem memoria para alocas.\n");
	}else{
		v->id = g->n_vertices;
		v->nome = malloc((strlen(nome) +1) *sizeof(char));
		strcopy(v->nome, nome);
		v->lista_vizinhos = constroi_lista();
		v->grau = 0;

		g->vertices[v->id] = v;
		g->n_vertices++;

		if ((g->n_vertices % VERTICE_BLOC) == 0) {
        		long int novo_tamanho = (g->n_vertices / VERTICE_BLOC + 1) * VERTICE_BLOC * sizeof(vertice);
        		g->vertices = realloc(g->vertices, novo_tamanho);
    		}

	}

	return v;
}

//------------------------------------------------------------------------------
// destroi um vertice
static void destroi_vertice(vertice v){
	destroi_lista(v->lista_vizinhos, (int (*)(void *) destroi_vizinho));
	free(v->nome);
	free(v);
}

//------------------------------------------------------------------------------
// busca um vertice pelo nome do grafo

static vertice v_busca(grafo g, char *nome){
	for(int i = 0; i < g->n_vertices; i++){
		if(strcmp(g->vertices[i]->nome, nome) == 0)
			return g->vertices[i];
	}
	return NULL;
}

//------------------------------------------------------------------------------
// devolve o peso de uma aresta no formato libcgraph

static long int get_peso(Agedge_t *Ae) {
    char p_str[5];
    strcpy(p_str, "peso");
    char *peso_c = agget(Ae, p_str);

    if (peso_c && *peso_c)
        return atol(peso_c);

    return 0;
}

//------------------------------------------------------------------------------
// devolve 1 se o grafo no formato libcgraph tem pesos nas arestas

static int contem_pesos(Agraph_t *Ag) {
    char p_str[5];
    strcpy(p_str, "peso");

    for ( Agsym_t *sym = agnxtattr(Ag, AGEDGE, NULL);
          sym;
          sym = agnxtattr(Ag, AGEDGE, sym) ) {
        if (strcmp(sym->name, p_str) == 0)
            return 1;
    }

    return 0;
}

//------------------------------------------------------------------------------
// devolve o nome do grafo g

char *nome_grafo(grafo g){
    char nome_g[1];
    nome_g[0] = '\0';
    return g ? g->nome : nome_g;
}

//------------------------------------------------------------------------------
// devolve 1, se g é direcionado, ou
//         0, caso contrário

int direcionado(grafo g){
    return g ? g->direcionado : 0;
}

//------------------------------------------------------------------------------
// devolve 1, se g tem pesos nas arestas/arcos,
//      ou 0, caso contrário

int ponderado(grafo g){
    return g ? g->ponderado : 0;
}

//------------------------------------------------------------------------------
// devolve o número de vértices do grafo g

unsigned int n_vertices(grafo g){
    return g ? g->n_vertices : 0;
}

//------------------------------------------------------------------------------
// devolve o número de arestas/arcos do grafo g

unsigned int n_arestas(grafo g){
    return g ? g->n_arestas : 0;
}

//------------------------------------------------------------------------------
// devolve o nome do vertice v

char *nome_vertice(vertice v){
    char nome_v[1];
    nome_v[0] = '\0';
    return v ? v->nome : nome_v;
}

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

grafo le_grafo(FILE *input){
	if (input == NULL)
		return NULL;
	
	Agraph_t *Ag = agread(input, NULL);
	
	if(Ag == NULL)
		return NULL;

	grafo g = cria_grafo(agnameof(Ag), agisdirected(Ag), contem_pesos(Ag));

	for (Agnode_t *Av=agfstnode(Ag); Av; Av=agnxtnode(Ag,Av)) {
        	cria_vertice(g, agnameof(Av));
	}

    	for (Agnode_t *Av=agfstnode(Ag); Av; Av=agnxtnode(Ag,Av)) {
        	for (Agedge_t *Ae=agfstout(Ag,Av); Ae; Ae=agnxtout(Ag,Ae)) {
            		vertice u = busca_vertice(g, agnameof(agtail(Ae)));
            		vertice v = busca_vertice(g, agnameof(aghead(Ae)));
            		cria_vizinhos(g, u, v, get_peso(Ae));
        	}
    	}

	agclose(Ag);
	return g;
}  

//------------------------------------------------------------------------------
// desaloca toda a memória usada em *g
// 
// devolve 1 em caso de sucesso ou
//         0 caso contrário
// 
// g é um (void *) para que destroi_grafo() possa ser usada como argumento de
// destroi_lista()

int destroi_grafo(void *g){
    if(g == NULL)
        return 0;
    for (int i=0; i<((grafo) g)->n_vertices; i++)
        destroi_vertice(((grafo) g)->vertices[i]);
        
    free(((grafo) g)->vertices);
    free(((grafo) g)->nome);
    free(g);

    return 1;
}

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

grafo escreve_grafo(FILE *output, grafo g){
	if(g == NULL || output == NULL)
		return NULL;

	Agraph_t *ag;
	Agsym_t *peso;

	char peso_s[MAX_STRING_SIZE];

	//criando a string "peso"
	char p_str[5];
	strcpy(p_str, "peso");

	//cria uma string vazia pra usar como valor default do atributo peso

	char default_s[1];
	default_s[0] = '\0';

	if(g->direcionado)
		ag = agopen(g->nome, Agstrictdirected, NULL);
	else
		ag= agopen(g->nome, Agstrictundirected, NULL);

	if(g->ponderado)
		peso = agattr(ag, AGEDGE, p_str, default_s);

	Agnode_t **nodes = malloc(g->n_vertices * sizeof(Agnode_t*));
	for(int i = 0; i < g->n_vertices; i++)
		nodes[g->vertices[i]->id] = agnode(ag, g->vertices[i]->nome, TRUE);

	for(int i = 0; i < g->n_vertices; i++){
		vertice v = g->vertices[i];

		for(no n = primeiro_no(v->lista_vizinhos); n != NULL; n = proximo_no(n)){
			struct_vizinhos viz = conteudo(n);

			Agedge_t *ae = agedge(ag, nodes[v->id], nodes [viz->v_destino->id], NULL, TRUE);
				
			if(g->ponderado){
				sprintf(peso, "%ld", viz->peso);
				agxset(ae, peso, peso_s);
			}
		}
	}

	free(nodes);
	agwrite(ag, output);
	agclose(ag);

	return g;
}

//------------------------------------------------------------------------------
// devolve um grafo igual a g

grafo copia_grafo(grafo g){
	if(g == NULL)
		printf("Nao tem grafo. \n");

	
    	grafo copy_g = malloc(sizeof(grafo)); //aloca memoria pro grafo

    	if(copy_g == NULL){
		printf("Sem memoria para alocas.\n");
    	}else{
		copy_g->nome = malloc((strlen(g->nome) +1) * sizeof(char));
		strcpy(copy_g->nome, g->nome);
		copy_g->direcionado = g->direcionado;
		copy_g->ponderado = g->ponderado;
		copy_g->n_vertices = g->n_vertices;
		copy_g->n_arestas = g->n_arestas;

		copy_g->vertices = malloc(VERTICE_BLOC * sizeof(vertice));
		//fiquei em duvida se o for resolvia a copia ou nao D:
		for(int i = 0; i < g->n_vertices; i++){
			copy_g->vertices[i] = g->vertices[i];
		}

	return g;
}

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

int clique(lista l, grafo g){
        char *


}

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
//grafo g = malloc(sizeof(struct grafo)); //aloca memoria pro grafo
