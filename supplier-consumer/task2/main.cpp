/*
	Поставщик-Потребитель задание 1.1
	Кублицкий Витовт 7372
	Версия:1.0, 19.02.2021
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <ctime>

struct ThreadProperties {
    int flag;
    sem_t* sem;
    sem_t* sem_full;
    sem_t* sem_empty;
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
        if (buffer == 0) {
            log("buffer is empty");
        }
        clock_gettime(CLOCK_REALTIME, &timerBr);
        timerBr.tv_sec += ((ThreadProperties*)threadProperties)->time * 2;
        if (sem_timedwait(((ThreadProperties*)threadProperties)->sem_empty, &timerBr) == -1) {
            log("Consumer finished work");
            ((ThreadProperties*)threadProperties)->flag = -1;
            pthread_exit(threadProperties);
        }
        sem_wait(((ThreadProperties*)threadProperties)->sem);
        log("Consumer entered the critical zone");
        printf("Buffer = %d: %f\n\n", --buffer, clock() / CLK_TCK);
        log("Consumer leaved the critical zone");
        sem_post(((ThreadProperties*)threadProperties)->sem_full);
        sem_post(((ThreadProperties*)threadProperties)->sem);
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
        if (buffer == size) {
            log("buffer is full");
        }
        clock_gettime(CLOCK_REALTIME, &timerBr);
        timerBr.tv_sec += ((ThreadProperties*)threadProperties)->time * 2;
        if (sem_timedwait(((ThreadProperties*)threadProperties)->sem_full, &timerBr) == -1) {
            log("Supplier finished work");
            ((ThreadProperties*)threadProperties)->flag = -1;
            pthread_exit(threadProperties);
        }
        sem_wait(((ThreadProperties*)threadProperties)->sem);
        log("Supplier entered the critical zone");
        printf("Buffer = %d: %f\n\n", ++buffer, clock() / CLK_TCK);
        log("Supplier leaved the critical zone");
        sem_post(((ThreadProperties*)threadProperties)->sem_empty);
        sem_post(((ThreadProperties*)threadProperties)->sem);
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
    sem_t sem;
    sem_t sem_full;
    sem_t sem_empty;
    void* pCode[2];
    sem_init(&sem, 0, 1);
    sem_init(&sem_full, 0, size);
    sem_init(&sem_empty, 0, 0);
    threadProp1.flag = threadProp2.flag = 1;
    threadProp1.sem = threadProp2.sem = &sem;
    threadProp1.sem_full = threadProp2.sem_full = &sem_full;
    threadProp1.sem_empty = threadProp2.sem_empty = &sem_empty;
    pthread_create(&thread1, NULL, &supplier, &threadProp1);
    pthread_create(&thread2, NULL, &consumer, &threadProp2);
    while (getchar() != 'x') {
    }
    threadProp1.flag = threadProp2.flag = 0;
    pthread_join(thread1, &pCode[0]);
    pthread_join(thread2, &pCode[1]);
    printf("Supplier %s completed work with code: %d\n\n", *(int*)pCode[0] == 5 ? " successfully" : " unsuccessfully", *(int*)pCode[0]);
    printf("Consumer %s completed work with code: %d\n\n", *(int*)pCode[1] == 4 ? " successfully" : " unsuccessfully", *(int*)pCode[1]);
    sem_destroy(&sem);
    sem_destroy(&sem_full);
    sem_destroy(&sem_empty);
    log("Program finished work");
    return 0;
}
