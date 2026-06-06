#include <string.h>
#include "gui.h"

int main(int argc, char *argv[])
{

    // Inicializa todas as estruturas de dados
    AppState state;
    memset(&state, 0, sizeof(AppState)); // zera tudo — ponteiros NULL, ints 0

    state.arvore = construir_arvore();
    state.fila = criar_fila();
    state.historico = criar_pilha();
    // teste
    //  ── Pacientes fictícios para teste ───────────────────────────

    // Paciente 1 — vermelho
    Paciente *p1 = criar_paciente("João Silva", 45, "12312", "12312", "10/05/1979");
    p1->prioridade = PRIORIDADE_VERMELHA;
    strcpy(p1->cpf, "111.111.111-11");
    strcpy(p1->data_nascimento, "10/05/1979");
    strcpy(p1->justificativa, "Paciente inconsciente - risco de vida");
    inserir_fila(state.fila, p1);

    // Paciente 2 — amarelo
    Paciente *p2 = criar_paciente("Maria Costa", 32, "123231", "123123", "12/12/1990");
    p2->prioridade = PRIORIDADE_AMARELA;
    strcpy(p2->cpf, "222.222.222-22");
    strcpy(p2->data_nascimento, "15/03/1992");
    strcpy(p2->justificativa, "Febre alta sem comprometimento respiratorio");
    inserir_fila(state.fila, p2);

    // Paciente 3 — verde
    Paciente *p3 = criar_paciente("Pedro Alves", 28, "123123", "123123", "22/07/1996");
    p3->prioridade = PRIORIDADE_VERDE;
    strcpy(p3->cpf, "333.333.333-33");
    strcpy(p3->data_nascimento, "22/07/1996");
    strcpy(p3->justificativa, "Sintomas leves - atendimento eletivo");
    inserir_fila(state.fila, p3);

    // Paciente já atendido — vai direto pro histórico
    Paciente *p4 = criar_paciente("Ana Lima", 55, "123123", "123123", "08/11/1969");
    p4->prioridade = PRIORIDADE_AMARELA;
    strcpy(p4->cpf, "444.444.444-44");
    strcpy(p4->data_nascimento, "08/11/1969");
    strcpy(p4->justificativa, "Dor intensa sem outros sintomas graves");
    empilhar(state.historico, p4);

    // Inicia a interface gráfica — só retorna quando o usuário fechar a janela
    gui_iniciar(&state, argc, argv);

    // Libera toda a memória ao encerrar
    destruir_arvore(state.arvore);
    destruir_fila(state.fila);
    destruir_pilha(state.historico);

    return 0;
}
