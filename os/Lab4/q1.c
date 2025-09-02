#include <stdio.h>
#include <unistd.h> 
#include <string.h>
#include <ctype.h>  
#include <stdlib.h>

int main() { 
	int fd1[2], fd2[2] ; 
	pipe(fd1) ; 
	pipe(fd2) ; 

	// pipes : so this one is unidirectional communication channel bw processes
	//  fd[0]->read end and fd[1] is the write end: if we need dono direction se communication we neeed to setup two pipes  
	if( fork() == 0){ 
		close(fd1[1]) ; 
		char buf[100]; 
		read(fd1[0], buf , sizeof(buf)) ; 
		printf("The child reads : %s \n", buf) ;
		char ans[100] ;
		int len = 0 ; 
		for(int i = 0 ; buf[i]!='\0'; i++ ) { 
			char curr = buf[i] ;
			len++ ; 
			char newc ; 
			if((int)curr > 90){
				newc = toupper(curr) ; 
			} 
			else{ 
				newc = tolower(curr) ; 
			} 

			ans[i] = newc ;
		}
		ans[len] = '\0' ; 
		close(fd2[0]) ; 
		write(fd2[1], ans , len + 1) ;
		close(fd2[1]) ; 
	}
	else{
		close(fd1[0]) ; 
		char mess[100] ;  
		scanf("%99[^\n]",mess) ; 
		write(fd1[1],mess , strlen(mess)+1) ; 
		close(fd1[1]) ;

		close(fd2[1]) ; 
		char buff[100]  ; 	
		read(fd2[0] , buff , sizeof(buff) ) ; 
		printf("this is what the parent recieves from the child: %s \n",buff) ;
		close(fd2[0]) ; 
		
	} 

} 
