#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "playlist.csv"

/* ═══════════════════════════════════════════════════
   CORES E ESTILOS ANSI
═══════════════════════════════════════════════════ */
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define DIM         "\033[2m"

// Cores de texto
#define PRETO       "\033[30m"
#define VERMELHO    "\033[31m"
#define VERDE       "\033[32m"
#define AMARELO     "\033[33m"
#define AZUL        "\033[34m"
#define CIANO       "\033[36m"
#define BRANCO      "\033[37m"

// Cores de fundo
#define BG_PRETO    "\033[40m"
#define BG_AZUL     "\033[44m"
#define BG_CIANO    "\033[46m"

// Limpar tela
#define LIMPAR()    printf("\033[2J\033[H")
// Ocultar/mostrar cursor
#define CURSOR_OFF  printf("\033[?25l")
#define CURSOR_ON   printf("\033[?25h")

/* ═══════════════════════════════════════════════════
   ESTRUTURA — LISTA ENCADEADA
═══════════════════════════════════════════════════ */
typedef struct Musica {
    int id;
    char titulo[50];
    char artista[50];
    int duracao;
    char genero[30];
    struct Musica *prox;
} Musica;

Musica *cabeca = NULL;
int quantidade = 0;

/* ═══════════════════════════════════════════════════
   PROTÓTIPOS
═══════════════════════════════════════════════════ */
void inserir();
void buscar();
void editar();
void excluir();
void listar();
void salvarArquivo();
void carregarArquivo();
Musica *buscarNo(int id);
void limparBuffer();
void limparTela();
void cabecalhoPrincipal();
void pausar();
void printDivisor(const char *c, int n);
void msgSucesso(const char *msg);
void msgErro(const char *msg);
void msgInfo(const char *msg);
void liberarLista();

/* ═══════════════════════════════════════════════════
   UTILITÁRIOS DE INTERFACE
═══════════════════════════════════════════════════ */
void limparTela() {
    LIMPAR();
}

void printDivisor(const char *c, int n) {
    printf(DIM CIANO);
    for (int i = 0; i < n; i++) printf("%s", c);
    printf(RESET "\n");
}

void msgSucesso(const char *msg) {
    printf("\n  " BOLD VERDE "✔  %s" RESET "\n", msg);
}

void msgErro(const char *msg) {
    printf("\n  " BOLD VERMELHO "✘  %s" RESET "\n", msg);
}

void msgInfo(const char *msg) {
    printf("\n  " BOLD AMARELO "ℹ  %s" RESET "\n", msg);
}

void pausar() {
    printf("\n  " DIM "Pressione ENTER para continuar..." RESET);
    limparBuffer();
}

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cabecalhoPrincipal() {
    limparTela();
    printf("\n");
    printf("  " BG_CIANO BRANCO BOLD "                                          " RESET "\n");
    printf("  " BG_CIANO BRANCO BOLD "      ♪  GERENCIADOR DE PLAYLIST  ♪      " RESET "\n");
    printf("  " BG_CIANO BRANCO BOLD "                                          " RESET "\n");
    printf("\n");
    printf("  " DIM CIANO "  %d música(s) na playlist" RESET "\n", quantidade);
    printDivisor("─", 46);
}

