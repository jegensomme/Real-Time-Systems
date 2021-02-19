/*
    Поставщик-Потребитель задание 1.2
    Кублицкий Витовт 7372
    Версия:1.0, 19.02.2021
*/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctime>

struct ThreadProperties {
    int flag;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    int time;
};

int buffer;
int size;

const double CLK_TCK = CLOCKS_PER_SEC / 1000;

time_t sec;

void log(char* text)
{
    printf("%s: %f\n\n", text, clock() / CLK_TCK);
}

void* consumer(void* threadProperties)
{
    log("Consumer start work");
    while ((((ThreadProperties*)threadProperties)->flag) != 0) {
        timespec timerBr;
        pthread_mutex_lock(((ThreadProperties*)threadProperties)->mutex);
        while (buffer == 0) {
            log("buffer is empty");
            clock_gettime(CLOCK_REALTIME, &timerBr);
            timerBr.tv_sec += ((ThreadProperties*)threadProperties)->time * 2;
            if (pthread_cond_timedwait(((ThreadProperties*)threadProperties)->cond, ((ThreadProperties*)threadProperties)->mutex, &timerBr) == ETIMEDOUT) {
                log("Consumer finished work");
                ((ThreadProperties*)threadProperties)->flag = -1;
                pthread_exit(threadProperties);
            }
        }
        log("Consumer entered the critical zone");
        printf("Buffer = %d: %f\n\n", --buffer, clock() / CLK_TCK);
        pthread_cond_signal(((ThreadProperties*)threadProperties)->cond);
        pthread_mutex_unlock(((ThreadProperties*)threadProperties)->mutex);
        log("Consumer leaved the critical zone");
        sleep(((ThreadProperties*)threadProperties)->time);
    }
    log("Consumer finished work");
    ((ThreadProperties*)threadProperties)->flag = 4;
    pthread_exit(threadProperties);
}

void* supplier(void* threadProperties)
{
    log("Supplier start work");
    while ((((ThreadProperties*)threadProperties)->flag) != 0) {
        timespec timerBr;
        pthread_mutex_lock(((ThreadProperties*)threadProperties)->mutex);
        while (buffer == size) {
            log("buffer is full");
            clock_gettime(CLOCK_REALTIME, &timerBr);
            timerBr.tv_sec += ((ThreadProperties*)threadProperties)->time * 2;
            if (pthread_cond_timedwait(((ThreadProperties*)threadProperties)->cond, ((ThreadProperties*)threadProperties)->mutex, &timerBr) == ETIMEDOUT) {
                log("Supplier leaved the critical zone");
                ((ThreadProperties*)threadProperties)->flag = -1;
                pthread_exit(threadProperties);
            }
        }
        log("Supplier entered the critical zone");
        printf("Buffer = %d: %f\n\n", ++buffer, clock() / CLK_TCK);
        pthread_cond_signal(((ThreadProperties*)threadProperties)->cond);
        pthread_mutex_unlock(((ThreadProperties*)threadProperties)->mutex);
        log("Supplier leaved the critical zone");
        sleep(((ThreadProperties*)threadProperties)->time);
    }
    log("Supplier finished work");
    ((ThreadProperties*)threadProperties)->flag = 5;
    pthread_exit(threadProperties);
}

int main(int argc, char* argv[])
{
    ThreadProperties threadProp1, threadProp2;
    printf("Enter buffer size and sleep time for supplier and consumer\n");
    scanf("%i %i %i", &size, &threadProp1.time, &threadProp2.time);
    log("Program start work, press 'x' to exit");
    pthread_t thread1, thread2;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    void* pCode[2];
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);
    threadProp1.flag = threadProp2.flag = 1;
    threadProp1.mutex = threadProp2.mutex = &mutex;
    threadProp1.cond = threadProp2.cond = &cond;
    pthread_create(&thread1, NULL, &supplier, &threadProp1);
    pthread_create(&thread2, NULL, &consumer, &threadProp2);
    while (getchar() != 'x') {
    }
    threadProp1.flag = threadProp2.flag = 0;
    pthread_join(thread1, &pCode[0]);
    pthread_join(thread2, &pCode[1]);
    printf("Supplier %s completed work with code: %d\n\n", *(int*)pCode[0] == 5 ? " successfully" : " unsuccessfully", *(int*)pCode[0]);
    printf("Consumer %s completed work with code: %d\n\n", *(int*)pCode[1] == 4 ? " successfully" : " unsuccessfully", *(int*)pCode[1]);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    log("Program finished work");
    return 0;
}
