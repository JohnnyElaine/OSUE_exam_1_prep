#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include <bits/types/sig_atomic_t.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "shared_mem.h"
#include "utils.h"

volatile sig_atomic_t quit = 0;

char *shm;
int fd = -1;

sem_t *sem_response;
sem_t *sem_request;
sem_t *sem_mutex;

void open_semaphores(){
    // Remove previous /dev/shm
    unlink_semaphores(false);

    int oflag = O_CREAT | O_EXCL;

    sem_mutex = sem_open(SEM_MUTEX, oflag, ACCESS_RIGHTS, 1);
    if (sem_mutex == SEM_FAILED) {
        print_err("server: sem_open(SEM_MUTEX)");
    }

    sem_request = sem_open(SEM_REQUEST, oflag, ACCESS_RIGHTS, 0);
    if (sem_request == SEM_FAILED) {
        print_err("server: sem_open(SEM_REQUEST)");
    }

    sem_response = sem_open(SEM_RESPONSE, oflag, ACCESS_RIGHTS, 0);
    if (sem_response == SEM_FAILED) {
        print_err("server: sem_open(SEM_RESPONSE)");
    }

}

void reverse_string(char *original, char *reverse){
    int size = (int) strlen(original);

    for (int i = 0;i < size;i++) {
        reverse[i] = original[size - i - 1];
    }
}

void handle_request(){

    if(sem_wait(sem_request) == -1){
        print_err("server: sem_wait(sem_request)");
    }

    // process request

    char original[SHM_SIZE];

    // read str from shm
    for (int i = 0; ; i++) {
        original[i] = shm[i];
        if (shm[i] == '\0') break;
    }

    // reverse string
    int size_original = (int) strlen(original);
    char reverse[size_original];
    reverse_string(original, reverse);

    // write reverse to shm
    for (int i = 0; i < size_original;i++) {
        shm[i] = reverse[i];
    }

    if (sem_post(sem_response)){
        print_err("server: sem_post(sem_response)");
    }
}

void handle_signal(int signal) { quit = 1; }


int main(int argc, char *argv[]){
    // signal handling
    struct sigaction sa = {.sa_handler = handle_signal};
    sigaction(SIGINT, &sa, NULL);

    // create shared memory
    shm = open_shared_mem_server(&fd);

    // create semaphores
    open_semaphores();

    while (!quit) {
        handle_request();
    }

    //close everything
    close_shared_mem_server(shm ,fd);
    close_semaphores(sem_mutex, sem_response, sem_request);
    unlink_semaphores(true);


    exit(EXIT_SUCCESS);
}