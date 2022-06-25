#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include"func.h"

#define DEPURACAO
#define MAX_NOMES 96
#define MAX_SALARIOS 20

char nomes[MAX_NOMES][15] = {"Andre","Ana","Anderson","Alice",
                             "Breno","Bruna","Bruno","Beatriz",
                             "Carlos","Carla","Claudio","Camila",
                             "Danilo","Diva","Drauzio","Dolores",
                             "Eduardo","Eva","Edir","Efigenia",
                             "Fabio","Fabiola","Felix","Francisca",
                             "Gustavo","Guilhermina","Guilherme","Gerzia",
                             "Henrique","Helena","Heracles","Honorina",
                             "Ivo","Irene","Icaro","Irma",
                             "Jonas","Jessica","Janio","Julia",
                             "Karl","Kamila","Kleber","Kendra",
                             "Lauro","Luana","Lucas","Lurdes",
                             "Marcos","Maria","Mirko","Monique",
                             "Nuno","Norma","Nando","Noemi",
                             "Otacilio","Olga","Otavio","Ofelia",
                             "Patrick","Paula","Paulo","Perla",
                             "Quinn","Quesia","Quemuel","Quiteria",
                             "Ramon","Rogeria","Rodrigo","Ronda",
                             "Silvio","Sandra","Socrates","Sofia",
                             "Tiago","Tereza","Tobias","Tabata",
                             "Ueverton","Uma","Urias","Ursula",
                             "Vinicius","Vitoria","Valter","Viviane",
                             "Xavier","Ximena","Xerxes","Xayane",
                             "Zorba","Zelia","Ziraldo","Zoe"};

char sobrenomes[MAX_NOMES][15] = {"Andrade","Antunes","Almeida","Alves",
                                  "Barbosa","Benitez","Borges","Bernardes",
                                  "Castro","Cruz","Campos","Cortez",
                                  "Dantas","da Costa","da Silva","Dias",
                                  "Encarnacao","Esteves","Evangelista","Ebert",
                                  "Ferreira","Fernandes","Fontes","Fagundes",
                                  "Goncalves","Gonzalo","Garcia","Gentil",
                                  "Hernandez","Henriques","Holanda","Hungria",
                                  "Idelfonso","Igrejas","Inacio","Iglesias",
                                  "Jardim","Jesus","Jacob","Johnson",
                                  "Keller","Klein","Kohler","Kruger",
                                  "Lacerda","Lins","Leite","Lage",
                                  "Montes","Monteiro","Macedo","Marques",
                                  "Nogueira","Neves","Nunes","Noronha",
                                  "Oliveira","Olivares","Oliva","Olimpio",
                                  "Pacheco","Paiva","Pereira","Pimenta",
                                  "Queiroz","Quercia","Queiros","Quintanilha",
                                  "Ruiz","Rodrigues","Rios","Rangel",
                                  "Santana","Soares","Souza","Silva",
                                  "Tavares","Teixeira","Teles","Trindade",
                                  "Ururahy","Uchoa","Uribe","Ullmann",
                                  "Veiga","Vargas","Venancio","Vicente",
                                  "Ximenes","Xu", "Xavi", "Xiao",
                                  "Zanetti","Zanatta","Zahn","Zhang"};

float salarios[20] = {2000.0,
                      2500.0,
                      3000.0,
                      3500.0,
                      4000.0,
                      4500.0,
                      5000.0,
                      5500.0,
                      6000.0,
                      6500.0,
                      7000.0,
                      8000.0,
                      8500.0,
                      9000.0,
                      10000.0,
                      13000.0,
                      15000.0,
                      18000.0,
                      20000.0,
                      25000.0};


char * func_gerar_nome(char nome[50]){
    memset(nome,0,50);
    int ind_nome = ((float)rand()/(float)RAND_MAX)*95;
    int ind_sobrenome =  ((float)rand()/(float)RAND_MAX)*95;
    strcpy(nome,nomes[ind_nome]);
    strcat(nome," ");
    strcat(nome,sobrenomes[ind_sobrenome]);

    int multiplicidade = (float)rand()/(float)RAND_MAX*2;
    for (int i=0;i<multiplicidade;i++){
        strcat(nome," ");
        ind_sobrenome =  ((float)rand()/(float)RAND_MAX)*95;
        strcat(nome,sobrenomes[ind_sobrenome]);
    }

    return nome;
}

