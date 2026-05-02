#ifndef ARVORE_H
#define ARVORE_H

#include "paciente.h" // para usar a struct Paciente e as constantes de prioridade

typedef struct Nodoarvore
{
    int eh_folha; // verificar se é folha ou nó de pergunta

    // Campos de pergunta (usados quando eh_folha == 0)
    char pergunta[200];
    struct Nodoarvore *sim; //
    struct Nodoarvore *nao;

    // Campos de folha (usados quando eh_folha == 1)
    int prioridade;          // valor da prioridade a ser aplicada ao paciente
    char justificativa[300]; // explicação do motivo da prioridade, para mostrar no resultado da triagem

} Nodoarvore;

Nodoarvore *criar_pergunta(const char *pergunta, Nodoarvore *sim, Nodoarvore *nao);
Nodoarvore *criar_folha(int prioridade, const char *justificativa);
// vai criar a arvore de baixo pra cima, primeiro as folhas, depois os nós de pergunta que apontam pra essas folhas, e por fim a raiz da árvore
void destruir_arvore(Nodoarvore *raiz);
// percorre recursivamente a árvore, destruindo ela toda, liberando a memória alocada
Nodoarvore *construir_arvore(void);
Nodoarvore *avancar_no(Nodoarvore *atual, int resposta);
void aplicar_triagem(Nodoarvore *folha, Paciente *p);
// debug
void imprimir_arvore(const Nodoarvore *raiz, int nivel);

#endif