#ifndef PACIENTE_H // evitar múltiplas inclusões
#define PACIENTE_H // paciente.h: definição da estrutura Paciente e constantes de prioridade

// constantes para as prioridades
#define PRIORIDADE_VERMELHA 1
#define PRIORIDADE_AMARELA 2
#define PRIORIDADE_VERDE 3

typedef struct Paciente
{
    char nome[100];
    int idade;
    int prioridade;          // usa as constantes acima
    char justificativa[300]; // explicação do motivo da prioridade, preenchida após a triagem
    struct Paciente *next;   // encadeamento da fila
} Paciente;

// isso aq vai ser exclusivamente pra debugar o codigo, não é necessário para a implementação da fila de pacientes mas é útil para verificar se os dados estão sendo armazenados corretamente
Paciente *criar_paciente(const char *nome, int idade);
void destruir_paciente(Paciente *p);
// Debug
void imprimir_paciente(const Paciente *p);
const char *prioridade_para_string(int prioridade);

#endif