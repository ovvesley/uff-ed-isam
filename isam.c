#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#include"TF.h"
#include"isam.h"
#include "func.h"

#ifndef INT_MAX
#define INFINITO __INT_MAX__
#define MAX_REG __INT_MAX__
#else
#define INFINITO INT_MAX
#define MAX_REG INT_MAX
#endif

#define INTERNO 0
#define FOLHA 1
#define OVERFLOW 2

#define NULO -1


/************************************************************************************/
/*Funcoes utilitarias                                                               */   
/************************************************************************************/

int isam_tam_reg_indices(T_ISAM *isam){
    return  sizeof(int)+  //tipo
            sizeof(int)+  //t
            sizeof(int)+  //n
            sizeof(long int)*((isam->t)-1)+ //chaves
            sizeof(long int)*(isam->t)+   //filhos
            sizeof(long int); //prox
}

void isam_copiar_arquivo_binario(FILE * arq_bin, char * nome_arq){
    char  byte[256];
    int   n_bytes;

    rewind(arq_bin);

    FILE *arq_bin_copia = fopen(nome_arq,"wb");
    if (!arq_bin_copia){
        printf("Falha na abertura de arquivo para copia!\n");
        exit(1);
    }

    while ((n_bytes = fread(byte,sizeof(char),256,arq_bin))>0){
        fwrite(byte,sizeof(char),n_bytes,arq_bin_copia);
    }

    fclose(arq_bin_copia);
}

TIndiceReg isam_pos_arq_ind(T_ISAM *isam){
    int pos = ftell(isam->arq_ind)/isam_tam_reg_indices(isam);   
    return pos;
}


TIndiceReg isam_pos_arq_dados(T_ISAM *isam){
    int pos = ftell(isam->arq_dados)/isam->calcula_tam_arq_reg(isam->arq_dados); 
    return pos;
}

/************************************************************************************/
/*Infrastrutura basica para gerenciamento de nos pagina                             */   
/************************************************************************************/

//Inicializa os valores de chave e filhos um no correspondente a uma pagina
void isam_inicializar_no(TNo_ISAM *no){

    no->n = 0;

    for (int i=0;i<no->t-1;i++){
        no->chaves[i] = NULO;
        no->filhos[i] = NULO;
    }
    
    no->filhos[no->t-1] = NULO;
    no->prox = NULO;

}

//Criar um no onde folha indica o tipo de no (INTERNO ou FOLHA)
TNo_ISAM *isam_criar_no(T_ISAM *isam, int tipo)
{
    TNo_ISAM * no = (TNo_ISAM*)malloc(sizeof(TNo_ISAM));
    no->tipo = tipo;
    no->t = isam->t;
    no->n = 0;
    no->chaves = (TIndiceReg*)malloc((isam->t-1)*sizeof(TIndiceReg)); //Criar espaco adicional para a pagina de dados
    no->filhos = (TIndiceReg*)malloc((isam->t)*sizeof(TIndiceReg));
    no->prox = NULO;
}

//Desaloca um no
TNo_ISAM *isam_liberar_no(TNo_ISAM *no){
    if (no){
        if (no->chaves){
            free(no->chaves);
        }
        if (no->filhos){
            free(no->filhos);
        }
        free(no);
    }
    return NULL;
}

//Imprime um no
void isam_imprimir_no(T_ISAM *isam, TNo_ISAM *no){
 
    switch(no->tipo){
        case INTERNO: fprintf(isam->arq_log,"|tipo:I|");
        break;
        case FOLHA: fprintf(isam->arq_log,"|tipo:F|");
        break;
        case OVERFLOW: fprintf(isam->arq_log,"|tipo:O|");
        break;
        default:
        break;
    };

    fprintf(isam->arq_log,"|n=%d|",no->n);
    for (int i=0;i<no->t-1;i++){
        if (no->chaves[i] != INFINITO){
            fprintf(isam->arq_log,"|c_(%3d)=%3d|",i,no->chaves[i]);
        }
        else{
            fprintf(isam->arq_log,"|c_(%3d)=INF|",i);
        }
    } 
    fprintf(isam->arq_log," ");

    for (int i=0;i<no->t;i++){
        fprintf(isam->arq_log,"|f_(%3d)=%3d|",i,no->filhos[i]);
        
    } 
    if (no->tipo !=INTERNO){
        fprintf(isam->arq_log,"|p=%d|",no->prox);
    }
    fprintf(isam->arq_log,"\n");
}


