/*
	Вызов лифта задание 2
	Кублицкий Витовт 7372
	Версия:1.0, 19.02.2021
*/

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define TOP 0 // лифт вверху
#define BOTTOM 1 // лифт внизу
#define THIS_FLOOR 2 // лифт на нашем этаже

#define UP 48 // Сигналы реального времени
#define DOWN 49

int state = THIS_FLOOR;

void move(int signal)
{
    int n = 3;
    if (signal == UP) {
        for (int i = 1; i <= n; i++) {
            printf("*** Go UP\n");
            usleep(500000);
            state = TOP;
        }
    }
    else {
        for (int i = 1; i <= n; i++) {
            printf("*** Go DOWN\n");
            usleep(500000);
            state = BOTTOM;
        }
    }
    printf("state %s\n", state == TOP ? "TOP" : "BOTTOM");
}

void lift_control(int signo, siginfo_t* info, void* nk)
{
    switch (state) {
    case THIS_FLOOR:
        if (signo == UP) {
            printf("This Floor, door is opened \n");
            sleep(1);
            move(UP);
        }
        if (signo == DOWN) {
            printf("This Floor, door is opened \n");
            sleep(1);
            move(DOWN);
        }
        break;
    case TOP:
        if (signo == UP) {
            move(DOWN);
            state = THIS_FLOOR;
            printf("This Floor, door is opened \n");
            sleep(1);
            move(UP);
        }
        if (signo == DOWN) {
            move(DOWN);
            state = THIS_FLOOR;
            printf("This Floor, door is opened \n");
            sleep(1);
            move(DOWN);
        }
        break;
    case BOTTOM:
        if (signo == UP) {
            move(UP);
            state = THIS_FLOOR;
            printf("This Floor, door is opened \n");
            sleep(1);
            move(UP);
        }
        if (signo == DOWN) {
            move(UP);
            state = THIS_FLOOR;
            printf("This Floor, door is opened \n");
            sleep(1);
            move(DOWN);
        }
        break;
    }
}

void* push_button(void* args)
{
    struct sigaction control_sigact;
    control_sigact.sa_sigaction = &lift_control;
    control_sigact.sa_mask = *((sigset_t*)args);
    control_sigact.sa_flags = SA_SIGINFO;
    sigaction(UP, &control_sigact, NULL);
    sigaction(DOWN, &control_sigact, NULL);
    siginfo_t info;
    info.si_value.sival_int = getpid();
    char ch;
    while (ch != 'q') {
        ch = getchar();
        switch (ch) {
        case 'u':
            sigqueue(getpid(), UP, info.si_value);
            break;
        case 'd':
            sigqueue(getpid(), DOWN, info.si_value);
            break;
        case 'q':
            break;
        }
        sigprocmask(SIG_UNBLOCK, (sigset_t*)args, NULL);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    pthread_t t;
    printf("Start\n");
    sigset_t control_sigset;
    sigemptyset(&control_sigset);
    sigaddset(&control_sigset, UP);
    sigaddset(&control_sigset, DOWN);
    sigprocmask(SIG_BLOCK, &control_sigset, NULL);
    void* control_sigset_ptr = &control_sigset;
    pthread_create(&t, NULL, &push_button, control_sigset_ptr);
    pthread_join(t, NULL);
    printf("Finish\n");
    return EXIT_SUCCESS;
}
