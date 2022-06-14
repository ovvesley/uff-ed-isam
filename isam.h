
#include<stdlib.h>

#define NULL_REG -1

typedef long int TIndiceReg;
  

typedef struct no_ISAM{int tipo; // Indica se o nó do arquivo de indices é folha (armazenada dados)
                       int t; //ordem da árvore (número de filhos)
                       int n; //numero de chaves efetivamente armazenadas
                       TIndiceReg *chaves; //array de chaves com (t-1) 
                       TIndiceReg *filhos; //array de filhos com t filhos 
                                           //(quando o nó é folha sao 2t filhos que apontam para registros no arquivo de dados)                    
                       TIndiceReg prox; //ponteiro para o próximo nó de overflow. Somente para nos folhas e de overflow
                       }TNo_ISAM;


typedef struct isam{int t; //Ordem da árvore
                    FILE * arq_dados; //Ponteiro para o arquivo de dados
                    FILE * arq_ind;   //Ponteiro para o arquivo de indices
                    FILE * arq_log;   //Ponteiro para aquivo de log das operacoes

                    long int tam_arq_ind; //Numero de registros no arquivo de indices
                    long int tam_arq_dados; //Numero de registros no arquivo de dados
                    int tam_pag_arq_dados; //Tamanho de uma página o arquivo de dados
                    int tam_pag_arq_ind; //Tamaho de uma página do arquivo de indices (neste trabalho = tam_pag_arq_dados)
                    int num_pag_dados; //Numero de paginas de dados (nos folhas) armazenadas no arquivo de indices
                    int num_pag_indices; //Numero de paginas de indices (nos internos) armazenadas no arquivo de indices
                    int ini_pag_dados; //Inicio da pagina de dados
                    int ini_pag_indices; //Inicio da pagina de indices
                    //indice da raiz da arvore    
                    TIndiceReg raiz;

                    /*--------------------------------------------------------------------------------------------
                    O uso destas funcoes torna o modulo isam agnóstico do esquema (descricao de campos) do arquivo
                    a ser indexado. As funcoes conhecem o dado mas o modulo isam.c nao conhece.
                    Isto cria um desacoplamento do modulo isam das outras partes da aplicacao 
                    *--------------------------------------------------------------------------------------------*/

                    //ponteiro para funcao que le um dado do arquivo de dados em pos
                    void (*ler_dado_pos)(FILE *f, void *dado, int pos);
                    //ponteiro para funcao que le um dado do arquivo de dados em pos
                    void (*salvar_dado_pos)(FILE *f, void *dado, int pos); 
                    //ponteiro para funcao que compara dos dados lidos do arquivo de dados
                    //ponteiro para uma funcao que compara o dado lido do arquivo com o dado de consulta com a chave
                    int (*comparar)(const void *a, const void *b);
                    //ponteiro para uma funcao que criar um dado em memoria correspondente aos
                    //campos de um registro do arquivo de dados
                    void * (*criar_dado)();
                    //ponteiro para uma funcao que libera um dado alocado
                    void * (*liberar_dado)();
                    //ponteiro para uma funcao que imprime os campos de um dado
                    void (*imprimir_dado)();
                    //ponteiro para uma funcao que retorna o tamanho do arquivo de dados
                    int (*calcula_tam_arq_dados)(FILE *f);
                    //ponteiro para uma funcao que retorna o tamanho do registro do arquivo de dados
                    int (*calcula_tam_arq_reg)(FILE *f);
                    
                    }T_ISAM; //Indice do no (pagina) raiz da arvore

/*
___________________________________________________________________________________________


                             Paginas de dados (nos folha)

___________________________________________________________________________________________

                            Paginas de indices (nos internos)

___________________________________________________________________________________________
Paginas de overflow (sao nos encadeados tendo como cabeca alguma pagina na secao de dados)

___________________________________________________________________________________________

/************************************************************************************/
/*Funcoes utilitarias                                                               */   
/************************************************************************************/

//Retorna o tamanho de um registro do arquivo de indices
int isam_tam_reg_indices(T_ISAM *isam);
//Copia um arquivo binario
void isam_copiar_arquivo_binario(FILE * arq_bin, char * nome_arq);
//Retorna a posicao do registro corrente do arquivo de indices
TIndiceReg isam_pos_arq_ind(T_ISAM *isam);
//Retorna a posicao do registro corrente do arquivo de dados
TIndiceReg isam_pos_arq_dados(T_ISAM *isam);


/************************************************************************************/
/*Infrastrutura basica para gerenciamento de nos pagina                             */   
/************************************************************************************/





