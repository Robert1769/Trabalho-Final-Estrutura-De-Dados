#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COR_RESET    "\033[0m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_AZUL     "\033[1;34m"
#define COR_VERMELHO "\033[1;31m"
#define COR_CIANO    "\033[1;36m"
#define COR_BRANCO   "\033[1;37m"

#define ARQUIVO_CSV "emails.csv"
#define ARQUIVO_RELATORIO "relatorio_emails.txt"

typedef struct {
    int id;
    char remetente[50];
    char assunto[100];
    char data[20];
} Email;

typedef struct No {
    Email dado;
    struct No *proximo;
} No;

typedef struct {
    No *topo;
    int quantidade;
} Pilha;

int totalLidosNaSessao = 0;

void inicializar(Pilha *p) {
    p->topo = NULL;
    p->quantidade = 0;
}

int vazia(Pilha *p) {
    return p->topo == NULL;
}

int quantidade(Pilha *p) {
    return p->quantidade;
}

int idExiste(Pilha *p, int id) {
    No *atual = p->topo;
    while (atual != NULL) {
        if (atual->dado.id == id) return 1;
        atual = atual->proximo;
    }
    return 0;
}

void receberEmail(Pilha *p, Email e) {
    if (idExiste(p, e.id)) {
        printf(COR_VERMELHO "\n[ERRO] Ja existe um e-mail com o ID %d.\n" COR_RESET, e.id);
        return;
    }

    No *novo = (No *) malloc(sizeof(No));
    if (novo == NULL) {
        printf(COR_VERMELHO "\n[ERRO] Memoria insuficiente para receber o e-mail!\n" COR_RESET);
        return;
    }

    novo->dado = e;
    novo->proximo = p->topo;
    p->topo = novo;
    p->quantidade++;

    printf(COR_VERDE "\n[OK] E-mail recebido com sucesso! (ID: %d)\n" COR_RESET, e.id);
}

Email lerEmail(Pilha *p) {
    Email e;
    e.id = -1;
    if (vazia(p)) {
        printf(COR_AMARELO "\n[AVISO] Nenhum e-mail nao lido na bandeja.\n" COR_RESET);
        return e;
    }

    No *removido = p->topo;
    e = removido->dado;
    p->topo = removido->proximo;
    free(removido);
    p->quantidade--;

    totalLidosNaSessao++;
    printf(COR_VERDE "\n[OK] E-mail lido e removido da bandeja.\n" COR_RESET);
    return e;
}

void consultarTopo(Pilha *p) {
    if (vazia(p)) {
        printf(COR_AMARELO "\n[AVISO] A bandeja esta vazia.\n" COR_RESET);
        return;
    }
    Email e = p->topo->dado;
    printf(COR_CIANO "\n=== ULTIMO E-MAIL RECEBIDO (TOPO) ===\n" COR_RESET);
    printf("  ID        : %d\n", e.id);
    printf("  Remetente : %s\n", e.remetente);
    printf("  Assunto   : %s\n", e.assunto);
    printf("  Data      : %s\n", e.data);
}

void mostrarEmails(Pilha *p) {
    if (vazia(p)) {
        printf(COR_AMARELO "\n[AVISO] Nenhum e-mail nao lido.\n" COR_RESET);
        return;
    }
    printf(COR_CIANO "\n========== BANDEJA DE E-MAILS NAO LIDOS ==========\n" COR_RESET);
    printf("  Total de e-mails: %d\n\n", quantidade(p));

    No *atual = p->topo;
    while (atual != NULL) {
        if (atual == p->topo)
            printf(COR_AMARELO "  [TOPO] " COR_RESET);
        else if (atual->proximo == NULL)
            printf(COR_AZUL "  [BASE] " COR_RESET);
        else
            printf("         ");

        printf("ID: %d | De: %s | Assunto: %s | Data: %s\n",
               atual->dado.id,
               atual->dado.remetente,
               atual->dado.assunto,
               atual->dado.data);

        atual = atual->proximo;
    }
    printf(COR_CIANO "===================================================\n" COR_RESET);
}

