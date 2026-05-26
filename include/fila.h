
#ifndef FILA_H
#define FILA_H
 
#include "paciente.h"
 
// A fila usa o ponteiro next que já existe dentro do Paciente
// não precisa de nó separado como a pilha
typedef struct {
    Paciente *head;     // primeiro da fila — próximo a ser atendido
    int       tamanho;  // quantos pacientes estão esperando
} Fila;
 
// Ciclo de vida
Fila     *criar_fila(void);
void      destruir_fila(Fila *fila);
 
// Operações
void      inserir_fila(Fila *fila, Paciente *paciente);
Paciente *remover_fila(Fila *fila);
int       fila_vazia(const Fila *fila);
 
// Debug
void      imprimir_fila(const Fila *fila);
 
#endif
 