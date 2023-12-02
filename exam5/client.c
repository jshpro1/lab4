#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 클라이언트 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버에 연결되었습니다.\n");

    // 메시지 수신 및 송신
    while (1) {
        printf("클라이언트: ");
        fgets(buffer, sizeof(buffer), stdin);

        // 메시지 송신
        send(client_socket, buffer, strlen(buffer), 0);

        // 서버로부터 메시지 수신
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            perror("메시지 수신 실패");
            break;
        }

        printf("서버: %s\n", buffer);
    }

    // 소켓 닫기
    close(client_socket);

    return 0;
}

