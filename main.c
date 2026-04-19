#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    GtkWidget *window;

    // Inicializa o GTK
    gtk_init(&argc, &argv);

    // Cria uma nova janela
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    // Define o título e o tamanho inicial da janela
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Conecta o botão "X" (fechar) para encerrar o programa corretamente
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Mostra a janela na tela
    gtk_widget_show_all(window);

    // Inicia o loop principal (fica esperando você clicar em algo)
    gtk_main();

    return 0;
}