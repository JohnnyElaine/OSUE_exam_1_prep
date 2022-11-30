//
// Created by elias on 30.11.22.
//

#ifndef EXAM_1_PREP_SHARED_MEM_H
#define EXAM_1_PREP_SHARED_MEM_H


#define SHM_NAME "/exam_1_prep_shm"
#define SHM_ACCESS_RIGHT 0600
#define SHM_SIZE 256

#define SEM_MUTEX "exam_1_sem_mutex"
#define SEM_RESPONSE "exam_1_sem_response"
#define SEM_REQUEST "exam_1_sem_request"
#define ACCESS_RIGHTS 0600

char *open_shared_mem_client(int *fd);
char *open_shared_mem_server(int *fd);
void close_shared_mem_client(char *shm, int fd);
void close_shared_mem_server(char *shm, int fd);
void close_semaphores(sem_t *sem_mutex, sem_t *sem_response, sem_t *sem_request);
void unlink_semaphores(bool printErr);
void clear_shm(char * shm);


#endif //EXAM_1_PREP_SHARED_MEM_H
