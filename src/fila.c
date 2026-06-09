#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

// ─── Ciclo de vida ───────────────────────────────────────────────

Fila *criar_fila(void) {
    Fila *fila = malloc(sizeof(Fila));
    if (!fila) {
        fprintf(stderr, "[ERRO] Falha ao alocar fila\n");
        return NULL;
    }

    fila->head    = NULL;
    fila->tamanho = 0;

    return fila;
}

void destruir_fila(Fila *fila) {
    if (!fila) return;

    // Percorre liberando cada NoFila e o Paciente dentro dele
    NoFila *atual = fila->head;
    while (atual) {
        NoFila *prox = atual->prox;
        destruir_paciente(atual->paciente); // libera o Paciente
        free(atual);                        // libera o NoFila
        atual = prox;
    }

    free(fila);
}

// ─── Operações ───────────────────────────────────────────────────

void inserir_fila(Fila *fila, Paciente *paciente) {
    if (!fila || !paciente) {
        fprintf(stderr, "[ERRO] inserir_fila recebeu NULL\n");
        return;
    }

    // Cria o nó auxiliar que vai encadear o paciente na fila
    NoFila *novo = malloc(sizeof(NoFila));
    if (!novo) {
        fprintf(stderr, "[ERRO] Falha ao alocar NoFila\n");
        return;
    }

    novo->paciente = paciente;
    novo->prox  = NULL;

    // Caso 1 — fila vazia ou paciente mais urgente que o head
    if (fila_vazia(fila) || paciente->prioridade < fila->head->paciente->prioridade) {
        novo->prox = fila->head;
        fila->head    = novo;
        fila->tamanho++;
        return;
    }

    // Caso 2 e 3 — percorre pelos NoFila até achar o lugar certo
    // para quando o próximo tem prioridade MAIOR que o novo paciente
    NoFila *atual = fila->head;
    while (atual->prox != NULL &&
           atual->prox->paciente->prioridade <= paciente->prioridade) {
        atual = atual->prox;
    }

    // Insere o novo nó entre atual e atual->proximo
    novo->prox  = atual->prox;
    atual->prox = novo;
    fila->tamanho++;
}

Paciente *remover_fila(Fila *fila) {
    if (!fila || fila_vazia(fila)) {
        fprintf(stderr, "[AVISO] Tentou remover de fila vazia\n");
        return NULL;
    }

    // Remove o head — o NoFila mais prioritário
    NoFila   *no_removido = fila->head;
    Paciente *paciente    = no_removido->paciente;

    fila->head = no_removido->prox;
    fila->tamanho--;

    // Libera só o NoFila — o Paciente continua vivo
    // quem chama decide o que fazer com ele (empilhar no histórico)
    free(no_removido);

    return paciente;
}

int fila_vazia(const Fila *fila) {
    return (!fila || fila->head == NULL);
}
