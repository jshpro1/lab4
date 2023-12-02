#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t read_size;

    // 클라이언트로부터 HTTP 요청 읽기
    read_size = read(client_socket, buffer, sizeof(buffer));
    if (read_size < 0) {
        perror("HTTP 요청 읽기 실패");
        exit(EXIT_FAILURE);
    }

    // HTTP 요청 분석
    if (strncmp(buffer, "GET", 3) == 0) {
        // GET 요청 처리
        // 여기에서는 간단한 "Hello, World!" 응답을 보냄
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!\r\n";
        write(client_socket, response, strlen(response));
    } else if (strncmp(buffer, "POST", 4) == 0) {
        // POST 요청 처리
        // 여기에서는 간단한 "POST 요청 수신" 응답을 보냄
        const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nPOST 요청 수신\r\n";
        write(client_socket, response, strlen(response));
    } else {
        // 알 수 없는 요청
        const char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nBad Request\r\n";
        write(client_socket, response, strlen(response));
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

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

    printf("웹 서버가 클라이언트 연결을 대기 중입니다...\n");

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("클라이언트 연결 수락 실패");
            exit(EXIT_FAILURE);
        }

        printf("클라이언트가 연결되었습니다.\n");

        // 새로운 프로세스를 생성하여 클라이언트 처리
        if (fork() == 0) {
            close(server_socket);  // 자식 프로세스에서는 서버 소켓을 사용하지 않음
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            close(client_socket);  // 부모 프로세스에서는 클라이언트 소켓을 사용하지 않음
        }
    }

    // 부모 프로세스에서는 소켓 닫기
    close(server_socket);

    return 0;
}

