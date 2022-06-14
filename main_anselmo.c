#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"func.h"
#include"isam.h"


/*-----------------------------------------------------------------------
       Define dois tipos de consulta, por codigo e por nome
------------------------------------------------------------------------*/

void *consulta_cod(const void * chave){
    int * cod = (int *)chave;
    TFunc * func = func_criar();
    func_set(func,*cod,"",0);
    return func;
}

void *consulta_nome(const void * chave){
    char * nome = (char *)chave;
    TFunc * func = func_criar();
    func_set(func,-1,nome,0);
    return func;
}

/*-----------------------------------------------------------------------
       Define as callbacks para manipular os dados do arquivo de dados
------------------------------------------------------------------------*/


int comparar_nome(const void *a, const void *b){
    TFunc *func_a = (TFunc*)a;
    TFunc *func_b = (TFunc*)b;
    return strcmp(func_a->nome,func_b->nome);
}

int comparar_cod(const void *a, const void *b){
    TFunc *func_a = (TFunc*)a;
    TFunc *func_b = (TFunc*)b;

    if (func_a->cod<func_b->cod){
        return -1;
    }
    else if (func_a->cod>func_b->cod){
        return 1;
    }
    else{
        return 0;
    }
}

//Esta funcao apenas encapsula a chamada para a funcao func_ler_pos
void ler_dado_func(FILE *f,void *dado,int pos){
    TFunc *func = (TFunc*)dado;
    func_ler_pos(f,func,pos);
}

//Esta funcao apenas encapsula a chamada para a funcao func_salvar_pos
void salvar_dado_func(FILE *f, void *dado, int pos){
    TFunc *func = (TFunc*)dado;
    func_salvar_pos(f,func,pos);
}

void *criar_dado_func(){
    return (void *)func_criar();
}

void *liberar_dado_func(void * dado){
    return (void *)func_liberar(dado);
}

void imprimir_dado_func(FILE * arq, const void *dado){
    func_imprimir_arq_txt(arq,(TFunc*)dado);
}

/*-----------------------------------------------------------------------
       Funcoes para interacao com o usuario
------------------------------------------------------------------------*/


void menu_consulta_por_codigo(T_ISAM *isam){
    int continua = 1;
    do{
        printf("Digite o codigo de busca. Digite um codigo<0 para interromper \n"); 
        
        int codigo;
        scanf("%d",&codigo);
        
        if (codigo>0){
            void * consulta = consulta_cod(&codigo);
            TFunc *func = isam_buscar(isam,consulta);
            free(consulta);
            
            if (func){
                printf("Registro encontrado\n");
                func_imprimir(func);
            }
            else{
                printf("Registro nao encontrado\n");
            }
            if (func){
                func = func_liberar(func);
            }
            
        }
        else{
            continua = 0;
        }

    }while (continua);

}

void menu_consulta_por_nome(T_ISAM *isam){
    int continua = 1;
    do{
        printf("Digite o nome da busca. Digite \"sair\" para interromper \n"); 
        
        char nome[50] ="";
        scanf("%[^\n]s",nome);

        if (strcmp(nome,"sair")!=0){
            void * consulta = consulta_nome(nome);
            TFunc *func = isam_buscar(isam,consulta);
            free(consulta);
            
            if (func){
                printf("Registro encontrado\n");
                func_imprimir(func);
            }
            else{
                printf("Registro nao encontrado\n");
            }
            if (func){
                func = func_liberar(func);
            }
            
        }
        else{
            continua = 0;
        }
        fflush(stdin);

    }while (continua);

}

void insere_registro(T_ISAM * isam_cod,T_ISAM *isam_nome){
    TFunc *func = func_criar();
    int cod;
    char nome[50];
    double salario;

    printf("\nDigite:\n");
    printf("\nCodigo:");
    scanf("%d",&cod);
    printf("\nDigite o nome:");
    scanf("%f",nome);
    printf("\nSalario:");
    fflush(stdin);
    scanf("%f",&salario);
    printf("\n");

    func_set(func,cod,nome,salario);
    printf("Atualizando indices de codigos para insercao\n");
    isam_insere(isam_cod,func);
    printf("Atualizando indices de nomes para insercao\n");
    isam_insere(isam_nome,func);
    printf("Salvando novo registro no arquivo de dados\n");
    func_salvar_pos(isam_cod->arq_dados,func,isam_cod->tam_arq_dados);
    fflush(isam_nome->arq_dados);
    func_exportar_arq_texto(isam_cod->arq_dados,"funcionarios_cod.txt");
    func_exportar_arq_texto(isam_nome->arq_dados,"funcionarios_nome.txt");
    
}