//Inicializa os valores de chave e filhos um no correspondente a uma pagina
void isam_inicializar_no(TNo_ISAM *no);
//Criar um no onde folha indica o tipo de no (INTERNO ou FOLHA)
TNo_ISAM *isam_criar_no(T_ISAM *isam, int folha);
//Desaloca um no
TNo_ISAM *isam_liberar_no(TNo_ISAM *no);
//Imprime um no
void isam_imprimir_no(T_ISAM *isam,TNo_ISAM *no);
//Imprime um no
void isam_imprimir_no_versao_antiga(TNo_ISAM *no);
//Salva um no pagina em disco na posicao corrente
void  isam_salvar_no(T_ISAM *isam, TNo_ISAM *no);
//Salva um no pagina em disco na posicao dada por indice
void  isam_salvar_no_pos(T_ISAM *isam, TNo_ISAM *no,TIndiceReg indice);
//Le um no pagina em disco da posicao corrente
void isam_ler_no(T_ISAM *isam, TNo_ISAM * no);
//Le um no pagina em disco da posicao dada por indice
void isam_ler_no_pos(T_ISAM *isam, TNo_ISAM * no, TIndiceReg indice);


/************************************************************************************/
/*Funcoes para criacao do arquivo de indices ISAM                                   */   
/************************************************************************************/

/* Inicializa a estrutura de um arquivo de indices*/
/* O tamanho da pagina do arquivo de dados (do arquivo a ser indexado) deve ser menor que t */
/* O tamanho da pagina do arquivo de dados indica o numero de indices de registros a ser escrito em uma pagina de dados */
/* use stdout para imprimir informacoes no console */
T_ISAM * isam_inicializar(char *nome_arq_ind, char *nome_arq_dados, FILE *arq_log, int t, int tam_pag_arq_dados, int tam_pag_arq_ind);
/* Finaliza a estrutura de indices */
void isam_finalizar(T_ISAM *isam);
/* Cria as paginas de dados e salva no inicio do arquivo de indices */
void isam_criar_paginas_de_dados(T_ISAM *isam);
//Imprime as paginas de dados para depuracao
void isam_imprimir_paginas_de_dados(T_ISAM *isam);
void isam_criar_paginas_de_indices(T_ISAM *isam);
//Imprime a pagina de indices
void isam_imprimir_paginas_de_indices(T_ISAM *isam);
//Cria as paginas de overflow
void isam_criar_paginas_de_overflow();
void isam_criar_paginas(T_ISAM *isam);



/************************************************************************************/
/*Funcoes para busca                                                                */   
/************************************************************************************/


//Dada uma chave lida de um no interno, representando o indice de registro de uma pagina de dados, retorna
//um ponteiro para o tipo estruturado contendo o registro do arquivo de dados 
//Deve ser implementada pela aplicacao
void* isam_ler_dado_chave_no_interno(T_ISAM *isam, int chave_reg);
//void* isam_ler_dado_chave_no_interno(T_ISAM *isam, int chave_reg);
//Procura na folha atual pelo dado no arquivo de dados que satisfaz a consulta
//Deve ser impelementado pela aplicacao
void *isam_ler_dado_chave_no_folha(T_ISAM *isam, TNo_ISAM *no_folha, void *consulta);
//Busca no folha que pode conter o dado desejado
TNo_ISAM* isam_buscar_no_folha(T_ISAM *isam, void *consulta);
//Busca pelo dado no arquivo de dados que satisfaz a consulta
void* isam_buscar(T_ISAM *isam, void *consulta);
//Insere um novo dado no arquivo de dados adicionando a chave do registro no no folha ou
//em um nó de overflow
void isam_insere(T_ISAM *isam, void *dado);


//Callbacks devem ser registradas apos a inicializacao
//Aqui deve ser passada um ponteiro para uma funcao que ira efetuar as acoes correspondentes

void isam_registra_callback_criar_dado(T_ISAM* isam, void *(*criar_dado)());
void isam_registra_callback_liberar_dado(T_ISAM* isam, void *(*liberar_dado)(void *dado));
void isam_registra_callback_ler_dado(T_ISAM *isam, void (*ler_dado_pos)(FILE* f,void *dado, int pos));
void isam_registra_callback_salvar_dado(T_ISAM *isam, void (*salvar_dado_pos)(FILE* f,void *dado, int pos));
void isam_registra_callback_comparar(T_ISAM *isam, int (*comparar)( const void *a, const void *b));
void isam_registra_callback_imprimir_dado(T_ISAM *isam, void (*imprimir_dado)(FILE *arq, const void *dado));
void isam_registra_callback_tam_arq_dados(T_ISAM *isam, int (*tam_arq_dados)(FILE *f));
void isam_registra_callback_tam_reg_dados(T_ISAM *isam, int (*tam_reg_dados)(FILE *f));
