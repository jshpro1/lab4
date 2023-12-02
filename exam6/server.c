#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

void handle_client(int client_socket, int *clients) {
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[read_size] = '\0';
        printf("클라이언트: %s", buffer);

        // 수신한 메시지를 모든 클라이언트에게 방송
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i] != 0 && clients[i] != client_socket) {
                send(clients[i], buffer, read_size, 0);
            }
        }
    }

    // 클라이언트 연결이 끊긴 경우
    printf("클라이언트가 연결을 종료했습니다.\n");
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 클라이언트 소켓 배열 초기화
    int clients[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = 0;
    }

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓을 주소에 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 연결 대기 상태 진입
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("대기열 설정 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버가 클라이언트 연결을 대기 중입니다...\n");

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("클라이언트 연결 수락 실패");
            exit(EXIT_FAILURE);
        }

        printf("클라이언트가 연결되었습니다.\n");

        // 클라이언트를 배열에 추가
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i] == 0) {
                clients[i] = client_socket;
                break;
            }
        }

        // 새로운 프로세스를 생성하여 클라이언트 처리
        if (fork() == 0) {
            close(server_socket);  // 자식 프로세스에서는 서버 소켓을 사용하지 않음
            handle_client(client_socket, clients);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);  // 부모 프로세스에서는 클라이언트 소켓을 사용하지 않음
        }

        // 클라이언트 배열에서 소켓 제거
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (clients[i] == client_socket) {
                clients[i] = 0;
                break;
            }
        }
    }

    // 부모 프로세스에서는 소켓 닫기
    close(server_socket);

    return 0;
}

