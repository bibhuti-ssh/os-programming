// author : bibhuti 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX 10
typedef struct{ 
	long fib_seq[MAX] ; 
	int seq_size ; 
} shared_data ; 

int main(int argc , char *argv[]){ 
        if( argc != 2) { 
                printf("Usage: %s <num_terms>\n", argv[0]) ; 
                return 1 ; 
        }
        int num = atoi(argv[1]) ; 
        if(num <= 0) { 
                printf("Error , the number entered cannt be negative") ; 
                return(1) ; 
        }

	if(num > MAX ) { 
		printf(" Error by parent , cauz the size breaks the limits  \n") ;
		return(1) ; 
	}
	const char *name = "/mysharedmemory"; 
	int sz = sizeof(shared_data) ; 	
	int f = shm_open(name , O_CREAT | O_RDWR , 0666) ;
	ftruncate(f, sz) ; 
	shared_data *shm_pointer = mmap(0 , sz , PROT_READ | PROT_WRITE , MAP_SHARED, f, 0);
	if( shm_pointer == MAP_FAILED ) { 
		perror("mmap") ; 
		exit(1) ; 
	}

        if(fork() == 0 ) { 
                //child it is
		shm_pointer->seq_size = num ;
		shm_pointer->fib_seq[0]= 0 ; 
		if(num >1) shm_pointer->fib_seq[1]= 1; 
		for(int i = 2; i< num ; i++ ) { 
			shm_pointer->fib_seq[i] = shm_pointer->fib_seq[i-1]+shm_pointer->fib_seq[i-2] ;  
		}
 
		munmap(shm_pointer , sz ) ; 
		close(f) ; 
                return(0) ;  

        } 
        else{ 
                //parent :

                wait(NULL) ;
               	printf("the wait for the child to do its operation is finished , now lets process .. \n") ; 
		printf("here is the fibonacci sequence \n") ;
		for(int i  = 0 ; i< shm_pointer->seq_size  ; ++i ) { 
			printf("%ld ", shm_pointer->fib_seq[i]) ; 
		}
		printf("\n") ; 

		munmap(shm_pointer , sz) ; 
		close(f) ; 
		shm_unlink(name) ; 

		return(0) ; 
                //waits for the child to finish
        }
} 


