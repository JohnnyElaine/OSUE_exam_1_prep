#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "shared_mem.h"
#include "utils.h"

volatile sig_atomic_t quit = 0;

char *shm;
int fd = -1;

sem_t *sem_response;
sem_t *sem_request;
sem_t *sem_mutex;

void reverse_str(char *str, char *reverse_str) {
    int size_str = (int) strlen(str);

    sem_wait(sem_mutex);

    for (int i = 0; i < size_str;i++) {
        shm[i] = str[i];
    }
    shm[size_str] = '\0';

    sem_post(sem_request); // send request to server
    sem_wait(sem_response); // wait for response

    for (int i = 0; i < size_str;i++) {
        reverse_str[i] = shm[i];
    }

    // clear the shm for next client
    sem_post(sem_mutex);
}

void open_semaphores(){
    sem_mutex = sem_open(SEM_MUTEX, 0);
    if (sem_mutex == SEM_FAILED) {
        print_err("client: sem_open(SEM_MUTEX)");
    }

    sem_request = sem_open(SEM_REQUEST, 0);
    if (sem_request == SEM_FAILED) {
        print_err("client: sem_open(SEM_REQUEST)");
    }

    sem_response = sem_open(SEM_RESPONSE, 0);
    if (sem_response == SEM_FAILED) {
        print_err("client: sem_open(SEM_RESPONSE)");
    }
}

void handle_signal(int signal) { quit = 1; }


int main(int argc, char *argv[]){
    // signal handling
    struct sigaction sa = {.sa_handler = handle_signal};
    sigaction(SIGINT, &sa, NULL);

    // input str
    char *str = argv[1];
    int size_str = (int) strlen(str);
    char reverse[size_str];

    shm = open_shared_mem_client(&fd);

    open_semaphores();

    while (!quit) {
        //reverse the given str
        reverse_str(str, reverse);

        //print the reversed str
        printf("%s, reversed: %s\n", str, reverse);
    }

    //close shm & sem
    close_shared_mem_client(shm, fd);
    close_semaphores(sem_mutex, sem_response, sem_request);

    exit(EXIT_SUCCESS);
}