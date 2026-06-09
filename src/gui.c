#include "gui.h"
#include <string.h>
#include <stdio.h>

// ─── Callbacks de navegação ──────────────────────────────────────

static void on_voltar_inicio(GtkWidget *widget, gpointer data)
{
    (void)widget;
    AppState *state = (AppState *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_inicial");
}

static void on_ir_historico(GtkWidget *widget, gpointer data)
{
    (void)widget;
    AppState *state = (AppState *)data;
    atualizar_grid_historico(state);
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_historico");
}

static void on_ir_chamada(GtkWidget *widget, gpointer data)
{
    (void)widget;
    AppState *state = (AppState *)data;
    atualizar_tela_chamada(state);
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_chamada");
}

static void on_ir_triagem(GtkWidget *widget, gpointer data)
{
    (void)widget;
    AppState *state = (AppState *)data;
    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_triagem");
}

// ─── Callback de atender paciente ───────────────────────────────

static void on_atender_paciente(GtkWidget *widget, gpointer data)
{
    (void)widget;
    AppState *state = (AppState *)data;

    if (fila_vazia(state->fila))
        return;

    // Remove o primeiro da fila e empilha no histórico
    Paciente *atendido = remover_fila(state->fila);
    empilhar(state->historico, atendido);

    // Atualiza as duas telas
    atualizar_tela_chamada(state);
    atualizar_grid_historico(state);
}

// ─── Desenho dos cards ───────────────────────────────────────────

static gboolean desenhar_card(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    (void)widget;
    Paciente *p = (Paciente *)data;

    int largura = gtk_widget_get_allocated_width(widget);
    int altura = gtk_widget_get_allocated_height(widget);

    // Cor de fundo conforme prioridade
    if (p && p->prioridade == PRIORIDADE_VERMELHA)
        cairo_set_source_rgb(cr, 1.0, 0.8, 0.8);
    else if (p && p->prioridade == PRIORIDADE_AMARELA)
        cairo_set_source_rgb(cr, 1.0, 0.97, 0.8);
    else if (p && p->prioridade == PRIORIDADE_VERDE)
        cairo_set_source_rgb(cr, 0.8, 1.0, 0.8);
    else
        cairo_set_source_rgb(cr, 0.94, 0.94, 0.94);

    // Fundo com cantos arredondados
    double raio = 8.0;
    cairo_new_path(cr);
    cairo_arc(cr, raio, raio, raio, G_PI, 3 * G_PI / 2);
    cairo_arc(cr, largura - raio, raio, raio, 3 * G_PI / 2, 0);
    cairo_arc(cr, largura - raio, altura - raio, raio, 0, G_PI / 2);
    cairo_arc(cr, raio, altura - raio, raio, G_PI / 2, G_PI);
    cairo_close_path(cr);
    cairo_fill(cr);

    // Borda
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
        return FALSE;

    // Texto
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
    char linha[300];

    // Nome em negrito
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 13);
    cairo_move_to(cr, 12, 26);
    cairo_show_text(cr, p->nome);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 11);

    snprintf(linha, sizeof(linha), "Idade: %d anos", p->idade);
    cairo_move_to(cr, 12, 46);
    cairo_show_text(cr, linha);

    snprintf(linha, sizeof(linha), "CPF: %s", p->cpf[0] ? p->cpf : "—");
    cairo_move_to(cr, 12, 62);
    cairo_show_text(cr, linha);

    snprintf(linha, sizeof(linha), "Nasc: %s", p->data_nascimento[0] ? p->data_nascimento : "—");
    cairo_move_to(cr, 12, 78);
    cairo_show_text(cr, linha);

    // Justificativa — quebra se necessário
    if (strlen(p->justificativa) > 38)
    {
        char p1[39];
        strncpy(p1, p->justificativa, 38);
        p1[38] = '\0';
        cairo_move_to(cr, 12, 94);
        cairo_show_text(cr, p1);
        cairo_move_to(cr, 12, 108);
        cairo_show_text(cr, p->justificativa + 38);
    }
    else
    {
        cairo_move_to(cr, 12, 94);
        cairo_show_text(cr, p->justificativa);
    }

    // Prioridade em negrito no rodapé
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    snprintf(linha, sizeof(linha), "Prioridade: %s", prioridade_para_string(p->prioridade));
    cairo_move_to(cr, 12, 128);
    cairo_show_text(cr, linha);

    return FALSE;
}

// ─── Histórico ───────────────────────────────────────────────────