void menu(T_ISAM * isam_cod,T_ISAM *isam_nome){
    int continua = 1;

    do{
        printf("Digite o tipo de operacao\n");
        printf("(i)-inserir\n");
        printf("(c)-consulta por codigo\n");
        printf("(n)-consulta por nome\n");
        printf("(x)-sair\n");
        
        char c;
        scanf("%c",&c);
        fflush(stdin);

        switch(c){
            case 'i': insere_registro(isam_cod,isam_nome);
            break;
            case 'n': menu_consulta_por_nome(isam_nome);
            break;
            
            case 'c': menu_consulta_por_codigo(isam_cod);
            break;
            case 'x': continua = 0;
        }
        
    }while (continua);
}

/*-----------------------------------------------------------------------
       Pre-processamento dos arquivos de indices 
------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
       Geracao dos arquivos
------------------------------------------------------------------------*/

void gera_arquivo_de_dados(char *nome_arq, int tam){
    //declara ponteiro para arquivo
    FILE *arq_bin;
    //abre arquivo de dados para leitura e escrita
    if ((arq_bin = fopen(nome_arq, "wb")) == NULL) {
        printf("Erro ao abrir arquivo:%s\n",nome_arq);
        exit(1);
    }

    //insere funcionários
    func_insere_arq(arq_bin,tam);
    fclose(arq_bin);
    printf("Arquivo gerado:%s\n",nome_arq);


}

void gera_arquivo_ordenado(char *nome_arq, char *nome_arq_ord, char *nome_arq_txt,
                           int (*comparar)(const void *a, const void *b)){

    FILE *arq_bin;
    //abre arquivo de dados para leitura e escrita
    if ((arq_bin = fopen(nome_arq, "rb")) == NULL) {
        printf("Erro ao abrir arquivo: %s\n",nome_arq);
        exit(1);
    }

    isam_copiar_arquivo_binario(arq_bin,nome_arq_ord);
    fclose(arq_bin);

    //Cria o arquivo binario ordenado 
    FILE * arq_bin_ord;

    if ((arq_bin_ord = fopen(nome_arq_ord, "rb+")) == NULL) {
        printf("Erro ao abrir arquivo:%s\n",nome_arq_ord);
        exit(1);
    }
 
    
    func_insertion_sort_disco(arq_bin_ord, func_tam_arq(arq_bin_ord),comparar);
    func_exportar_arq_texto(arq_bin_ord,nome_arq_txt);
    printf("Arquivo ordenado: %s\n",nome_arq_ord);
    fclose(arq_bin_ord);

}

