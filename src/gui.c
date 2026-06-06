#include "gui.h"
#include <string.h>

// ─── Callbacks de navegação ──────────────────────────────────────

// Volta para a tela inicial ao clicar na casinha
static void on_voltar_inicio(GtkWidget *widget, gpointer data)
{
    AppState *state = (AppState *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_inicial");
}

// Vai para a tela de histórico ao clicar no bloco
static void on_ir_historico(GtkWidget *widget, gpointer data)
{
    AppState *state = (AppState *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_historico");
}

// Vai para a tela de triagem ao clicar no bloco (implementado depois)
static void on_ir_triagem(GtkWidget *widget, gpointer data)
{
    AppState *state = (AppState *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_triagem");
}

// ─── Desenho dos cards do histórico ─────────────────────────────

// Dados passados para o callback de desenho de cada card
typedef struct
{
    Paciente *paciente; // paciente que esse card representa
} DadosCard;

// Callback chamado pelo GTK para desenhar cada card
static gboolean desenhar_card(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    DadosCard *dados = (DadosCard *)data;
    Paciente *p = dados->paciente;

    int largura = gtk_widget_get_allocated_width(widget);
    int altura = gtk_widget_get_allocated_height(widget);

    // Cor de fundo conforme prioridade
    if (p && p->prioridade == PRIORIDADE_VERMELHA)
    {
        cairo_set_source_rgb(cr, 1.0, 0.8, 0.8); // vermelho claro
    }
    else if (p && p->prioridade == PRIORIDADE_AMARELA)
    {
        cairo_set_source_rgb(cr, 1.0, 0.97, 0.8); // amarelo claro
    }
    else if (p && p->prioridade == PRIORIDADE_VERDE)
    {
        cairo_set_source_rgb(cr, 0.8, 1.0, 0.8); // verde claro
    }
    else
    {
        cairo_set_source_rgb(cr, 0.94, 0.94, 0.94); // cinza — card vazio
    }

    // Desenha o fundo com cantos arredondados
    double raio = 10.0;
    cairo_new_path(cr);
    cairo_arc(cr, raio, raio, raio, G_PI, 3 * G_PI / 2);
    cairo_arc(cr, largura - raio, raio, raio, 3 * G_PI / 2, 0);
    cairo_arc(cr, largura - raio, altura - raio, raio, 0, G_PI / 2);
    cairo_arc(cr, raio, altura - raio, raio, G_PI / 2, G_PI);
    cairo_close_path(cr);
    cairo_fill(cr);

    // Borda sutil
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_set_line_width(cr, 1.0);
    cairo_new_path(cr);
    cairo_arc(cr, raio, raio, raio, G_PI, 3 * G_PI / 2);
    cairo_arc(cr, largura - raio, raio, raio, 3 * G_PI / 2, 0);
    cairo_arc(cr, largura - raio, altura - raio, raio, 0, G_PI / 2);
    cairo_arc(cr, raio, altura - raio, raio, G_PI / 2, G_PI);
    cairo_close_path(cr);
    cairo_stroke(cr);

    if (!p)
        return FALSE; // card vazio — só fundo cinza

    // Texto do card
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

    // Nome do paciente em negrito e maior
    cairo_set_font_size(cr, 13);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_move_to(cr, 12, 28);
    cairo_show_text(cr, p->nome);

    // Campos do paciente
    cairo_set_font_size(cr, 11);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);

    char linha[200];

    snprintf(linha, sizeof(linha), "Idade: %d anos", p->idade);
    cairo_move_to(cr, 12, 50);
    cairo_show_text(cr, linha);

    snprintf(linha, sizeof(linha), "CPF: %s", p->cpf[0] ? p->cpf : "—");
    cairo_move_to(cr, 12, 68);
    cairo_show_text(cr, linha);

    snprintf(linha, sizeof(linha), "Nasc: %s", p->data_nascimento[0] ? p->data_nascimento : "—");
    cairo_move_to(cr, 12, 86);
    cairo_show_text(cr, linha);

    // Justificativa — quebra em duas linhas se necessário
    if (strlen(p->justificativa) > 35)
    {
        char parte1[36], parte2[300];
        strncpy(parte1, p->justificativa, 35);
        parte1[35] = '\0';
        strcpy(parte2, p->justificativa + 35);

        cairo_move_to(cr, 12, 104);
        cairo_show_text(cr, parte1);
        cairo_move_to(cr, 12, 120);
        cairo_show_text(cr, parte2);
    }
    else
    {
        cairo_move_to(cr, 12, 104);
        cairo_show_text(cr, p->justificativa);
    }

    // Prioridade no rodapé em negrito
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 11);
    snprintf(linha, sizeof(linha), "Prioridade: %s", prioridade_para_string(p->prioridade));
    cairo_move_to(cr, 12, 140);
    cairo_show_text(cr, linha);

    return FALSE;
}

// ─── Histórico visual ────────────────────────────────────────────

void adicionar_ao_historico_visual(AppState *state, Paciente *paciente)
{
    // Desloca todos uma posição para a direita
    // o da posição 5 cai fora da vista (mas continua na pilha)
    for (int i = MAX_HISTORICO_VISUAL - 1; i > 0; i--)
    {
        state->pacientes_historico[i] = state->pacientes_historico[i - 1];
    }

    // Coloca o novo paciente na primeira posição
    state->pacientes_historico[0] = paciente;

    // Atualiza o contador — máximo 6
    if (state->total_historico < MAX_HISTORICO_VISUAL)
    {
        state->total_historico++;
    }

    // Redesenha a grade
    atualizar_grid_historico(state);
}

void atualizar_grid_historico(AppState *state)
{
    if (!state->grid_historico)
        return;

    // Remove todos os cards atuais da grade
    GList *filhos = gtk_container_get_children(GTK_CONTAINER(state->grid_historico));
    for (GList *l = filhos; l != NULL; l = l->next)
    {
        gtk_widget_destroy(GTK_WIDGET(l->data));
    }
    g_list_free(filhos);

    // Reconstrói os cards com os pacientes atuais
    for (int i = 0; i < state->total_historico; i++)
    {
        DadosCard *dados = g_new(DadosCard, 1);
        dados->paciente = state->pacientes_historico[i];

        GtkWidget *card = gtk_drawing_area_new();
        gtk_widget_set_size_request(card, 280, 160);

        // Libera os dados quando o widget for destruído
        g_signal_connect_data(card, "draw",
                              G_CALLBACK(desenhar_card), dados,
                              (GClosureNotify)g_free, 0);

        // 2 colunas — coluna 0 para índices pares, coluna 1 para ímpares
        int coluna = i % 2;
        int linha = i / 2;
        gtk_grid_attach(GTK_GRID(state->grid_historico), card, coluna, linha, 1, 1);
    }

    gtk_widget_show_all(state->grid_historico);
}

// ─── Tela inicial ────────────────────────────────────────────────

GtkWidget *criar_tela_inicial(AppState *state)
{
    GtkWidget *fixed = gtk_fixed_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Título
    GtkWidget *titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo),
                         "<span font='Arial Bold 22' foreground='#111111'>"
                         "SERVIÇO DE ATENDIMENTO HOSPITALAR"
                         "</span>");
    gtk_widget_set_margin_top(titulo, 60);
    gtk_widget_set_margin_bottom(titulo, 40);
    gtk_box_pack_start(GTK_BOX(box), titulo, FALSE, FALSE, 0);

    // Grade dos 3 blocos
    GtkWidget *grade = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grade), 20);
    gtk_widget_set_halign(grade, GTK_ALIGN_CENTER);

    // Nomes e callbacks de cada bloco
    const char *nomes[3] = {"TRIAGEM", "HISTÓRICO", "CHAMADA"};
    GCallback callbacks[3] = {
        G_CALLBACK(on_ir_triagem),
        G_CALLBACK(on_ir_historico),
        NULL // chamada ainda não implementada
    };

    for (int i = 0; i < 3; i++)
    {
        GtkWidget *btn = gtk_button_new_with_label(nomes[i]);
        gtk_widget_set_size_request(btn, 280, 220);

        // Estilo do botão — cinza claro como no wireframe
        GtkCssProvider *css = gtk_css_provider_new();
        gtk_css_provider_load_from_data(css,
                                        "button {"
                                        "  background: #E0E0E0;"
                                        "  border-radius: 4px;"
                                        "  font-size: 16px;"
                                        "  font-weight: bold;"
                                        "  color: #333333;"
                                        "  border: 1px solid #CCCCCC;"
                                        "}"
                                        "button:hover { background: #D0D0D0; }",
                                        -1, NULL);
        gtk_style_context_add_provider(
            gtk_widget_get_style_context(btn),
            GTK_STYLE_PROVIDER(css),
            GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(css);

        if (callbacks[i])
        {
            g_signal_connect(btn, "clicked", callbacks[i], state);
        }

        gtk_grid_attach(GTK_GRID(grade), btn, i, 0, 1, 1);
    }

    gtk_box_pack_start(GTK_BOX(box), grade, FALSE, FALSE, 0);

    // Logo UNIVASF
    GError *erro = NULL;
    GdkPixbuf *orig = gdk_pixbuf_new_from_file("univasf_logo.png", &erro);
    if (orig)
    {
        GdkPixbuf *redim = gdk_pixbuf_scale_simple(orig, 120, 60, GDK_INTERP_BILINEAR);
        GtkWidget *logo = gtk_image_new_from_pixbuf(redim);
        g_object_unref(orig);
        g_object_unref(redim);

        gtk_widget_set_margin_top(logo, 40);
        gtk_widget_set_halign(logo, GTK_ALIGN_START);
        gtk_widget_set_margin_start(logo, 20);
        gtk_box_pack_end(GTK_BOX(box), logo, FALSE, FALSE, 0);
    }
    else
    {
        g_error_free(erro);
    }

    gtk_fixed_put(GTK_FIXED(fixed), box, 0, 0);
    gtk_widget_set_size_request(box, 1200, 700);

    return fixed;
}

