#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// 공유 자원
int shared_resource = 0;

// 뮤텍스 선언
pthread_mutex_t mutex;

// 쓰레드 함수 1
void *thread_function1(void *arg) {
    for (int i = 0; i < 5; ++i) {
        // 뮤텍스 락
        pthread_mutex_lock(&mutex);

        // 공유 자원 조작
        shared_resource++;
        printf("쓰레드 1: %d\n", shared_resource);

        // 뮤텍스 언락
        pthread_mutex_unlock(&mutex);

        // 잠시 대기
        sleep(1);
    }
    return NULL;
}

// 쓰레드 함수 2
void *thread_function2(void *arg) {
    for (int i = 0; i < 5; ++i) {
        // 뮤텍스 락
        pthread_mutex_lock(&mutex);

        // 공유 자원 조작
        shared_resource--;
        printf("쓰레드 2: %d\n", shared_resource);

        // 뮤텍스 언락
        pthread_mutex_unlock(&mutex);

        // 잠시 대기
        sleep(1);
    }
    return NULL;
}

int main() {
    // 뮤텍스 초기화
    pthread_mutex_init(&mutex, NULL);

    // 쓰레드 생성
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_function1, NULL);
    pthread_create(&thread2, NULL, thread_function2, NULL);

    // 쓰레드 종료 대기
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // 뮤텍스 해제
    pthread_mutex_destroy(&mutex);

    return 0;
}