//Salva um no pagina em disco na posicao corrente
void  isam_salvar_no(T_ISAM *isam, TNo_ISAM *no){

    fwrite(&no->tipo,sizeof(int),1,isam->arq_ind);
    fwrite(&no->t,sizeof(int),1,isam->arq_ind);
    fwrite(&no->n,sizeof(int),1,isam->arq_ind);
    fwrite(no->chaves,sizeof(long int),(isam->t-1),isam->arq_ind);
    fwrite(no->filhos,sizeof(long int),(isam->t),isam->arq_ind);
    fwrite(&no->prox,sizeof(long int),1,isam->arq_ind);
}

//Salva um no pagina em disco na posicao dada por indice
void  isam_salvar_no_pos(T_ISAM *isam, TNo_ISAM *no,TIndiceReg indice){       
    fseek(isam->arq_ind,(indice-1)*isam_tam_reg_indices(isam),SEEK_SET);
    isam_salvar_no(isam,no);
}   

//Le um no pagina em disco da posicao corrente
void isam_ler_no(T_ISAM *isam, TNo_ISAM * no){
    fread(&no->tipo,sizeof(int),1,isam->arq_ind);
    fread(&no->t,sizeof(int),1,isam->arq_ind);
    fread(&no->n,sizeof(int),1,isam->arq_ind);
    fread(no->chaves,sizeof(long int),(isam->t-1),isam->arq_ind);
    fread(no->filhos,sizeof(long int),(isam->t),isam->arq_ind);
    fread(&no->prox,sizeof(long int),1,isam->arq_ind);
}                               

//Le um no pagina em disco da posicao dada por indice
void isam_ler_no_pos(T_ISAM *isam, TNo_ISAM * no, TIndiceReg indice){    
    fseek(isam->arq_ind,(indice-1)*isam_tam_reg_indices(isam),SEEK_SET);
    isam_ler_no(isam,no);

}


/************************************************************************************/
/*Funcoes para criacao do arquivo de indices ISAM                                   */   
/************************************************************************************/


/* Inicializa a estrutura de um arquivo de indices*/
T_ISAM * isam_inicializar(char *nome_arq_ind, char *nome_arq_dados, FILE * arq_log, int t, int tam_pag_arq_dados, int tam_pag_arq_ind){
    T_ISAM * isam = (T_ISAM*)malloc(sizeof(T_ISAM));

    if (!isam){
        printf("Criacao do arquivo de indices falhou:\n");
        exit(1);
    }

    isam->arq_log = arq_log;

    fprintf(isam->arq_log,"_________________________________________________________________________________________________________________________________________\n");
    fprintf(isam->arq_log,"                                                           CRIACAO DE INDICE ISAM                                                        \n");
    fprintf(isam->arq_log,"_________________________________________________________________________________________________________________________________________\n");


    isam->t = t;
    fprintf(isam->arq_log,"Arquivo ISAM inicializado com t = %d  (%d filhos e %d chaves) \n",isam->t,isam->t,isam->t-1);
    
    FILE * arq_i = fopen(nome_arq_ind,"wb+");
    if (!arq_i){
        fprintf(isam->arq_log,"Nao foi possivel criar arquivo de indices\n");
        free(isam);
        exit(1);
    }

    isam->arq_ind = arq_i;
    isam->tam_arq_ind = 0;
    isam->tam_pag_arq_ind = tam_pag_arq_ind;
    fprintf(isam->arq_log,"Arquivo de indices %s para o arquivo %s foi criado\n",nome_arq_ind,nome_arq_dados);
    fprintf(isam->arq_log,"Tamanho da pagina do arquivo de indices: %d\n",isam->tam_pag_arq_ind);
 
    FILE * arq_d = fopen(nome_arq_dados,"rb+");
    if (!arq_d){
        fprintf(isam->arq_log,"Nao foi possivel criar arquivo de dados\n");
        free(isam);
        exit(1);
    }

  
    isam->arq_dados = arq_d;
    isam->tam_arq_dados = 0;
    isam->tam_pag_arq_dados = tam_pag_arq_dados;

    fprintf(isam->arq_log,"Arquivo de dados %s aberto\n",nome_arq_dados);
    fprintf(isam->arq_log,"Tamanho da pagina do arquivo de dados: %d\n",isam->tam_pag_arq_dados);

    fprintf(isam->arq_log,"Tamanho em bytes do registro da pagina de dados: %d\n",isam_tam_reg_indices(isam));
    fprintf(isam->arq_log,"Tamanho em bytes do registro da pagina de interno: %d\n",isam_tam_reg_indices(isam));


    return isam;
}

