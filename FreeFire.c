/*
  Torre de Resgate - Sistema de priorização e montagem de componentes
  - Cadastro de até 20 componentes (nome, tipo, prioridade)
  - Bubble sort por nome (conta comparações + tempo)
  - Insertion sort por tipo (conta comparações + tempo)
  - Selection sort por prioridade (conta comparações + tempo)
  - Busca binária por nome (após ordenação por nome) com contagem de comparações
  - Menu interativo e exibição formatada dos componentes

  Autor: Enigma Studios (exemplo)
  Compilação: gcc -std=c11 -Wall -Wextra -o torre_resgate torre_resgate.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_COMPONENTES 20
#define MAX_NOME 30
#define MAX_TIPO 20

/* ------------------- Definição da estrutura ------------------- */
typedef struct {
    char nome[MAX_NOME];
    char tipo[MAX_TIPO];
    int prioridade; // 1..10
} Componente;

/* ------------------- Prototipos de funções -------------------- */

/* Ordenações - cada função recebe o vetor, seu tamanho e ponteiro para contador de comparações */
void bubbleSortNome(Componente arr[], int n, long *comparacoes);
void insertionSortTipo(Componente arr[], int n, long *comparacoes);
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes);

/* Busca binária por nome (retorna índice ou -1). Comparações contadas em comparacoes. */
int buscaBinariaPorNome(Componente arr[], int n, const char chave[], long *comparacoes);

/* Mostra componentes formatados */
void mostrarComponentes(Componente arr[], int n);

/* Leitura segura de linha (fgets wrapper que remove \n) */
void lerLinha(char *buffer, int tamanho);

/* Medidor de tempo: executa 'algoritmo' que segue a assinatura (Componente*,int,long*)
   Retorna o tempo em segundos e preenche comparacoes. */
double medirTempo(void (*algoritmo)(Componente*, int, long*), Componente arr[], int n, long *comparacoes);

/* Função utilitária: troca dois componentes */
void trocar(Componente *a, Componente *b);

/* Função que padroniza caixa (opcional) - para busca case-insensitive se desejado */
void str_tolower_inplace(char *s);

/* ------------------- Implementações ---------------------------- */

/* Ler linha com fgets e remover newline final */
void lerLinha(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin) == NULL) {
        // Entrada falhou: coloca string vazia
        buffer[0] = '\0';
        return;
    }
    // Remove \n ou \r\n do fim
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
        buffer[len-1] = '\0';
        len--;
    }
}

