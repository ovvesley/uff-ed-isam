#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct func{int cod;
                    char nome[50];
                    float salario;}TFunc;

typedef struct pag_func{int tam_pag;
                        TFunc *regs;} TPag_Func;

char * func_gerar_nome(char nome[50]);

float func_gerar_salario();

void func_imprimir(TFunc *func);

void func_imprimir_arq_txt(FILE * arq_txt,TFunc *func);

void func_imprimir_arq_console(FILE *arq_bin);

void func_exportar_arq_texto(FILE *arq_bin,char *nome_arq_txt);

TFunc *func_criar();

TFunc *func_liberar(TFunc *func);

void func_set(TFunc *func, int cod, char *nome, double salario);

void func_salvar(FILE * arq, TFunc *func);

void func_salvar_pos(FILE * arq, TFunc *func, int pos);

void func_ler(FILE *arq, TFunc *func);

void func_ler_pos(FILE *arq, TFunc *func, int pos);

void func_imprimir_arq(FILE *arq);

int func_tam_reg();

int func_tam_arq(FILE *f);

void func_insere_arq(FILE *f, int tam);

void func_insertion_sort_disco(FILE *f, int tam, int (*comparar)(const void *a, const void *b));