float func_gerar_salario(){
    int ind_sal = (float)rand()/(float)RAND_MAX*MAX_SALARIOS;
    return salarios[ind_sal];
}

void func_imprimir(TFunc *func){
    if (!func) return;
    printf("________________________________________________________________________________________________________________________________________\n");
    printf("Codigo:%d\n",func->cod);
    printf("Nome:%s\n",func->nome);
    printf("Salario:%8.2f\n",func->salario);
    printf("________________________________________________________________________________________________________________________________________\n");
}

void func_imprimir_arq_txt(FILE * arq_txt,TFunc *func){

    if ((!func)||(!arq_txt)) return;
    //fprintf(arq_txt,"_________________________________________________________________\n");
    //fprintf(arq_txt,"Codigo:%d\n",func->cod);
    //fprintf(arq_txt,"Nome:%s\n",func->nome);
    //fprintf(arq_txt,"Salario:%8.2f\n",func->salario);
    //fprintf(arq_txt,"_________________________________________________________________\n");
    fprintf(arq_txt,"%5d %50s %8.2ff\n",func->cod,func->nome,func->salario);
    //fprintf(arq_txt,"Nome:%s\n",func->nome);
    //fprintf(arq_txt,"Salario:%8.2f\n",func->salario);
}



TFunc *func_criar() {
    TFunc *func = (TFunc *) malloc(sizeof(TFunc));
    if (func) memset(func, 0, sizeof(TFunc));
    return func;
}

TFunc *func_liberar(TFunc *func){
    if (func){
        free(func);
    }
    return NULL;
}

void func_set(TFunc *func, int cod, char *nome, double salario) {
    if (func) memset(func, 0, sizeof(TFunc));
    func->cod = cod;
    strcpy(func->nome, nome);
    func->salario = salario;
}

void func_salvar(FILE * f, TFunc *func) {
    fwrite(&func->cod, sizeof(int), 1, f);
    fwrite(func->nome, sizeof(char), sizeof(func->nome), f);
    fwrite(&func->salario, sizeof(double), 1, f);
}


void func_salvar_pos(FILE * f, TFunc *func, int pos) {
    fseek(f, (pos-1) * func_tam_reg(), SEEK_SET);
    func_salvar(f,func);
}

void func_ler(FILE *f, TFunc *func) {
    if (fread(&func->cod, sizeof(int), 1, f)<=0) {
        return;
    }
    fread(func->nome, sizeof(char), sizeof(func->nome), f);
    fread(&func->salario, sizeof(double), 1, f);
}

void func_ler_pos(FILE *f, TFunc *func, int pos){
    fseek(f, (pos-1) * func_tam_reg(), SEEK_SET);
    func_ler(f,func);
}


void func_imprimir_arq_console(FILE *f) {
    TFunc *func = func_criar();
    //le o arquivo e coloca no vetor
    rewind(f); //posiciona cursor no inicio do arquivo
    // func_ler(f,func);
    int i = 0;
    while (!feof(f)) {
        func_ler(f,func);
        func_imprimir(func);
        //func_ler(f,func);
    }
    func = func_liberar(func);
}

void func_exportar_arq_texto(FILE *arq_bin,char *nome_arq_txt){
    //Aloca um funcionario para leitura dos dados
    TFunc *func = func_criar();
    FILE * arq_txt = fopen(nome_arq_txt,"w");
    if (!arq_txt){
        printf("Falha na abertura do arquivo texto para escrita!\n");
        exit(1);
    }
    //le o arquivo e coloca no vetor
    rewind(arq_bin); //posiciona cursor no inicio do arquivo
    func_ler(arq_bin,func);
    int i = 0;
    while (!feof(arq_bin)) {
        func_imprimir_arq_txt(arq_txt,func);
        func_ler(arq_bin,func);
    }
    func = func_liberar(func);
    fclose(arq_txt);
}