/* Troca dois componentes */
void trocar(Componente *a, Componente *b) {
    Componente tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Converte string para minúsculas (inplace) */
void str_tolower_inplace(char *s) {
    for (; *s; ++s) *s = (char) tolower((unsigned char)*s);
}

/* ---------------- Bubble Sort por nome (string) ------------------ */
/* Conta cada strcmp efetuado como 1 comparação. */
void bubbleSortNome(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    if (n < 2) return;
    int i, j;
    for (i = 0; i < n - 1; ++i) {
        int trocou = 0;
        for (j = 0; j < n - 1 - i; ++j) {
            (*comparacoes)++;
            if (strcmp(arr[j].nome, arr[j+1].nome) > 0) {
                trocar(&arr[j], &arr[j+1]);
                trocou = 1;
            }
        }
        if (!trocou) break; // otimização
    }
}

/* --------------- Insertion Sort por tipo (string) --------------- */
/* Conta comparações realizadas entre tipos (strcmp). */
void insertionSortTipo(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    int i;
    for (i = 1; i < n; ++i) {
        Componente key = arr[i];
        int j = i - 1;
        // comparamos key.tipo com arr[j].tipo
        while (j >= 0) {
            (*comparacoes)++;
            if (strcmp(arr[j].tipo, key.tipo) > 0) {
                arr[j+1] = arr[j];
                j--;
            } else {
                break;
            }
        }
        arr[j+1] = key;
    }
}

/* ------------- Selection Sort por prioridade (int) -------------- */
/* Conta cada comparação entre prioridades como 1. */
void selectionSortPrioridade(Componente arr[], int n, long *comparacoes) {
    *comparacoes = 0;
    int i, j;
    for (i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (j = i + 1; j < n; ++j) {
            (*comparacoes)++;
            if (arr[j].prioridade < arr[min_idx].prioridade) {
                min_idx = j;
            }
        }
        if (min_idx != i) trocar(&arr[i], &arr[min_idx]);
    }
}

/* ------------------ Busca binária por nome ----------------------- */
/* Pré-condição: arr[] está ordenado por nome (lexicograficamente).
   Realiza busca binária padrão. Conta cada strcmp entre chave e meio como 1 comparação.
   Retorna índice ou -1 se não encontrado. */
int buscaBinariaPorNome(Componente arr[], int n, const char chave[], long *comparacoes) {
    int low = 0, high = n - 1;
    *comparacoes = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        (*comparacoes)++;
        int cmp = strcmp(arr[mid].nome, chave);
        if (cmp == 0) return mid;
        else if (cmp < 0) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

/* ----------------- Medidor de tempo (wrapper) -------------------- */
/* Executa o algoritmo passado, mede tempo com clock() e retorna o tempo em segundos.
   O algoritmo deve ter assinatura: void algoritmo(Componente*, int, long*)
   e atualizar o contador 'comparacoes'. */
double medirTempo(void (*algoritmo)(Componente*, int, long*), Componente arr[], int n, long *comparacoes) {
    clock_t inicio = clock();
    algoritmo(arr, n, comparacoes);
    clock_t fim = clock();
    return ((double)(fim - inicio)) / CLOCKS_PER_SEC;
}

/* ------------------ Exibição dos componentes -------------------- */
void mostrarComponentes(Componente arr[], int n) {
    if (n == 0) {
        printf("\n[Sem componentes cadastrados]\n");
        return;
    }
    printf("\nLista de componentes (total: %d)\n", n);
    printf("---------------------------------------------------------\n");
    printf("| %-2s | %-28s | %-10s | %s\n", "ID", "NOME", "TIPO", "PRIOR.");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < n; ++i) {
        printf("| %2d | %-28s | %-10s | %4d\n", i, arr[i].nome, arr[i].tipo, arr[i].prioridade);
    }
    printf("---------------------------------------------------------\n");
}

/* -------------------------- MAIN --------------------------------- */
int main(void) {
    Componente *componentes = (Componente*) malloc(MAX_COMPONENTES * sizeof(Componente));
    if (!componentes) {
        fprintf(stderr, "Erro: falha ao alocar memória.\n");
        return 1;
    }
    int n = 0; // número atual de componentes cadastrados

    int ordenado_por_nome = 0; // flag para permitir busca binária
    int opcao;

    printf("=== Torre de Resgate - Organizador de Componentes ===\n");

    do {
        printf("\nMenu:\n");
        printf("1 - Cadastrar componentes (até %d)\n", MAX_COMPONENTES);
        printf("2 - Mostrar componentes\n");
        printf("3 - Ordenar por NOME (Bubble Sort)\n");
        printf("4 - Ordenar por TIPO (Insertion Sort)\n");
        printf("5 - Ordenar por PRIORIDADE (Selection Sort)\n");
        printf("6 - Buscar componente por NOME (Busca binaria - requer ordenado por nome)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf("%d", &opcao) != 1) {
            // limpar entrada
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        // consumir newline pendente
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        if (opcao == 1) {
            printf("\nQuantos componentes deseja cadastrar (max %d)? ", MAX_COMPONENTES);
            int qtd;
            if (scanf("%d", &qtd) != 1) {
                int c; while ((c = getchar()) != '\n' && c != EOF) {}
                printf("Entrada invalida.\n");
                continue;
            }
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            if (qtd < 1 || qtd > MAX_COMPONENTES) {
                printf("Quantidade invalida. Deve ser entre 1 e %d.\n", MAX_COMPONENTES);
                continue;
            }
            n = qtd;
            for (int i = 0; i < n; ++i) {
                printf("\nComponente %d:\n", i);
                printf("Nome: ");
                lerLinha(componentes[i].nome, MAX_NOME);
                // garantir string não vazia
                if (strlen(componentes[i].nome) == 0) {
                    strncpy(componentes[i].nome, "Componente sem nome", MAX_NOME-1);
                    componentes[i].nome[MAX_NOME-1] = '\0';
                }
                printf("Tipo: ");
                lerLinha(componentes[i].tipo, MAX_TIPO);
                if (strlen(componentes[i].tipo) == 0) {
                    strncpy(componentes[i].tipo, "geral", MAX_TIPO-1);
                    componentes[i].tipo[MAX_TIPO-1] = '\0';
                }
                printf("Prioridade (1..10): ");
                int p;
                if (scanf("%d", &p) != 1) {
                    int d; while ((d = getchar()) != '\n' && d != EOF) {}
                    p = 5; // valor default
                } else {
                    int d; while ((d = getchar()) != '\n' && d != EOF) {}
                }
                if (p < 1) p = 1;
                if (p > 10) p = 10;
                componentes[i].prioridade = p;
            }
            ordenado_por_nome = 0; // novo cadastro invalida ordenacao
            printf("\nCadastro concluido com %d componentes.\n", n);
        }
        else if (opcao == 2) {
            mostrarComponentes(componentes, n);
        }
        else if (opcao == 3) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
            } else {
                long comparacoes = 0;
                // medir tempo com wrapper
                double tempo = medirTempo(bubbleSortNome, componentes, n, &comparacoes);
                printf("\nOrdenacao por NOME (Bubble Sort) concluida.\n");
                printf("Comparacoes realizadas: %ld\n", comparacoes);
                printf("Tempo gasto (s): %.6f\n", tempo);
                ordenado_por_nome = 1;
                mostrarComponentes(componentes, n);
            }
        }
        else if (opcao == 4) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
            } else {
                long comparacoes = 0;
                double tempo = medirTempo(insertionSortTipo, componentes, n, &comparacoes);
                printf("\nOrdenacao por TIPO (Insertion Sort) concluida.\n");
                printf("Comparacoes realizadas: %ld\n", comparacoes);
                printf("Tempo gasto (s): %.6f\n", tempo);
                // ordenar por tipo invalida ordenacao por nome (para busca binaria)
                ordenado_por_nome = 0;
                mostrarComponentes(componentes, n);
            }
        }
        else if (opcao == 5) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
            } else {
                long comparacoes = 0;
                double tempo = medirTempo(selectionSortPrioridade, componentes, n, &comparacoes);
                printf("\nOrdenacao por PRIORIDADE (Selection Sort) concluida.\n");
                printf("Comparacoes realizadas: %ld\n", comparacoes);
                printf("Tempo gasto (s): %.6f\n", tempo);
                ordenado_por_nome = 0;
                mostrarComponentes(componentes, n);
            }
        }
        else if (opcao == 6) {
            if (n == 0) {
                printf("Nenhum componente cadastrado.\n");
                continue;
            }
            if (!ordenado_por_nome) {
                printf("A busca binaria exige que o vetor esteja ordenado por NOME (opcao 3).\n");
                printf("Deseja ordenar por NOME agora? (1-sim / 0-nao): ");
                int resp;
                if (scanf("%d", &resp) != 1) {
                    int d; while ((d = getchar()) != '\n' && d != EOF) {}
                    resp = 0;
                } else {
                    int d; while ((d = getchar()) != '\n' && d != EOF) {}
                }
                if (resp == 1) {
                    long compAux = 0;
                    double tAux = medirTempo(bubbleSortNome, componentes, n, &compAux);
                    printf("Ordenacao por NOME concluida (comparacoes=%ld, tempo=%.6f s)\n", compAux, tAux);
                    ordenado_por_nome = 1;
                } else {
                    printf("Operacao de busca abortada.\n");
                    continue;
                }
            }
            char chave[MAX_NOME];
            printf("Digite o NOME do componente a buscar (exato): ");
            lerLinha(chave, MAX_NOME);
            if (strlen(chave) == 0) {
                printf("Nome vazio. Abortando busca.\n");
                continue;
            }
            long comparacoes_busca = 0;
            int idx = buscaBinariaPorNome(componentes, n, chave, &comparacoes_busca);
            printf("Comparacoes na busca binaria: %ld\n", comparacoes_busca);
            if (idx >= 0) {
                printf("Componente encontrado no indice %d:\n", idx);
                printf("  Nome: %s\n  Tipo: %s\n  Prioridade: %d\n",
                       componentes[idx].nome, componentes[idx].tipo, componentes[idx].prioridade);
            } else {
                printf("Componente NAO encontrado.\n");
            }
        }
        else if (opcao == 0) {
            printf("Encerrando o organizador de componentes.\n");
        }
        else {
            printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 0);

    free(componentes);
    return 0;
}
