#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tFuncionario{
    int id;
    int matricula;
    char nome_completo[256];
    float salario;
}TFuncionario;

char CHAR_VIRGULA = ',';

char CHAR_NEW_LINE = '\n';

void read_file(char *filename){
    char current_char;
    FILE *file = fopen(filename, "r");

    if (! file )
    {
        printf("oops, file can't be read\n");
        return;
    }
    int cont = 0;
    int cont_char_matricula =0;


//    while ((file = fgetc(file)) != '\n' && file != EOF)
//    {
//        current_char = (char )fgetc(file);
//        int id;
//        int matricula;
//        float salario;
//        char nome_completo[256];
//
//        if (cont == 0){
//            id = (int) (current_char);
//        }
//        if (cont == 1){
//            matricula = fprintf('')
//        }
//
//        if(current_char == CHAR_VIRGULA)
//        {
//            cont+=1;
//        }
//
//        if(current_char == CHAR_NEW_LINE)
//        {
//            printf("\n\n -- \n\n");
//            continue;
//        }
//
//        printf("%c", current_char);

//        printf("");
//    }
//    fclose(file);

    char line[256];
    while((fgets (line, 256, file)) != NULL) {
        int start_index = 0;
        int end_index = 0;
        for (int index=0; index<256; index ++){
            printf("%c", line[index]);

            if(line[index] == CHAR_VIRGULA){
                end_index = index;
            }

            if(line[index] == CHAR_NEW_LINE)break;
        }

    }

}

TFuncionario* create_funcionario(int id, int matricula, char *nome_completo, float salario){
    TFuncionario *funcionario = (TFuncionario*) malloc(sizeof(TFuncionario));
    funcionario->id = id;
    funcionario->matricula = matricula;
     strcpy(funcionario->nome_completo, nome_completo);
    funcionario->salario = salario;
    return funcionario;
}

int main() {

    typedef struct reg_TIndiceReg {
        int *index;
    }TIndiceReg;

    typedef struct no_ISAM{
        int tipo;
        int n;
        TIndiceReg *chaves;
        TIndiceReg *filhos;
        TIndiceReg prox;
    }TNo_ISAM;

    read_file("file.csv");





    return 0;
}
