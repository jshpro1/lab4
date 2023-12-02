#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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
    if (listen(server_socket, 5) == -1) {
        perror("대기열 설정 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버가 클라이언트 연결을 대기 중입니다...\n");

    // 클라이언트 연결 수락
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("클라이언트 연결 수락 실패");
        exit(EXIT_FAILURE);
    }

    printf("클라이언트가 연결되었습니다.\n");

    // 클라이언트로부터 메시지 수신 및 송신
    while (1) {
        memset(buffer, 0, sizeof(buffer)); // 버퍼 초기화

        // 클라이언트로부터 메시지 수신
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            perror("메시지 수신 실패");
            break;
        }

        printf("클라이언트: %s\n", buffer);

        // 메시지 송신
        printf("서버: ");
        fgets(buffer, sizeof(buffer), stdin);
        send(client_socket, buffer, strlen(buffer), 0);
    }

    // 소켓 닫기
    close(server_socket);
    close(client_socket);

    return 0;
}