void imprime_resultado_busca(T_ISAM *isam,TFunc *func){
    if (func){
        fprintf(isam->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
        fprintf(isam->arq_log,"                                                          REGISTRO ENCONTRADO                                                           \n");
        fprintf(isam->arq_log,"\n");
        func_imprimir_arq_txt(isam->arq_log,func);
    }
    else{
        fprintf(isam->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
        fprintf(isam->arq_log,"                                                         REGISTRO INEXISTENTE                                                           \n");
        fprintf(isam->arq_log,"\n");
    }
}

void consulta_por_cod(T_ISAM *isam, int cod){
    fprintf(isam->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam->arq_log,"|                                                    Consulta pelo codigo:%d                                                          |\n",cod);
    fprintf(isam->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");
    void *consulta = consulta_cod(&cod);
    TFunc *func = isam_buscar(isam,consulta);
    imprime_resultado_busca(isam,func);
    fprintf(isam->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(isam->arq_log,"________________________________________________________________________________________________________________________________________\n");
    fprintf(isam->arq_log,"\n\n");    
}

void consulta_por_nome(T_ISAM *isam, char nome[]){
    fprintf(isam->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam->arq_log,"|                                     Consulta pelo nome: %50s                            |\n",nome);
    fprintf(isam->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");
    void *consulta = consulta_nome(nome);
    TFunc *func = isam_buscar(isam,consulta);
    fprintf(isam->arq_log,"\n");
    imprime_resultado_busca(isam,func);
    fprintf(isam->arq_log,"\n");
    fprintf(isam->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(isam->arq_log,"________________________________________________________________________________________________________________________________________\n");
    fprintf(isam->arq_log,"\n\n");    
    
}

void insercao(T_ISAM *isam_cod,T_ISAM *isam_nome, int cod, char nome[], float salario){
    
    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                            Inserindo novo registro com codigo %5d no arquivo de dados                                               |\n",cod);
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");


    TFunc *func = func_criar();
    func_set(func,cod,nome,salario);
    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    func_imprimir_arq_txt(isam_cod->arq_log,func);
    fprintf(isam_cod->arq_log,"________________________________________________________________________________________________________________________________________\n");
    

    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(isam_cod->arq_log,"Atualizando indices de codigos\n");
    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");

    isam_insere(isam_cod,func);

    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(isam_cod->arq_log,"Atualizando indices de nomes\n");
    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");

    isam_insere(isam_nome,func);
    
    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(isam_cod->arq_log,"Registro salvo\n");
    
    isam_cod->salvar_dado_pos(isam_cod->arq_dados,func,isam_cod->tam_arq_dados);
    fflush(isam_cod->arq_dados);
    isam_nome->salvar_dado_pos(isam_nome->arq_dados,func,isam_nome->tam_arq_dados);
    fflush(isam_nome->arq_dados);
    func = func_liberar(func);
    fprintf(isam_cod->arq_log,"----------------------------------------------------------------------------------------------------------------------------------------\n");


    func_exportar_arq_texto(isam_cod->arq_dados,"funcionarios_cod.txt");

    fprintf(isam_cod->arq_log,"________________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"\n\n");

}

void teste_consulta_simples_por_codigo(T_ISAM *isam_cod, T_ISAM *isam_nome){
    
    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                             TESTES DE CONSULTA SIMPLES POR CODIGO                                                     |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");



    consulta_por_cod(isam_cod,1144);
    consulta_por_cod(isam_cod,1122);
    consulta_por_cod(isam_cod,1377);
    consulta_por_cod(isam_cod,1511);
    

}


void teste_consulta_simples_por_nome(T_ISAM *isam_cod, T_ISAM *isam_nome){

    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                               TESTES DE CONSULTA SIMPLES POR NOME                                                     |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");



    
    consulta_por_nome(isam_nome,"Bruna Andrade Garcia");
    consulta_por_nome(isam_nome,"Fabiola Keller Monteiro");
    consulta_por_nome(isam_nome,"Andre Iglesias Nunes");
    consulta_por_nome(isam_nome,"Carlos Augusto Pedreira");



}


void teste_insercao_simples(T_ISAM *isam_cod, T_ISAM *isam_nome){

    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                           TESTES DE INSERCAO SIMPLES (SEMPRE POR CODIGO)                                              |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");


    insercao(isam_cod,isam_nome,1002,"Antonio Carlos",2345.0);
    insercao(isam_cod,isam_nome,2304,"Marcelo Moreira da Silva",1234.0);
    insercao(isam_cod,isam_nome,1890,"Pedro Moreira da Silva",4321.0);


}


void teste_insercao_overflow(T_ISAM *isam_cod, T_ISAM *isam_nome){

    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                              TESTES DE INSERCAO (SEMPRE POR CODIGO)                                                   |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");



    //Testa overflow
    insercao(isam_cod,isam_nome,2500,"Bianca Souza da Silva",2000.0);
    insercao(isam_cod,isam_nome,2550,"Andre Souza da Silva",2300.0);
    insercao(isam_cod,isam_nome,2560,"Marcos Paulo da Silva",2500.0);
    insercao(isam_cod,isam_nome,2580,"Lucas da Silva Viana",7000.0);
    insercao(isam_cod,isam_nome,2585,"Bruno da Silva Viana",4000.0);



}

void teste_consulta_overflow_por_codigo(T_ISAM *isam_cod, T_ISAM *isam_nome){
    

    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                              TESTES DE CONSULTA OVERFLOW POR CODIGO                                                   |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");



    consulta_por_cod(isam_cod,1002);
    consulta_por_cod(isam_cod,2304);
    consulta_por_cod(isam_cod,1890);
    consulta_por_cod(isam_cod,2500);
    consulta_por_cod(isam_cod,2550);
    consulta_por_cod(isam_cod,2560);
    consulta_por_cod(isam_cod,2580);
    consulta_por_cod(isam_cod,2585);

}


void teste_consulta_overflow_por_nome(T_ISAM *isam_cod, T_ISAM *isam_nome){
    

    fprintf(isam_cod->arq_log," _______________________________________________________________________________________________________________________________________\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");
    fprintf(isam_cod->arq_log,"|                                              TESTES DE CONSULTA OVERFLOW POR NOME                                                     |\n");
    fprintf(isam_cod->arq_log,"|                                                                                                                                       |\n");    
    fprintf(isam_cod->arq_log,"|_______________________________________________________________________________________________________________________________________|\n");


    consulta_por_nome(isam_nome,"Antonio Carlos");
    consulta_por_nome(isam_nome,"Marcelo Moreira da Silva");
    consulta_por_nome(isam_nome,"Pedro Moreira da Silva");
    consulta_por_nome(isam_nome,"Bianca Souza da Silva");
    consulta_por_nome(isam_nome,"Lucas da Silva Viana");
    consulta_por_nome(isam_nome,"Marcos Paulo da Silva");

}


void testes(T_ISAM *isam_cod, T_ISAM *isam_nome){
    teste_consulta_simples_por_codigo(isam_cod,isam_nome);
    teste_consulta_simples_por_nome(isam_cod,isam_nome);
    teste_insercao_simples(isam_cod,isam_nome);
    teste_insercao_overflow(isam_cod,isam_nome);
    teste_consulta_overflow_por_codigo(isam_cod,isam_nome);
    teste_consulta_overflow_por_nome(isam_cod,isam_nome);
}

int main(){


    printf(" _______________________________________________________________________________________________________________________________________\n");
    printf("|                                                                                                                                       |\n");
    printf("|                                            ______________________________________________                                             |\n");
    printf("|                                           |   ________________________________________   |                                            |\n");
    printf("|                                           |  |       ESTRUTURAS DE DADOS PARA SI      |  |                                            |\n");
    printf("|                                           |  |        TRABALHO  DE IMPLEMENTACAO      |  |                                            |\n");
    printf("|                                           |  |       INDEXACAO DE ARQUIVOS - ISAM     |  |                                            |\n");
    printf("|                                           |  |                  2022-1                |  |                                            |\n");
    printf("|                                           |  |                versao 1.0              |  |                                            |\n");
    printf("|                                           |  |                                        |  |                                            |\n");
    printf("|                                           |  |  Arquivo de log dos testes em: log.txt |  |                                            |\n");
    printf("|                                           |  |________________________________________|  |                                            |\n"); 
    printf("|                                           |______________________________________________|                                            |\n");
    printf("|                                                                                                                                       |\n");
    printf("|_______________________________________________________________________________________________________________________________________|\n");



    //Criar o arquivo de dados
    gera_arquivo_de_dados("funcionarios.bin",50);

    //Cria o arquivo binario ordenado por codigo
    gera_arquivo_ordenado("funcionarios.bin","funcionarios_cod.bin","funcionarios_cod.txt",comparar_cod);
    
    FILE *arq_log = fopen("log.txt","w");
    if (!arq_log){
        printf("Arquivo de log nao pode ser criado. Usando o console\n");
        arq_log = stdout;
    }

    //Cria o indice de codigos
    T_ISAM * isam_cod = isam_inicializar("ind_cod.bin","funcionarios_cod.bin",arq_log,5,2,4);
    isam_registra_callback_comparar(isam_cod, comparar_cod);
    isam_registra_callback_ler_dado(isam_cod, ler_dado_func);
    isam_registra_callback_salvar_dado(isam_cod, salvar_dado_func);
    isam_registra_callback_criar_dado(isam_cod,criar_dado_func);
    isam_registra_callback_liberar_dado(isam_cod,liberar_dado_func);
    isam_registra_callback_imprimir_dado(isam_cod,imprimir_dado_func);
    isam_registra_callback_tam_arq_dados(isam_cod, func_tam_arq);
    isam_registra_callback_tam_reg_dados(isam_cod, func_tam_reg);
    isam_criar_paginas(isam_cod);
    
    //Cria o arquivo binario ordenado por nome
    gera_arquivo_ordenado("funcionarios.bin","funcionarios_nome.bin","funcionarios_nome.txt",comparar_nome);
    
    //Criar o indice de nomes
    T_ISAM * isam_nome = isam_inicializar("ind_nome.bin","funcionarios_nome.bin",arq_log,5,2,4);
    isam_registra_callback_comparar(isam_nome, comparar_nome);
    isam_registra_callback_ler_dado(isam_nome, ler_dado_func);
    isam_registra_callback_salvar_dado(isam_nome, salvar_dado_func);
    isam_registra_callback_criar_dado(isam_nome,criar_dado_func);
    isam_registra_callback_liberar_dado(isam_nome,liberar_dado_func);
    isam_registra_callback_imprimir_dado(isam_nome,imprimir_dado_func);
    isam_registra_callback_tam_arq_dados(isam_nome, func_tam_arq);
    isam_registra_callback_tam_reg_dados(isam_nome, func_tam_reg);
    isam_criar_paginas(isam_nome);
    

    //menu(isam_cod,isam_nome);
    
    testes(isam_cod,isam_nome);        

    isam_finalizar(isam_cod);
    isam_finalizar(isam_nome);
    fclose(arq_log);

    return 0;
}