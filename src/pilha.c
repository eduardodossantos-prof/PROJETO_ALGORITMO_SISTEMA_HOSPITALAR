#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

// ─── Ciclo de vida ───────────────────────────────────────────────

Pilha *criar_pilha(void)
{
    Pilha *pilha = malloc(sizeof(Pilha));
    if (!pilha)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar pilha\n");
        return NULL;
    }

    pilha->topo = NULL;
    pilha->tam = 0;

    return pilha;
}

void destruir_pilha(Pilha *pilha)
{
    if (!pilha)
        return;

    // Desempilha tudo liberando cada nó e cada paciente
    NoPilha *atual = pilha->topo;
    while (atual)
    {
        NoPilha *prox = atual->prox;
        destruir_paciente(atual->paciente);
        free(atual);
        atual = prox;
    }

    free(pilha);
}

// ─── Operações ───────────────────────────────────────────────────

void empilhar(Pilha *pilha, Paciente *paciente)
{
    if (!pilha || !paciente)
    {
        fprintf(stderr, "[ERRO] empilhar recebeu ponteiro NULL\n");
        return;
    }

    NoPilha *no = malloc(sizeof(NoPilha));
    if (!no)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar no da pilha\n");
        return;
    }

    // O novo nó aponta para o topo atual
    // depois vira o novo topo
    no->paciente = paciente;
    no->prox = pilha->topo;
    pilha->topo = no;
    pilha->tam++;
}

Paciente *desempilhar(Pilha *pilha)
{
    if (!pilha || pilha_vazia(pilha))
    {
        fprintf(stderr, "[AVISO] Tentou desempilhar pilha vazia\n");
        return NULL;
    }

    // Guarda o nó do topo
    // o topo passa a ser o nó prox
    // libera o nó mas não o paciente — ele será usado pelo chamador
    NoPilha *no = pilha->topo;
    Paciente *paciente = no->paciente;

    pilha->topo = no->prox;
    pilha->tam--;
    free(no);

    return paciente;
}

Paciente *topo_pilha(const Pilha *pilha)
{
    if (!pilha || pilha_vazia(pilha))
    {
        fprintf(stderr, "[AVISO] Tentou ver topo de pilha vazia\n");
        return NULL;
    }

    return pilha->topo->paciente;
}

int pilha_vazia(const Pilha *pilha)
{
    return (!pilha || pilha->topo == NULL);
}

//  Debug // Teste

void imprimir_pilha(const Pilha *pilha)
{
    if (!pilha)
    {
        printf("[DEBUG] Pilha NULL\n");
        return;
    }

    printf("=== HISTORICO DE ATENDIMENTOS (%d) ===\n", pilha->tam);

    if (pilha_vazia(pilha))
    {
        printf("  (nenhum atendimento ainda)\n");
        printf("======================================\n");
        return;
    }

    NoPilha *atual = pilha->topo;
    int posicao = 1;

    while (atual)
    {
        const char *cor = "";
        if (atual->paciente->prioridade == PRIORIDADE_VERMELHA)
            cor = "[VERMELHO]";
        else if (atual->paciente->prioridade == PRIORIDADE_AMARELA)
            cor = "[AMARELO] ";
        else
            cor = "[VERDE]   ";

        printf("  %d. %s %s, %d anos\n",
               posicao++,
               cor,
               atual->paciente->nome,
               atual->paciente->idade);
        printf("     %s\n", atual->paciente->justificativa);

        atual = atual->prox;
    }

    printf("======================================\n");
}
