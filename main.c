#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "include/paciente.h"
#include "include/arvore.h"

typedef struct {
    GtkWidget *entry_nome;
    GtkWidget *entry_rg;
    GtkWidget *entry_cpf;
    GtkWidget *entry_data_nascimento;
    GtkWidget *stack;

    GtkWidget *label_pergunta;
    GtkWidget *botao_sim;
    GtkWidget *botao_nao;

    Nodoarvore *raiz;
    Nodoarvore *atual;

    Paciente *paciente_atual;
} ContextoTriagem;

GtkWidget* criar_modulo_consulta(void);
GtkWidget* criar_modulo_triagem(GtkWidget *stack_principal);

/* ============================================================
   FUNÇÕES AUXILIARES DA TRIAGEM
   ============================================================ */

const char *nome_prioridade(int prioridade)
{
    switch (prioridade)
    {
        case PRIORIDADE_VERMELHA:
            return "VERMELHA";

        case PRIORIDADE_AMARELA:
            return "AMARELA";

        case PRIORIDADE_VERDE:
            return "VERDE";

        default:
            return "DESCONHECIDA";
    }
}

void atualizar_pergunta_triagem(ContextoTriagem *ctx)
{
    if (!ctx || !ctx->atual)
        return;

    if (ctx->atual->eh_folha)
    {
        aplicar_triagem(ctx->atual, ctx->paciente_atual);

        char texto[800];

        snprintf(
            texto,
            sizeof(texto),
            "Classificação final: %s\n\nJustificativa: %s",
            nome_prioridade(ctx->paciente_atual->prioridade),
            ctx->paciente_atual->justificativa
        );

        gtk_label_set_text(GTK_LABEL(ctx->label_pergunta), texto);

        gtk_widget_set_sensitive(ctx->botao_sim, FALSE);
        gtk_widget_set_sensitive(ctx->botao_nao, FALSE);

        g_print("\n--- TRIAGEM FINALIZADA ---\n");
        g_print("Paciente: %s\n", ctx->paciente_atual->nome);
        g_print("Prioridade: %s\n", nome_prioridade(ctx->paciente_atual->prioridade));
        g_print("Justificativa: %s\n", ctx->paciente_atual->justificativa);

        /*
           Se quiser ir automaticamente para a tela de consulta
           depois da classificação, descomente a linha abaixo:
        */

        /*
        gtk_stack_set_visible_child_name(
            GTK_STACK(ctx->stack),
            "tela_consulta"
        );
        */

        return;
    }

    gtk_label_set_text(GTK_LABEL(ctx->label_pergunta), ctx->atual->pergunta);

    gtk_widget_set_sensitive(ctx->botao_sim, TRUE);
    gtk_widget_set_sensitive(ctx->botao_nao, TRUE);
}

void ao_responder_triagem(GtkButton *button, gpointer data)
{
    ContextoTriagem *ctx = (ContextoTriagem *)data;

    if (!ctx)
        return;

    if (!ctx->paciente_atual)
    {
        gtk_label_set_text(
            GTK_LABEL(ctx->label_pergunta),
            "Primeiro preencha os dados do paciente e clique em Confirmar."
        );
        return;
    }

    if (!ctx->atual || ctx->atual->eh_folha)
        return;

    int resposta = GPOINTER_TO_INT(
        g_object_get_data(G_OBJECT(button), "resposta")
    );

    ctx->atual = avancar_no(ctx->atual, resposta);

    atualizar_pergunta_triagem(ctx);
}

/* ============================================================
   CLIQUES E DESENHOS
   ============================================================ */

gboolean ao_clicar_no_quadrado(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GtkStack *stack = GTK_STACK(data);

    gtk_stack_set_visible_child_name(stack, "tela_triagem");

    g_print("Trocando para a tela de triagem...\n");

    return TRUE;
}

