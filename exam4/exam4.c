#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 5

// 뮤텍스와 조건 변수 선언
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

// 메시지 버퍼
char messages[MAX_CLIENTS][256];
int num_clients = 0;

// 클라이언트 쓰레드 함수
void *client_thread(void *arg) {
    int client_id = *(int *)arg;
    free(arg);

    while (1) {
        // 클라이언트 쓰레드가 메시지 전송을 요청할 때까지 대기
        pthread_mutex_lock(&mutex);
        while (num_clients == 0) {
            pthread_cond_wait(&cond_var, &mutex);
        }

        // 메시지를 출력하고 메시지 버퍼 비우기
        printf("클라이언트 %d: %s\n", client_id, messages[client_id]);
        messages[client_id][0] = '\0';
        num_clients--;

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

// 부모(서버) 쓰레드 함수
void *server_thread(void *arg) {
    while (1) {
        // 메시지 입력 받기
        char message[256];
        printf("서버: ");
        fgets(message, sizeof(message), stdin);

        // 메시지를 모든 클라이언트에게 방송
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (messages[i][0] == '\0') {
                snprintf(messages[i], sizeof(messages[i]), "%s", message);
                num_clients++;
            }
        }

        // 클라이언트 쓰레드에게 메시지 전송을 알리기
        pthread_cond_broadcast(&cond_var);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    pthread_t server, clients[MAX_CLIENTS];

    // 부모(서버) 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        int *client_id = (int *)malloc(sizeof(int));
        *client_id = i;
        pthread_create(&clients[i], NULL, client_thread, client_id);
    }

    // 쓰레드 종료 대기
    pthread_join(server, NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }

    return 0;
}

