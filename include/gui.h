#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "paciente.h"
#include "arvore.h"
#include "fila.h"
#include "pilha.h"

// Estado global da aplicação — passado para todos os callbacks
typedef struct
{
    // ── Estruturas de dados ──────────────────────────────────────
    Nodoarvore *arvore; // árvore de decisão clínica
    Fila *fila;         // fila de espera ordenada por prioridade
    Pilha *historico;   // pilha de atendimentos concluídos

    // ── Estado da triagem em andamento ───────────────────────────
    Nodoarvore *no_atual; // nó onde está agora na árvore
    Paciente *paciente;   // paciente sendo triado no momento

    // ── Widgets que os callbacks precisam atualizar ───────────────
    GtkWidget *stack;          // gerencia qual tela está visível
    GtkWidget *grid_historico; // grade onde os cards do histórico são desenhados
    GtkWidget *box_chamada;    // onde os cards da fila são desenhados
    GtkWidget *label_contador; // "NÚMERO DE PACIENTES NA FILA: N"

} AppState;

// ── Protótipos ───────────────────────────────────────────────────

// Inicializa a janela e entra no loop GTK
void gui_iniciar(AppState *state, int argc, char *argv[]);

// Cria cada tela
GtkWidget *criar_tela_inicial(AppState *state);
GtkWidget *criar_tela_historico(AppState *state);
GtkWidget *criar_tela_chamada(AppState *state);
GtkWidget *criar_tela_triagem(AppState *state);

// Atualiza as telas após mudanças nas estruturas
void atualizar_grid_historico(AppState *state);
void atualizar_tela_chamada(AppState *state);

#endif