#include <gtk/gtk.h>

gboolean desenhar_quadrado(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    // Retângulo vai do x=100 até 400, e do y=50 até 270
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

GtkWidget* criar_modulo_triagem(){
    GtkWidget *rectangle_information;
    GtkWidget *fixed;
    GtkWidget *titulo_triagem;
    GtkWidget *label_nome, *entry_name;

    fixed = gtk_fixed_new();
    rectangle_information = gtk_drawing_area_new();
    
    titulo_triagem = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(titulo_triagem), 
        "<span face='Arial' size='20000' foreground='#000000'><b>Serviço de Atendimento Hospitalar</b></span>");
    gtk_fixed_put(GTK_FIXED(fixed), titulo_triagem, 200, 40);

    label_nome = gtk_label_new("Nome Completo: ");
    gtk_fixed_put(GTK_FIXED(fixed), label_nome, 150, 150);
    entry_name = gtk_entry_new();
    gtk_widget_set_size_request(entry_name, 500, -1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_name), "Ex: Eduardo dos Santos");
    gtk_fixed_put(GTK_FIXED(fixed), entry_name, 250, 143);


    gtk_widget_set_size_request(rectangle_information, 1200, 300);
    g_signal_connect(rectangle_information, "draw", G_CALLBACK(desenhar_quadrado_triagem), NULL);
    gtk_fixed_put(GTK_FIXED(fixed), rectangle_information, 35, 10);
    
    return fixed;
}

gboolean ao_clicar_no_quadrado(GtkWidget *widget, GdkEventButton *event, gpointer data){
    GtkStack *stack = GTK_STACK(data);
    gtk_stack_set_visible_child_name(stack, "tela_triagem");
    g_print("Trocando para a tela de triagem... \n");
    return TRUE;
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
    GError *erro = NULL;

    gtk_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    
    // AJUSTE: Janela um pouco maior para acomodar os 3 blocos lado a lado com folga
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);

    fixed = gtk_fixed_new();

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), 
        "<span face='Arial' size='30000' foreground='#000000'><b>SERVIÇO DE ATENDIMENTO HOSPITALAR</b></span>");
    gtk_fixed_put(GTK_FIXED(fixed), label, 300, 50); // Subi um pouco o título (y=50)

    imagem_original = gdk_pixbuf_new_from_file("univasf_logo.png", &erro);

    if (erro != NULL){
        g_print("Erro ao carregar a imagem: %s\n", erro->message);
        g_error_free(erro);
    } else {
        imagem_redimensionada = gdk_pixbuf_scale_simple(imagem_original, 120, 60, GDK_INTERP_BILINEAR);
        janela_logo = gtk_image_new_from_pixbuf(imagem_redimensionada);

        g_object_unref(imagem_original);
        g_object_unref(imagem_redimensionada);

        // AJUSTE: Subi a logo para y=600 para garantir que fique visível na tela
        gtk_fixed_put(GTK_FIXED(fixed), janela_logo, 20, 600);
    }
  
    area_desenho = gtk_drawing_area_new();
    area_desenho_2 = gtk_drawing_area_new();
    area_desenho_3 = gtk_drawing_area_new();

    // CORREÇÃO CRUCIAL: Tamanho exato de cada área para evitar que se sobreponham
    gtk_widget_set_size_request(area_desenho, 400, 300);
    gtk_widget_set_size_request(area_desenho_2, 400, 300);
    gtk_widget_set_size_request(area_desenho_3, 400, 300);

    gtk_widget_add_events(area_desenho, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(area_desenho_2, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(area_desenho_3, GDK_BUTTON_PRESS_MASK);

    g_signal_connect(area_desenho, "draw", G_CALLBACK(desenhar_quadrado), NULL);    
    g_signal_connect(area_desenho_2, "draw", G_CALLBACK(desenhar_quadrado), NULL);
    g_signal_connect(area_desenho_3, "draw", G_CALLBACK(desenhar_quadrado), NULL);

    // Posicionando sem colisão (x=0, x=400, x=800)
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho, 0, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_2, 400, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_3, 800, 180);

    g_signal_connect(area_desenho, "button-press-event", G_CALLBACK(ao_clicar_no_quadrado), stack);

    gtk_stack_add_named(GTK_STACK(stack), fixed, "tela_inicial");

    // Tela de triagem
    tela_triagem = criar_modulo_triagem();
    gtk_stack_add_named(GTK_STACK(stack), tela_triagem, "tela_triagem");

    gtk_stack_set_visible_child_name(GTK_STACK(stack), "tela_inicial");

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}