void atualizar_grid_historico(AppState *state)
{
    if (!state->grid_historico)
        return;

    // Remove todos os cards atuais
    GList *filhos = gtk_container_get_children(GTK_CONTAINER(state->grid_historico));
    for (GList *l = filhos; l != NULL; l = l->next)
        gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(filhos);

    // Percorre a pilha do topo ao fundo redesenhando os cards
    NoPilha *no = state->historico->topo;
    int indice = 0;

    while (no)
    {
        GtkWidget *card = gtk_drawing_area_new();
        gtk_widget_set_size_request(card, 280, 145);

        // Passa o paciente como dado para o callback de desenho
        g_signal_connect(card, "draw", G_CALLBACK(desenhar_card), no->paciente);

        int coluna = indice % 2;
        int linha = indice / 2;
        gtk_grid_attach(GTK_GRID(state->grid_historico), card, coluna, linha, 1, 1);

        no = no->abaixo;
        indice++;
    }

    gtk_widget_show_all(state->grid_historico);
}

// ─── Chamada ─────────────────────────────────────────────────────

void atualizar_tela_chamada(AppState *state)
{
    if (!state->box_chamada)
        return;

    // Atualiza o contador
    if (state->label_contador)
    {
        char texto[64];
        snprintf(texto, sizeof(texto),
                 "NÚMERO DE PACIENTES NA FILA:  %d", state->fila->tamanho);
        gtk_label_set_text(GTK_LABEL(state->label_contador), texto);
    }

    // Remove todos os cards atuais
    GList *filhos = gtk_container_get_children(GTK_CONTAINER(state->box_chamada));
    for (GList *l = filhos; l != NULL; l = l->next)
        gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(filhos);

    if (fila_vazia(state->fila))
    {
        GtkWidget *aviso = gtk_label_new("Nenhum paciente na fila.");
        gtk_widget_set_margin_top(aviso, 40);
        gtk_box_pack_start(GTK_BOX(state->box_chamada), aviso, FALSE, FALSE, 0);
        gtk_widget_show_all(state->box_chamada);
        return;
    }

    // Percorre a fila pelos NoFila e cria uma linha por paciente
    NoFila *atual = state->fila->head;
    int indice = 0;

    while (atual)
    {
        // Linha horizontal — card + botão (só no primeiro)
        GtkWidget *linha_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
        gtk_widget_set_margin_bottom(linha_box, 8);

        // Card do paciente — passa atual->paciente para o callback de desenho
        GtkWidget *card = gtk_drawing_area_new();
        gtk_widget_set_size_request(card, 480, 145);
        g_signal_connect(card, "draw", G_CALLBACK(desenhar_card), atual->paciente);
        gtk_box_pack_start(GTK_BOX(linha_box), card, FALSE, FALSE, 0);

        // Botão — só no primeiro da fila
        if (indice == 0)
        {
            GtkWidget *btn = gtk_button_new_with_label("APERTE\nPARA\nATENDER");
            gtk_widget_set_size_request(btn, 120, 145);

            GtkCssProvider *css = gtk_css_provider_new();
            gtk_css_provider_load_from_data(css,
                                            "button {"
                                            "  background: #E0E0E0;"
                                            "  font-weight: bold;"
                                            "  font-size: 13px;"
                                            "  border-radius: 4px;"
                                            "  border: 1px solid #AAAAAA;"
                                            "}"
                                            "button:hover { background: #C8E6C9; }",
                                            -1, NULL);
            gtk_style_context_add_provider(
                gtk_widget_get_style_context(btn),
                GTK_STYLE_PROVIDER(css),
                GTK_STYLE_PROVIDER_PRIORITY_USER);
            g_object_unref(css);

            g_signal_connect(btn, "clicked", G_CALLBACK(on_atender_paciente), state);
            gtk_box_pack_start(GTK_BOX(linha_box), btn, FALSE, FALSE, 0);
        }

        gtk_box_pack_start(GTK_BOX(state->box_chamada), linha_box, FALSE, FALSE, 0);

        atual = atual->prox;
        indice++;
    }

    gtk_widget_show_all(state->box_chamada);
}

// ─── Tela inicial ────────────────────────────────────────────────

