#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    //Variável auxiliar
    GtkWidget *window;
    GtkWidget *janela_logo; //Adicionando uma váriavel auxiliar para logo
    GtkWidget *fixed; 

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
        gtk_fixed_put(GTK_FIXED(fixed), janela_logo, 20, 530);
        gtk_container_add(GTK_CONTAINER(window), janela_logo);
    }

    // 3. MOSTRA A JANELA NA TELA (Sempre deve ser a última coisa antes do loop)
    gtk_widget_show_all(window);

    // Inicia o loop principal (fica esperando você clicar em algo)
    gtk_main();

    return 0;
}