gboolean ao_clicar_no_quadrado_2(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    ContextoTriagem *ctx = (ContextoTriagem *)data;

    if (!ctx)
        return TRUE;

    if (ctx->paciente_atual != NULL)
    {
        g_free(ctx->paciente_atual);
        ctx->paciente_atual = NULL;
    }

    Paciente *novo_paciente = g_new0(Paciente, 1);

    novo_paciente->next = NULL;

    strncpy(
        novo_paciente->nome,
        gtk_entry_get_text(GTK_ENTRY(ctx->entry_nome)),
        99
    );
    novo_paciente->nome[99] = '\0';

    strncpy(
        novo_paciente->cpf,
        gtk_entry_get_text(GTK_ENTRY(ctx->entry_cpf)),
        14
    );
    novo_paciente->cpf[14] = '\0';

    strncpy(
        novo_paciente->rg,
        gtk_entry_get_text(GTK_ENTRY(ctx->entry_rg)),
        14
    );
    novo_paciente->rg[14] = '\0';

    strncpy(
        novo_paciente->data_nascimento,
        gtk_entry_get_text(GTK_ENTRY(ctx->entry_data_nascimento)),
        10
    );
    novo_paciente->data_nascimento[10] = '\0';

    ctx->paciente_atual = novo_paciente;

    g_print("\n--- NOVO PACIENTE CADASTRADO ---\n");
    g_print("Nome: %s\n", novo_paciente->nome);
    g_print("CPF: %s\n", novo_paciente->cpf);
    g_print("RG: %s\n", novo_paciente->rg);
    g_print("Data de nascimento: %s\n", novo_paciente->data_nascimento);

    /*
       Reinicia a triagem sempre pela raiz da árvore.
    */
    ctx->atual = ctx->raiz;

    atualizar_pergunta_triagem(ctx);

    return TRUE;
}

gboolean desenhar_quadrado(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    cairo_rectangle(cr, 100, 50, 1000, 1000);
    cairo_fill(cr);

    return FALSE;
}

gboolean desenhar_quadrado_triagem(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    cairo_rectangle(cr, 100, 50, 3000, 50);
    cairo_fill(cr);

    return FALSE;
}

gboolean desenhar_quadrado_perguntas(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    cairo_rectangle(cr, 0, 0, 1000, 200);
    cairo_fill(cr);

    return FALSE;
}

/* ============================================================
   TELA DE TRIAGEM
   ============================================================ */

