#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

// 공유 자원: 제한된 크기의 버퍼
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

// 세마포어 선언
sem_t empty, full, mutex;

// 생산자 함수
void *producer(void *arg) {
    for (int i = 0; i < 5; ++i) {
        sem_wait(&empty); // 빈 자리가 있는지 확인
        sem_wait(&mutex); // 크리티컬 섹션에 진입

        // 생산자 동작: 아이템을 버퍼에 추가
        buffer[in] = i;
        printf("생산자: %d\n", buffer[in]);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 크리티컬 섹션을 나감
        sem_post(&full);  // 버퍼에 아이템이 추가됨을 알림
    }
    return NULL;
}

// 소비자 함수
void *consumer(void *arg) {
    for (int i = 0; i < 5; ++i) {
        sem_wait(&full);  // 버퍼에 아이템이 있는지 확인
        sem_wait(&mutex); // 크리티컬 섹션에 진입

        // 소비자 동작: 버퍼에서 아이템을 소비
        printf("소비자: %d\n", buffer[out]);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex); // 크리티컬 섹션을 나감
        sem_post(&empty); // 버퍼에 빈 자리가 생겼음을 알림
    }
    return NULL;
}

int main() {
    // 세마포어 초기화
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    // 생산자 및 소비자 쓰레드 생성
    pthread_t producer_thread1, producer_thread2, consumer_thread1, consumer_thread2;
    pthread_create(&producer_thread1, NULL, producer, NULL);
    pthread_create(&producer_thread2, NULL, producer, NULL);
    pthread_create(&consumer_thread1, NULL, consumer, NULL);
    pthread_create(&consumer_thread2, NULL, consumer, NULL);

    // 쓰레드 종료 대기
    pthread_join(producer_thread1, NULL);
    pthread_join(producer_thread2, NULL);
    pthread_join(consumer_thread1, NULL);
    pthread_join(consumer_thread2, NULL);

    // 세마포어 해제
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}

