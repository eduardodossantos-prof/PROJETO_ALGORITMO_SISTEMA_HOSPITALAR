#include <gtk/gtk.h>

GtkWidget* criar_modulo_consulta(void);

gboolean ao_clicar_no_quadrado(GtkWidget *widget, GdkEventButton *event, gpointer data){
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "tela_triagem");
    g_print("Trocando para a tela de triagem... \n");
    return TRUE;
}
gboolean ao_clicar_no_quadrado_2(GtkWidget *widget, GdkEventButton *event, gpointer data){
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "tela_consulta");
    g_print("Trocando para a tela de triagem... \n");
    return TRUE;
}
gboolean desenhar_quadrado(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    cairo_rectangle(cr, 100, 50, 300, 220);
    cairo_fill(cr);
    return FALSE;
}
gboolean desenhar_quadrado_triagem(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    // Retângulo vai do x=100 até 400, e do y=50 até 270
    cairo_rectangle(cr, 100, 50, 3000, 50);
    cairo_fill(cr);
    return FALSE;
}

GtkWidget* criar_modulo_triagem(GtkWidget *stack_principal){
    GtkWidget *fixed;
    GtkWidget *titulo_triagem;
    GtkWidget *label_nome, *entry_name;
    GtkWidget *label_rg, *entry_rg;
    GtkWidget *label_cpf, *entry_cpf;
    GtkWidget *label_data_de_nascimento, *entry_data_de_nascimento;
    GtkWidget *label_descricao, *entry_descricao;
    GtkWidget *label_classificao, *entry_classificacao;
    GtkWidget *proximo_triagem, *name_triagem, *stack_1, *tela_consulta;
    GtkWidget *event_box;

    fixed = gtk_fixed_new();

    titulo_triagem = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_triagem),
    "<span face='Arial' size='30000' foreground='#000000' ><b>Serviço de Atendimento Hospitalar</b></span>");
    gtk_fixed_put(GTK_FIXED(fixed), titulo_triagem, 380, 60);

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

    entry_descricao = gtk_entry_new();
    gtk_widget_set_size_request(entry_descricao, 1000, 200);  
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_descricao), "DESCRICAO:");
    gtk_fixed_put(GTK_FIXED(fixed), entry_descricao, 200, 320);

    entry_classificacao = gtk_combo_box_text_new();
    gtk_widget_set_size_request(entry_classificacao, 1000, -1);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry_classificacao), "Verde");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry_classificacao), "Amarelo");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry_classificacao), "Vermelho");
    gtk_combo_box_set_active(GTK_COMBO_BOX(entry_classificacao), 0);
    gtk_fixed_put(GTK_FIXED(fixed), entry_classificacao, 200, 260);

    proximo_triagem = gtk_drawing_area_new();
    gtk_widget_set_size_request(proximo_triagem, 400, 200);
    g_signal_connect(proximo_triagem, "draw", G_CALLBACK(desenhar_quadrado_triagem), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), proximo_triagem, 450, 520);
    gtk_widget_add_events(proximo_triagem, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(proximo_triagem, "button-press-event", G_CALLBACK(ao_clicar_no_quadrado_2), stack_principal);

    titulo_triagem = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_triagem),
    "<span face='Arial' size='20000' foreground='#000000' ><b>Próximo</b></span>");
    gtk_fixed_put(GTK_FIXED(fixed), titulo_triagem, 650, 580);

    return fixed;
}