GtkWidget* criar_modulo_triagem(GtkWidget *stack_principal)
{
    GtkWidget *fixed;

    GtkWidget *titulo_triagem;

    GtkWidget *entry_name;
    GtkWidget *entry_rg;
    GtkWidget *entry_cpf;
    GtkWidget *entry_data_de_nascimento;

    GtkWidget *proximo_triagem;

    GtkWidget *quadrado_perguntas;
    GtkWidget *label_pergunta;

    GtkWidget *botao_sim;
    GtkWidget *botao_nao;

    fixed = gtk_fixed_new();

    titulo_triagem = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(titulo_triagem),
        "<span face='Arial' size='30000' foreground='#000000'><b>Tela de Triagem</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), titulo_triagem, 560, 60);

    entry_name = gtk_entry_new();
    gtk_widget_set_size_request(entry_name, 1000, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_name), "NOME COMPLETO:");
    gtk_fixed_put(GTK_FIXED(fixed), entry_name, 200, 143);

    entry_rg = gtk_entry_new();
    gtk_widget_set_size_request(entry_rg, 300, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_rg), "RG:");
    gtk_fixed_put(GTK_FIXED(fixed), entry_rg, 200, 200);

    entry_cpf = gtk_entry_new();
    gtk_widget_set_size_request(entry_cpf, 300, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_cpf), "CPF:");
    gtk_fixed_put(GTK_FIXED(fixed), entry_cpf, 525, 200);

    entry_data_de_nascimento = gtk_entry_new();
    gtk_widget_set_size_request(entry_data_de_nascimento, 350, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_data_de_nascimento), "DATA DE NASCIMENTO:");
    gtk_fixed_put(GTK_FIXED(fixed), entry_data_de_nascimento, 850, 200);

    /*
       Quadrado do botão Confirmar
    */
    proximo_triagem = gtk_drawing_area_new();
    gtk_widget_set_size_request(proximo_triagem, 400, 200);
    g_signal_connect(
        proximo_triagem,
        "draw",
        G_CALLBACK(desenhar_quadrado_triagem),
        NULL
    );
    gtk_fixed_put(GTK_FIXED(fixed), proximo_triagem, 450, 220);
    gtk_widget_add_events(proximo_triagem, GDK_BUTTON_PRESS_MASK);

    GtkWidget *label_confirmar = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(label_confirmar),
        "<span face='Arial' size='20000' foreground='#000000'><b>Confirmar</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), label_confirmar, 640, 280);

    /*
       Quadrado onde aparece a pergunta
    */
    quadrado_perguntas = gtk_drawing_area_new();
    gtk_widget_set_size_request(quadrado_perguntas, 1000, 200);
    g_signal_connect(
        quadrado_perguntas,
        "draw",
        G_CALLBACK(desenhar_quadrado_perguntas),
        NULL
    );
    gtk_fixed_put(GTK_FIXED(fixed), quadrado_perguntas, 200, 350);

    /*
       Texto da pergunta dentro do quadrado
    */
    label_pergunta = gtk_label_new(
        "Preencha os dados do paciente e clique em Confirmar."
    );

    gtk_widget_set_size_request(label_pergunta, 900, 150);
    gtk_label_set_line_wrap(GTK_LABEL(label_pergunta), TRUE);
    gtk_label_set_justify(GTK_LABEL(label_pergunta), GTK_JUSTIFY_CENTER);

    gtk_fixed_put(GTK_FIXED(fixed), label_pergunta, 250, 380);

    /*
       Botão SIM
    */
    botao_sim = gtk_button_new_with_label("SIM");
    gtk_widget_set_size_request(botao_sim, 300, 50);
    gtk_fixed_put(GTK_FIXED(fixed), botao_sim, 300, 600);

    /*
       Botão NÃO
    */
    botao_nao = gtk_button_new_with_label("NÃO");
    gtk_widget_set_size_request(botao_nao, 300, 50);
    gtk_fixed_put(GTK_FIXED(fixed), botao_nao, 800, 600);

    gtk_widget_set_sensitive(botao_sim, FALSE);
    gtk_widget_set_sensitive(botao_nao, FALSE);

    /*
       Contexto da triagem
    */
    ContextoTriagem *contexto_triagem = g_new0(ContextoTriagem, 1);

    contexto_triagem->entry_nome = entry_name;
    contexto_triagem->entry_cpf = entry_cpf;
    contexto_triagem->entry_rg = entry_rg;
    contexto_triagem->entry_data_nascimento = entry_data_de_nascimento;
    contexto_triagem->stack = stack_principal;

    contexto_triagem->label_pergunta = label_pergunta;
    contexto_triagem->botao_sim = botao_sim;
    contexto_triagem->botao_nao = botao_nao;

    contexto_triagem->raiz = construir_arvore();
    contexto_triagem->atual = contexto_triagem->raiz;
    contexto_triagem->paciente_atual = NULL;

    /*
       Clique no Confirmar
    */
    g_signal_connect(
        proximo_triagem,
        "button-press-event",
        G_CALLBACK(ao_clicar_no_quadrado_2),
        contexto_triagem
    );

    /*
       Clique no SIM
    */
    g_object_set_data(
        G_OBJECT(botao_sim),
        "resposta",
        GINT_TO_POINTER(1)
    );

    g_signal_connect(
        botao_sim,
        "clicked",
        G_CALLBACK(ao_responder_triagem),
        contexto_triagem
    );

    /*
       Clique no NÃO
    */
    g_object_set_data(
        G_OBJECT(botao_nao),
        "resposta",
        GINT_TO_POINTER(0)
    );

    g_signal_connect(
        botao_nao,
        "clicked",
        G_CALLBACK(ao_responder_triagem),
        contexto_triagem
    );

    return fixed;
}

/* ============================================================
   TELA DE CONSULTA
   ============================================================ */

GtkWidget* criar_modulo_consulta()
{
    GtkWidget *fixed2;
    GtkWidget *frame_titulo;
    GtkWidget *label_titulo;

    fixed2 = gtk_fixed_new();

    frame_titulo = gtk_frame_new(NULL);
    gtk_widget_set_size_request(frame_titulo, 800, 50);
    gtk_fixed_put(GTK_FIXED(fixed2), frame_titulo, 200, 50);

    label_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(label_titulo),
        "<span face='Arial' size='20000' foreground='#000000'><b>Tela de Chamada</b></span>"
    );
    gtk_container_add(GTK_CONTAINER(frame_titulo), label_titulo);

    for (int i = 0; i < 3; i++)
    {
        int y_offset = 120 + (i * 140);

        GtkWidget *frame_bloco = gtk_frame_new(NULL);
        gtk_widget_set_size_request(frame_bloco, 800, 120);
        gtk_fixed_put(GTK_FIXED(fixed2), frame_bloco, 200, y_offset);

        GtkWidget *fixed_interno = gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(frame_bloco), fixed_interno);

        GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
        gtk_widget_set_size_request(separador, 2, 120);
        gtk_fixed_put(GTK_FIXED(fixed_interno), separador, 550, 0);

        GtkWidget *label_paciente = gtk_label_new(NULL);
        gtk_label_set_markup(
            GTK_LABEL(label_paciente),
            "<span face='Arial' size='12000' foreground='#000000'>Paciente:</span>"
        );
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_paciente, 10, 10);

        GtkWidget *label_classificacao = gtk_label_new(NULL);
        gtk_label_set_markup(
            GTK_LABEL(label_classificacao),
            "<span face='Arial' size='12000' foreground='#000000'>Classificação: Verde/Amarelo/Vermelho</span>"
        );
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_classificacao, 10, 90);

        GtkWidget *label_senha = gtk_label_new(NULL);
        gtk_label_set_markup(
            GTK_LABEL(label_senha),
            "<span face='Arial' size='12000' foreground='#000000'>Senha:</span>"
        );
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_senha, 560, 10);
    }

    return fixed2;
}

