#ifndef PILHA_H // evitar múltiplas inclusões
#define PILHA_H // pilha.h: definição da estrutura Pilha e funções para manipulação da pilha de pacientes
//A pilha vai ser usada pra fazer o histórico de pacientes que foi atendido apos passar pela fila de prioridade
#include "paciente.h" // incluir a definição da struct Paciente
typedef struct NoPilha{
    Paciente  *paciente;
    struct NoPilha *prox;
} NoPilha;
typedef struct Pilha{
    NoPilha *topo;
    int tam;
} Pilha;
// Operações de criação e destruição
Pilha   *criar_pilha(void);
void     destruir_pilha(Pilha *pilha);
 
// Operações
void     empilhar(Pilha *pilha, Paciente *paciente);
Paciente *desempilhar(Pilha *pilha);
Paciente *topo_pilha(const Pilha *pilha);
int      pilha_vazia(const Pilha *pilha);
 
// Debug
void     imprimir_pilha(const Pilha *pilha);
 
#endif