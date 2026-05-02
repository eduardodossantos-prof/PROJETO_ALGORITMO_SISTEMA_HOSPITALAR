#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arvore.h"

//  Criação de nós

Nodoarvore *criar_pergunta(const char *pergunta, Nodoarvore *sim, Nodoarvore *nao)
{
    Nodoarvore *no = malloc(sizeof(Nodoarvore));
    if (!no)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar no de pergunta\n");
        // tratamento de erro: se a alocação falhar, retorna NULL para indicar que o nó não foi criado e manda pra outro lugar do código lidar com isso (como imprimir uma mensagem de erro e encerrar o programa, ou tentar alocar novamente, etc)
        return NULL;
    }

    no->eh_folha = 0;
    strncpy(no->pergunta, pergunta, sizeof(no->pergunta) - 1);
    no->pergunta[sizeof(no->pergunta) - 1] = '\0';
    // liga os filhos do nó de pergunta, apontando para os nós de resposta "sim" e "não" que foram passados como argumentos
    no->sim = sim;
    no->nao = nao;

    return no;
}

Nodoarvore *criar_folha(int prioridade, const char *justificativa) // nao recebe filhos porque é uma folha, ou seja, um resultado final da triagem, sem perguntas seguintes
{
    Nodoarvore *no = malloc(sizeof(Nodoarvore));
    if (!no)
    {
        fprintf(stderr, "[ERRO] Falha ao alocar no folha\n");
        return NULL;
    }

    no->eh_folha = 1;
    no->prioridade = prioridade;
    strncpy(no->justificativa, justificativa, sizeof(no->justificativa) - 1);
    no->justificativa[sizeof(no->justificativa) - 1] = '\0';

    // Garantir que campos de pergunta não fiquem com lixo
    no->sim = NULL;
    no->nao = NULL;

    return no;
}

// Construção da árvore

Nodoarvore *construir_arvore(void)
{
    // Folhas — resultados finais da triagem
    Nodoarvore *f1a = criar_folha(PRIORIDADE_VERMELHA,
                                  "Paciente inconsciente - risco imediato de vida");

    Nodoarvore *f1b = criar_folha(PRIORIDADE_VERMELHA,
                                  "Insuficiencia respiratoria aguda - intervençao imediata");

    Nodoarvore *f2a = criar_folha(PRIORIDADE_AMARELA,
                                  "Febre alta sem comprometimento respiratorio - monitoramento necessario");

    Nodoarvore *f2b = criar_folha(PRIORIDADE_AMARELA,
                                  "Dor intensa sem outros sintomas graves - atendimento prioritario");

    Nodoarvore *f3 = criar_folha(PRIORIDADE_VERDE,
                                 "Sintomas leves - atendimento eletivo");

    // Nós internos — montados de baixo para cima
    Nodoarvore *dor = criar_pergunta(
        "Esta com dor intensa?", f2b, f3);

    Nodoarvore *febre = criar_pergunta(
        "Tem febre acima de 39 C?", f2a, dor);

    Nodoarvore *resp = criar_pergunta(
        "Tem dificuldade de respirar?", f1b, febre);

    Nodoarvore *raiz = criar_pergunta(
        "Esta inconsciente?", f1a, resp);

    return raiz;
}

// Navegação

Nodoarvore *avancar_no(Nodoarvore *atual, int resposta)
{
    if (!atual)
    {
        fprintf(stderr, "[ERRO] No atual eh NULL\n");
        return NULL;
    }
    if (atual->eh_folha)
    {
        fprintf(stderr, "[AVISO] Tentou avançar em uma folha\n");
        return atual;
    }
    // operador ternário: se resposta for verdadeira (1), retorna atual->sim, caso contrário (0), retorna atual->nao
    return resposta ? atual->sim : atual->nao;
}

void aplicar_triagem(Nodoarvore *folha, Paciente *p)
{
    // conexao entre a folha da árvore, que contém o resultado da triagem, e o paciente, para aplicar a prioridade e justificativa ao paciente
    if (!folha || !p)
        return;
    if (folha->eh_folha == 0)
    {
        fprintf(stderr, "[ERRO] aplicar_triagem chamado em no nao-folha\n");
        return;
    }

    p->prioridade = folha->prioridade;
    strncpy(p->justificativa, folha->justificativa, sizeof(p->justificativa) - 1);
    p->justificativa[sizeof(p->justificativa) - 1] = '\0';
}

//  Destruição

void destruir_arvore(Nodoarvore *raiz)
{
    // destruição em pos-ordem: primeiro destrói os filhos, depois o nó atual, garantindo que toda a memória alocada para a árvore seja liberada corretamente
    if (!raiz)
        return;

    if (raiz->eh_folha == 0)
    {
        destruir_arvore(raiz->sim);
        destruir_arvore(raiz->nao);
    }

    free(raiz);
}

//  Debug

void imprimir_arvore(const Nodoarvore *raiz, int nivel)
{
    if (!raiz)
        return;

    // Indentação visual por nível
    for (int i = 0; i < nivel; i++)
        printf("    ");

    if (raiz->eh_folha)
    {
        printf(" FOLHA - Prioridade %d | %s\n",
               raiz->prioridade, raiz->justificativa);
    }
    else
    {
        printf("? %s\n", raiz->pergunta);

        for (int i = 0; i < nivel; i++)
            printf("    ");
        printf(" SIM:\n");
        imprimir_arvore(raiz->sim, nivel + 2);

        for (int i = 0; i < nivel; i++)
            printf("    ");
        printf("  NAO:\n");
        imprimir_arvore(raiz->nao, nivel + 2);
    }
}