/* Finaliza a estrutura de indices */
void isam_finalizar(T_ISAM *isam){
    if (isam){
        if (isam->arq_ind)
            fclose(isam->arq_ind);
        if (isam->arq_dados)
            fclose(isam->arq_dados);
    }
}

/* Cria as paginas de dados e salva no inicio do arquivo de indices */
void isam_criar_paginas_de_dados(T_ISAM *isam){
 
    fprintf(isam->arq_log,"Tamanho (#registros) do arquivo de dados: %d\n",isam->tam_arq_dados);
    isam->tam_arq_dados = isam->calcula_tam_arq_dados(isam->arq_dados);

    void *dado = isam->criar_dado();

    //Cria um no_isam em memoria principal para trabalho */
    TNo_ISAM * no_isam = isam_criar_no(isam,FOLHA);
    
    //Inicializa a pagina de dados
    isam->num_pag_dados = 0;

    //Faz o cursor do arquivo apontar para o inicio 
    fseek(isam->arq_ind,0,SEEK_SET);

    // Cria cada uma das paginas
    for (long int i=1;i<=isam->tam_arq_dados;){
        //Inicializa os dados do no
        isam_inicializar_no(no_isam);

        //Coloca tam_arq_dados registros na pagina de dados do arquivo de indices
        //Deixa uma folga para futuras insercoes (tam_pag_arq_dados < t-1)

        for (int j=0;j<isam->tam_pag_arq_dados;j++){
            isam->ler_dado_pos(isam->arq_dados,dado,i+j);
            #ifdef DEPURACAO
            fprintf(isam->arq_log,"Registro do arquivo de dados lido da posicao %d\n",i+j);
            #endif
            no_isam->chaves[j] = i+j;
            no_isam->filhos[j] = i+j;            
            no_isam->n++;
        }
        //Incrementa o numero de paginas de dados
        isam->num_pag_dados++;
        //Salva o no no arquivo de indices na secao de paginas de dados
        isam_salvar_no(isam,no_isam);
        
        #ifdef DEPURACAO
        isam_imprimir_no(isam,no_isam);
        #endif

        //i += isam->tam_pag_arq_dados-1;
        i += isam->tam_pag_arq_dados;
    }
    //Faz flush no arquivo atual
    fflush(isam->arq_ind);
    //Libera estruturas em memoria
    dado = isam->liberar_dado(dado);
    no_isam = isam_liberar_no(no_isam);
}