// ─── Tela histórico ──────────────────────────────────────────────

GtkWidget *criar_tela_historico(AppState *state)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // ── Header com título e botão casinha ────────────────────────
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(header, 0);

    // Fundo cinza no header
    GtkCssProvider *css_header = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_header,
                                    "box#header { background: #DDDDDD; padding: 10px; }", -1, NULL);
    gtk_widget_set_name(header, "header");
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(header),
        GTK_STYLE_PROVIDER(css_header),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_header);

    // Título do header
    GtkWidget *titulo_header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_header),
                         "<span font='Arial Bold 16'>Histórico</span>");
    gtk_widget_set_hexpand(titulo_header, TRUE);
    gtk_widget_set_halign(titulo_header, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(header), titulo_header, TRUE, TRUE, 0);

    // Botão casinha
    GdkPixbuf *icone_orig = gdk_pixbuf_new_from_file("casaicone.png", NULL);
    GdkPixbuf *icone_redim = gdk_pixbuf_scale_simple(icone_orig, 24, 24, GDK_INTERP_BILINEAR);
    GtkWidget *img = gtk_image_new_from_pixbuf(icone_redim);
    g_object_unref(icone_orig);
    g_object_unref(icone_redim);

    GtkWidget *btn_casa = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(btn_casa), img);
    gtk_widget_set_size_request(btn_casa, 40, 40);
    g_signal_connect(btn_casa, "clicked", G_CALLBACK(on_voltar_inicio), state);
    gtk_box_pack_end(GTK_BOX(header), btn_casa, FALSE, FALSE, 8);

    gtk_box_pack_start(GTK_BOX(box), header, FALSE, FALSE, 0);

    // ── Grade dos cards ──────────────────────────────────────────
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);

    state->grid_historico = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(state->grid_historico), 16);
    gtk_grid_set_row_spacing(GTK_GRID(state->grid_historico), 16);
    gtk_widget_set_margin_top(state->grid_historico, 20);
    gtk_widget_set_margin_start(state->grid_historico, 20);
    gtk_widget_set_margin_end(state->grid_historico, 20);
    gtk_widget_set_halign(state->grid_historico, GTK_ALIGN_CENTER);

    gtk_container_add(GTK_CONTAINER(scroll), state->grid_historico);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    return box;
}

// ─── Inicialização ───────────────────────────────────────────────

void gui_iniciar(AppState *state, int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // Janela principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Stack — gerencia as telas
    state->stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), state->stack);

    // Adiciona as telas ao stack
    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_inicial(state), "tela_inicial");

    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_historico(state), "tela_historico");

    // Começa na tela inicial
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_inicial");

    gtk_widget_show_all(window);
    gtk_main();
}