/* ═══════════════════════════════════════════════════
   OPERAÇÕES COM LISTA ENCADEADA
═══════════════════════════════════════════════════ */
Musica *buscarNo(int id) {
    Musica *atual = cabeca;
    while (atual != NULL) {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

void liberarLista() {
    Musica *atual = cabeca;
    while (atual != NULL) {
        Musica *prox = atual->prox;
        free(atual);
        atual = prox;
    }
    cabeca = NULL;
    quantidade = 0;
}

/* ═══════════════════════════════════════════════════
   MENU PRINCIPAL
═══════════════════════════════════════════════════ */
int main() {
    int opcao;
    carregarArquivo();

    do {
        cabecalhoPrincipal();

        printf("  " BOLD CIANO "  O que você quer fazer hoje?\n\n" RESET);
        printf("  " AZUL "  [1]" RESET "  ♪  Adicionar nova música\n");
        printf("  " AZUL "  [2]" RESET "  🔍 Buscar música por ID\n");
        printf("  " AZUL "  [3]" RESET "  ✏  Editar música\n");
        printf("  " AZUL "  [4]" RESET "  🗑  Remover música\n");
        printf("  " AZUL "  [5]" RESET "  📋 Ver toda a playlist\n");
        printf("  " AZUL "  [6]" RESET "  💾 Salvar em CSV\n");
        printf("  " VERMELHO "  [7]" RESET "  ✖  Sair\n");

        printDivisor("─", 46);
        printf("  " BOLD "  Escolha: " RESET);
        scanf("%d", &opcao);
        limparBuffer();

        limparTela();

        switch (opcao) {
            case 1: inserir();      break;
            case 2: buscar();       break;
            case 3: editar();       break;
            case 4: excluir();      break;
            case 5: listar();       break;
            case 6:
                salvarArquivo();
                msgSucesso("Playlist salva no arquivo CSV com sucesso!");
                pausar();
                break;
            case 7:
                salvarArquivo();
                limparTela();
                printf("\n  " BOLD CIANO "♪  Até a próxima! Sua playlist foi salva automaticamente." RESET "\n\n");
                liberarLista();
                return 0;
            default:
                msgErro("Opção inválida. Escolha um número entre 1 e 7.");
                pausar();
        }

    } while (1);
}

/* ═══════════════════════════════════════════════════
   INSERIR
═══════════════════════════════════════════════════ */
void inserir() {
    printf("\n  " BOLD BG_AZUL BRANCO "   ♪  ADICIONAR MÚSICA   " RESET "\n\n");

    Musica *nova = (Musica *) malloc(sizeof(Musica));
    if (!nova) {
        msgErro("Erro de memória ao alocar música.");
        pausar();
        return;
    }
    nova->prox = NULL;

    printf("  " AMARELO "ID da música: " RESET);
    scanf("%d", &nova->id);
    limparBuffer();

    if (buscarNo(nova->id) != NULL) {
        free(nova);
        msgErro("Já existe uma música com esse ID. Tente outro número.");
        pausar();
        return;
    }

    printf("  " AMARELO "Título:       " RESET);
    fgets(nova->titulo, 50, stdin);
    nova->titulo[strcspn(nova->titulo, "\n")] = '\0';

    printf("  " AMARELO "Artista:      " RESET);
    fgets(nova->artista, 50, stdin);
    nova->artista[strcspn(nova->artista, "\n")] = '\0';

    printf("  " AMARELO "Duração (seg):" RESET " ");
    scanf("%d", &nova->duracao);
    limparBuffer();

    printf("  " AMARELO "Gênero:       " RESET);
    fgets(nova->genero, 30, stdin);
    nova->genero[strcspn(nova->genero, "\n")] = '\0';

    // Insere no início da lista
    nova->prox = cabeca;
    cabeca = nova;
    quantidade++;

    msgSucesso("Música adicionada com sucesso à sua playlist!");
    pausar();
}

/* ═══════════════════════════════════════════════════
   BUSCAR
═══════════════════════════════════════════════════ */
void buscar() {
    printf("\n  " BOLD BG_AZUL BRANCO "   🔍  BUSCAR MÚSICA   " RESET "\n\n");

    int id;
    printf("  " AMARELO "Digite o ID da música: " RESET);
    scanf("%d", &id);
    limparBuffer();

    Musica *m = buscarNo(id);

    if (m == NULL) {
        msgErro("Nenhuma música encontrada com esse ID.");
        pausar();
        return;
    }

    printf("\n");
    printDivisor("─", 46);
    printf("  " BOLD CIANO "♪  Música encontrada!\n" RESET);
    printDivisor("─", 46);
    printf("  " CIANO "ID:      " RESET BOLD "%d\n" RESET, m->id);
    printf("  " CIANO "Título:  " RESET "%s\n", m->titulo);
    printf("  " CIANO "Artista: " RESET "%s\n", m->artista);
    printf("  " CIANO "Duração: " RESET "%d:%02d min\n", m->duracao / 60, m->duracao % 60);
    printf("  " CIANO "Gênero:  " RESET "%s\n", m->genero);
    printDivisor("─", 46);

    pausar();
}

/* ═══════════════════════════════════════════════════
   EDITAR
═══════════════════════════════════════════════════ */
void editar() {
    printf("\n  " BOLD BG_AZUL BRANCO "   ✏  EDITAR MÚSICA   " RESET "\n\n");

    int id;
    printf("  " AMARELO "ID da música a editar: " RESET);
    scanf("%d", &id);
    limparBuffer();

    Musica *m = buscarNo(id);

    if (m == NULL) {
        msgErro("Nenhuma música encontrada com esse ID.");
        pausar();
        return;
    }

    msgInfo("Deixe em branco e pressione ENTER para manter o valor atual.\n");

    char tmp[50];

    printf("  " AMARELO "Título   " DIM "[%s]: " RESET, m->titulo);
    fgets(tmp, 50, stdin);
    tmp[strcspn(tmp, "\n")] = '\0';
    if (strlen(tmp) > 0) strncpy(m->titulo, tmp, 49);

    printf("  " AMARELO "Artista  " DIM "[%s]: " RESET, m->artista);
    fgets(tmp, 50, stdin);
    tmp[strcspn(tmp, "\n")] = '\0';
    if (strlen(tmp) > 0) strncpy(m->artista, tmp, 49);

    printf("  " AMARELO "Duração  " DIM "[%d seg] (0 = manter): " RESET, m->duracao);
    int dur;
    scanf("%d", &dur);
    limparBuffer();
    if (dur > 0) m->duracao = dur;

    printf("  " AMARELO "Gênero   " DIM "[%s]: " RESET, m->genero);
    fgets(tmp, 30, stdin);
    tmp[strcspn(tmp, "\n")] = '\0';
    if (strlen(tmp) > 0) strncpy(m->genero, tmp, 29);

    msgSucesso("Música atualizada com sucesso!");
    pausar();
}

/* ═══════════════════════════════════════════════════
   EXCLUIR
═══════════════════════════════════════════════════ */
void excluir() {
    printf("\n  " BOLD BG_AZUL BRANCO "   🗑  REMOVER MÚSICA   " RESET "\n\n");

    int id;
    printf("  " AMARELO "ID da música a remover: " RESET);
    scanf("%d", &id);
    limparBuffer();

    if (cabeca == NULL) {
        msgErro("A playlist já está vazia!");
        pausar();
        return;
    }

    Musica *atual = cabeca, *anterior = NULL;
    while (atual != NULL && atual->id != id) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        msgErro("Nenhuma música encontrada com esse ID.");
        pausar();
        return;
    }

    printf("  " DIM "Remover \"" BOLD "%s" RESET DIM "\" de " BOLD "%s" RESET DIM "? [s/N]: " RESET,
           atual->titulo, atual->artista);
    char conf;
    scanf("%c", &conf);
    limparBuffer();

    if (conf != 's' && conf != 'S') {
        msgInfo("Remoção cancelada. Nenhuma alteração foi feita.");
        pausar();
        return;
    }

    if (anterior == NULL) {
        cabeca = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    free(atual);
    quantidade--;

    msgSucesso("Música removida da playlist!");
    pausar();
}

/* ═══════════════════════════════════════════════════
   LISTAR
═══════════════════════════════════════════════════ */
void listar() {
    printf("\n  " BOLD BG_AZUL BRANCO "   📋  SUA PLAYLIST   " RESET "\n\n");

    if (cabeca == NULL) {
        msgInfo("Sua playlist está vazia. Que tal adicionar algumas músicas? ♪");
        pausar();
        return;
    }

    printf("  " BOLD CIANO "%-5s  %-24s  %-18s  %-8s  %s\n" RESET,
           "ID", "TÍTULO", "ARTISTA", "DURAÇÃO", "GÊNERO");
    printDivisor("─", 76);

    Musica *atual = cabeca;
    int i = 0;
    while (atual != NULL) {
        // Alterna cor das linhas
        if (i % 2 == 0)
            printf(RESET);
        else
            printf(DIM);

        printf("  %-5d  %-24s  %-18s  %d:%02d     %-15s" RESET "\n",
               atual->id,
               atual->titulo,
               atual->artista,
               atual->duracao / 60,
               atual->duracao % 60,
               atual->genero);

        atual = atual->prox;
        i++;
    }

    printDivisor("─", 76);
    printf("  " BOLD VERDE "  Total: %d música(s)" RESET "\n", quantidade);
    pausar();
}

/* ═══════════════════════════════════════════════════
   SALVAR / CARREGAR CSV
═══════════════════════════════════════════════════ */
void salvarArquivo() {
    FILE *arquivo = fopen(ARQUIVO, "w");
    if (!arquivo) {
        msgErro("Não foi possível salvar o arquivo. Verifique as permissões.");
        return;
    }

    fprintf(arquivo, "id;titulo;artista;duracao;genero\n");

    Musica *atual = cabeca;
    while (atual != NULL) {
        fprintf(arquivo, "%d;%s;%s;%d;%s\n",
                atual->id, atual->titulo, atual->artista,
                atual->duracao, atual->genero);
        atual = atual->prox;
    }

    fclose(arquivo);
}

void carregarArquivo() {
    FILE *arquivo = fopen(ARQUIVO, "r");
    if (!arquivo) return;

    char linha[256];
    fgets(linha, sizeof(linha), arquivo); // pula cabeçalho

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        Musica *nova = (Musica *) malloc(sizeof(Musica));
        if (!nova) break;

        char *token;

        token = strtok(linha, ";");
        if (!token) { free(nova); continue; }
        nova->id = atoi(token);

        token = strtok(NULL, ";");
        if (!token) { free(nova); continue; }
        strncpy(nova->titulo, token, 49); nova->titulo[49] = '\0';

        token = strtok(NULL, ";");
        if (!token) { free(nova); continue; }
        strncpy(nova->artista, token, 49); nova->artista[49] = '\0';

        token = strtok(NULL, ";");
        if (!token) { free(nova); continue; }
        nova->duracao = atoi(token);

        token = strtok(NULL, "\n");
        if (!token) { free(nova); continue; }
        strncpy(nova->genero, token, 29); nova->genero[29] = '\0';

        if (buscarNo(nova->id) != NULL) {
            free(nova);
            continue;
        }

        // Insere no início
        nova->prox = cabeca;
        cabeca = nova;
        quantidade++;
    }

    fclose(arquivo);
}