/* ============================================================
   MAIN
   ============================================================ */

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *janela_logo;
    GtkWidget *fixed;
    GtkWidget *tela_triagem;
    GtkWidget *area_desenho;
    GtkWidget *area_desenho_2;
    GtkWidget *area_desenho_3;
    GtkWidget *label;
    GtkWidget *stack;
    GtkWidget *name_label_triagem;
    GtkWidget *name_label_historico;
    GtkWidget *name_label_configuracoes;
    GtkWidget *tela_consulta;

    GdkPixbuf *imagem_original;
    GdkPixbuf *imagem_redimensionada;
    GError *erro = NULL;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 700);

    g_signal_connect(
        window,
        "destroy",
        G_CALLBACK(gtk_main_quit),
        NULL
    );

    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);

    fixed = gtk_fixed_new();

    label = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(label),
        "<span face='Arial' size='30000' weight='bold' foreground='#000000'><b>SERVIÇO DE ATENDIMENTO HOSPITALAR</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), label, 300, 80);

    imagem_original = gdk_pixbuf_new_from_file("univasf_logo.png", &erro);

    if (erro != NULL)
    {
        g_print("Erro ao carregar a imagem: %s\n", erro->message);
        g_error_free(erro);
    }
    else
    {
        imagem_redimensionada = gdk_pixbuf_scale_simple(
            imagem_original,
            120,
            60,
            GDK_INTERP_BILINEAR
        );

        janela_logo = gtk_image_new_from_pixbuf(imagem_redimensionada);

        g_object_unref(imagem_original);
        g_object_unref(imagem_redimensionada);

        gtk_fixed_put(GTK_FIXED(fixed), janela_logo, 20, 660);
    }

    area_desenho = gtk_drawing_area_new();
    area_desenho_2 = gtk_drawing_area_new();
    area_desenho_3 = gtk_drawing_area_new();

    gtk_widget_set_size_request(area_desenho, 400, 300);
    gtk_widget_set_size_request(area_desenho_2, 400, 300);
    gtk_widget_set_size_request(area_desenho_3, 400, 300);

    gtk_widget_add_events(area_desenho, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(area_desenho_2, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(area_desenho_3, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(
        area_desenho,
        "draw",
        G_CALLBACK(desenhar_quadrado),
        NULL
    );

    g_signal_connect(
        area_desenho_2,
        "draw",
        G_CALLBACK(desenhar_quadrado),
        NULL
    );

    g_signal_connect(
        area_desenho_3,
        "draw",
        G_CALLBACK(desenhar_quadrado),
        NULL
    );

    gtk_fixed_put(GTK_FIXED(fixed), area_desenho, 90, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_2, 440, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_3, 790, 180);

    name_label_triagem = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(name_label_triagem),
        "<span face='Arial' size='20500' foreground='#000000'><b>Tela de Triagem</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), name_label_triagem, 240, 320);

    name_label_historico = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(name_label_historico),
        "<span face='Arial' size='20500' foreground='#000000'><b>Tela de histórico</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), name_label_historico, 585, 320);

    name_label_configuracoes = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(name_label_configuracoes),
        "<span face='Arial' size='20000' foreground='#000000'><b>Tela de Configurações</b></span>"
    );
    gtk_fixed_put(GTK_FIXED(fixed), name_label_configuracoes, 905, 320);

    g_signal_connect(
        area_desenho,
        "button-press-event",
        G_CALLBACK(ao_clicar_no_quadrado),
        stack
    );

    gtk_stack_add_named(GTK_STACK(stack), fixed, "tela_inicial");

    tela_triagem = criar_modulo_triagem(stack);
    gtk_stack_add_named(GTK_STACK(stack), tela_triagem, "tela_triagem");

    tela_consulta = criar_modulo_consulta();
    gtk_stack_add_named(GTK_STACK(stack), tela_consulta, "tela_consulta");

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "tela_inicial");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}