GtkWidget* criar_modulo_consulta(){
   GtkWidget *fixed2;
    GtkWidget *frame_titulo;
    GtkWidget *label_titulo;

    fixed2 = gtk_fixed_new();

    // 1. BARRA DE TÍTULO ("Chamada")
    frame_titulo = gtk_frame_new(NULL);
    // Dimensões: Largura de 800px, Altura de 50px
    gtk_widget_set_size_request(frame_titulo, 800, 50);
    // Centralizando na janela de 1200px (x = 200)
    gtk_fixed_put(GTK_FIXED(fixed2), frame_titulo, 200, 50);

    label_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_titulo), 
        "<span face='Arial' size='20000' foreground='#000000'><b>Tela de Chamada</b></span>");
    gtk_container_add(GTK_CONTAINER(frame_titulo), label_titulo);

    // 2. BLOCOS DE PACIENTES
    // Utilizando um loop for para criar os 3 blocos idênticos e economizar código
    for (int i = 0; i < 3; i++) {
        // Calcula a posição vertical (Y) de cada bloco: 120, 260, 400
        int y_offset = 120 + (i * 140); 

        // Borda do bloco inteiro (Retângulo externo)
        GtkWidget *frame_bloco = gtk_frame_new(NULL);
        gtk_widget_set_size_request(frame_bloco, 800, 120);
        gtk_fixed_put(GTK_FIXED(fixed2), frame_bloco, 200, y_offset);

        // Container interno do bloco (para usarmos coordenadas X e Y relativas ao bloco)
        GtkWidget *fixed_interno = gtk_fixed_new();
        gtk_container_add(GTK_CONTAINER(frame_bloco), fixed_interno);

        // Linha divisória vertical (Separando a coluna "Paciente" da coluna "Senha")
        GtkWidget *separador = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
        gtk_widget_set_size_request(separador, 2, 120);
        // Posicionada no X = 550 (Deixa mais espaço para o nome do paciente)
        gtk_fixed_put(GTK_FIXED(fixed_interno), separador, 550, 0); 

        // Textos do lado esquerdo (Paciente e Classificação)
        GtkWidget *label_paciente = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label_paciente), 
            "<span face='Arial' size='12000' foreground='#000000'>Paciente:</span>");
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_paciente, 10, 10);

        GtkWidget *label_classificacao = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label_classificacao), 
            "<span face='Arial' size='12000' foreground='#000000'>Classificação: Verde/Amarelo/Vermelho</span>");
        // Posicionado na parte inferior do bloco
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_classificacao, 10, 90);

        // Texto do lado direito (Senha)
        GtkWidget *label_senha = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label_senha), 
            "<span face='Arial' size='12000' foreground='#000000'>Senha:</span>");
        // Posicionado do lado direito da linha divisória
        gtk_fixed_put(GTK_FIXED(fixed_interno), label_senha, 560, 10);
    }

    return fixed2;
}


int main(int argc, char *argv[]) {
  
    GtkWidget *window;
    GtkWidget *janela_logo; 
    GtkWidget *fixed; 
    GtkWidget *tela_triagem;
    GtkWidget *area_desenho;
    GtkWidget *area_desenho_2;
    GtkWidget *area_desenho_3;
    GtkWidget *label;
    GtkWidget *stack;
    GdkPixbuf *imagem_original;
    GdkPixbuf *imagem_redimensionada;
    GtkWidget *tela_consulta;
    GError *erro = NULL;

    gtk_init(&argc, &argv);
    
    //Criação da janela da Haba hospitalar.
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);

    fixed = gtk_fixed_new();

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), 
        "<span face='Arial' size='30000' foreground='#000000'><b>SERVIÇO DE ATENDIMENTO HOSPITALAR</b></span>");
    gtk_fixed_put(GTK_FIXED(fixed), label, 300, 80); 

    imagem_original = gdk_pixbuf_new_from_file("univasf_logo.png", &erro);

    if (erro != NULL){
        g_print("Erro ao carregar a imagem: %s\n", erro->message);
        g_error_free(erro);
    } else {
        imagem_redimensionada = gdk_pixbuf_scale_simple(imagem_original, 120, 60, GDK_INTERP_BILINEAR);
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

    g_signal_connect(area_desenho, "draw", G_CALLBACK(desenhar_quadrado), NULL);    
    g_signal_connect(area_desenho_2, "draw", G_CALLBACK(desenhar_quadrado), NULL);
    g_signal_connect(area_desenho_3, "draw", G_CALLBACK(desenhar_quadrado), NULL);

    gtk_fixed_put(GTK_FIXED(fixed), area_desenho, 90, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_2, 440, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_3, 790, 180);

    g_signal_connect(area_desenho, "button-press-event", G_CALLBACK(ao_clicar_no_quadrado), stack);

    gtk_stack_add_named(GTK_STACK(stack), fixed, "tela_inicial");

    //Tela de Triagem
    tela_triagem = criar_modulo_triagem(stack);
    gtk_stack_add_named(GTK_STACK(stack), tela_triagem, "tela_triagem");

    tela_consulta = criar_modulo_consulta();
    gtk_stack_add_named(GTK_STACK(stack), tela_consulta, "tela_consulta");

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "tela_inicial");

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}