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

    // Inicia a interface gráfica — só retorna quando o usuário fechar a janela
    gui_iniciar(&state, argc, argv);

    // Libera toda a memória ao encerrar
    destruir_arvore(state.arvore);
    destruir_fila(state.fila);
    destruir_pilha(state.historico);

    return 0;
}
