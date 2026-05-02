#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paciente.h"

Paciente *criar_paciente(const char *nome, int idade)
{
    Paciente *p = malloc(sizeof(Paciente));
    if (!p)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar paciente\n");
        return NULL;
    }

    strncpy(p->nome, nome, sizeof(p->nome) - 1);
    p->nome[sizeof(p->nome) - 1] = '\0'; // garante terminador
    p->idade = idade;
    p->prioridade = 0;          // ainda não triado
    p->justificativa[0] = '\0'; // vazio até a triagem
    p->next = NULL;

    return p;
}

void destruir_paciente(Paciente *p)
{
    if (!p)
        return;
    free(p);
}

const char *prioridade_para_string(int prioridade)
{
    switch (prioridade)
    {
    case PRIORIDADE_VERMELHA:
        return "VERMELHO (Urgente)";
    case PRIORIDADE_AMARELA:
        return "AMARELO (Moderado)";
    case PRIORIDADE_VERDE:
        return "VERDE (Leve)";
    default:
        return "Nao TRIADO";
    }
}
// Debug
void imprimir_paciente(const Paciente *p)
{
    if (!p)
    {
        printf("[DEBUG] Paciente NULL\n");
        return;
    }

    printf("-----------------------------\n");
    printf("[DEBUG] Nome       : %s\n", p->nome);
    printf("[DEBUG] Idade      : %d\n", p->idade);
    printf("[DEBUG] Prioridade : %s\n", prioridade_para_string(p->prioridade));
    printf("[DEBUG] Justif.    : %s\n", p->justificativa);
    printf("[DEBUG] Next       : %p\n", (void *)p->next);
    printf("-----------------------------\n");
}