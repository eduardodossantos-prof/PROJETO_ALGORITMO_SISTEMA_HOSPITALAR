#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "paciente.h"
#include "arvore.h"
#include "fila.h"
#include "pilha.h"

// Máximo de cards visíveis no histórico
#define MAX_HISTORICO_VISUAL 6

// Estado global da aplicação — passado para todos os callbacks
typedef struct {
    // ── Estruturas de dados ──────────────────────────────────────
    Nodoarvore *arvore;       // árvore de decisão clínica
    Fila       *fila;         // fila de espera ordenada por prioridade
    Pilha      *historico;    // pilha de atendimentos concluídos

    // ── Estado da triagem em andamento ───────────────────────────
    Nodoarvore *no_atual;     // nó onde está agora na árvore
    Paciente   *paciente;     // paciente sendo triado no momento

    // ── Widgets que os callbacks precisam atualizar ───────────────
    GtkWidget  *stack;            // gerencia qual tela está visível
    GtkWidget  *grid_historico;   // grade onde os cards são desenhados

    // ── Array visual do histórico ─────────────────────────────────
    // guarda os últimos MAX_HISTORICO_VISUAL pacientes atendidos
    // quando chega o 7º, o mais antigo (posição 5) é descartado
    // e todos deslocam uma posição para abrir espaço no início
    Paciente   *pacientes_historico[MAX_HISTORICO_VISUAL];
    int         total_historico;  // quantos cards estão preenchidos agora

} AppState;

// ── Protótipos ───────────────────────────────────────────────────

// Inicializa a janela e entra no loop GTK
void gui_iniciar(AppState *state, int argc, char *argv[]);

// Cria cada tela — retornam um widget pronto para o GtkStack
GtkWidget *criar_tela_inicial(AppState *state);
GtkWidget *criar_tela_historico(AppState *state);

// Atualiza a grade visual do histórico após novo atendimento
void atualizar_grid_historico(AppState *state);

// Adiciona paciente ao array visual e redesenha o histórico
void adicionar_ao_historico_visual(AppState *state, Paciente *paciente);

#endif