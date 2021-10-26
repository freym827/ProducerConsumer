/*********************************************
consumer.c
Matthew Frey
**********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>       
#include <fcntl.h> 
#include <semaphore.h>

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                           } while (0)

#define SEM1 "sem1"
#define SEM2 "sem2"
#define SEM3 "sem3"

int main(int argc, char *argv[]) {
    
    sem_t *doubleready = sem_open(SEM3, O_CREAT, 0660, 0);
    if(doubleready == SEM_FAILED) {
        perror("sem_open failure");
        exit(0);
    }
    //wait for producer to set up shared memory
    sem_wait(doubleready);

    const int SHAREDSIZE = 4096;
    const char *sharedmem = "sharedmem";
    int fd;
    void *mmapptr;
    
    //open shared memory
    fd = shm_open(sharedmem, O_RDWR, 0);
    if (fd == -1)
       errExit("shm_open");

    mmapptr = mmap(0, SHAREDSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmapptr == MAP_FAILED)
                   errExit("mmap");

    //open semaphore
    int semval;
    sem_t *prodwait = sem_open(SEM1, 0);
    if(prodwait == SEM_FAILED) {
        perror("sem_open failure");
        exit(0);
    }
    sem_t *conswait = sem_open(SEM2, 0);
    if(conswait == SEM_FAILED) {
        perror("sem_open failure");
        exit(0);
    }

    char test;
    int offset = 0;
    printf("Press 'q' to quit or...\n");

    //consume items
    while (1) {
        printf("Press ENTER to consume an item...");
        test = getchar();
        if(test == 'q') {
            break;
        }
        
        //wait for producer to produce an item
        sem_wait(conswait);

        //print items and remove from memory
        if(offset == 0) {
            printf("%s\n", (char*)mmapptr);
            offset = 2047;
            memset((char*)mmapptr,0,2047);
        }else if(offset == 2047) {
            mmapptr += 2047;
            if(strlen((char*)mmapptr) == 0) {
                mmapptr -= 2047;
                printf("%s\n", (char*)mmapptr);
                offset = 2047;
                memset((char*)mmapptr,0,2047);
            }else if(strlen((char*)mmapptr) > 0) {
                printf("%s\n", (char*)mmapptr);
                memset((char*)mmapptr,0,2047);
                mmapptr -= 2047;
                offset = 0;
            }
        }
        
        //tell producer we consumed an item
        sem_post(prodwait);
    }
    
    shm_unlink(sharedmem);
    sem_unlink(SEM3);
    return 0;
}