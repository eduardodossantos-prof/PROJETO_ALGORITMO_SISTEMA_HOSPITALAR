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
//
// ─── Tela triagem ────────────────────────────────────────────────

// Widgets internos da triagem que os callbacks precisam acessar
typedef struct
{
    AppState *state;

    // Fase cadastro
    GtkWidget *entry_nome;
    GtkWidget *entry_rg;
    GtkWidget *entry_nasc;
    GtkWidget *entry_cpf;
    GtkWidget *entry_idade;
    GtkWidget *btn_confirmar;

    // Fase perguntas
    GtkWidget *box_perguntas; // container que aparece após confirmar
    GtkWidget *label_pergunta;
    GtkWidget *btn_sim;
    GtkWidget *btn_nao;

    // Fase resultado
    GtkWidget *box_resultado;
    GtkWidget *btn_concluir;
    GtkWidget *btn_outro;
} TriagemCtx;

// ── helpers de estilo ────────────────────────────────────────────

static void aplicar_css(GtkWidget *w, const char *css_str)
{
    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, css_str, -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(w),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css);
}

// ── callbacks de triagem ─────────────────────────────────────────

static void triagem_resetar(TriagemCtx *ctx)
{
    AppState *state = ctx->state;

    // Limpa os campos
    gtk_entry_set_text(GTK_ENTRY(ctx->entry_nome), "");
    gtk_entry_set_text(GTK_ENTRY(ctx->entry_rg), "");
    gtk_entry_set_text(GTK_ENTRY(ctx->entry_nasc), "");
    gtk_entry_set_text(GTK_ENTRY(ctx->entry_cpf), "");
    gtk_entry_set_text(GTK_ENTRY(ctx->entry_idade), "");
    // Restaura label do botão confirmar e reabilita campos
    gtk_button_set_label(GTK_BUTTON(ctx->btn_confirmar), "APERTE PARA CONFIRMAR");
    aplicar_css(ctx->btn_confirmar,
                "button { background:#CCCCCC; font-weight:bold; font-size:12px;"
                "         border-radius:3px; border:1px solid #AAAAAA; }"
                "button:hover { background:#BBBBBB; }");
    gtk_widget_set_sensitive(ctx->entry_nome, TRUE);
    gtk_widget_set_sensitive(ctx->entry_rg, TRUE);
    gtk_widget_set_sensitive(ctx->entry_nasc, TRUE);
    gtk_widget_set_sensitive(ctx->entry_cpf, TRUE);
    gtk_widget_set_sensitive(ctx->btn_confirmar, TRUE);

    // Esconde perguntas e resultado
    gtk_widget_hide(ctx->box_perguntas);
    gtk_widget_hide(ctx->box_resultado);

    // Reinicia a árvore e descarta paciente anterior
    state->no_atual = state->arvore;
    if (state->paciente)
    {
        destruir_paciente(state->paciente);
        state->paciente = NULL;
    }
}

static void on_triagem_confirmar(GtkWidget *widget, gpointer data)
{
    (void)widget;
    TriagemCtx *ctx = (TriagemCtx *)data;
    AppState *state = ctx->state;

    const char *nome = gtk_entry_get_text(GTK_ENTRY(ctx->entry_nome));
    const char *rg = gtk_entry_get_text(GTK_ENTRY(ctx->entry_rg));
    const char *nasc = gtk_entry_get_text(GTK_ENTRY(ctx->entry_nasc));
    const char *cpf = gtk_entry_get_text(GTK_ENTRY(ctx->entry_cpf));
    const char *idade_str = gtk_entry_get_text(GTK_ENTRY(ctx->entry_idade));

    if (!nome || strlen(nome) == 0)
        return; // nome obrigatório

    // Cria o paciente
    if (state->paciente)
        destruir_paciente(state->paciente);
    state->paciente = criar_paciente(nome, atoi(idade_str), cpf, rg, nasc);

    // Bloqueia os campos e muda o botão para "CONFIRMADO"
    gtk_widget_set_sensitive(ctx->entry_nome, FALSE);
    gtk_widget_set_sensitive(ctx->entry_rg, FALSE);
    gtk_widget_set_sensitive(ctx->entry_nasc, FALSE);
    gtk_widget_set_sensitive(ctx->entry_cpf, FALSE);
    gtk_widget_set_sensitive(ctx->btn_confirmar, FALSE);
    gtk_button_set_label(GTK_BUTTON(ctx->btn_confirmar), "CONFIRMADO");
    aplicar_css(ctx->btn_confirmar,
                "button { background:#333333; color:white; font-weight:bold;"
                "         font-size:12px; border-radius:3px; border:1px solid #111111; }");

    // Reinicia a árvore e exibe a primeira pergunta
    state->no_atual = state->arvore;
    gtk_label_set_text(GTK_LABEL(ctx->label_pergunta),
                       state->no_atual->pergunta);
    gtk_widget_show(ctx->box_perguntas);
    gtk_widget_hide(ctx->box_resultado);
}

