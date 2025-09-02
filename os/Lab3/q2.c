#include <stdio.h> 
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(){
	pid_t pid1 = -1 ; 
	pid_t pid2 = -1 ; 
	const char* ROLE = "PARENT" ; 
	printf("PREFORK ") ;

	pid1 = fork();
	if(pid1 < 0) { 
		perror("fork of child1 has been failed " ) ; 
		return 1 ; 
	} 
	if(pid1 == 0) { 
		ROLE = "CHILD 1"  ; 
	}else { 
		pid2 = fork() ; 
		if(pid2 < 0) { 
			perror("fork of child2 failed to be created") ; 
			return 1 ; 
		} 
		if(pid2== 0 ) { 
			ROLE = "CHILD 2";
		}else{ 
			ROLE= "PARENT" ; 
		} 
	}
	for(int i = 1 ; i<=3 ; i++ ) { 
		 printf("Message %d from %s | PID: %d | PPID: %d\n",
               i, ROLE, (int)getpid(), (int)getppid());
        fflush(stdout);    
	sleep(1);
	}
	if (pid1 > 0 && pid2 > 0) { 
	int status;
        waitpid(pid1, &status, 0);
        waitpid(pid2, &status, 0);
        printf("Both children finished execution\n");
    }
	return 0 ; 

//	printf("POSTFORK ") ;
}
