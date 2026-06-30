#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLUE    "\033[34m"
#define YELLOW  "\033[33m"
#define RESET   "\033[0m"

typedef struct {
    int numero;
    char tipo;        // 'N' = Normal | 'P' = Preferencial
    char horario[6];  // HH:MM
} Senha;

typedef struct No {
    Senha dado;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int quantidade;
} Fila;

void inicializarFila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->quantidade = 0;
}

int filaVazia(Fila *f) {
    return f->quantidade == 0;
}

int enfileirar(Fila *f, Senha s) {
    No *novo = (No *) malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }

    novo->dado = s;
    novo->proximo = NULL;

    if (filaVazia(f)) {
        f->inicio = novo;
    } else {
        f->fim->proximo = novo;
    }

    f->fim = novo;
    f->quantidade++;

    return 1;
}

Senha desenfileirar(Fila *f) {
    No *removido = f->inicio;
    Senha s = removido->dado;

    f->inicio = removido->proximo;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }

    free(removido);
    f->quantidade--;

    return s;
}

void liberarFila(Fila *f) {
    No *atual = f->inicio;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    f->inicio = NULL;
    f->fim = NULL;
    f->quantidade = 0;
}

void exibirFila(Fila *f, char *nomeFila) {

    printf("\n========== %s ==========\n", nomeFila);

    if (filaVazia(f)) {
        printf("Fila vazia.\n");
        return;
    }

    No *atual = f->inicio;
    int i = 1;

    while (atual != NULL) {

        printf("%d) Senha: %d | Tipo: %c | Horario: %s\n",
               i,
               atual->dado.numero,
               atual->dado.tipo,
               atual->dado.horario);

        atual = atual->proximo;
        i++;
    }
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {

    Fila filaNormal;
    Fila filaPreferencial;

    inicializarFila(&filaNormal);
    inicializarFila(&filaPreferencial);

    int contadorGlobalSenhas = 1;
    int proximasNormais = 2;
    int opcao;

    do {

        printf(BLUE "\n=================================\n");
        printf(" SISTEMA DE ATENDIMENTO BANCARIO\n");
        printf("=================================\n" RESET);

        printf("1. Cadastrar Senha Normal\n");
        printf("2. Cadastrar Senha Preferencial\n");
        printf("3. Chamar Proximo Cliente\n");
        printf("4. Visualizar Filas\n");
        printf("0. Sair\n");
        printf("\nEscolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf("\nOpcao invalida!\n");
            continue;
        }

        limparBuffer();

        switch (opcao) {

        case 1: {

            Senha novaSenha;

            novaSenha.numero = contadorGlobalSenhas++;
            novaSenha.tipo = 'N';

            printf("Digite o horario (HH:MM): ");
            scanf("%5s", novaSenha.horario);
            limparBuffer();

            if (enfileirar(&filaNormal, novaSenha)) {
                printf("\nSenha Normal #%d cadastrada com sucesso!\n",
                       novaSenha.numero);
            } else {
                printf(YELLOW "\nMemoria insuficiente para cadastrar a senha!\n" RESET);
                contadorGlobalSenhas--;
            }

            break;
        }

        case 2: {

            Senha novaSenha;

            novaSenha.numero = contadorGlobalSenhas++;
            novaSenha.tipo = 'P';

            printf("Digite o horario (HH:MM): ");
            scanf("%5s", novaSenha.horario);
            limparBuffer();

            if (enfileirar(&filaPreferencial, novaSenha)) {
                printf("\nSenha Preferencial #%d cadastrada com sucesso!\n",
                       novaSenha.numero);
            } else {
                printf(YELLOW "\nMemoria insuficiente para cadastrar a senha!\n" RESET);
                contadorGlobalSenhas--;
            }

            break;
        }

        case 3: {

            if (filaVazia(&filaNormal) && filaVazia(&filaPreferencial)) {

                printf("\nNao ha clientes aguardando.\n");
                break;
            }

            Senha atendida;

            if (!filaVazia(&filaNormal) && !filaVazia(&filaPreferencial)) {

                if (proximasNormais > 0) {

                    atendida = desenfileirar(&filaNormal);
                    proximasNormais--;

                } else {

                    atendida = desenfileirar(&filaPreferencial);
                    proximasNormais = 2;
                }

            } else if (!filaVazia(&filaNormal)) {

                atendida = desenfileirar(&filaNormal);

            } else {

                atendida = desenfileirar(&filaPreferencial);
            }

            printf(BLUE "\n========== ATENDIMENTO ==========\n" RESET);
            printf("Senha: %d\n", atendida.numero);
            printf("Tipo : %s\n",
                   atendida.tipo == 'N' ? "Normal" : "Preferencial");
            printf("Horario de chegada: %s\n", atendida.horario);

            break;
        }

        case 4:

            exibirFila(&filaNormal, "FILA NORMAL");
            exibirFila(&filaPreferencial, "FILA PREFERENCIAL");

            break;

        case 0:

            printf("\nEncerrando o sistema...\n");
            liberarFila(&filaNormal);
            liberarFila(&filaPreferencial);
            break;

        default:

            printf("\nOpcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}