static void responder(TriagemCtx *ctx, int sim)
{
    AppState *state = ctx->state;

    state->no_atual = avancar_no(state->no_atual, sim);
    if (!state->no_atual)
        return;

    if (state->no_atual->eh_folha)
    {
        // Aplica resultado ao paciente e insere na fila
        aplicar_triagem(state->no_atual, state->paciente);
        inserir_fila(state->fila, state->paciente);
        state->paciente = NULL; // fila agora é dona do ponteiro

        // Atualiza tela de chamada em segundo plano
        atualizar_tela_chamada(state);

        // Mostra fase de resultado, esconde perguntas
        gtk_widget_hide(ctx->box_perguntas);
        gtk_widget_show(ctx->box_resultado);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(ctx->label_pergunta),
                           state->no_atual->pergunta);
    }
}

static void on_triagem_sim(GtkWidget *w, gpointer data)
{
    (void)w;
    responder((TriagemCtx *)data, 1);
}

static void on_triagem_nao(GtkWidget *w, gpointer data)
{
    (void)w;
    responder((TriagemCtx *)data, 0);
}

static void on_triagem_concluida(GtkWidget *w, gpointer data)
{
    (void)w;
    TriagemCtx *ctx = (TriagemCtx *)data;
    // Volta para a tela inicial
    gtk_stack_set_visible_child_name(
        GTK_STACK(ctx->state->stack), "tela_inicial");
    triagem_resetar(ctx);
}

static void on_triar_outro(GtkWidget *w, gpointer data)
{
    (void)w;
    triagem_resetar((TriagemCtx *)data);
}

// ── construção da tela ───────────────────────────────────────────