//Imprime as paginas de dados para depuracao
void isam_imprimir_paginas_de_dados(T_ISAM *isam){
    fprintf(isam->arq_log,"\n\n");
    fprintf(isam->arq_log,"Dumping %d paginas de dados do isam\n",isam->num_pag_dados);
    //Cria um no para trabalho em memoria principal
    TNo_ISAM *no_isam = isam_criar_no(isam,FOLHA);
    //Aponta para o inicio do arquivo
    fseek(isam->arq_ind,0,SEEK_SET);
    //Le cada uma das paginas de dados e  imprime
    for (int i=1;i<=isam->num_pag_dados;i++){
        isam_ler_no(isam,no_isam);
        isam_imprimir_no(isam,no_isam);    
     }
    //Libera o no de trabalho
    no_isam = isam_liberar_no(no_isam);
    fprintf(isam->arq_log,"\n\n");
}

void isam_inicializa_filas_paginas_folha(T_ISAM *isam, TF * f_ind, TF *f_chaves, TF *f_niveis){
    

    //Cria um no tipo folha para trabalho em memoria principal
    TNo_ISAM *no_isam_folha = isam_criar_no(isam,FOLHA);

    //Faz o cursor do arquivo apontar para o inicio
    fseek(isam->arq_ind,0,SEEK_SET);

    //Coloca os indices, chaves maximas da subarvore de cada no, e nivel (= 0)
    //das paginas de dados em tres filas
 
    for (int i=1;i<=isam->num_pag_dados;i++){
        isam_ler_no(isam,no_isam_folha);
        #ifdef DEPURACAO
        isam_imprimir_no(isam,no_isam_folha);
        #endif
        TF_insere(f_ind,i);
        TF_insere(f_chaves,no_isam_folha->chaves[isam->tam_pag_arq_dados-1]);
        TF_insere(f_niveis,0);
    }

    //Libera o no folha de trabalho
    no_isam_folha = isam_liberar_no(no_isam_folha);

}

