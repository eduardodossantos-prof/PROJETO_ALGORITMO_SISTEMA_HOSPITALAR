#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// ─── Ciclo de vida ───────────────────────────────────────────────

Fila *criar_fila(void)
{
    Fila *fila = malloc(sizeof(Fila));
    if (!fila)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar fila\n");
        return NULL;
    }

    fila->head = NULL;
    fila->tamanho = 0;

    return fila;
}

void destruir_fila(Fila *fila)
{
    if (!fila)
        return;

    // Percorre a fila liberando cada paciente
    Paciente *atual = fila->head;
    while (atual)
    {
        Paciente *proximo = atual->next;
        destruir_paciente(atual);
        atual = proximo;
    }

    free(fila);
}

// ─── Operações ───────────────────────────────────────────────────

void inserir_fila(Fila *fila, Paciente *paciente)
{
    if (!fila || !paciente)
    {
        fprintf(stderr, "[ERRO] inserir_fila recebeu ponteiro NULL\n");
        return;
    }

    // Garante que o novo paciente não carrega ligação antiga
    paciente->next = NULL;

    // Caso 1 — fila vazia ou paciente tem prioridade menor que o head
    // prioridade menor = número menor = mais urgente
    if (fila_vazia(fila) || paciente->prioridade < fila->head->prioridade)
    {
        paciente->next = fila->head;
        fila->head = paciente;
        fila->tamanho++;
        return;
    }

    // Caso 2 e 3 — percorre até achar o lugar certo
    // para quando o próximo tem prioridade MAIOR que o novo paciente
    // ou quando chega no fim da fila
    Paciente *atual = fila->head;
    while (atual->next != NULL && atual->next->prioridade <= paciente->prioridade)
    {
        atual = atual->next;
    }

    // Insere o paciente entre atual e atual->next
    paciente->next = atual->next;
    atual->next = paciente;
    fila->tamanho++;
}

Paciente *remover_fila(Fila *fila)
{
    if (!fila || fila_vazia(fila))
    {
        fprintf(stderr, "[AVISO] Tentou remover de fila vazia\n");
        return NULL;
    }

    // Remove o head — sempre o mais prioritário
    Paciente *removido = fila->head;
    fila->head = removido->next;
    removido->next = NULL; // desliga o paciente da fila
    fila->tamanho--;

    return removido;
}

int fila_vazia(const Fila *fila)
{
    return (!fila || fila->head == NULL);
}

// ─── Debug ───────────────────────────────────────────────────────

void imprimir_fila(const Fila *fila)
{
    if (!fila)
    {
        printf("[DEBUG] Fila NULL\n");
        return;
    }

    printf("=== FILA DE ATENDIMENTO (%d pacientes) ===\n", fila->tamanho);

    if (fila_vazia(fila))
    {
        printf("  (fila vazia)\n");
        printf("==========================================\n");
        return;
    }

    Paciente *atual = fila->head;
    int posicao = 1;

    while (atual)
    {
        const char *cor = "";
        if (atual->prioridade == PRIORIDADE_VERMELHA)
            cor = "[VERMELHO]";
        else if (atual->prioridade == PRIORIDADE_AMARELA)
            cor = "[AMARELO] ";
        else
            cor = "[VERDE]   ";

        printf("  %d. %s %s, %d anos\n",
               posicao++,
               cor,
               atual->nome,
               atual->idade);
        printf("     %s\n", atual->justificativa);

        atual = atual->next;
    }

    printf("==========================================\n");
}