GtkWidget *criar_tela_triagem(AppState *state)
{
    // ctx vive enquanto a tela existir — vazamento intencional mínimo,
    // pois a tela dura o mesmo tempo que o processo.
    TriagemCtx *ctx = g_new0(TriagemCtx, 1);
    ctx->state = state;

    GtkWidget *box_raiz = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // ── Header ───────────────────────────────────────────────────
    GtkWidget *header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    aplicar_css(header, "box { background:#DDDDDD; }");

    GtkWidget *titulo_header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_header),
                         "<span font='Arial Bold 16'>TRIAGEM</span>");
    gtk_widget_set_hexpand(titulo_header, TRUE);
    gtk_widget_set_halign(titulo_header, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_top(titulo_header, 12);
    gtk_widget_set_margin_bottom(titulo_header, 12);
    gtk_box_pack_start(GTK_BOX(header), titulo_header, TRUE, TRUE, 0);

    // Botão casa
    GdkPixbuf *icone_orig = gdk_pixbuf_new_from_file("casaicone.png", NULL);
    GtkWidget *btn_casa;
    if (icone_orig)
    {
        GdkPixbuf *redim = gdk_pixbuf_scale_simple(icone_orig, 24, 24, GDK_INTERP_BILINEAR);
        btn_casa = gtk_button_new();
        gtk_button_set_image(GTK_BUTTON(btn_casa),
                             gtk_image_new_from_pixbuf(redim));
        g_object_unref(icone_orig);
        g_object_unref(redim);
    }
    else
    {
        btn_casa = gtk_button_new_with_label("🏠");
    }
    gtk_widget_set_size_request(btn_casa, 40, 40);
    // volta para início SEM descartar paciente em andamento
    g_signal_connect(btn_casa, "clicked",
                     G_CALLBACK(on_voltar_inicio), state);
    gtk_box_pack_end(GTK_BOX(header), btn_casa, FALSE, FALSE, 8);

    gtk_box_pack_start(GTK_BOX(box_raiz), header, FALSE, FALSE, 0);

    // ── Área de conteúdo (scroll) ────────────────────────────────
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll, TRUE);

    GtkWidget *box_conteudo = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(box_conteudo, 20);
    gtk_widget_set_margin_start(box_conteudo, 40);
    gtk_widget_set_margin_end(box_conteudo, 40);
    gtk_widget_set_margin_bottom(box_conteudo, 20);

    // ── Seção CADASTRO ───────────────────────────────────────────
    GtkWidget *lbl_cadastro = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_cadastro),
                         "<b>CADASTRO</b>");
    aplicar_css(lbl_cadastro,
                "label { background:#CCCCCC; padding:6px; }");
    gtk_widget_set_halign(lbl_cadastro, GTK_ALIGN_FILL);
    gtk_box_pack_start(GTK_BOX(box_conteudo), lbl_cadastro, FALSE, FALSE, 0);

    // Nome
    ctx->entry_nome = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_nome), "Nome Completo:");
    gtk_box_pack_start(GTK_BOX(box_conteudo), ctx->entry_nome, FALSE, FALSE, 0);

    // RG + Data de Nascimento na mesma linha
    GtkWidget *linha_rg_nasc = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ctx->entry_rg = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_rg), "RG:");
    gtk_widget_set_hexpand(ctx->entry_rg, TRUE);

    ctx->entry_nasc = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_nasc), "Data de Nascimento:");
    gtk_widget_set_hexpand(ctx->entry_nasc, TRUE);

    gtk_box_pack_start(GTK_BOX(linha_rg_nasc), ctx->entry_rg, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(linha_rg_nasc), ctx->entry_nasc, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_conteudo), linha_rg_nasc, FALSE, FALSE, 0);

    // CPF + botão confirmar na mesma linha
    GtkWidget *linha_cpf_btn = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    ctx->entry_cpf = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_cpf), "CPF:");
    gtk_widget_set_hexpand(ctx->entry_cpf, TRUE);

    ctx->entry_idade = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->entry_idade), "Idade:");
    gtk_widget_set_size_request(ctx->entry_idade, 80, -1);
    ctx->btn_confirmar = gtk_button_new_with_label("APERTE PARA CONFIRMAR");
    gtk_widget_set_size_request(ctx->btn_confirmar, 200, -1);
    aplicar_css(ctx->btn_confirmar,
                "button { background:#CCCCCC; font-weight:bold; font-size:12px;"
                "         border-radius:3px; border:1px solid #AAAAAA; }"
                "button:hover { background:#BBBBBB; }");
    g_signal_connect(ctx->btn_confirmar, "clicked",
                     G_CALLBACK(on_triagem_confirmar), ctx);

    gtk_box_pack_start(GTK_BOX(linha_cpf_btn), ctx->entry_cpf, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(linha_cpf_btn), ctx->entry_idade, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(linha_cpf_btn), ctx->btn_confirmar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_conteudo), linha_cpf_btn, FALSE, FALSE, 0);

    // ── Seção PERGUNTAS (oculta inicialmente) ────────────────────
    ctx->box_perguntas = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(ctx->box_perguntas, 8);

    ctx->label_pergunta = gtk_label_new("");
    gtk_widget_set_halign(ctx->label_pergunta, GTK_ALIGN_CENTER);
    aplicar_css(ctx->label_pergunta,
                "label { background:#EEEEEE; padding:10px; font-size:13px; }");
    gtk_box_pack_start(GTK_BOX(ctx->box_perguntas),
                       ctx->label_pergunta, FALSE, FALSE, 0);

    GtkWidget *linha_sim_nao = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(linha_sim_nao, GTK_ALIGN_CENTER);

    ctx->btn_sim = gtk_button_new_with_label("SIM");
    gtk_widget_set_size_request(ctx->btn_sim, 160, 40);
    aplicar_css(ctx->btn_sim,
                "button { border:2px solid #2E7D32; color:#2E7D32; font-weight:bold;"
                "         background:white; border-radius:3px; }"
                "button:hover { background:#E8F5E9; }");
    g_signal_connect(ctx->btn_sim, "clicked",
                     G_CALLBACK(on_triagem_sim), ctx);

    ctx->btn_nao = gtk_button_new_with_label("NÃO");
    gtk_widget_set_size_request(ctx->btn_nao, 160, 40);
    aplicar_css(ctx->btn_nao,
                "button { border:2px solid #C62828; color:#C62828; font-weight:bold;"
                "         background:white; border-radius:3px; }"
                "button:hover { background:#FFEBEE; }");
    g_signal_connect(ctx->btn_nao, "clicked",
                     G_CALLBACK(on_triagem_nao), ctx);

    gtk_box_pack_start(GTK_BOX(linha_sim_nao), ctx->btn_sim, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(linha_sim_nao), ctx->btn_nao, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ctx->box_perguntas), linha_sim_nao, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_conteudo), ctx->box_perguntas, FALSE, FALSE, 0);

    // ── Seção RESULTADO (oculta inicialmente) ────────────────────
    ctx->box_resultado = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(ctx->box_resultado, 8);

    ctx->btn_concluir = gtk_button_new_with_label("TRIAGEM CONCLUÍDA");
    gtk_widget_set_size_request(ctx->btn_concluir, -1, 44);
    aplicar_css(ctx->btn_concluir,
                "button { background:#333333; color:white; font-weight:bold;"
                "         font-size:13px; border-radius:3px; border:none; }"
                "button:hover { background:#555555; }");
    g_signal_connect(ctx->btn_concluir, "clicked",
                     G_CALLBACK(on_triagem_concluida), ctx);

    ctx->btn_outro = gtk_button_new_with_label("APERTE PARA TRIAR OUTRO PACIENTE");
    gtk_widget_set_size_request(ctx->btn_outro, -1, 44);
    aplicar_css(ctx->btn_outro,
                "button { background:#888888; color:white; font-weight:bold;"
                "         font-size:13px; border-radius:3px; border:none; }"
                "button:hover { background:#666666; }");
    g_signal_connect(ctx->btn_outro, "clicked",
                     G_CALLBACK(on_triar_outro), ctx);

    gtk_box_pack_start(GTK_BOX(ctx->box_resultado), ctx->btn_concluir, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ctx->box_resultado), ctx->btn_outro, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_conteudo), ctx->box_resultado, FALSE, FALSE, 0);

    // Monta scroll
    gtk_container_add(GTK_CONTAINER(scroll), box_conteudo);
    gtk_box_pack_start(GTK_BOX(box_raiz), scroll, TRUE, TRUE, 0);

    // Estado inicial
    gtk_widget_hide(ctx->box_perguntas);
    gtk_widget_hide(ctx->box_resultado);
    state->box_perguntas_ref = ctx->box_perguntas;
    state->box_resultado_ref = ctx->box_resultado;

    return box_raiz;
}
//
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

        // Box horizontal para logo + nomes lado a lado
        GtkWidget *box_rodape = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
        gtk_widget_set_margin_top(box_rodape, 40);
        gtk_widget_set_margin_start(box_rodape, 20);
        gtk_widget_set_margin_bottom(box_rodape, 10);

        gtk_box_pack_start(GTK_BOX(box_rodape), logo, FALSE, FALSE, 0);

        // Nomes
        GtkWidget *nomes = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(nomes),
                             "<span font='Arial 11' foreground='#555555'>"
                             "Hiago de Oliveira Lima\n"
                             "Eduardo dos Santos Rodrigues"
                             "</span>");
        gtk_widget_set_valign(nomes, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(box_rodape), nomes, FALSE, FALSE, 0);

        gtk_box_pack_end(GTK_BOX(box), box_rodape, FALSE, FALSE, 0);
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
                        criar_tela_triagem(state), "tela_triagem");
    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_historico(state), "tela_historico");
    gtk_stack_add_named(GTK_STACK(state->stack),
                        criar_tela_chamada(state), "tela_chamada");

    gtk_stack_set_visible_child_name(GTK_STACK(state->stack), "tela_inicial");

    gtk_widget_show_all(window);
    if (state->box_perguntas_ref)
        gtk_widget_hide(state->box_perguntas_ref);
    if (state->box_resultado_ref)
        gtk_widget_hide(state->box_resultado_ref);
    gtk_main();
}