GtkWidget *criar_tela_inicial(AppState *state)
{
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

    const char *nomes[3] = {"TRIAGEM", "HISTÓRICO", "CHAMADA"};
    GCallback callbacks[3] = {
        G_CALLBACK(on_ir_triagem),
        G_CALLBACK(on_ir_historico),
        G_CALLBACK(on_ir_chamada)};

    for (int i = 0; i < 3; i++)
    {
        GtkWidget *btn = gtk_button_new_with_label(nomes[i]);
        gtk_widget_set_size_request(btn, 280, 220);

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

        g_signal_connect(btn, "clicked", callbacks[i], state);
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

    return box;
}

// ─── Tela histórico ──────────────────────────────────────────────

GtkWidget *criar_tela_historico(AppState *state)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Header
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *titulo_header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_header),
                         "<span font='Arial Bold 16'>Histórico</span>");
    gtk_widget_set_hexpand(titulo_header, TRUE);
    gtk_widget_set_halign(titulo_header, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(titulo_header, 12);
    gtk_widget_set_margin_bottom(titulo_header, 12);
    gtk_box_pack_start(GTK_BOX(header), titulo_header, TRUE, TRUE, 0);

    // Botão casinha
    GdkPixbuf *icone_orig = gdk_pixbuf_new_from_file("casaicone.png", NULL);
    GtkWidget *btn_casa;
    if (icone_orig)
    {
        GdkPixbuf *icone_redim = gdk_pixbuf_scale_simple(icone_orig, 24, 24, GDK_INTERP_BILINEAR);
        GtkWidget *img = gtk_image_new_from_pixbuf(icone_redim);
        g_object_unref(icone_orig);
        g_object_unref(icone_redim);
        btn_casa = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(btn_casa), img);
    }
    else
    {
        btn_casa = gtk_button_new_with_label("🏠");
    }
    gtk_widget_set_size_request(btn_casa, 40, 40);
    g_signal_connect(btn_casa, "clicked", G_CALLBACK(on_voltar_inicio), state);
    gtk_box_pack_end(GTK_BOX(header), btn_casa, FALSE, FALSE, 8);

    // Fundo cinza no header
    GtkCssProvider *css_header = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_header,
                                    "box { background: #DDDDDD; }", -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(header),
        GTK_STYLE_PROVIDER(css_header),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_header);

    gtk_box_pack_start(GTK_BOX(box), header, FALSE, FALSE, 0);

    // Scroll com grade de cards
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

// ─── Tela chamada ────────────────────────────────────────────────

GtkWidget *criar_tela_chamada(AppState *state)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Header
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    GtkWidget *titulo_header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_header),
                         "<span font='Arial Bold 16'>Chamada</span>");
    gtk_widget_set_hexpand(titulo_header, TRUE);
    gtk_widget_set_halign(titulo_header, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(titulo_header, 12);
    gtk_widget_set_margin_bottom(titulo_header, 12);
    gtk_box_pack_start(GTK_BOX(header), titulo_header, TRUE, TRUE, 0);

    // Botão casinha
    GdkPixbuf *icone_orig = gdk_pixbuf_new_from_file("casaicone.png", NULL);
    GtkWidget *btn_casa;
    if (icone_orig)
    {
        GdkPixbuf *icone_redim = gdk_pixbuf_scale_simple(icone_orig, 24, 24, GDK_INTERP_BILINEAR);
        GtkWidget *img = gtk_image_new_from_pixbuf(icone_redim);
        g_object_unref(icone_orig);
        g_object_unref(icone_redim);
        btn_casa = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(btn_casa), img);
    }
    else
    {
        btn_casa = gtk_button_new_with_label("🏠");
    }
    gtk_widget_set_size_request(btn_casa, 40, 40);
    g_signal_connect(btn_casa, "clicked", G_CALLBACK(on_voltar_inicio), state);
    gtk_box_pack_end(GTK_BOX(header), btn_casa, FALSE, FALSE, 8);

    GtkCssProvider *css_header = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_header,
                                    "box { background: #DDDDDD; }", -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(header),
        GTK_STYLE_PROVIDER(css_header),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_header);

    gtk_box_pack_start(GTK_BOX(box), header, FALSE, FALSE, 0);

    // Contador de pacientes
    state->label_contador = gtk_label_new("NÚMERO DE PACIENTES NA FILA:  0");
    gtk_widget_set_margin_top(state->label_contador, 12);
    gtk_widget_set_margin_bottom(state->label_contador, 12);

    GtkCssProvider *css_contador = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_contador,
                                    "label {"
                                    "  background: #EEEEEE;"
                                    "  font-weight: bold;"
                                    "  font-size: 14px;"
                                    "  padding: 8px;"
                                    "  border: 1px solid #CCCCCC;"
                                    "}",
                                    -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(state->label_contador),
        GTK_STYLE_PROVIDER(css_contador),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_contador);

    gtk_box_pack_start(GTK_BOX(box), state->label_contador, FALSE, FALSE, 0);

    // Scroll com lista de pacientes
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(
        GTK_SCROLLED_WINDOW(scroll),
        GTK_POLICY_NEVER,
        GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);

    state->box_chamada = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_top(state->box_chamada, 12);
    gtk_widget_set_margin_start(state->box_chamada, 20);
    gtk_widget_set_margin_end(state->box_chamada, 20);

    gtk_container_add(GTK_CONTAINER(scroll), state->box_chamada);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    return box;
}

// ─── Inicialização ───────────────────────────────────────────────

void gui_iniciar(AppState *state, int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    state->stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), state->stack);

    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_inicial(state), "tela_inicial");
    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_historico(state), "tela_historico");
    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_chamada(state), "tela_chamada");

    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_inicial");

    gtk_widget_show_all(window);
    gtk_main();
}