void isam_criar_paginas_de_indices(T_ISAM *isam){
    //Poderia-se criar um fila de struct especifica, mas por simplificacao 
    //criou-se 3 fila de inteiros
    //Inicializa a fila de indices, a fila de chaves maximas da subarvore e a fila de niveis das paginas 
    TF *f_ind = TF_inicializa(); 
    TF *f_chaves = TF_inicializa();
    TF *f_niveis = TF_inicializa();

    //Calcula a posicao do inicio das paginas de indices. Esta posicao e o final das paginas de dados
    fseek(isam->arq_ind,0,SEEK_END);
    isam->ini_pag_indices = ftell(isam->arq_ind);

    fprintf(isam->arq_log,"Posicao do arquivo apos as paginas de dados %d\n",isam->ini_pag_indices);
    fprintf(isam->arq_log,"Numero de registros de dados: %d\n",isam->num_pag_dados);

    //Adiciona os dados das paginas folhas nas filas para contrucao da arvore
    isam_inicializa_filas_paginas_folha(isam, f_ind, f_chaves, f_niveis);

   
    fprintf(isam->arq_log,"Criando paginas de indices do arquivo de indices...\n");
    fseek(isam->arq_ind,isam->ini_pag_indices,SEEK_SET);

    isam->num_pag_indices = 0; //Inicializa o numero de pagina de indices
    int nivel_atual = -1;      //Inicializa o nivel atual = -1

    //Cria um no interno para trabalho
    TNo_ISAM * no_isam_interno = isam_criar_no(isam,INTERNO);

    //Enquanto a fila for vazia adiciona paginas internas no arquivo de indices
    //na forma de uma arvore
    //Nos da arvore somente podem apontar para filhos de mesmo nivel
    while (!TF_vazia(f_ind)){

        #ifdef DEPURACAO
        TF_imprime(f_ind);
        TF_imprime(f_chaves);
        TF_imprime(f_niveis);
        #endif

        //Obtem o id de registro, a chave maxima e o nivel da pagina atual das filas
        int reg_id = TF_retira(f_ind);
        int chave = TF_retira(f_chaves);
        int nivel = TF_retira(f_niveis);

        //Faz a chave maxima ser a chave da primeira pagina removida da fila
        int max_chave = chave;

        //Atualiza o nivel atual se o nivel da pagina for diferente do atual
        if (nivel != nivel_atual){
            nivel_atual = nivel;
            fprintf(isam->arq_log,"Paginas de indice do nivel %d \n",nivel_atual);
        }
        

        //Inicializa os campos de indice 0 no nó interno
        isam_inicializar_no(no_isam_interno);
        no_isam_interno->tipo = INTERNO;
        //Armazena a primeira chave e o primeiro filho
        no_isam_interno->chaves[0] = chave+1;
        no_isam_interno->filhos[0] = reg_id;  
        no_isam_interno->n++;
        //Remove no maximo mais t-1 paginas da fila para formar a nova pagina interna
        //A fila pode ficar vazia ou o nivel mudar o que significa que a pagina deve ser encerrada

        for (int i=1;i<isam->t;i++){
            //Se a fila nao estiver vazia
            if (TF_vazia(f_ind)) break;        
            //Consulta sem remover o nivel da pagina
            int nivel_no = TF_peek(f_niveis);
            //Se a pagina for de mesmo nivel que o nivel atual
            if (nivel_no != nivel_atual) break;

            //Obtem o registro e a chave maxima
            reg_id = TF_retira(f_ind);
            chave = TF_retira(f_chaves);
            nivel = TF_retira(f_niveis);
            //Atualiza a chave maxima do grupo de paginas do novo no
            max_chave = chave>max_chave?chave:max_chave;
        
            //Armazena a chave como sendo o sucessor da chave maxima da subárvore
            //ou MAX_REG se ultrapassar o total de registro do arquivo de dados
            
            if (i<isam->t-1){
                if (chave<isam->tam_arq_dados){
                    no_isam_interno->chaves[i] = chave+1;    
                }
                else{
                    no_isam_interno->chaves[i] = INFINITO;
                }
                no_isam_interno->n++;
            }

            no_isam_interno->filhos[i] = reg_id;
        }


       //Salva a nova pagina no disco 
       isam_salvar_no(isam,no_isam_interno);
       //Imprime para depuracao
       isam_imprimir_no(isam,no_isam_interno);
       //Incrementa o numero de paginas
       isam->num_pag_indices++;
       //Se fila nao estiver vazia adiciona o novo no a fila,
       if (!TF_vazia(f_ind)){
            //Adiciona os dados (reg_id, chave maxima e nivel) nas filas correspondentes
            TF_insere(f_ind,isam->num_pag_dados+isam->num_pag_indices);
            TF_insere(f_chaves,max_chave);
            TF_insere(f_niveis,nivel_atual+1);
       }
       //Senao ele se torna a raiz da arvore
       else{
           //Armazena o indice do registro da ultima pagina no campo raiz do ISAM 
           isam->raiz = isam->num_pag_dados+isam->num_pag_indices; 
       }
    }


   //Libera as filas
   TF_libera(f_ind);
   TF_libera(f_chaves);
   TF_libera(f_niveis);

   //Descarrega os registros do buffer para o disco 
   fflush(isam->arq_ind);
   //Libera o no ISAM
   no_isam_interno = isam_liberar_no(no_isam_interno);
       
}



//Imprime a pagina de indices
void isam_imprimir_paginas_de_indices(T_ISAM *isam){
    fprintf(isam->arq_log,"\n\n");
    fprintf(isam->arq_log,"Dumping %d paginas de indices do isam\n",isam->num_pag_indices);
    TNo_ISAM *no_isam = isam_criar_no(isam,INTERNO);

    fseek(isam->arq_ind,isam->ini_pag_indices,SEEK_SET);

    for (int i=1;i<=isam->num_pag_indices;i++){
        isam_ler_no(isam,no_isam);
        isam_imprimir_no(isam,no_isam);    
    }
    no_isam = isam_liberar_no(no_isam);
    fprintf(isam->arq_log,"\n\n");
}