void marcarComoLido(Pilha *p, int id) {
    if (vazia(p)) {
        printf(COR_AMARELO "\n[AVISO] A bandeja esta vazia.\n" COR_RESET);
        return;
    }

    No *atual = p->topo;
    No *anterior = NULL;

    while (atual != NULL && atual->dado.id != id) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (atual == NULL) {
        printf(COR_VERMELHO "\n[ERRO] E-mail com ID %d nao encontrado.\n" COR_RESET, id);
        return;
    }

    if (anterior == NULL) {
                p->topo = atual->proximo;
    } else {
        anterior->proximo = atual->proximo;
    }

    printf(COR_VERDE "\n[OK] E-mail ID %d (\"%s\") marcado como lido e removido.\n" COR_RESET,
           atual->dado.id, atual->dado.assunto);

    free(atual);
    p->quantidade--;
    totalLidosNaSessao++;
}

void salvarCSV(Pilha *p) {
    FILE *arq = fopen(ARQUIVO_CSV, "w");
    if (arq == NULL) {
        printf(COR_VERMELHO "\n[ERRO] Nao foi possivel abrir o arquivo para salvar.\n" COR_RESET);
        return;
    }
    fprintf(arq, "id;remetente;assunto;data\n");

    No *atual = p->topo;
    while (atual != NULL) {
        fprintf(arq, "%d;%s;%s;%s\n",
                atual->dado.id,
                atual->dado.remetente,
                atual->dado.assunto,
                atual->dado.data);
        atual = atual->proximo;
    }

    fclose(arq);
    printf(COR_VERDE "\n[OK] Dados salvos em '%s'.\n" COR_RESET, ARQUIVO_CSV);
}