/*
 * Retorna tamanho do registro de funcionario em bytes
 */
int func_tam_reg() {
    return sizeof(int)  //cod
           + sizeof(char) * 50 //nome
           + sizeof(double); //salario
}

int func_tam_arq(FILE *f) {
    fseek(f, 0, SEEK_END);
    int tam = trunc(ftell(f) / func_tam_reg());
    return tam;
}

void func_insere_arq(FILE *arq_bin, int tam){
    TFunc *func = func_criar();

    int cod = 1000;
    char nome[50];

    for (int i=1;i<=tam;i++){

        //func_set(func,cod,func_gerar_nome(nome),func_gerar_salario());
#ifdef ARQUIVO_DEPURACAO
        func_set(func,i,func_gerar_nome(nome),func_gerar_salario());
#else
        func_set(func,cod,func_gerar_nome(nome),func_gerar_salario());
#endif
        //func_imprimir(func);
        func_salvar(arq_bin, func);
        int deslocamento = (float)rand()/(float)(RAND_MAX)*100+1;
        cod += deslocamento;

    }

    func = func_liberar(func);
    fflush(arq_bin);

}




/*
 * arq é o ponteiro para o arquivo
 * tam é a quantidade de registros que o arquivo contém
 */
void func_insertion_sort_disco(FILE *arq, int tam, int (*comparar)(const void *a, const void *b)) {
    int i;
    TFunc *fi, *fj;
    fi = func_criar();
    fj = func_criar();
    //faz o insertion sort
    for (int j = 2; j <= tam; j++) {
        //posiciona o arquivo no registro j
        func_ler_pos(arq,fj,j);
#ifdef DEPURACAO
        printf("\n********* Funcionario atual: %d\n", fj->cod);
#endif
        i = j - 1;
        //posiciona o cursor no registro i
        func_ler_pos(arq,fi,i);
#ifdef DEPURACAO
        printf("fi = %d\n", fi->cod);
#endif
        while ((i > 0) && (comparar(fi,fj)>0)) {
            //posiciona o cursor no registro i+1

#ifdef DEPURACAO
            printf("Salvando funcionario %d na posicao %d\n", fi->cod, i+1);
#endif

            func_salvar_pos(arq,fi,i+1);
            i = i - 1;

            //lê registro i
            func_ler_pos(arq,fi,i);
#ifdef DEPURACAO
            printf("fi = %d; i = %d\n", fi->cod, i);
#endif
        }
        //salva registro j na posição i
        //func_salvar(arq, fj);
        func_salvar_pos(arq,fj,i+1);
    }
    //descarrega o buffer para ter certeza que dados foram gravados
    fflush(arq);
}

TPag_Func *criar_pag(int tam_pag){
    TPag_Func * pag_func = (TPag_Func*)malloc(sizeof(TPag_Func));
    pag_func->tam_pag = tam_pag;
    pag_func->regs = (TFunc*)malloc(sizeof(TFunc)*pag_func->tam_pag);
}

void ler_pag(FILE *arq,TPag_Func *pag_func){
    fread(&pag_func->regs,sizeof(TFunc),pag_func->tam_pag,arq);
}

void ler_pag_pos(FILE *arq,TPag_Func *pag_func, int pos){
    fseek(arq, (pos-1) * pag_func->tam_pag*func_tam_reg(), SEEK_SET);
    fread(&pag_func->regs,sizeof(TFunc),pag_func->tam_pag,arq);
}


void salvar_pag(FILE *arq,TPag_Func *pag_func){
    fwrite(&pag_func->regs,sizeof(TFunc),pag_func->tam_pag,arq);
}

void salvar_pag_pos(FILE *arq,TPag_Func *pag_func, int pos){
    fseek(arq, (pos-1) * pag_func->tam_pag*func_tam_reg(), SEEK_SET);
    fwrite(&pag_func->regs,sizeof(TFunc),pag_func->tam_pag,arq);
}
