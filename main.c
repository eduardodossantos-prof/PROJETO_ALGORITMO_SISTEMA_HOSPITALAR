#include <gtk/gtk.h>

gboolean desenhar_quadrado(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, 0.85, 0.85, 0.85);
    cairo_rectangle(cr, 100, 50, 300, 220);
    cairo_fill(cr);
    return FALSE;
}

int main(int argc, char *argv[]) {
    //Variável auxiliar
    GtkWidget *window;
    GtkWidget *janela_logo; //Adicionando uma váriavel auxiliar para logo
    GtkWidget *fixed; 
    GtkWidget *area_desenho;
    GtkWidget *area_desenho_2;
    GtkWidget *area_desenho_3;

    //Variáveis para mexer na memória da imagem;
    GdkPixbuf *imagem_original;
    GdkPixbuf *imagem_redimensionada;
    GError *erro = NULL;

    gtk_init(&argc, &argv);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    //criando minha imagem + identificação de erro
    imagem_original = gdk_pixbuf_new_from_file("univasf_logo.png", &erro);

    //Identificação de erro do carregamento da imagem + redimensionamento da imagem
    if (erro != NULL){
        g_print("Erro ao carregar a imagem");
        g_error_free(erro);
    }else{
        imagem_redimensionada = gdk_pixbuf_scale_simple(imagem_original, 120, 60, GDK_INTERP_BILINEAR);
        fixed = gtk_fixed_new();
        janela_logo = gtk_image_new_from_pixbuf(imagem_redimensionada);

        //Limpar os pixbufs da memória, importante para não tracar o PC!
        g_object_unref(imagem_original);
        g_object_unref(imagem_redimensionada);

        //Adicionar a imagem redimensionada na tela
        gtk_container_add(GTK_CONTAINER(window), fixed);
        gtk_fixed_put(GTK_FIXED(fixed), janela_logo, 20, 670);
        gtk_container_add(GTK_CONTAINER(window), janela_logo);
    }

    //Cria a área em branco para desenhar  
    area_desenho = gtk_drawing_area_new();
    area_desenho_2 = gtk_drawing_area_new();
    area_desenho_3 = gtk_drawing_area_new();
    //Define a largura e a altura máxima que a área vai ocupar
    gtk_widget_set_size_request(area_desenho, 2000, 2000);
    gtk_widget_set_size_request(area_desenho_2, 2000, 2000);
    gtk_widget_set_size_request(area_desenho_3, 2000, 2000);
    //Conectar o evento "draw" desenhar com a função "desenhar_quadrado" 
    g_signal_connect(area_desenho, "draw", G_CALLBACK(desenhar_quadrado), NULL);    
    g_signal_connect(area_desenho_2, "draw", G_CALLBACK(desenhar_quadrado), NULL);
    g_signal_connect(area_desenho_3, "draw", G_CALLBACK(desenhar_quadrado), NULL);
    //Coloca o quadrado na tela usando o GtkFixed
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho, 80, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_2, 430, 180);
    gtk_fixed_put(GTK_FIXED(fixed), area_desenho_3, 780, 180);
    // 3. MOSTRA A JANELA NA TELA (Sempre deve ser a última coisa antes do loop)
    gtk_widget_show_all(window);

    // Inicia o loop principal (fica esperando você clicar em algo)
    gtk_main();

    return 0;
}