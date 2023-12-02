#include <gtk/gtk.h>

// "Hello, GTK!" 메시지를 출력하는 콜백 함수
static void print_hello(GtkWidget *widget, gpointer data) {
    g_print("Hello, GTK!\n");
}

// 프로그램 종료를 처리하는 콜백 함수
static void activate(GtkApplication *app, gpointer user_data) {
    // 윈도우 생성
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Hello, GTK!");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);

    // 버튼 생성
    GtkWidget *button = gtk_button_new_with_label("Click me!");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

    // 버튼을 윈도우에 추가
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    // 윈도우를 화면에 표시
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // GTK 애플리케이션 초기화
    GtkApplication *app = gtk_application_new("org.example.HelloGTK", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // 명령행 인자 처리 및 GTK 애플리케이션 실행
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // GTK 애플리케이션 해제
    g_object_unref(app);

    return status;
}