void liberarPilha(Pilha *p) {
    No *atual = p->topo;
    while (atual != NULL) {
        No *proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    p->topo = NULL;
    p->quantidade = 0;
}

void carregarCSV(Pilha *p) {
    FILE *arq = fopen(ARQUIVO_CSV, "r");
    if (arq == NULL) {
        printf(COR_AMARELO "\n[AVISO] Arquivo '%s' nao encontrado. Iniciando com bandeja vazia.\n" COR_RESET, ARQUIVO_CSV);
        return;
    }

    char linha[300];
    fgets(linha, sizeof(linha), arq);

        liberarPilha(p);

        int count = 0;
    Email emails[1000];
    int total = 0;

    while (fgets(linha, sizeof(linha), arq) && total < 1000) {
        linha[strcspn(linha, "\n")] = '\0';
        if (strlen(linha) == 0) continue;

        Email e;
        char *token = strtok(linha, ";");
        if (token == NULL) continue;
        e.id = atoi(token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(e.remetente, token, sizeof(e.remetente) - 1);
        e.remetente[sizeof(e.remetente) - 1] = '\0';

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(e.assunto, token, sizeof(e.assunto) - 1);
        e.assunto[sizeof(e.assunto) - 1] = '\0';

        token = strtok(NULL, ";");
        if (token == NULL) continue;
        strncpy(e.data, token, sizeof(e.data) - 1);
        e.data[sizeof(e.data) - 1] = '\0';

        emails[total++] = e;
    }
    fclose(arq);

        for (int i = total - 1; i >= 0; i--) {
        No *novo = (No *) malloc(sizeof(No));
        if (novo == NULL) {
            printf(COR_VERMELHO "[ERRO] Memoria insuficiente durante carregamento.\n" COR_RESET);
            break;
        }
        novo->dado = emails[i];
        novo->proximo = p->topo;
        p->topo = novo;
        p->quantidade++;
        count++;
    }

    printf(COR_VERDE "\n[OK] %d e-mail(s) carregado(s) de '%s'.\n" COR_RESET, count, ARQUIVO_CSV);
}

void limparPilha(Pilha *p) {
    if (vazia(p)) {
        printf(COR_AMARELO "\n[AVISO] A bandeja ja esta vazia.\n" COR_RESET);
        return;
    }
    printf(COR_VERMELHO "\n[ATENCAO] Isso ira remover todos os %d e-mail(s). Tem certeza? (s/n): " COR_RESET, quantidade(p));
    char resp;
    scanf(" %c", &resp);
    if (resp == 's' || resp == 'S') {
        liberarPilha(p);
        printf(COR_VERDE "[OK] Bandeja limpa com sucesso.\n" COR_RESET);
    } else {
        printf("[INFO] Operacao cancelada.\n");
    }
}

void exportarRelatorio(Pilha *p) {
    FILE *arq = fopen(ARQUIVO_RELATORIO, "w");
    if (arq == NULL) {
        printf(COR_VERMELHO "\n[ERRO] Nao foi possivel criar o relatorio.\n" COR_RESET);
        return;
    }
    fprintf(arq, "========================================\n");
    fprintf(arq, "  RELATORIO - BANDEJA DE E-MAILS\n");
    fprintf(arq, "========================================\n\n");
    fprintf(arq, "E-mails lidos nesta sessao: %d\n", totalLidosNaSessao);
    fprintf(arq, "E-mails ainda nao lidos   : %d\n\n", quantidade(p));

    if (!vazia(p)) {
        fprintf(arq, "--- E-mails pendentes ---\n\n");
        No *atual = p->topo;
        while (atual != NULL) {
            fprintf(arq, "ID        : %d\n", atual->dado.id);
            fprintf(arq, "Remetente : %s\n", atual->dado.remetente);
            fprintf(arq, "Assunto   : %s\n", atual->dado.assunto);
            fprintf(arq, "Data      : %s\n", atual->dado.data);
            fprintf(arq, "---\n");
            atual = atual->proximo;
        }
    } else {
        fprintf(arq, "Nenhum e-mail pendente.\n");
    }
    fclose(arq);
    printf(COR_VERDE "\n[OK] Relatorio exportado para '%s'.\n" COR_RESET, ARQUIVO_RELATORIO);
}

int lerInteiro(const char *msg) {
    int valor;
    char linha[50];
    while (1) {
        printf("%s", msg);
        if (fgets(linha, sizeof(linha), stdin)) {
            if (sscanf(linha, "%d", &valor) == 1) return valor;
        }
        printf(COR_VERMELHO "[ERRO] Entrada invalida. Digite um numero inteiro.\n" COR_RESET);
    }
}

void lerString(const char *msg, char *destino, int tamanho) {
    while (1) {
        printf("%s", msg);
        if (fgets(destino, tamanho, stdin)) {
            destino[strcspn(destino, "\n")] = '\0';
            if (strlen(destino) > 0) return;
        }
        printf(COR_VERMELHO "[ERRO] Campo nao pode ser vazio.\n" COR_RESET);
    }
}

Email coletarEmail(Pilha *p) {
    Email e;
    while (1) {
        e.id = lerInteiro("  ID (numero unico): ");
        if (e.id <= 0) {
            printf(COR_VERMELHO "[ERRO] ID deve ser maior que zero.\n" COR_RESET);
            continue;
        }
        if (idExiste(p, e.id)) {
            printf(COR_VERMELHO "[ERRO] ID %d ja esta em uso.\n" COR_RESET, e.id);
            continue;
        }
        break;
    }
    lerString("  Remetente        : ", e.remetente, sizeof(e.remetente));
    lerString("  Assunto          : ", e.assunto, sizeof(e.assunto));
    lerString("  Data (dd/mm/aaaa): ", e.data, sizeof(e.data));
    return e;
}

void exibirEmail(Email e) {
    printf(COR_BRANCO "  ID        : " COR_RESET "%d\n", e.id);
    printf(COR_BRANCO "  Remetente : " COR_RESET "%s\n", e.remetente);
    printf(COR_BRANCO "  Assunto   : " COR_RESET "%s\n", e.assunto);
    printf(COR_BRANCO "  Data      : " COR_RESET "%s\n", e.data);
}

void exibirCabecalho() {
    printf(COR_AZUL "\n");
    printf("  ╔══════════════════════════════════════════╗\n");
    printf("  ║       BANDEJA DE E-MAILS NAO LIDOS       ║\n");
    printf("  ║   Estruturas de Dados - Pilha (Lista     ║\n");
    printf("  ║   Encadeada / LIFO)                      ║\n");
    printf("  ╚══════════════════════════════════════════╝\n");
    printf(COR_RESET "\n");
}

void exibirMenu(Pilha *p) {
    printf(COR_CIANO "\n  ┌─────────────────────────────────────────┐\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET "            MENU PRINCIPAL               " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " E-mails na bandeja: %-4d                " COR_CIANO "│\n" COR_RESET, quantidade(p));
    printf(COR_CIANO "  │" COR_RESET " Lidos nesta sessao: %-4d                " COR_CIANO "│\n" COR_RESET, totalLidosNaSessao);
    printf(COR_CIANO "  ├─────────────────────────────────────────┤\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 1. Receber e-mail (push)                " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 2. Ler e-mail do topo (pop)             " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 3. Ver ultimo e-mail (peek)             " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 4. Listar todos os e-mails              " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 5. Marcar e-mail especifico como lido   " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  ├─────────────────────────────────────────┤\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 6. Salvar dados em CSV                  " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 7. Carregar dados do CSV                " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  ├─────────────────────────────────────────┤\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 8. Exportar relatorio (.txt)            " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 9. Limpar toda a bandeja                " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  │" COR_RESET " 0. Sair                                 " COR_CIANO "│\n" COR_RESET);
    printf(COR_CIANO "  └─────────────────────────────────────────┘\n" COR_RESET);
    printf("  Opcao: ");
}

int main() {
    Pilha bandeja;
    inicializar(&bandeja);

    exibirCabecalho();
    printf("[INFO] Verificando dados salvos...\n");
    carregarCSV(&bandeja);

    int opcao;
    do {
        exibirMenu(&bandeja);

        char linha[10];
        if (fgets(linha, sizeof(linha), stdin)) {
            sscanf(linha, "%d", &opcao);
        } else {
            opcao = -1;
        }

        switch (opcao) {
            case 1: {
                printf(COR_AZUL "\n--- NOVO E-MAIL ---\n" COR_RESET);
                Email e = coletarEmail(&bandeja);
                receberEmail(&bandeja, e);
                break;
            }
            case 2: {
                Email e = lerEmail(&bandeja);
                if (e.id != -1) {
                    printf(COR_CIANO "\n--- E-MAIL LIDO ---\n" COR_RESET);
                    exibirEmail(e);
                }
                break;
            }
            case 3:
                consultarTopo(&bandeja);
                break;
            case 4:
                mostrarEmails(&bandeja);
                break;
            case 5: {
                if (vazia(&bandeja)) {
                    printf(COR_AMARELO "\n[AVISO] A bandeja esta vazia.\n" COR_RESET);
                    break;
                }
                mostrarEmails(&bandeja);
                int id = lerInteiro("\n  Informe o ID do e-mail a marcar como lido: ");
                marcarComoLido(&bandeja, id);
                break;
            }
            case 6:
                salvarCSV(&bandeja);
                break;
            case 7:
                carregarCSV(&bandeja);
                break;
            case 8:
                exportarRelatorio(&bandeja);
                break;
            case 9:
                limparPilha(&bandeja);
                break;
            case 0:
                printf(COR_AMARELO "\n[INFO] Salvando dados antes de sair...\n" COR_RESET);
                salvarCSV(&bandeja);
                liberarPilha(&bandeja);
                printf(COR_VERDE "Ate logo!\n\n" COR_RESET);
                break;
            default:
                printf(COR_VERMELHO "\n[ERRO] Opcao invalida. Tente novamente.\n" COR_RESET);
        }

    } while (opcao != 0);

    return 0;
}