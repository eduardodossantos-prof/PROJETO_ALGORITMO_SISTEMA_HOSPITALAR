#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    //Variável auxiliar
    GtkWidget *window;
    GtkWidget *logo_univasf;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema Hospitalar - Triagem");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 1. CRIA A LOGO
    logo_univasf = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(logo_univasf), "<span font='40' weight='bold' height='20'>UNIVASF</span>");
    gtk_widget_set_halign(logo_univasf, GTK_ALIGN_START);
    
    gtk_container_add(GTK_CONTAINER(window), logo_univasf);

    // 3. MOSTRA A JANELA NA TELA (Sempre deve ser a última coisa antes do loop)
    gtk_widget_show_all(window);

    // Inicia o loop principal (fica esperando você clicar em algo)
    gtk_main();

    return 0;
}