void isam_criar_paginas(T_ISAM *isam){
    isam_criar_paginas_de_dados(isam);
    isam_imprimir_paginas_de_dados(isam);
    isam_criar_paginas_de_indices(isam);
    isam_imprimir_paginas_de_indices(isam);
}

void isam_registra_callback_ler_dado(T_ISAM *isam ,void (*ler_dado_pos)(FILE* f,void *dado, int pos)){
    isam->ler_dado_pos = ler_dado_pos;
}

void isam_registra_callback_salvar_dado(T_ISAM *isam ,void (*salvar_dado_pos)(FILE* f,void *dado, int pos)){
    isam->salvar_dado_pos = salvar_dado_pos;
}

void isam_registra_callback_comparar(T_ISAM *isam, int (*comparar)( const void *a, const void *b)){
    isam->comparar = comparar;
}

void isam_registra_callback_criar_dado(T_ISAM *isam, void * (*criar_dado)()){
    isam->criar_dado = criar_dado;
}

void isam_registra_callback_liberar_dado(T_ISAM *isam, void * (*liberar_dado)(void *dado)){
    isam->liberar_dado = liberar_dado;
}

void isam_registra_callback_imprimir_dado(T_ISAM *isam, void (*imprimir_dado)(FILE *arq,const void *dado)){
    isam->imprimir_dado = imprimir_dado;
}

void isam_registra_callback_tam_arq_dados(T_ISAM *isam, int (*tam_arq_dados)(FILE *f)){
    isam->calcula_tam_arq_dados = tam_arq_dados;
}

void isam_registra_callback_tam_reg_dados(T_ISAM *isam, int (*tam_arq_reg)(FILE *f)){
    isam->calcula_tam_arq_reg = tam_arq_reg;
}

/************************************************************************************/
/*Funcoes para busca                                                                */   
/************************************************************************************/


//Dada uma chave lida de um no interno, representando
//o indice de registro de uma pagina de dados, retorna
//um ponteiro para o tipo estruturado contendo os dados do registro
//do arquivo de dados 
void* isam_ler_dado_chave_no_interno(T_ISAM *isam, int chave_reg){
    TNo_ISAM *no_isam = isam_criar_no(isam,INTERNO);

    int aux =0;
    int pos = 0;

    // Iteração não necessário, mas fazendo por garantia que os dados do indice esteja na mesma posição
    for (int index_i = 1; index_i < isam->num_pag_dados; index_i++)
    {
        isam_ler_no_pos(isam, no_isam, index_i);
        for (int index_j =0; index_j < isam->tam_pag_arq_dados; index_j++)
        {
            pos++;
            if(no_isam->chaves[index_j] == chave_reg)
            {
                no_isam->chaves[chave_reg];
                aux = 1;
                break;
            }
        }
        if (aux == 1) break;
    }

    TFunc *dado =(TFunc *) func_criar();

    isam->ler_dado_pos(isam->arq_dados,dado, pos);
    return dado;

}

//Procura na folha atual pelo dado no arquivo de dados 
//que satisfaz a consulta
//Obs.: Funcao auxiliar usada no gabarito para melhor modularizacao - nao e obrigatorio implementa-la
void *isam_ler_dado_chave_no_folha(T_ISAM *isam, TNo_ISAM *no_folha, void *consulta){

//A implementar
}

//Obs.: Funcao auxiliar usada no gabarito para melhor modularizacao - nao e obrigatorio implementa-la
TNo_ISAM *isam_buscar_no_folha(T_ISAM *isam, void *consulta){



}


//Busca pelo dado no arquivo de dados que satisfaz a consulta
void* isam_buscar(T_ISAM *isam, void *consulta){

//A implementar
}




void isam_insere(T_ISAM *isam,void *dado){

//A implementar                
}


