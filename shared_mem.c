#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>

#include "shared_mem.h"
#include "utils.h"


char *open_shared_mem_server(int *fd){
    //Remove previous /dev/shm
    shm_unlink(SHM_NAME);

    int oflag = O_CREAT | O_RDWR;
    *fd = shm_open(SHM_NAME, oflag, SHM_ACCESS_RIGHT);

    if (*fd == -1) {
        print_err("server: shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(*fd, SHM_SIZE) == -1) {
        print_err("server: ftruncate");
        exit(EXIT_FAILURE);
    }

    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);

    if (shm == MAP_FAILED) {
        print_err("server: mmap");
        exit(EXIT_FAILURE);
    }

    return shm;
}

char *open_shared_mem_client(int *fd){

    int oflag = O_RDWR;
    *fd = shm_open(SHM_NAME, oflag, SHM_ACCESS_RIGHT);

    if (*fd == -1) {
        print_err("client: shm_open");
        exit(EXIT_FAILURE);
    }

    char *shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, *fd, 0);

    if (shm == MAP_FAILED) {
        print_err("client: mmap");
        exit(EXIT_FAILURE);
    }

    return shm;
}

void close_shared_mem_client(char *shm, int fd){
    if(close(fd) == -1) {
        print_err("client: close(fd)");
    }

    if(munmap(shm, SHM_SIZE)){
        print_err("client: mmunmap");
    }
}

void close_shared_mem_server(char *shm, int fd){
    if(close(fd) == -1) {
        print_err("server: close(fd)");
    }

    if(munmap(shm, SHM_SIZE) == -1){
        print_err("server: mmunmap");
    }

    if(shm_unlink(SHM_NAME) == -1){
        print_err("server: shm_unlink");
    }
}


void unlink_semaphores(bool printErr){
    if (sem_unlink(SEM_REQUEST) == -1 && printErr) {
        print_err("sever: sem_unlink(SEM_REQUEST)");
    }

    if (sem_unlink(SEM_RESPONSE) == -1 && printErr) {
        print_err("sever: sem_unlink(SEM_RESPONSE)");
    }

    if (sem_unlink(SEM_MUTEX) == -1 && printErr) {
        print_err("sever: sem_unlink(SEM_MUTEX)");
    }
}

void close_semaphores(sem_t *sem_mutex, sem_t *sem_response, sem_t *sem_request){
    if (sem_close(sem_request) == -1) {
        print_err("sem_close(SEM_REQUEST)");
    }

    if (sem_close(sem_response) == -1) {
        print_err("sem_close(SEM_RESPONSE)");
    }

    if (sem_close(sem_mutex) == -1) {
        print_err("sem_close(SEM_MUTEX)");
    }
}

void clear_shm(char * shm){
    for (int i = 0; shm[i] != '\0'; i++) {
        shm[i] = '\0';
    }
}


