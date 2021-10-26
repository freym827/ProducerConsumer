/*********************************************
producer.c
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

    const int SHAREDSIZE = 4096;
    const char *sharedmem = "sharedmem";
    shm_unlink(sharedmem);
    char item[2046];

    int fd;
    void *mmapptr;

    //Set up shared memory
    fd = shm_open(sharedmem, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);

    if (fd == -1)
       errExit("shm_open");

    if (ftruncate(fd, SHAREDSIZE) == -1)
        errExit("ftruncate");

    mmapptr = mmap(0, SHAREDSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (mmapptr == MAP_FAILED)
                   errExit("mmap");

    //initialize semaphores
    sem_unlink(SEM1);
    sem_unlink(SEM2);
    int semval;
    sem_t *prodwait = sem_open(SEM1, O_CREAT, 0660, 2);
    if(prodwait == SEM_FAILED) {
        perror("sem_open failure");
        exit(0);
    }
    sem_t *conswait = sem_open(SEM2, O_CREAT, 0660, 0);
    if(conswait == SEM_FAILED) {
        perror("sem_open failure");
        exit(0);
    }

    //tell comsumer we are ready
    sem_post(doubleready);

    //produce items
    int offset = 2047;
    printf("To exit program enter 'q::'.\n");
    while (1) {
        memset(item, 0, sizeof item);
        
        printf("Enter item for the table: ");
        fgets(item, sizeof item, stdin);
        item[strcspn(item, "\r\n")] = 0;
        if(strcmp(item, "q::") == 0) {
            break;
        }
        
        //check table
        sem_wait(prodwait);
        
        //put item on table
        if(strlen((char*)mmapptr) != 0) {
            mmapptr += offset;
            sprintf(mmapptr, "%s", (char*)item);
            mmapptr -= offset;
        }else {
            sprintf(mmapptr, "%s", (char*)item);
        }
        
        //tell consumer an item is ready
        sem_post(conswait);   
    }
    shm_unlink(sharedmem);
    sem_unlink(SEM3);
    return 0;
}