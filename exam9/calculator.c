#include <gtk/gtk.h>

// 계산기의 상태를 나타내는 열거형
typedef enum {
    OPERATOR_NONE,
    OPERATOR_ADD,
    OPERATOR_SUBTRACT,
    OPERATOR_MULTIPLY,
    OPERATOR_DIVIDE
} Operator;

// 계산기 구조체
typedef struct {
    GtkWidget *entry;  // 표시창
    double result;     // 현재 결과
    double operand;    // 현재 입력된 피연산자
    Operator operator; // 현재 선택된 연산자
} CalculatorData;

// 숫자 버튼이 클릭될 때 호출되는 콜백 함수
static void on_number_button_clicked(GtkWidget *widget, gpointer data) {
    CalculatorData *calc_data = (CalculatorData *)data;

    // 클릭된 버튼의 레이블(숫자)을 가져옴
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    // 현재 표시된 텍스트를 가져옴
    const char *current_text = gtk_entry_get_text(GTK_ENTRY(calc_data->entry));

    // 숫자 버튼이 클릭되면 현재 텍스트에 숫자를 추가
    char new_text[50];
    snprintf(new_text, sizeof(new_text), "%s%s", current_text, button_label);
    gtk_entry_set_text(GTK_ENTRY(calc_data->entry), new_text);
}

// 연산자 버튼이 클릭될 때 호출되는 콜백 함수
static void on_operator_button_clicked(GtkWidget *widget, gpointer data) {
    CalculatorData *calc_data = (CalculatorData *)data;

    // 현재 표시된 텍스트를 가져와서 피연산자로 변환
    const char *current_text = gtk_entry_get_text(GTK_ENTRY(calc_data->entry));
    sscanf(current_text, "%lf", &(calc_data->operand));

    // 현재 연산자를 설정
    const char *button_label = gtk_button_get_label(GTK_BUTTON(widget));
    if (strcmp(button_label, "+") == 0) {
        calc_data->operator = OPERATOR_ADD;
    } else if (strcmp(button_label, "-") == 0) {
        calc_data->operator = OPERATOR_SUBTRACT;
    } else if (strcmp(button_label, "*") == 0) {
        calc_data->operator = OPERATOR_MULTIPLY;
    } else if (strcmp(button_label, "/") == 0) {
        calc_data->operator = OPERATOR_DIVIDE;
    }

    // 표시창을 비움
    gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "");
}

// 등호 버튼이 클릭될 때 호출되는 콜백 함수
static void on_equal_button_clicked(GtkWidget *widget, gpointer data) {
    CalculatorData *calc_data = (CalculatorData *)data;

    // 현재 표시된 텍스트를 가져와서 피연산자로 변환
    const char *current_text = gtk_entry_get_text(GTK_ENTRY(calc_data->entry));
    sscanf(current_text, "%lf", &(calc_data->operand));

    // 현재 설정된 연산자에 따라 계산 수행
    switch (calc_data->operator) {
        case OPERATOR_ADD:
            calc_data->result = calc_data->result + calc_data->operand;
            break;
        case OPERATOR_SUBTRACT:
            calc_data->result = calc_data->result - calc_data->operand;
            break;
        case OPERATOR_MULTIPLY:
            calc_data->result = calc_data->result * calc_data->operand;
            break;
        case OPERATOR_DIVIDE:
            if (calc_data->operand != 0) {
                calc_data->result = calc_data->result / calc_data->operand;
            } else {
                // 0으로 나누는 경우 에러 처리
                gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "Error");
                return;
            }
            break;
        case OPERATOR_NONE:
            // 연산자가 선택되지 않은 경우
            calc_data->result = calc_data->operand;
            break;
    }

    // 결과를 표시창에 표시
    char result_text[50];
    snprintf(result_text, sizeof(result_text), "%f", calc_data->result);
    gtk_entry_set_text(GTK_ENTRY(calc_data->entry), result_text);

    // 현재 연산자를 초기화
    calc_data->operator = OPERATOR_NONE;
}

// C 버튼이 클릭될 때 호출되는 콜백 함수 (Clear)
static void on_clear_button_clicked(GtkWidget *widget, gpointer data) {
    CalculatorData *calc_data = (CalculatorData *)data;

    // 계산기 데이터 초기화
    calc_data->result = 0;
    calc_data->operand = 0;
    calc_data->operator = OPERATOR_NONE;

    // 표시창을 비움
    gtk_entry_set_text(GTK_ENTRY(calc_data->entry), "");
}

int main(int argc, char **argv) {
    // GTK 애플리케이션 초기화
    gtk_init(&argc, &argv);

    // 계산기 데이터 초기화
    CalculatorData calc_data;
    calc_data.result = 0;
    calc_data.operand = 0;
    calc_data.operator = OPERATOR_NONE;

    // 윈도우 및 레이아웃 생성
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *grid = gtk_grid_new();

    // 표시창 생성
    calc_data.entry = gtk_entry_new();
    gtk_widget_set_hexpand(calc_data.entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), calc_data.entry, 0, 0, 4, 1);

    // 버튼 생성 및 이벤트 핸들러 연결
    const char *button_labels[] = {"7", "8", "9", "/",
                                   "4", "5", "6", "*",
                                   "1", "2", "3", "-",
                                   "0", ".", "=", "+"};
    for (int i = 0; i < 16; ++i) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_number_button_clicked), &calc_data);

        if (i == 11 || i == 14) {
            // 연산자 버튼
            g_signal_connect(button, "clicked", G_CALLBACK(on_operator_button_clicked), &calc_data);
        } else if (i == 15) {
            // 등호 버튼
            g_signal_connect(button, "clicked", G_CALLBACK(on_equal_button_clicked), &calc_data);
        }

        gtk_grid_attach(GTK_GRID(grid), button, i % 4, 1 + i / 4, 1, 1);
    }

    // C 버튼 (Clear)
    GtkWidget *clear_button = gtk_button_new_with_label("C");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), &calc_data);
    gtk_grid_attach(GTK_GRID(grid), clear_button, 3, 5, 1, 1);

    // 윈도우 설정
    gtk_container_add(GTK_CONTAINER(window), grid);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // 윈도우를 화면에 표시
    gtk_widget_show_all(window);

    // GTK 메인 루프 실행
    gtk_main();

    return 0;
}

